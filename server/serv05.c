/*****************************************************************
 *  --------     字节流管道       -----------
 * | 子进程1 *<------            *listenfd   |
 *  --------        |---------->*           |
 *  --------     字节流管道      |           |
 * | 子进程2 *<----------------->*           |
 *  --------                    |   父进程   |
 *     ...       字节流管道      |           |
 *  --------        |---------->*           |
 * | 子进程N *<------            |           |
 *  --------                     -----------
 *****************************************************************/

/* include serv05a */
#include	"unp.h"
#include	"child.h"

static int		nchildren;

int
main(int argc, char **argv)
{
	int			listenfd, i, navail, maxfd, nsel, connfd, rc;
	void		sig_int(int);
	pid_t		child_make(int, int, int);
	ssize_t		n;
	fd_set		rset, masterset;
	socklen_t	addrlen, clilen;
	struct sockaddr	*cliaddr;

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv05 [ <host> ] <port#> <#children>");

	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd;
	cliaddr = Malloc(addrlen);

	//nchildren：预先派生的子进程个数
	nchildren = atoi(argv[argc-1]);
	//navail表示当前空闲可用的子进程个数
	navail = nchildren;
	cptr = Calloc(nchildren, sizeof(Child));

		/* 4prefork all the children */
	for (i = 0; i < nchildren; i++) {
		child_make(i, listenfd, addrlen);	/* parent returns */
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = max(maxfd, cptr[i].child_pipefd);
	}

	Signal(SIGINT, sig_int);

	for ( ; ; ) {
		//masterset保存所有需设置的描述符，每次select之前赋给rset
		rset = masterset;
		//如果当前没有可用子进程(所有子进程都处于忙碌状态)，就从select的读描述符集中关掉与
		//监听套接字对应的位。防止父进程在无可用子进程的情况下accept新连接。内核仍然将这些
		//外来连接排入队列，直到达到listen的backlog数为止，不过在没有得到已准备好处理客户
		//的子进程之前不想accept它们
		if (navail <= 0)
			FD_CLR(listenfd, &rset);	/* turn off if no available children */
		nsel = Select(maxfd + 1, &rset, NULL, NULL, NULL);

			/* 4check for new connections */
		if (FD_ISSET(listenfd, &rset)) {
			clilen = addrlen;
			connfd = Accept(listenfd, cliaddr, &clilen);

			//搜索第一个空闲的子进程
			//总是从Child子结构数组的第一个元素开始搜索可用子进程。意味着该数组中靠前排列
			//的子进程总比靠后排列的子进程更优先接收新的连接
			for (i = 0; i < nchildren; i++)
				if (cptr[i].child_status == 0)
					break;				/* available */

			if (i == nchildren)
				err_quit("no available children");
			
			//修改这个空闲子进程的状态等信息
			cptr[i].child_status = 1;	/* mark child as busy */
			cptr[i].child_count++;
			navail--;//空闲可用的子进程数减1

			//将新的连接描述符通过与空闲子进程的管道流，传输到子进程
			n = Write_fd(cptr[i].child_pipefd, "", 1, connfd);
			Close(connfd);
			if (--nsel == 0)
				continue;	/* all done with select() results */
		}

			/* 4find any newly-available children */
		for (i = 0; i < nchildren; i++) {
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				//如果子进程意外终止,它的字节流管道拥有端将被关闭，因而read将返回0
				if ( (n = Read(cptr[i].child_pipefd, &rc, 1)) == 0)
					err_quit("child %d terminated unexpectedly", i);
				cptr[i].child_status = 0;//修改这个子进程的状态为空闲
				navail++;//空闲可用的子进程数加1
				if (--nsel == 0)
					break;	/* all done with select() results */
			}
		}
	}
}
/* end serv05a */

void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

		/* 4terminate all children */
	for (i = 0; i < nchildren; i++)
		kill(cptr[i].child_pid, SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();

	for (i = 0; i < nchildren; i++)
		printf("child %d, %ld connections\n", i, cptr[i].child_count);

	exit(0);
}
