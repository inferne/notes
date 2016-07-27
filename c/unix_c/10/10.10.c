#include "../lib/apue.h"
#include <error.h>

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