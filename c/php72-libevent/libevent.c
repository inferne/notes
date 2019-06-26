/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Antony Dovgal <tony@daylessday.org>                          |
  |         Arnaud Le Blanc <lbarnaud@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_streams.h"
#include "php_network.h"
#include "php_libevent.h"

#include <signal.h>

#if PHP_VERSION_ID >= 50301 && (HAVE_SOCKETS || defined(COMPILE_DL_SOCKETS))
# include "ext/sockets/php_sockets.h"
# define LIBEVENT_SOCKETS_SUPPORT
#endif

#ifndef ZEND_FETCH_RESOURCE_NO_RETURN //(stream, php_stream *, fd, -1, NULL, php_file_le_stream())) 
# define ZEND_FETCH_RESOURCE_NO_RETURN(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type) \
	(rsrc = (rsrc_type) zend_fetch_resource(Z_RES_P(passed_id), resource_type_name, resource_type))
#endif

#ifdef PHP_WIN32
/* XXX compiling with 2.x on Windows. Luckily the ext code works thanks to the
compat exports from the libevent. However it might need to be adapted to the
never version, so this ifdefs would go away. */
# include <event2/event.h>
# include <event2/event_compat.h>
# include <event2/event_struct.h>
# include <event2/bufferevent.h>
# include <event2/bufferevent_compat.h>
#else
# include <event.h>
#endif

#if PHP_MAJOR_VERSION < 5
# ifdef PHP_WIN32
typedef SOCKET php_socket_t;
# else
typedef int php_socket_t;
# endif

# ifdef ZTS
#  define TSRMLS_FETCH_FROM_CTX(ctx)  void ***tsrm_ls = (void ***) ctx
#  define TSRMLS_SET_CTX(ctx)     ctx = (void ***) tsrm_ls
# else
#  define TSRMLS_FETCH_FROM_CTX(ctx)
#  define TSRMLS_SET_CTX(ctx)
# endif

# ifndef Z_ADDREF_P
#  define Z_ADDREF_P(x) (x)->refcount++
# endif
#endif

static int le_event_base;
static int le_event;
static int le_bufferevent;

#ifdef COMPILE_DL_LIBEVENT
ZEND_GET_MODULE(libevent)
#endif

typedef struct _php_event_base_t { /* {{{ */
	struct event_base *base;
	zend_resource *rsrc;
	uint32_t events;
} php_event_base_t;
/* }}} */

typedef struct _php_event_callback_t { /* {{{ */
	zval *func;
	zval *arg;
} php_event_callback_t;
/* }}} */

typedef struct _php_event_t { /* {{{ */
	struct event *event;
	zend_resource *rsrc;
	zend_resource *stream;
	php_event_base_t *base;
	php_event_callback_t *callback;
#ifdef ZTS
	void ***thread_ctx;
#endif
	int in_free;
} php_event_t;
/* }}} */

typedef struct _php_bufferevent_t { /* {{{ */
	struct bufferevent *bevent;
	zend_resource *rsrc;
	php_event_base_t *base;
	zval *readcb;
	zval *writecb;
	zval *errorcb;
	zval *arg;
#ifdef ZTS
	void ***thread_ctx;
#endif
} php_bufferevent_t;
/* }}} */
// zend_resource *res, const char *resource_type_name, int resource_type
#define ZVAL_TO_BASE(zval, base) \
	(base = zend_fetch_resource(Z_RES_P(zval), "event base", le_event_base))

#define ZVAL_TO_EVENT(zval, event) \
	(event = zend_fetch_resource(Z_RES_P(zval), "event", le_event))

#define ZVAL_TO_BEVENT(zval, bevent) \
	(bevent = zend_fetch_resource(Z_RES_P(zval), "buffer event", le_bufferevent))
//#define ADDREF_P(res) (*res).gc.refcount++
/* {{{ internal funcs */

static inline void _php_event_callback_free(php_event_callback_t *callback) /* {{{ */
{
	if (!callback) {
		return;
	}

	zval_ptr_dtor(callback->func);
	efree(callback->func);
	if (callback->arg) {
		zval_ptr_dtor(callback->arg);
		efree(callback->arg);
	}
	efree(callback);
}
/* }}} */

static void _php_event_base_dtor(zend_resource *rsrc TSRMLS_DC) /* {{{ */
{
	php_event_base_t *base = (php_event_base_t*)rsrc->ptr;

	event_base_free(base->base);
	efree(base);
}
/* }}} */

static void _php_event_dtor(zend_resource *rsrc TSRMLS_DC) /* {{{ */
{
	php_event_t *event = (php_event_t*)rsrc->ptr;
	int base_id = -1;

	if (event->in_free) {
		return;
	}
	//printf("_php_event_dtor: event->stream=%p, event->stream->refcount=%d, event->stream->handle=%d\n", event->stream, event->stream->gc.refcount, event->stream->handle);
	event->in_free = 1;

	if (event->base) {
		base_id = event->base->rsrc->handle;
		--event->base->events;
	}
	if (event->stream->handle >= 0) {
		zend_list_delete(event->stream);
	}
	event_del(event->event);
	
	_php_event_callback_free(event->callback);
	efree(event->event);
	efree(event);
	
	if (base_id >= 0) {
		//zend_list_delete(event->base->rsrc);
	}
}
/* }}} */

static void _php_bufferevent_dtor(zend_resource *rsrc TSRMLS_DC) /* {{{ */
{
	php_bufferevent_t *bevent = (php_bufferevent_t*)rsrc->ptr;
	int base_id = -1;
	//printf("_php_bufferevent_dtor: bevent=%p, bevent->bevent=%p, bevent->rsrc=%p, bevent->base->rsrc=%p\n", bevent, bevent->bevent, bevent->rsrc, bevent->base->rsrc);
	if (bevent->base) {
		base_id = bevent->base->rsrc->handle;
		--bevent->base->events;
	}
	//printf("_php_bufferevent_dtor: readcb=%p, writecb=%p, errorcb=%p, arg=%p\n", bevent->readcb, bevent->writecb, bevent->errorcb, bevent->arg);
	if (bevent->readcb) {
		zval_ptr_dtor(bevent->readcb);
		efree(bevent->readcb);
	}
	if (bevent->writecb) {
		zval_ptr_dtor(bevent->writecb);
		efree(bevent->writecb);
	}
	if (bevent->errorcb) {
		zval_ptr_dtor(bevent->errorcb);
		efree(bevent->errorcb);
	}
	if (bevent->arg) {
		zval_ptr_dtor(bevent->arg);
		//printf("_php_bufferevent_dtor: bevent->arg->refcount=%d, bevent->arg->handle=%d\n", Z_REFCOUNT_P(bevent->arg), Z_RES_HANDLE_P(bevent->arg));
		efree(bevent->arg);
	}
	
	bufferevent_free(bevent->bevent);
	efree(bevent);
	
	if (base_id >= 0) {
		//zend_list_delete(bevent->base->rsrc);
	}
}
/* }}} */

static void _php_event_callback(int fd, short events, void *arg) /* {{{ */
{
	zval args[3];
	php_event_t *event = (php_event_t *)arg;
	php_event_callback_t *callback;
	zval retval;
	TSRMLS_FETCH_FROM_CTX(event ? event->thread_ctx : NULL);

	if (!event || !event->callback || !event->base) {
		return;
	}
	
	callback = event->callback;
	//printf("_php_event_callback: fd=%d, event=%p, event->stream=%p, callback->func->refcount=%d\n", fd, event, event->stream, (*callback->func->value.str).gc.refcount);
	//MAKE_STD_ZVAL(args[0]);
	if (event->stream->handle >= 0) {
		ZVAL_RES(&args[0], event->stream);
		//ADDREF_P(event->stream);
	} else if (events & EV_SIGNAL) {
		ZVAL_LONG(&args[0], fd);
	} else {
		ZVAL_NULL(&args[0]);
	}
	//printf("_php_event_callback: callback=%p, callback->func=%s, event->stream=%p, callback->arg=%p\n", callback, Z_STRVAL_P(callback->func), event->stream, callback->arg);
	//MAKE_STD_ZVAL(args[1]);
	ZVAL_LONG(&args[1], events);
	
	args[2] = *(callback->arg);
	//Z_ADDREF_P(callback->arg);
	//printf("_php_event_callback: event->stream->refcount=%d\n", (*event->stream).gc.refcount);
	if (call_user_function(EG(function_table), NULL, callback->func, &retval, 3, args TSRMLS_CC) == SUCCESS) {
		zval_dtor(&retval); // stack ?printf("_php_event_callback: event->stream->refcount=%d, &retval=%p\n", (*event->stream).gc.refcount, &retval);
	}
	//printf("_php_event_callback: callback->func=%s, event->stream=%p, &args[0]=%p &args[1]=%p &args[2]=%p\n", Z_STR_P(callback->func), event->stream, &args[0], &args[1], &args[2]);
	//zval_ptr_dtor(&args[0]);
	//zval_ptr_dtor(&args[1]);
	//zval_ptr_dtor(&args[2]); 
	
}
/* }}} */

static void _php_bufferevent_readcb(struct bufferevent *be, void *arg) /* {{{ */
{
	zval args[2];
	zval retval;
	php_bufferevent_t *bevent = (php_bufferevent_t *)arg;
	TSRMLS_FETCH_FROM_CTX(bevent ? bevent->thread_ctx : NULL);

	if (!bevent || !bevent->base || !bevent->readcb) {
		return;
	}

	//MAKE_STD_ZVAL(args[0]);
	ZVAL_RES(&args[0], bevent->rsrc);
	//ADDREF_P(bevent->rsrc); /* we do refcount-- later in zval_ptr_dtor */
	
	args[1] = *(bevent->arg);
	//Z_ADDREF(args[1]);
	
	if (call_user_function(EG(function_table), NULL, bevent->readcb, &retval, 2, args TSRMLS_CC) == SUCCESS) {
		zval_dtor(&retval);
	}

	//zval_ptr_dtor(&args[0]);
	//zval_ptr_dtor(&args[1]); 

}
/* }}} */

static void _php_bufferevent_writecb(struct bufferevent *be, void *arg) /* {{{ */
{
	zval args[2];
	zval retval;
	php_bufferevent_t *bevent = (php_bufferevent_t *)arg;
	TSRMLS_FETCH_FROM_CTX(bevent ? bevent->thread_ctx : NULL);

	if (!bevent || !bevent->base || !bevent->writecb) {
		return;
	}

	//MAKE_STD_ZVAL(args[0]);
	ZVAL_RES(&args[0], bevent->rsrc);
	//ADDREF_P(bevent->rsrc); /* we do refcount-- later in zval_ptr_dtor */
	
	args[1] = *(bevent->arg);
	//Z_ADDREF(args[1]);
	
	if (call_user_function(EG(function_table), NULL, bevent->writecb, &retval, 2, args TSRMLS_CC) == SUCCESS) {
		zval_dtor(&retval);
	}

	//zval_ptr_dtor(&args[0]);
	//zval_ptr_dtor(&args[1]); 
	
}
/* }}} */

static void _php_bufferevent_errorcb(struct bufferevent *be, short what, void *arg) /* {{{ */
{
	zval args[3];
	zval retval;
	php_bufferevent_t *bevent = (php_bufferevent_t *)arg;
	TSRMLS_FETCH_FROM_CTX(bevent ? bevent->thread_ctx : NULL);

	if (!bevent || !bevent->base || !bevent->errorcb) {
		return;
	}

	//MAKE_STD_ZVAL(args[0]);
	ZVAL_RES(&args[0], bevent->rsrc);
	//ADDREF_P(bevent->rsrc); /* we do refcount-- later in zval_ptr_dtor */
	
	//MAKE_STD_ZVAL(args[1]);
	ZVAL_LONG(&args[1], what);

	args[2] = *(bevent->arg);
	//Z_ADDREF(args[2]);
	
	if (call_user_function(EG(function_table), NULL, bevent->errorcb, &retval, 3, args TSRMLS_CC) == SUCCESS) {
		zval_dtor(&retval);
	}

	//zval_ptr_dtor(&args[0]);
	//zval_ptr_dtor(&args[1]);
	//zval_ptr_dtor(&args[2]); 
	
}
/* }}} */

/* }}} */


/* {{{ proto resource event_base_new() 
 */
static PHP_FUNCTION(event_base_new)
{
	php_event_base_t *base;
	zval *zr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") != SUCCESS) {
		return;
	}

	base = emalloc(sizeof(php_event_base_t));
	base->base = event_base_new();
	if (!base->base) {
		efree(base);
		RETURN_FALSE;
	}
	event_init(); // init global base
	base->events = 0;
	
#if PHP_MAJOR_VERSION >= 5 && PHP_MINOR_VERSION >= 4
	zr = zend_list_insert(base, le_event_base TSRMLS_CC);
#else
	zr = zend_list_insert(base, le_event_base);
#endif
	zval_add_ref(zr);
	base->rsrc = Z_RES_P(zr);
	RETURN_RES(base->rsrc);
}
/* }}} */

/* {{{ proto bool event_base_reinit()
 */
static PHP_FUNCTION(event_base_reinit) {
    zval *zbase;
    php_event_base_t *base;
    int r = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zbase) != SUCCESS) {
        return;
    }

    ZVAL_TO_BASE(zbase, base);
    r = event_reinit(base->base);
    if (r == -1) {
        RETURN_FALSE
    } else {
        RETURN_TRUE;
    }
}
/* }}} */

/* {{{ proto void event_base_free(resource base) 
 */
static PHP_FUNCTION(event_base_free)
{
	zval *zbase;
	php_event_base_t *base;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zbase) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);

	if (base->events > 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "base has events attached to it and cannot be freed");
		RETURN_FALSE;
	}

	zend_list_delete(base->rsrc);
}
/* }}} */

/* {{{ proto int event_base_loop(resource base[, int flags]) 
 */
static PHP_FUNCTION(event_base_loop)
{
	zval *zbase;
	php_event_base_t *base;
	long flags = 0;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zbase, &flags) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);//printf("event_base_loop: base=%p\n", base);
	//Z_ADDREF_P(base->rsrc); /* make sure the base cannot be destroyed during the loop */
	ret = event_base_loop(base->base, flags);
	zend_list_delete(base->rsrc);

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool event_base_loopbreak(resource base) 
 */
static PHP_FUNCTION(event_base_loopbreak)
{
	zval *zbase;
	php_event_base_t *base;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zbase) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);
	ret = event_base_loopbreak(base->base);
	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_base_loopexit(resource base[, int timeout]) 
 */
static PHP_FUNCTION(event_base_loopexit)
{
	zval *zbase;
	php_event_base_t *base;
	int ret;
	long timeout = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zbase, &timeout) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);

	if (timeout < 0) {
		ret = event_base_loopexit(base->base, NULL);
	} else {
		struct timeval time;
		
		time.tv_usec = timeout % 1000000;
		time.tv_sec = timeout / 1000000;
		ret = event_base_loopexit(base->base, &time);
	}

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_base_set(resource event, resource base) 
 */
static PHP_FUNCTION(event_base_set)
{
	zval *zbase, *zevent;
	php_event_base_t *base, *old_base;
	php_event_t *event;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &zevent, &zbase) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);
	ZVAL_TO_EVENT(zevent, event);

	old_base = event->base;//printf("event_base_set: old_base=%p, base=%p\n", old_base, base);
	ret = event_base_set(base->base, event->event);

	if (ret == 0) {
		if (base != old_base) {
			/* make sure the base is destroyed after the event */
			//Z_ADDREF_P(base->rsrc);
			++base->events;
		}

		if (old_base && base != old_base) {
			--old_base->events;
			zend_list_delete(old_base->rsrc);
		}

		event->base = base;//printf("event_base_set: event=%p\n", event);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_base_priority_init(resource base, int npriorities) 
 */
static PHP_FUNCTION(event_base_priority_init)
{
	zval *zbase;
	php_event_base_t *base;
	long npriorities;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zbase, &npriorities) != SUCCESS) {
		return;
	}

	ZVAL_TO_BASE(zbase, base);

	if (npriorities < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "npriorities cannot be less than zero");
		RETURN_FALSE;
	}

	ret = event_base_priority_init(base->base, npriorities);
	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */


/* {{{ proto resource event_new() 
 */
static PHP_FUNCTION(event_new)
{
	php_event_t *event;
	zval *zr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") != SUCCESS) {
		return;
	}

	event = emalloc(sizeof(php_event_t));
	event->event = ecalloc(1, sizeof(struct event));

	event->stream = NULL;
	event->callback = NULL;
	event->base = NULL;
	event->in_free = 0;
	TSRMLS_SET_CTX(event->thread_ctx);

#if PHP_MAJOR_VERSION >= 5 && PHP_MINOR_VERSION >= 4
	zr = zend_list_insert(event, le_event TSRMLS_CC);
#else
	zr = zend_list_insert(event, le_event);
#endif
	zval_add_ref(zr);
	event->rsrc = Z_RES_P(zr);
	RETURN_RES(event->rsrc);
}
/* }}} */

/* {{{ proto void event_free(resource event) 
 */
static PHP_FUNCTION(event_free)
{
	zval *zevent;
	php_event_t *event;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zevent) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);
	zend_list_delete(event->rsrc);
}
/* }}} */

/* {{{ proto bool event_add(resource event[, int timeout])
 */
static PHP_FUNCTION(event_add)
{
	zval *zevent;
	php_event_t *event;
	int ret;
	long timeout = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zevent, &timeout) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);//printf("event_add: event=%p\n", event);

	if (!event->base) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to add event without an event base");
		RETURN_FALSE;
	}

	if (timeout < 0) {
		ret = event_add(event->event, NULL);
	} else {
		struct timeval time;
		
		time.tv_usec = timeout % 1000000;
		time.tv_sec = timeout / 1000000;
		ret = event_add(event->event, &time);
	}

	if (ret != 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool event_set(resource event, mixed fd, int events, mixed callback[, mixed arg]) 
 */
static PHP_FUNCTION(event_set)
{
	zval *zevent, *fd, *zcallback, *zarg = NULL;
	php_event_t *event;
	long events;
	php_event_callback_t *callback, *old_callback;
	//zend_string *func_name;
	php_stream *stream;
	php_socket_t file_desc;
#ifdef LIBEVENT_SOCKETS_SUPPORT
	php_socket *php_sock;
#endif
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzlz|z", &zevent, &fd, &events, &zcallback, &zarg) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);
	//printf("event_set: event=%p\n", event);
	if (events & EV_SIGNAL) {
		/* signal support */
		convert_to_long_ex(fd);
		file_desc = Z_LVAL_P(fd);
		if (file_desc < 0 || file_desc >= NSIG) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid signal passed");
			RETURN_FALSE;
		}
	} else {
		if (Z_TYPE_P(fd) == IS_RESOURCE) {
			if (ZEND_FETCH_RESOURCE_NO_RETURN(stream, php_stream *, fd, -1, NULL, php_file_le_stream())) {
				if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&file_desc, 1) != SUCCESS || file_desc < 0) {
					RETURN_FALSE;
				}
			} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
				if (ZEND_FETCH_RESOURCE_NO_RETURN(php_sock, php_socket *, fd, -1, NULL, php_sockets_le_socket())) {
					file_desc = php_sock->bsd_socket;
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be either valid PHP stream or valid PHP socket resource");
					RETURN_FALSE;
				}
#else
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource");
				RETURN_FALSE;
#endif
			}
		} else if (Z_TYPE_P(fd) == IS_LONG) {
			file_desc = Z_LVAL_P(fd);
			if (file_desc < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid file descriptor passed");
				RETURN_FALSE;
			}
		} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream or socket resource or a file descriptor of type long");
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource or a file descriptor of type long");
#endif
			RETURN_FALSE;
		}
	}

	if (!zend_is_callable(zcallback, 0, NULL TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid callback", Z_STRVAL_P(zcallback));
		//efree(func_name); // if efree func_name->val?
		RETURN_FALSE;
	}
	//efree(func_name); // because foo is interned string so refcount is 1
	//printf("event_set: zcallback=%p\n", zcallback);
	//zval_add_ref_unref(zcallback);printf("event_set: zcallback=%p, Z_TYPE_FLAGS(zcallback)=%d, Z_TYPE_FLAGS(zarg)=%d\n", zcallback, Z_TYPE_FLAGS_P(zcallback), Z_TYPE_FLAGS_P(zarg));
	if (zarg) {
		//zval_add_ref(zarg);
	} else {
		//ALLOC_INIT_ZVAL(zarg);
	}
	//printf("event_set: zarg=%p, zarg->refcount=%d, zcallback->refcount=%d\n", zarg, (*zarg->value.arr).gc.refcount, (*zcallback->value.str).gc.refcount);
	callback = emalloc(sizeof(php_event_callback_t));
	callback->func = emalloc(sizeof(zval));
	callback->arg = emalloc(sizeof(zval));
	ZVAL_COPY(callback->func, zcallback);//callback->func = zcallback;
	ZVAL_COPY(callback->arg, zarg);//ZVAL_NEW_STR()

	old_callback = event->callback;
	event->callback = callback;
	if (events & EV_SIGNAL) {
		event->stream = NULL;
	} else {
		//Z_ADDREF_P(fd);
		event->stream = Z_RES_P(fd);
	}
	//printf("event_set: fd=%p, fd->value->res=%p, old_callback=%p, callback=%p, file_desc=%d, fd->refcount=%d\n", fd, fd->value.res, old_callback, callback, (int)file_desc, (*fd->value.res).gc.refcount);
	event_set(event->event, (int)file_desc, (short)events, _php_event_callback, event);

	if (old_callback) {
		_php_event_callback_free(old_callback); // if free the prev callback will be free! old_callback address is prev callback? why?
	}
	//printf("event_set: zarg=%p, zarg->refcount=%d, zcallback->refcount=%d\n", zarg, (*zarg->value.arr).gc.refcount, (*zcallback->value.str).gc.refcount);
	if (event->base) {
		ret = event_base_set(event->base->base, event->event);
		if (ret != 0) {
			RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool event_del(resource event) 
 */
static PHP_FUNCTION(event_del)
{
	zval *zevent;
	php_event_t *event;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zevent) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);

	if (!event->base) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to delete event without an event base");
		RETURN_FALSE;
	}

	if (event_del(event->event) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_priority_set(resource event, int priority) 
 */
static PHP_FUNCTION(event_priority_set)
{
	zval *zevent;
	php_event_t *event;
	long priority;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zevent, &priority) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);

	if (!event->base) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set event priority without an event base");
		RETURN_FALSE;
	}

	ret = event_priority_set(event->event, priority);

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_timer_set(resource event, mixed callback[, mixed arg]) 
 */
static PHP_FUNCTION(event_timer_set)
{
	zval *zevent, *zcallback, *zarg = NULL;
	php_event_t *event;
	php_event_callback_t *callback, *old_callback;
	zend_string *func_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz|z", &zevent, &zcallback, &zarg) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);

	if (!zend_is_callable(zcallback, 0, &func_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid callback", func_name->val);
		//efree(func_name);
		RETURN_FALSE;
	}
	//efree(func_name);

	zval_add_ref(zcallback);
	if (zarg) {
		zval_add_ref(zarg);
	} else {
		ALLOC_INIT_ZVAL(zarg);
	}

	callback = emalloc(sizeof(php_event_callback_t));
	callback->func = zcallback;
	callback->arg = zarg;

	old_callback = event->callback;
	event->callback = callback;
	if (event->stream->handle >= 0) {
		zend_list_delete(event->stream); // maybe need stream
	}
	event->stream = NULL;

	event_set(event->event, -1, 0, _php_event_callback, event);

	if (old_callback) {
		//_php_event_callback_free(old_callback);
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool event_timer_pending(resource event[, int timeout])
 */
static PHP_FUNCTION(event_timer_pending)
{
	zval *zevent;
	php_event_t *event;
	int ret;
	long timeout = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zevent, &timeout) != SUCCESS) {
		return;
	}

	ZVAL_TO_EVENT(zevent, event);

	if (timeout < 0) {
		ret = event_pending(event->event, EV_TIMEOUT, NULL);
	} else {
		struct timeval time;
		
		time.tv_usec = timeout % 1000000;
		time.tv_sec = timeout / 1000000;
		ret = event_pending(event->event, EV_TIMEOUT, &time);
	}

	if (ret != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto resource event_buffer_new(mixed fd, mixed readcb, mixed writecb, mixed errorcb[, mixed arg]) 
 */
static PHP_FUNCTION(event_buffer_new)
{
	php_bufferevent_t *bevent;
	php_stream *stream;
	zval *zfd, *zreadcb, *zwritecb, *zerrorcb, *zarg = NULL, *zr;
	php_socket_t fd;
	zend_string *func_name;
#ifdef LIBEVENT_SOCKETS_SUPPORT
	php_socket *php_sock;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzz|z", &zfd, &zreadcb, &zwritecb, &zerrorcb, &zarg) != SUCCESS) {
		return;
	}
	
	if (Z_TYPE_P(zfd) == IS_RESOURCE) {
		if (ZEND_FETCH_RESOURCE_NO_RETURN(stream, php_stream *, zfd, -1, NULL, php_file_le_stream())) {
			if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&fd, 1) != SUCCESS || fd < 0) {
				RETURN_FALSE;
			}
		} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
			if (ZEND_FETCH_RESOURCE_NO_RETURN(php_sock, php_socket *, zfd, -1, NULL, php_sockets_le_socket())) {
				fd = php_sock->bsd_socket;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream or socket resource or a file descriptor of type long");
				RETURN_FALSE;
			}
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource or a file descriptor of type long");
			RETURN_FALSE;
#endif
		}
	} else if (Z_TYPE_P(zfd) == IS_LONG) {
		fd = Z_LVAL_P(zfd);
	} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream or socket resource or a file descriptor of type long");
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource or a file descriptor of type long");
#endif
		RETURN_FALSE;
	}

	if (Z_TYPE_P(zreadcb) != IS_NULL) {
		if (!zend_is_callable(zreadcb, 0, &func_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid read callback", func_name->val);
			//efree(func_name);
			RETURN_FALSE;
		}
		//efree(func_name);
	} else {
		zreadcb = NULL;
	}

	if (Z_TYPE_P(zwritecb) != IS_NULL) {
		if (!zend_is_callable(zwritecb, 0, &func_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid write callback", func_name->val);
			//efree(func_name);
			RETURN_FALSE;
		}
		//efree(func_name);
	} else {
		zwritecb = NULL;
	}

	if (!zend_is_callable(zerrorcb, 0, &func_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid error callback", func_name->val);
		//efree(func_name);
		RETURN_FALSE;
	}
	//efree(func_name);

	bevent = emalloc(sizeof(php_bufferevent_t));
	bevent->bevent = bufferevent_new(fd, _php_bufferevent_readcb, _php_bufferevent_writecb, _php_bufferevent_errorcb, (void *)bevent);
	bevent->rsrc = NULL;
	bevent->base = NULL;
	bevent->readcb = emalloc(sizeof(zval));
	if (zreadcb) {
		zval_add_ref(zreadcb);
	}
	ZVAL_COPY(bevent->readcb, zreadcb);
	bevent->writecb = NULL;
	if (zwritecb) {
		bevent->writecb = emalloc(sizeof(zval));
		zval_add_ref(zwritecb);
		ZVAL_COPY(bevent->writecb, zwritecb);
	}
	bevent->errorcb = emalloc(sizeof(zval));
	zval_add_ref(zerrorcb);
	ZVAL_COPY(bevent->errorcb, zerrorcb);
	bevent->arg = emalloc(sizeof(zval));
	if (zarg) {
		//zval_add_ref(zarg);
		ZVAL_COPY(bevent->arg, zarg);
	    //printf("event_buffer_new: bevent->arg->refcount=%d, bevent->arg->handle=%d\n", Z_REFCOUNT_P(bevent->arg), Z_RES_HANDLE_P(bevent->arg));
	} else {
		//ALLOC_INIT_ZVAL(bevent->arg);
	}

	//printf("event_buffer_new: zfd->refcount=%d, zfd->h=%d, stream=%p, fd=%d\n", Z_REFCOUNT_P(zfd), Z_RES_HANDLE_P(zfd), stream, fd);
	TSRMLS_SET_CTX(bevent->thread_ctx);
	//printf("event_buffer_new: bevent=%p, le_bufferevent=%d\n", bevent, le_bufferevent);
#if PHP_MAJOR_VERSION >= 5 && PHP_MINOR_VERSION >= 4
	zr = zend_list_insert(bevent, le_bufferevent TSRMLS_CC);
#else
	zr = zend_list_insert(bevent, le_bufferevent);
#endif
	zval_add_ref(zr);
	bevent->rsrc = Z_RES_P(zr);
	//printf("event_buffer_new: bevent->rsrc=%p, bevent->rsrc->handle=%d, bevent->base=%p\n", bevent->rsrc, bevent->rsrc->handle, bevent->base);
	RETURN_RES(bevent->rsrc);
}
/* }}} */

/* {{{ proto void event_buffer_free(resource bevent) 
 */
static PHP_FUNCTION(event_buffer_free)
{
	zval *zbevent;
	php_bufferevent_t *bevent;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zbevent) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);
	zend_list_delete(bevent->rsrc);
}
/* }}} */

/* {{{ proto bool event_buffer_base_set(resource bevent, resource base) 
 */
static PHP_FUNCTION(event_buffer_base_set)
{
	zval *zbase, *zbevent;
	php_event_base_t *base, *old_base;
	php_bufferevent_t *bevent;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &zbevent, &zbase) != SUCCESS) {
		return;
	}
	
	ZVAL_TO_BASE(zbase, base);
	ZVAL_TO_BEVENT(zbevent, bevent);
	//printf("event_buffer_base_set: base=%p, base->refcount=%d\n", base, base->rsrc->gc.refcount);
	old_base = bevent->base;
	ret = bufferevent_base_set(base->base, bevent->bevent);

	if (ret == 0) {
		if (base != old_base) {
			/* make sure the base is destroyed after the event */
			//Z_ADDREF_P(base->rsrc);
			++base->events;
		}

		if (old_base) {
			--old_base->events;
			zend_list_delete(old_base->rsrc);
		}

		bevent->base = base;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_buffer_priority_set(resource bevent, int priority) 
 */
static PHP_FUNCTION(event_buffer_priority_set)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	long priority;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zbevent, &priority) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	if (!bevent->base) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to set event priority without an event base");
		RETURN_FALSE;
	}

	ret = bufferevent_priority_set(bevent->bevent, priority);

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_buffer_write(resource bevent, string data[, int data_size]) 
 */
static PHP_FUNCTION(event_buffer_write)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	char *data;
	size_t data_len;
	size_t data_size = -1;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &zbevent, &data, &data_len, &data_size) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	if (ZEND_NUM_ARGS() < 3 || data_size < 0) {
		data_size = data_len;
	} else if (data_size > data_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_size out of range");
		RETURN_FALSE;
	}

	ret = bufferevent_write(bevent->bevent, (const void *)data, data_size);

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string event_buffer_read(resource bevent, int data_size) 
 */
static PHP_FUNCTION(event_buffer_read)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	zend_string *data;
	size_t data_size;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zbevent, &data_size) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	if (data_size == 0) {
		RETURN_EMPTY_STRING();
	} else if (data_size < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "data_size cannot be less than zero");
		RETURN_FALSE;
	}
	data = zend_string_alloc((int)data_size * sizeof(char), 0);//	data = safe_emalloc((int)data_size, sizeof(char), 1);

	ret = bufferevent_read(bevent->bevent, ZSTR_VAL(data), data_size);
	if (ret > 0) {
		if (ret > data_size) { /* paranoia */
			ret = data_size;
		}
		ZSTR_VAL(data)[ret] = '\0';ZSTR_LEN(data) = ret;
		RETURN_STR(data);//RETURN_STRINGL(data, ret);
	}
	efree(data);
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ proto bool event_buffer_enable(resource bevent, int events) 
 */
static PHP_FUNCTION(event_buffer_enable)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	long events;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zbevent, &events) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	ret = bufferevent_enable(bevent->bevent, events);

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool event_buffer_disable(resource bevent, int events) 
 */
static PHP_FUNCTION(event_buffer_disable)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	long events;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zbevent, &events) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	ret = bufferevent_disable(bevent->bevent, events);

	if (ret == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void event_buffer_timeout_set(resource bevent, int read_timeout, int write_timeout) 
 */
static PHP_FUNCTION(event_buffer_timeout_set)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	long read_timeout, write_timeout;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &zbevent, &read_timeout, &write_timeout) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);
	bufferevent_settimeout(bevent->bevent, read_timeout, write_timeout);
}
/* }}} */

/* {{{ proto void event_buffer_watermark_set(resource bevent, int events, int lowmark, int highmark) 
 */
static PHP_FUNCTION(event_buffer_watermark_set)
{
	zval *zbevent;
	php_bufferevent_t *bevent;
	long events, lowmark, highmark;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlll", &zbevent, &events, &lowmark, &highmark) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);
	bufferevent_setwatermark(bevent->bevent, events, lowmark, highmark);
}
/* }}} */

/* {{{ proto void event_buffer_fd_set(resource bevent, resource fd) 
 */
static PHP_FUNCTION(event_buffer_fd_set)
{
	zval *zbevent, *zfd;
	php_stream *stream;
	php_bufferevent_t *bevent;
	php_socket_t fd;
#ifdef LIBEVENT_SOCKETS_SUPPORT
	php_socket *php_sock;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &zbevent, &zfd) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	if (Z_TYPE_P(zfd) == IS_RESOURCE) {
		if (ZEND_FETCH_RESOURCE_NO_RETURN(stream, php_stream *, zfd, -1, NULL, php_file_le_stream())) {
			if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void*)&fd, 1) != SUCCESS || fd < 0) {
				RETURN_FALSE;
			}
		} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
			if (ZEND_FETCH_RESOURCE_NO_RETURN(php_sock, php_socket *, zfd, -1, NULL, php_sockets_le_socket())) {
				fd = php_sock->bsd_socket;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream or socket resource or a file descriptor of type long");
				RETURN_FALSE;
			}
#else
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource or a file descriptor of type long");
			RETURN_FALSE;
#endif
		}
	} else if (Z_TYPE_P(zfd) == IS_LONG) {
		fd = Z_LVAL_P(zfd);
	} else {
#ifdef LIBEVENT_SOCKETS_SUPPORT
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream or socket resource or a file descriptor of type long");
#else
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "fd argument must be valid PHP stream resource or a file descriptor of type long");
#endif
		RETURN_FALSE;
	}

	bufferevent_setfd(bevent->bevent, fd);
}
/* }}} */

/* {{{ proto resource event_buffer_set_callback(resource bevent, mixed readcb, mixed writecb, mixed errorcb[, mixed arg]) 
 */
static PHP_FUNCTION(event_buffer_set_callback)
{
	php_bufferevent_t *bevent;
	zval *zbevent, *zreadcb, *zwritecb, *zerrorcb, *zarg = NULL;
	zend_string *func_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rzzz|z", &zbevent, &zreadcb, &zwritecb, &zerrorcb, &zarg) != SUCCESS) {
		return;
	}

	ZVAL_TO_BEVENT(zbevent, bevent);

	if (Z_TYPE_P(zreadcb) != IS_NULL) {
		if (!zend_is_callable(zreadcb, 0, &func_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid read callback", func_name->val);
			//efree(func_name);
			RETURN_FALSE;
		}
		//efree(func_name);
	} else {
		zreadcb = NULL;
	}

	if (Z_TYPE_P(zwritecb) != IS_NULL) {
		if (!zend_is_callable(zwritecb, 0, &func_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid write callback", func_name->val);
			//efree(func_name);
			RETURN_FALSE;
		}
		//efree(func_name);
	} else {
		zwritecb = NULL;
	}

	if (Z_TYPE_P(zerrorcb) != IS_NULL) {
		if (!zend_is_callable(zerrorcb, 0, &func_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "'%s' is not a valid error callback", func_name->val);
			//efree(func_name);
			RETURN_FALSE;
		}
		//efree(func_name);
	} else {
		zerrorcb = NULL;
	}

	if (zreadcb) {
		if (bevent->readcb) {
			zval_ptr_dtor(bevent->readcb);
		} else {
			bevent->readcb = emalloc(sizeof(zval));
		}
		ZVAL_COPY(bevent->readcb, zreadcb);
	} else {
		if (bevent->readcb) {
			zval_ptr_dtor(bevent->readcb);
			efree(bevent->readcb);
		}
		bevent->readcb = NULL;
	}

	if (zwritecb) {
		if (bevent->writecb) {
			zval_ptr_dtor(bevent->writecb);
		} else {
			bevent->writecb = emalloc(sizeof(zval));
		}
		ZVAL_COPY(bevent->writecb, zwritecb);
	} else {
		if (bevent->writecb) {
			zval_ptr_dtor(bevent->writecb);
			efree(bevent->writecb);
		}
		bevent->writecb = NULL;
	}
	
	if (zerrorcb) {
		if (bevent->errorcb) {
			zval_ptr_dtor(bevent->errorcb);
		} else {
			bevent->errorcb = emalloc(sizeof(zval));
		}
		ZVAL_COPY(bevent->errorcb, zerrorcb);
	}
	
	if (zarg) {
		if (bevent->arg) {
			zval_ptr_dtor(bevent->arg);
		} else {
			bevent->arg = emalloc(sizeof(zval));
		}
		ZVAL_COPY(bevent->arg, zarg);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void event_last_error(int error) 
 */
static PHP_FUNCTION(event_last_error)
{
	// zend_long error;
	// zend_string *data;
	// char *str;
	// size_t str_len;

	// if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &error) != SUCCESS) {
	// 	return;
	// }

	// if (error) {
	// 	/* code */
	// } else {
	// 	str = evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR());
	// 	str_len = strlen(str);
	// 	data = zend_string_init(str, str_len, 0);
	// }
	printf("event_last_error: %s\n", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));
	// RETURN_STR(data);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
static PHP_MINIT_FUNCTION(libevent)
{
	le_event_base = zend_register_list_destructors_ex(_php_event_base_dtor, NULL, "event base", module_number);
	le_event = zend_register_list_destructors_ex(_php_event_dtor, NULL, "event", module_number);
	le_bufferevent = zend_register_list_destructors_ex(_php_bufferevent_dtor, NULL, "buffer event", module_number);

	REGISTER_LONG_CONSTANT("EV_TIMEOUT", EV_TIMEOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_READ", EV_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_WRITE", EV_WRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_SIGNAL", EV_SIGNAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EV_PERSIST", EV_PERSIST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVLOOP_NONBLOCK", EVLOOP_NONBLOCK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVLOOP_ONCE", EVLOOP_ONCE, CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("EVBUFFER_READ", EVBUFFER_READ, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVBUFFER_WRITE", EVBUFFER_WRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVBUFFER_EOF", EVBUFFER_EOF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVBUFFER_ERROR", EVBUFFER_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVBUFFER_TIMEOUT", EVBUFFER_TIMEOUT, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
static PHP_MINFO_FUNCTION(libevent)
{
	char buf[64];


	php_info_print_table_start();
	php_info_print_table_header(2, "libevent support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_LIBEVENT_VERSION);
	php_info_print_table_row(2, "Revision", "$Revision$");
	
	snprintf(buf, sizeof(buf) - 1, "%s", event_get_version());
	php_info_print_table_row(2, "libevent version", buf);

	php_info_print_table_end();
}
/* }}} */

#if PHP_MAJOR_VERSION >= 5
/* {{{ arginfo */
#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 3) || PHP_MAJOR_VERSION > 5
# define EVENT_ARGINFO
#else
# define EVENT_ARGINFO static
#endif

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_base_loop, 0, 0, 1)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_base_loopbreak, 0, 0, 1)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_base_loopexit, 0, 0, 1)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_base_set, 0, 0, 2)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_base_priority_init, 0, 0, 2)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, npriorities)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO(arginfo_event_new, 0)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_add, 0, 0, 1)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_set, 0, 0, 4)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, fd)
	ZEND_ARG_INFO(0, events)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_del, 0, 0, 1)
	ZEND_ARG_INFO(0, event)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_priority_set, 0, 0, 2)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_new, 0, 0, 4)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, readcb)
	ZEND_ARG_INFO(0, writecb)
	ZEND_ARG_INFO(0, errorcb)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_free, 0, 0, 1)
	ZEND_ARG_INFO(0, bevent)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_base_set, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_priority_set, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_write, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, data_size)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_read, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, data_size)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_disable, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, events)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_timeout_set, 0, 0, 3)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, read_timeout)
	ZEND_ARG_INFO(0, write_timeout)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_watermark_set, 0, 0, 4)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, events)
	ZEND_ARG_INFO(0, lowmark)
	ZEND_ARG_INFO(0, highmark)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_fd_set, 0, 0, 2)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_buffer_set_callback, 0, 0, 4)
	ZEND_ARG_INFO(0, bevent)
	ZEND_ARG_INFO(0, readcb)
	ZEND_ARG_INFO(0, writecb)
	ZEND_ARG_INFO(0, errorcb)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_timer_set, 0, 0, 2)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_timer_pending, 0, 0, 1)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

EVENT_ARGINFO
ZEND_BEGIN_ARG_INFO_EX(arginfo_event_last_error, 0, 0, 0)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ libevent_functions[]
 */
#if ZEND_MODULE_API_NO >= 20071006
const 
#endif
zend_function_entry libevent_functions[] = {
	PHP_FE(event_base_new, 				arginfo_event_new)
	PHP_FE(event_base_reinit, 			arginfo_event_base_loopbreak)
	PHP_FE(event_base_free, 			arginfo_event_base_loopbreak)
	PHP_FE(event_base_loop, 			arginfo_event_base_loop)
	PHP_FE(event_base_loopbreak, 		arginfo_event_base_loopbreak)
	PHP_FE(event_base_loopexit, 		arginfo_event_base_loopexit)
	PHP_FE(event_base_set, 				arginfo_event_base_set)
	PHP_FE(event_base_priority_init, 	arginfo_event_base_priority_init)
	PHP_FE(event_new, 					arginfo_event_new)
	PHP_FE(event_free, 					arginfo_event_del)
	PHP_FE(event_add, 					arginfo_event_add)
	PHP_FE(event_set, 					arginfo_event_set)
	PHP_FE(event_del, 					arginfo_event_del)
	PHP_FE(event_priority_set, 			arginfo_event_priority_set)
	PHP_FE(event_buffer_new, 			arginfo_event_buffer_new)
	PHP_FE(event_buffer_free, 			arginfo_event_buffer_free)
	PHP_FE(event_buffer_base_set, 		arginfo_event_buffer_base_set)
	PHP_FE(event_buffer_priority_set, 	arginfo_event_buffer_priority_set)
	PHP_FE(event_buffer_write, 			arginfo_event_buffer_write)
	PHP_FE(event_buffer_read, 			arginfo_event_buffer_read)
	PHP_FE(event_buffer_enable, 		arginfo_event_buffer_disable)
	PHP_FE(event_buffer_disable, 		arginfo_event_buffer_disable)
	PHP_FE(event_buffer_timeout_set, 	arginfo_event_buffer_timeout_set)
	PHP_FE(event_buffer_watermark_set, 	arginfo_event_buffer_watermark_set)
	PHP_FE(event_buffer_fd_set, 		arginfo_event_buffer_fd_set)
	PHP_FE(event_buffer_set_callback, 	arginfo_event_buffer_set_callback)
	PHP_FALIAS(event_timer_new,			event_new,		arginfo_event_new)
	PHP_FE(event_timer_set,				arginfo_event_timer_set)
	PHP_FE(event_timer_pending,			arginfo_event_timer_pending)
	PHP_FALIAS(event_timer_add,			event_add,		arginfo_event_add)
	PHP_FALIAS(event_timer_del,			event_del,		arginfo_event_del)
	PHP_FE(event_last_error,			arginfo_event_last_error)
	{NULL, NULL, NULL}
};
/* }}} */
#else
/* {{{ libevent_functions[]
 */
zend_function_entry libevent_functions[] = {
	PHP_FE(event_base_new, 				NULL)
	PHP_FE(event_base_reinit, 			NULL)
	PHP_FE(event_base_free, 			NULL)
	PHP_FE(event_base_loop, 			NULL)
	PHP_FE(event_base_loopbreak, 		NULL)
	PHP_FE(event_base_loopexit, 		NULL)
	PHP_FE(event_base_set, 				NULL)
	PHP_FE(event_base_priority_init,	NULL)
	PHP_FE(event_new, 					NULL)
	PHP_FE(event_free, 					NULL)
	PHP_FE(event_add, 					NULL)
	PHP_FE(event_set, 					NULL)
	PHP_FE(event_del, 					NULL)
	PHP_FE(event_priority_set, 			NULL)
	PHP_FE(event_buffer_new, 			NULL)
	PHP_FE(event_buffer_free, 			NULL)
	PHP_FE(event_buffer_base_set, 		NULL)
	PHP_FE(event_buffer_priority_set, 	NULL)
	PHP_FE(event_buffer_write, 			NULL)
	PHP_FE(event_buffer_read, 			NULL)
	PHP_FE(event_buffer_enable, 		NULL)
	PHP_FE(event_buffer_disable, 		NULL)
	PHP_FE(event_buffer_timeout_set, 	NULL)
	PHP_FE(event_buffer_watermark_set, 	NULL)
	PHP_FE(event_buffer_fd_set, 		NULL)
	PHP_FALIAS(event_timer_new,			event_new,	NULL)
	PHP_FE(event_timer_set,				NULL)
	PHP_FE(event_timer_pending,			NULL)
	PHP_FALIAS(event_timer_add,			event_add,	NULL)
	PHP_FALIAS(event_timer_del,			event_del,	NULL)
	PHP_FE(event_last_error,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */
#endif

static const zend_module_dep libevent_deps[] = { /* {{{ */
	ZEND_MOD_OPTIONAL("sockets")
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ libevent_module_entry
 */
zend_module_entry libevent_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	libevent_deps,
	"libevent",
	libevent_functions,
	PHP_MINIT(libevent),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(libevent),
	PHP_LIBEVENT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
