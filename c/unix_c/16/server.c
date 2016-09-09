#include "../lib/apue.h"
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>

#define MAXLEEP 128

#define BUFLEN 128
#define QLEN 10
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

/* 初始化server */
int init_server(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd;
    int err = 0;

    /* 创建socket */
    if ((fd = socket(addr->sa_family, type, 0)) < 0) 
        return -1;
    /* 绑定地址 */
    if (bind(fd, addr, alen) < 0) {
        err = errno;
        goto errout;
    }
    /* SOCK_STREAM 和 SOCK_SEQPACKET 类型需要listen */
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(fd, qlen) < 0) {
            err = errno;
            goto errout;
        }
    }
    return fd;
errout: /* 出错返回 */
    close(fd);
    errno = err;
    return -1;
}
/* server */
void serve(int sockfd)
{
    int clfd;
    FILE *fp;
    char buf[BUFLEN];

    for ( ; ; ) {
        /* 阻塞接收连接请求 */
        clfd = accept(sockfd, NULL, NULL);
        if (clfd < 0) {
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
            exit(1);
        }
        /* 执行uptime，若出错则返回客户端 */
        if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
            sprintf(buf, "error: %s\n", strerror(errno));
            send(clfd, buf, strlen(buf), 0);
        } else {
            /* 接收uptime输出结果并发送客户端 */
            while (fgets(buf, BUFLEN, fp) != NULL)
                send(clfd, buf, strlen(buf), 0);
            pclose(fp);/* 关闭fp */
        }
        close(clfd);/* 关闭客户端连接 */
    }
}
/* server main */
int server_main(int argc)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo  hint;
    int              sockfd, err, n;
    char            *host;

    if (argc != 1)
        err_quit("usage: ruptimed");
#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if (n < 0) /* best guess */
#endif
    n = HOST_NAME_MAX;
    host = malloc(n);
    if (host == NULL)
        err_sys("malloc error");
    if (gethostname(host, n) < 0)
        err_sys("gethostname error");
    //daemonize("ruptimed");
    /* 初始化hint */
    hint.ai_flags     = AI_CANONNAME;
    hint.ai_family    = 0;
    hint.ai_socktype  = SOCK_STREAM;
    hint.ai_protocol  = 0;
    hint.ai_addrlen   = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr      = NULL;
    hint.ai_next      = NULL;
    /* 获取地址信息 */
    if ((err = getaddrinfo(host, "ruptimed", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s", gai_strerror(err));
        exit(1);
    }
    /* 在第一个可用的地址上启动server */
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = init_server(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }
    exit(1);
}

int tcp_client(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
    int nsec;

    for (nsec = 1; nsec <= MAXLEEP; nsec <<= 1) {
        if (connect(sockfd, addr, alen) == 0) {
            return 0;
        }
        if (nsec <= MAXLEEP/2)
            sleep(nsec);
    }
    return -1;
}

int main(int argc, char const *argv[])
{
    int n = 64;
    char *host;
    host = malloc(n);
    gethostname(host, n);
    printf("%s\n", host);

    server_main(argc);
    return 0;
}