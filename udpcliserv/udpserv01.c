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
	struct sockaddr_in	servaddr, cliaddr;

	//第二个参数指定为SOCK_DGRAM创建一个UDP套接字
	sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//通配地址INADDR_ANY(0)
	servaddr.sin_port        = htons(SERV_PORT);	//9877号端口

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

	//回射函数
	//该函数循环调用recvfrom和sendto，接收客户端的数据报并回射，
	//提供的是一个迭代服务器，而不是像TCP服务器那样可以提供一个
	//并发服务器。其中没有对fork的调用，因此单个服务器进程就得处
	//理所有客户。一般来说，大多数TCP服务器是并发的，而大多数UDP服务器是迭代的
	dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
}
