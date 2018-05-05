/* include connect_timeo */
#include	"unp.h"

static void	connect_alarm(int);

int
connect_timeo(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
	Sigfunc	*sigfunc;
	int		n;

	sigfunc = Signal(SIGALRM, connect_alarm);
	//把本进程的报警时钟设置成由调用者指定的秒数
	//如果此前已经给本进程设置过报警时钟，那么alarm的返回值是这个报警时钟的当前剩余秒数
	//否则，alarm的返回值为0
	if (alarm(nsec) != 0)
		err_msg("connect_timeo: alarm was already set");

	//当被捕获的信号为SIGALRM时，signal函数不设置SA_RESTART标准，因此connect不会重新被调用
	if ( (n = connect(sockfd, saptr, salen)) < 0) {
		close(sockfd);
		//如果connect被中断，即返回EINTR错误，就把errno值改设为ETIMEDOUT
		if (errno == EINTR)
			errno = ETIMEDOUT;
	}
	//关闭进程的报警时钟
	alarm(0);					/* turn off the alarm */
	//恢复原来的信号处理函数
	Signal(SIGALRM, sigfunc);	/* restore previous signal handler */

	return(n);
}

static void
connect_alarm(int signo)
{
	return;		/* just interrupt the connect() */
}
/* end connect_timeo */

void
Connect_timeo(int fd, const SA *sa, socklen_t salen, int sec)
{
	if (connect_timeo(fd, sa, salen, sec) < 0)
		err_sys("connect_timeo error");
}
