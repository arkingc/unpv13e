#include	"unp.h"

int
my_open(const char *pathname, int mode)
{
	int			fd, sockfd[2], status;
	pid_t		childpid;
	char		c, argsockfd[10], argmode[10];

	//创建一个流管道，返回2个描述符
	Socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

	if ( (childpid = Fork()) == 0) {		/* child process */
		//子进程关闭流管道的一端
		Close(sockfd[0]);
		//流管道另一端的描述符号格式化输出到argsockfd字符数组
		snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
		//打开方式格式化输出到argmode字符数组
		snprintf(argmode, sizeof(argmode), "%d", mode);
		//调用execl执行openfile程序，一旦成功，openfile的main函数就开始执行
		execl("./openfile", "openfile", argsockfd, pathname, argmode,
			  (char *) NULL);
		err_sys("execl error");
	}

	Close(sockfd[1]);			//父进程关闭流管道的另一端

	//等待子进程终止，子进程的终止状态在status变量中返回
	Waitpid(childpid, &status, 0);
	if (WIFEXITED(status) == 0)
		err_quit("child did not terminate");
	//若子进程正常终止，则调用WEXITSTATUS宏把终止状态转换成退出状态
	if ( (status = WEXITSTATUS(status)) == 0)
		//通过流管道接收描述符。除了描述符外，还读取1个字节的数据，但不对数据进程处理
		//通过流管道发送和接收描述符时，总是发送至少1个字节的数据，即便接收进程不对数
		//据做任何处理。否则，接收进程将难以判别read_fd返回0是意味着“没有数据”（但可
		//能伴有一个描述符）还是“文件已结束”
		//第4个参数是指向某个整数的指针，用以返回收取的描述符
		Read_fd(sockfd[0], &c, 1, &fd);
	else {
		errno = status;		/* set errno value from child's status */
		fd = -1;
	}

	Close(sockfd[0]);
	return(fd);
}
