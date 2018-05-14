/*****************************************************************
 *     子进程                  父进程
 *   ---------              ----------
 *  |         |   字节流    |          |
 *  |   stderr*<---------->*sockfd[0] |
 *  |         |    管道     |          |
 *   ---------              ----------
 *****************************************************************/

/* include child_make */
#include	"unp.h"
#include	"child.h"

pid_t
child_make(int i, int listenfd, int addrlen)
{
	int		sockfd[2];
	pid_t	pid;
	void	child_main(int, int, int);

	Socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

	if ( (pid = Fork()) > 0) {
		Close(sockfd[1]);
		cptr[i].child_pid = pid;
		cptr[i].child_pipefd = sockfd[0];
		cptr[i].child_status = 0;
		return(pid);		/* parent */
	}

	//子进程把流管道的自身拥有端(sockfd[1])复制到标准错误输出
	//这样每个子进程就通过读写标准错误输出和父进程通信
	Dup2(sockfd[1], STDERR_FILENO);		/* child's stream pipe to parent */
	Close(sockfd[0]);
	Close(sockfd[1]);
	//关闭每个子进程中的监听套接字，因为只有父进程才调用accept，
	//父进程必须处理监听套接字以及所有字节流套接字
	Close(listenfd);					/* child does not need this open */
	child_main(i, listenfd, addrlen);	/* never returns */
}
/* end child_make */

/* include child_main */
void
child_main(int i, int listenfd, int addrlen)
{
	char			c;
	int				connfd;
	ssize_t			n;
	void			web_child(int);

	printf("child %ld starting\n", (long) getpid());
	for ( ; ; ) {
		//读取从父进程传递来的已连接描述符，保存在connfd中
		if ( (n = Read_fd(STDERR_FILENO, &c, 1, &connfd)) == 0)
			err_quit("read_fd returned 0");
		if (connfd < 0)
			err_quit("no descriptor from read_fd");

		web_child(connfd);				/* process request */
		Close(connfd);

		//通过与父进程的字节流管道告诉父进程子进程空闲，可以重新分配
		Write(STDERR_FILENO, "", 1);	/* tell parent we're ready again */
	}
}
/* end child_main */
