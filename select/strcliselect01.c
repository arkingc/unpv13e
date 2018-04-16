/*                     客户
 *              ------------------
 *             |                  |
 *  数据或EOF-->* 标准输入          |  在标准输入或套接字
 *             |                  |  上select可读条件
 *             |      套接字       |
 *              --------*---------
 *                     /|\
 *                错误 / | \ EOF
 *                    / |  \
 *                   |  |   |
 *             --------------------
 *            |      |  |   |      |
 *         TCP|      |  |   |      |
 *            |      |  |   |      |
 *             --------------------
 *                   |  |   |      
 *                 RST 数据 FIN
 */          
#include	"unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	int			maxfdp1;
	fd_set		rset;
	char		sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);//清除读描述符集的所有位
	for ( ; ; ) {
		//设置读描述符集中的fileno(fd)位
		FD_SET(fileno(fp), &rset);
		//设置读描述符集中的sockfd位
		FD_SET(sockfd, &rset);
		//指定待测试的描述符个数
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		//超时时间设为NULL因此会一直等待下去
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		//如果检测sockfd是可读的，那么先用readline读入回射文本行，再用fputs输出
		if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
			if (Readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
			Fputs(recvline, stdout);
		}

		//如果检测到标准输入是可读的，那么读入一行文本，再把它写到套接字中
		if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
			if (Fgets(sendline, MAXLINE, fp) == NULL)
				return;		/* all done */
			Writen(sockfd, sendline, strlen(sendline));
		}
	}
}
