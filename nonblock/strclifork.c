/********************************************************************
 *                 客户
 *               -------              同一TCP连接
 * 标准输入----->| 父进程 |------------  (全双工)
 *               -------             |   --     --------
 *                  |                 --|--|-->|  服务器 |
 *                  | fork            --|--|---|        |
 *                 \|/               |   --     --------
 *               -------             |        
 * 标准输出----->| 子进程 |<-----------
 *               -------
 ********************************************************************/

#include	"unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	pid_t	pid;
	char	sendline[MAXLINE], recvline[MAXLINE];

	if ( (pid = Fork()) == 0) {		/* child: server -> stdout */
		while (Readline(sockfd, recvline, MAXLINE) > 0)
			Fputs(recvline, stdout);

		//服务器进程可能过早终止。子进程将在套接字上读到EOF。这样的子进程必须
		//告知父进程停止从标准输入到套接字复制数据，因此发送一个SIGTERM信号，
		//以防父进程仍在运行
		kill(getppid(), SIGTERM);	/* in case parent still running */
		exit(0);
	}

		/* parent: stdin -> server */
	while (Fgets(sendline, MAXLINE, fp) != NULL)
		Writen(sockfd, sendline, strlen(sendline));

	Shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */
	pause();//pause进入睡眠，直到捕获一个信号，SIGTERM信号的默认行为是终止进程
	return;
}
