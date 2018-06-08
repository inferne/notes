#include "unp.h"

void main()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t chilen;
	struct sockaddr_in cliaddr, servaddr;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld); /* must call waitpid */
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = Accept(listenfd, (SA *) &cliaddr, &clilen)) < 0 ) {
			if ( errno == EINTR ) {
				continue;      /* back to for */
			}else
				err_sys("accept error");
		}
		if ( (childpid = Fork()) == 0 ) { /* child process */
			Close(listenfd);    /* close listening socket */
			str_echo(connfd);   /* process the request */
			exit(0);
		}
		Close(connfd);          /* parent close connected socket */
	}

	return 0;
}