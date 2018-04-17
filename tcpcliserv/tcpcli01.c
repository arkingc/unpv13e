/*
 *            fgets --------- writen       read -----------
 * 标准输入-------->|         |---------------->|           |
 *                 | TCP客户 |                  | TCP服务器 |
 * 标准输出<--------|         |<----------------|           |
 *            fputs --------- readline   writen -----------
 *            
 */
#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");
	
	//1.创建套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	//2.创建服务器地址结构
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);//服务器监听套接字的端口号
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);//从输入读取服务器ip

	//3.发起TCP 3次握手建立连接
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	//4.从输入按行读取文本，发送至服务器，接收并显示回显
	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
