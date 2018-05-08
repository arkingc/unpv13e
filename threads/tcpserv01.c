#include	"unpthread.h"

static void	*doit(void *);		/* each thread executes this function */

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	pthread_t		tid;
	socklen_t		addrlen, len;
	struct sockaddr	*cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: tcpserv01 [ <host> ] <service or port>");

	cliaddr = Malloc(addrlen);

	for ( ; ; ) {
		len = addrlen;
		connfd = Accept(listenfd, cliaddr, &len);
		//这里把connfd类型强制转换成void指针，而不是简单地把connfd的地址传递
		//给新线程，如果传递connfd的地址，从语言层面看是可以接受的：ANSI C保证
		//能够把一个整数指针类型强制转换为void *，然后把这个void *指针类型强制
		//转换回原来的整数指针。但是传connfd的地址可能会有问题：
		//  1）accept返回，主线程把返回值存入connfd后调用pthread_create，最后
		//     一个参数是指向connfd的指针而不是connfd的内容
		//  2）Pthread函数库创建一个线程，并准备调度doit函数启动执行
		//  3）另一个连接就绪且主线程在新创建的线程开始运行之前再次运行。accept
		//     返回，主线程把返回值存入connfd后调用pthread_create
		//尽管主线程共创建了两个线程，但是它们操作的都是存放在connfd中的最终值
		//问题出在多个线程不是同步地访问一个共享变量。通过把connfd的值传递给
		//pthread_create来解决这个问题
		Pthread_create(&tid, NULL, &doit, (void *) connfd);
		//主线程不关闭已连接套接字，而在调用fork的并发服务器中却需要
		//因为同一进程内的所有线程共享全部描述符，如果主线程调用close，
		//就会终止相应的连接。创建新线程并不影响已打开描述符的引用计数，
		//这一点不同于fork
	}
}

static void *
doit(void *arg)
{
	//线程首先让自己脱离，因为主线程没有理由等待它创建的每个线程
	Pthread_detach(pthread_self());
	str_echo((int) arg);	/* same function as before */
	//str_echo返回后必须close已连接套接字，因为本线程和主线程共享
	//所有的描述符，对于fork，子进程不必close已连接套接字，因为子进程结束时
	//所有打开的描述符都会被关闭
	Close((int) arg);		/* done with connected socket */
	return(NULL);
}
