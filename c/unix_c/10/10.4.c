#include <signal.h>
#include <unistd.h>

static void 
sig_alrm(int signo)
{
    /* nothing to do,jast return to wake up the pause */
}

unsigned int
sleep1(unsigned int nsecs)
{
    if(signal(SIGALRM, sig_alrm) == SIG_ERR){
        return nsecs;
    }
    unsigned int n;
    n = alarm(nsecs);     /* start the timer */
    if(n < nsecs){
        alarm(n);
    }
    pause();          /* next caught signal wakes us up */
    if(n > nsecs){
        alarm(n-nsecs); //reset
    }else{
        return alarm(0);  /* turn off timer, return unslept time */
    }
}