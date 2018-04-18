#include	"unp.h"

int
main(int argc, char **argv)
{
	int					i, sockfd[5];
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	//发起5个连接
	for (i = 0; i < 5; i++) {
		sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		Connect(sockfd[i], (SA *) &servaddr, sizeof(servaddr));
	}

	//只在第一个连接处进程“回射”
	str_cli(stdin, sockfd[0]);		/* do it all */

	//当键入EOF时，客户端退出，几乎在同一时刻向服务器发起5个FIN
	//服务器也几乎在同一时刻终止5个子进程
	exit(0);
}
