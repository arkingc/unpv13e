/*
 *            fgets --------- sendto   recvfrom -----------
 * 标准输入-------->|         |---------------->|           |
 *                 | UDP客户 |                  | UDP服务器 |
 * 标准输出<--------|         |<----------------|           |
 *            fputs --------- recvfrom   sendto -----------
 */

#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: udpcli <IPaddress>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);//服务器端口
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);//传入服务器地址参数

	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

	exit(0);
}
