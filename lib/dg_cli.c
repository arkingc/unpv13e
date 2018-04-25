#include	"unp.h"

void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int	n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];

	//循环从标准输入读取数据，发生到服务器，并调用Recvfrom接收回射信息，输出
	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		//对于一个UDP套接字，如果其进程首次调用sndto时，它没有绑定一个本地端口，那么内核就在此时为它选择一个临时端口
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		//最后两个参数为NULL表明不关心服务器的套接字地址结构，因为在这不需使用
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
}
