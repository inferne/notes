#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	socklen_t			salen;
	struct sockaddr		*sa;

	if (argc != 3)
		err_quit("usage: udpcli01 <hostname> <service>");

	sockfd = Udp_client(argv[1], argv[2], &sa, &salen);

	dg_cli(stdin, sockfd, sa, salen);

	exit(0);
}
