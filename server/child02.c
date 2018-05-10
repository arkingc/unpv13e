/* include child_make */
#include	"unp.h"

pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;
	void	child_main(int, int, int);

	//派生子进程后父进程返回
	if ( (pid = Fork()) > 0)
		return(pid);		/* parent */

	child_main(i, listenfd, addrlen);	/* never returns */
}
/* end child_make */

/* include child_main */
void
child_main(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());
	//每个子进程调用accept返回一个已连接套接字，然后调用web_child处理客户请求
	//最后关闭连接，子进程一直在这个循环中反复，直到被父进程终止
	for ( ; ; ) {
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}
/* end child_main */
