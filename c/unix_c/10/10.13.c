#include "../lib/apue.h"

/* Reliable version of signal(), using POSIX sigaction(). */
Sigfunc *
signal_intr(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    
    act.sa_handle = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    if(sigaction(signo, &act, &oact) < 0){
        return SIG_ERR;
    }
    return oact.sa_handle;
}