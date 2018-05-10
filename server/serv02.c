/* include serv02 */
#include	"unp.h"

static int		nchildren;
static pid_t	*pids;

int
main(int argc, char **argv)
{
	int			listenfd, i;
	socklen_t	addrlen;
	void		sig_int(int);
	pid_t		child_make(int, int, int);

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv02 [ <host> ] <port#> <#children>");
	nchildren = atoi(argv[argc-1]);//最后一个参数指明子进程的数量
	//pids指向子进程ID的数组
	pids = Calloc(nchildren, sizeof(pid_t));

	//派生nchildren个子进程
	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);	/* parent returns */

	Signal(SIGINT, sig_int);//ctrl+c会发送SIGINT信号，终止进程

	for ( ; ; )
		pause();	/* everything done by children */
}
/* end serv02 */

/* include sigint */
void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

	//终止nchildren个子进程
	for (i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();	//统计CPU使用情况，用于观察进程控制带来的开销
	exit(0);
}
/* end sigint */
