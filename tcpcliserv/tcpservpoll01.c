/*
 *  select版必须分配一个client数组以及一个名为rset的描述符集；
 *  该用poll后，只需分配一个pollfd结构的数组来维护客户信息，而不必分配另外一个数组
 */

/* include fig01 */
#include	"unp.h"
#include	<limits.h>		/* for OPEN_MAX */

int
main(int argc, char **argv)
{
	/*
	 * maxi：client数组当前正在使用的最大下标值
	 */
	int					i, maxi, listenfd, connfd, sockfd;
	int					nready;					//poll返回时，准备好的描述符数
	ssize_t				n;
	char				buf[MAXLINE];			//接收来自客户端数据的缓冲区
	socklen_t			clilen;
	struct pollfd		client[OPEN_MAX];		//最多OPEN_MAX个描述符（客户）
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	/* client数组的第一项用于监听套接字，其余各项的描述符成员置为-1 */
	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;				//设置事件，当有新连接准备好被接受时poll会通知
	for (i = 1; i < OPEN_MAX; i++)
		client[i].fd = -1;		/* -1 indicates available entry */
	maxi = 0;					/* max index into client[] array */
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		nready = Poll(client, maxi+1, INFTIM);

		//如果有新的连接到达
		if (client[0].revents & POLLRDNORM) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
#ifdef	NOTDEF
			printf("new client: %s\n", Sock_ntop((SA *) &cliaddr, clilen));
#endif

			//找到第一个描述符成员为负的可用项
			for (i = 1; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
					client[i].fd = connfd;	/* save descriptor */
					break;
				}
			if (i == OPEN_MAX)
				err_quit("too many clients");

			//设置新连接描述符上的事件
			client[i].events = POLLRDNORM;
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 1; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i].fd) < 0)
				continue;
			//检查两个事件：
			//    POLLRDNORM
			//    POLLERR：原因在于，有些实现在一个连接上接收到RST时返回的是POLLERR事件
			//             而其它实现返回的只是POLLRDNORM事件
			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ( (n = read(sockfd, buf, MAXLINE)) < 0) {
					//不管哪种实现，接收到RST时，read都返回错误
					if (errno == ECONNRESET) {
							/*4connection reset by client */
#ifdef	NOTDEF
						printf("client[%d] aborted connection\n", i);
#endif
						Close(sockfd);
						client[i].fd = -1;
					} else
						err_sys("read error");
				} else if (n == 0) {
						/*4connection closed by client */
#ifdef	NOTDEF
					printf("client[%d] closed connection\n", i);
#endif
					Close(sockfd);
					client[i].fd = -1;
				} else
					Writen(sockfd, buf, n);

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}
/* end fig02 */
