#include	"unp.h"

int
main(int argc, char **argv)
{
	int				sockfd, n;
	char			recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr_storage	ss;

	if (argc != 3)
		err_quit("usage: daytimetcpcli <hostname/IPaddress> <service/port#>");

	sockfd = Tcp_connect(argv[1], argv[2]);

	//tcp_connect并不返回内部connect用到的套接字地址结构大小。可以增设一个指针
	//参数来返回该值，然而本函数的设计目的之一是相比getaddrinfo减少参数的数目。
	//于是改用一个sockaddr_storage套接字地址结构，它大得足以存放系统支持的任何
	//套接字地址类型，又能满足它们的对齐限制
	len = sizeof(ss);
	//获取服务器的协议地址并显示
	Getpeername(sockfd, (SA *)&ss, &len);
	printf("connected to %s\n", Sock_ntop_host((SA *)&ss, len));

	while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
	exit(0);
}
