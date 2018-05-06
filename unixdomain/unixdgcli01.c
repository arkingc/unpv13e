#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_un	cliaddr, servaddr;

	sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);

	bzero(&cliaddr, sizeof(cliaddr));		/* bind an address for us */
	cliaddr.sun_family = AF_LOCAL;
	strcpy(cliaddr.sun_path, tmpnam(NULL));

	//与UDP回射客户端不同，UDP回射客户端不需要bind
	//一个未绑定的Unix域数据报套接字发送数据报不会隐式地给这个套接字捆绑一个
	//路径名。因此，如果省略这一步，服务器在dg_echoH函数中的recvfrom调用将
	//返回一个空路径名，这个空路径名将导致服务器在调用sendto时发生错误
	Bind(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

	bzero(&servaddr, sizeof(servaddr));	/* fill in server's address */
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path, UNIXDG_PATH);//UNIXDG_PATH：/tmp/unix.dg

	dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));

	exit(0);
}
