
/*
 * UDP回射服务器回射函数
 */

#include	"unp.h"

void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int			n;
	socklen_t	len;
	char		mesg[MAXLINE];//4K大小的缓冲

	//循环读取并回射下一个到达服务器的数据报
	for ( ; ; ) {
		len = clilen;
		//Recvfrom返回时，发送数据报的客户端套接字地址信息存在pcliaddr中
		//len是Recvfrom实际在pcliaddr指向结构中填写的数据量
		//风险：任何进程不论是在与本客户进程相同的主机上还是在不同的主机上，都
		//     可以向本客户的IP地址和端口发送数据报，这些数据报将被客户读入并被
		//     认为是服务器的应答
		n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

		//将接收到的数据报发回客户端
		Sendto(sockfd, mesg, n, 0, pcliaddr, len);
	}
}
