#include "../lib/apue.h"
#include <setjmp.h>
#include <time.h>
#include <error.h>

static void    sig_usr1(int), sig_alrm(int);

static sigjmp_buf    jmpbuf;
static volatile sig_atomic_t    canjump;

void pr_mask(const char *str);

int
main(void)
{
    if(signal(SIGUSR1, sig_usr1) == SIG_ERR)
        err_sys("signal(SIGUSR1) error");
    if(signal(SIGALRM, sig_alrm) == SIG_ERR)
        err_sys("signal(SIGALRM) error");
    pr_mask("starting main:");

    if(sigsetjmp(jmpbuf, 1)){
        pr_mask("ending main:");
        exit(0);
    }
    canjump = 1; /* now sigsetjmp() is ok */

    for( ; ; )
        pause();
}

static void
sig_usr1(int signo)
{
    time_t starttime;

    if(canjump == 0)
        return ; /* unexpected signal,ignore */

    pr_mask("starting sig_usr1:");
    alarm(3);
    starttime = time(NULL);
    for( ; ; ) /* busy wait for 5 seconds */
        if(time(NULL) > starttime + 5)
            break;
    pr_mask("finishing sig_usr1:");

    canjump = 0;
    siglongjmp(jmpbuf, 1); /* jump back to main, don't return */
}

static void
sig_alrm(int signo)
{
    pr_mask("in sig_alrm:");
}

void
pr_mask(const char *str)
{
    sigset_t sigset;
    int      errno_save;

    errno_save = errno;    /* we can be called by signal handlers */
    if(sigprocmask(0, NULL, &sigset) < 0){
        err_sys("sigprocmask error");
    }
    printf("%s\n", str);
    if(sigismember(&sigset, SIGINT))  printf("SIGINT\n");
    if(sigismember(&sigset, SIGQUIT)) printf("SIGQUIT\n");
    if(sigismember(&sigset, SIGUSR1)) printf("SIGUSR1\n");
    if(sigismember(&sigset, SIGALRM)) printf("SIGALRM\n");

    /* remaining signals can go here */

    printf("\n");
    errno = errno_save;
}

// ./10.14.out &
// [2] 27605
// starting main: 
// kill -USR1 27605
// starting sig_usr1:
// in sig_alrm:
// finishing sig_usr1:
// ending main:
// [2]+  Done                    ./10.14.out
