/***************************************************************
 * 读取一行文本行
 *
 * 相比于前一个版本更高效。因为使用my_read替代read，my_read一次可以读取
 * 最多MAXLINE个字符到readline的内部缓存区中，如果内部缓冲区有数据，
 * my_read直接从内部缓存区取数据，而不需要每取一个字符都调用一次read
 *
 * 使用静态变量实现跨相继函数调用的状态信息维护，其结果是这些函数变得不可
 * 重入或者说非线程安全了
 ***************************************************************/

/* include readline */
#include	"unp.h"

static int	read_cnt;				//readline内部缓存区中的(可用)字节数
static char	*read_ptr;				//指向内部缓冲区中第一个可读字符
static char	read_buf[MAXLINE];		//readline内部缓冲区

//返回的字符保存在ptr指向的字符中
static ssize_t
my_read(int fd, char *ptr)
{
	//只有当readline内部缓冲区为空时，才会读取，否则直接使用缓冲区中的字符数据
	if (read_cnt <= 0) {
again:
		//read每次最多读取MAXLINE个字节，存入readline内部缓冲区
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			//发生EINTR错误表示read调用被一个捕获的信号中断
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	//可用字节数减一（因为需要返回缓冲区第一个字节）
	read_cnt--;
	//将ptr指向的字符设为内部缓冲区中的第一个字符，更新read_ptr指针
	*ptr = *read_ptr++;
	return(1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		//使用my_read替代read，因此更高效
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}


/*
 * readlinebuf这个新函数能够展露内部缓冲区的状态，便于调用者查看在当前文本行
 * 之后是否收到了新的数据
 */
ssize_t
readlinebuf(void **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}
/* end readline */

ssize_t
Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		err_sys("readline error");
	return(n);
}
