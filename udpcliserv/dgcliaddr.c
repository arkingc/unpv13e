#include	"unp.h"

void
dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int				n;
	char			sendline[MAXLINE], recvline[MAXLINE + 1];
	socklen_t		len;
	struct sockaddr	*preply_addr;	//保存向客户端发送数据报的套接字地址结构

	preply_addr = Malloc(servlen);

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		len = servlen;
		//和前一版本相比，最后两个参数已经不是NULL了
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len);
		//验证接收到的数据报来源是否和发送的目的地址一致（即是否是来自服务器的数据报）
		if (len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
			//如果不是来自服务器的数据报，则打印地址信息然后继续循环
			printf("reply from %s (ignored)\n",
					Sock_ntop(preply_addr, len));
			continue;
		}

		recvline[n] = 0;	/* null terminate */
		Fputs(recvline, stdout);
	}
}
