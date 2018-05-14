/* include serv08 */
#include	"unpthread.h"
#include	"pthread08.h"

static int			nthreads;
pthread_mutex_t		clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t		clifd_cond = PTHREAD_COND_INITIALIZER;

int
main(int argc, char **argv)
{
	int			i, listenfd, connfd;
	void		sig_int(int), thread_make(int);
	socklen_t	addrlen, clilen;
	struct sockaddr	*cliaddr;

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv08 [ <host> ] <port#> <#threads>");
	cliaddr = Malloc(addrlen);

	nthreads = atoi(argv[argc-1]);
	tptr = Calloc(nthreads, sizeof(Thread));
	//iget：第一个已连接但未分配线程进行处理的客户端描述符在数组clifd中的下标
	//iput：第一个可以使用的下标
	iget = iput = 0;

		/* 4create all the threads */
	for (i = 0; i < nthreads; i++)
		thread_make(i);		/* only main thread returns */

	Signal(SIGINT, sig_int);

	for ( ; ; ) {
		clilen = addrlen;
		//主线程大部分时间阻塞在accept调用中，等待各个客户连接的到达。一旦某个客户
		//连接到达，主线程就把它的已连接套接字描述符存入clifd数组的下一个元素，但需要
		//先获取保护该数组的互斥锁
		//主线程还必须检查iput下标有没有赶上iget下标，若赶上则说明数组不够大
		//然后发送信号到条件变量信号，释放互斥锁，以允许线程池中某个线程为这个客户服务
		connfd = Accept(listenfd, cliaddr, &clilen);

		Pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;
		//假设线程数量nthreads小于MAXNCLI，没一个新连接到达时，iput先前移，然后分配一个
		//线程进程处理，iget随后紧跟iput进行前移，并且假设每个连接会保持很长时间。当所有线程
		//分配完之后，新连接到达时，iput会前移，但是没有可用的空闲线程处理新的连接，所以iget
		//保持不变，当新连接一个个到达时，会旋转一圈重新回到iget处，此时iput和iget相等，此时
		//如果不对这种情况进行错误处理，前面占用iget位置的连接描述符将被覆盖，永远得不到处理
		if (++iput == MAXNCLI)
			iput = 0;
		if (iput == iget)
			err_quit("iput = iget = %d", iput);
		Pthread_cond_signal(&clifd_cond);
		Pthread_mutex_unlock(&clifd_mutex);
	}
}
/* end serv08 */

void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

	exit(0);
}
