#include	"unp.h"
#include	<time.h>

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	socklen_t		len;
	char			buff[MAXLINE];
	time_t			ticks;
	struct sockaddr_storage	cliaddr;

	if (argc != 2)
		err_quit("usage: daytimetcpsrv1 <service or port#>");

	//第1个参数设为NULL，而且由tcp_listen内部指定的地址族为AF_UNSPEC，两者结合
	//    可能导致getaddrinfo返回非期望地址族的套接字地址结构。举例来说，在双栈
	//    主机上返回的第一个套接字地址结构将是IPv6的，但是我们可能希望该服务器仅仅
	//    处理IPv4
	//第3个参数设为NULL表示不关心当前地址族在使用多大的地址结构，将使用sockaddr_storage
	listenfd = Tcp_listen(NULL, argv[1], NULL);

	for ( ; ; ) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *)&cliaddr, &len);
		printf("connection from %s\n", Sock_ntop((SA *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}
