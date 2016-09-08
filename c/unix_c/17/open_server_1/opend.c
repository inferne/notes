#include "opend.h"
#include <fcntl.h>

void request(char *buf, int nread, int fd)
{
    int   newfd;
    if (buf[nread-1] != 0) {
        sprintf(errmsg, "request not null terminated: %*.*s\n");
        send_err(fd, -1, errmsg);
        return;
    }
    if (buf_args(buf, cli_args) < 0) { /* parse args & set options */
        send_err(fd, -1, errmsg);
        return;
    }
    if ((newfd = open(pathname, oflag)) < 0) {
        sprintf(errmsg, "can't open %s: %s\n", pathname, strerror(errno));
        return;
    }
    if (send_fd(fd, newfd) < 0) /* send the descriptor */
        err_sys("send_fd error");
    close(newfd);    /* we're done with descriptor */
}