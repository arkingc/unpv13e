/********************************************************************
 *                 客户
 *               ----------              
 * 标准输入----->| copyto线程|------------  
 *               ----------             |          --------
 *                 /|\                   -------->|  服务器 |
 *                  | pthread_create     ---------|        |
 *                  |                   |          --------
 *               ---------              |        
 * 标准输出<-----| main线程 |<------------
 *               ---------
 ********************************************************************/

#include	"unpthread.h"

void	*copyto(void *);

static int	sockfd;		/* global for both threads to access */
static FILE	*fp;

void
str_cli(FILE *fp_arg, int sockfd_arg)
{
	char		recvline[MAXLINE];
	pthread_t	tid;

	sockfd = sockfd_arg;	/* copy arguments to externals */
	fp = fp_arg;

	//新线程ID返回到tid中，新线程执行copyto函数
	Pthread_create(&tid, NULL, copyto, NULL);

	while (Readline(sockfd, recvline, MAXLINE) > 0)
		Fputs(recvline, stdout);
}

void *
copyto(void *arg)
{
	char	sendline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL)
		Writen(sockfd, sendline, strlen(sendline));

	Shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */

	//通过return来终止线程
	return(NULL);
		/* 4return (i.e., thread terminates) when EOF on stdin */
}
