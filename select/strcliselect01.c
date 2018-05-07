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
 *
 *  问题一：当我们把标准输入和标准输出重定向到文件来运行新的客户程序时，会发现输出文件总是小于输入
 *         文件（对于TCP回射服务器而言，它们理应相等）。问题的原因在于，我们对标准输入中的EOF的处
 *         理：str_cli函数中Fgets读到EOF后返回NULL，就此返回到main函数，而main函数随后终止。然
 *         而在批量方式下，标准输入中的EOF并不意味着我们同时也完成了从套接字的读入，可能仍有请求在
 *         去往服务器的路上，或者仍有应答在返回客户的路上（改用shutdown而不是return）
 *  问题二：为提升性能而引入缓冲机制增加了网络应用程序的复杂性
 *         1）考虑有来自标准输入的多个文本输入行可用的情况。Fgets使用fgets读取输入，这又转而使已
 *            可用的文本输入行被读入到stdio所用的缓冲区中。然而fgets只返回其中第一行，其余输入行
 *            仍在stdio缓冲区中。Writen函数把单个输入行写给服务器，随后select再次被调用以等待新
 *            的工作，而不管stdio缓冲区中还有额外的输入行待消费。究其原因在于select不知道stdio
 *            使用了缓冲区——它只是从read的角度指出是否有数据可读。因此混合使用stdio和select被认
 *            为是非常容易犯错的
 *         2）同样的问题存在于Readline调用中，这回select不可见的数据不是隐藏在stdio缓冲区中，而
 *            是隐藏在Readline自己的缓冲区中。可能的解决办法是修改代码，在调用select之前调用
 *            Readline查看是否存在已读入而尚未消费的数据。然而为了处理Readline缓冲区中既可能有
 *            不完整的输入行（意味着需要继续读入），也可能有一个或多个完整的输入行（这些行我们可
 *            以直接消费）这两种情况而引入的复杂性会迅速增长到难以控制的地步
 *         
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
