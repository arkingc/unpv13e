
/***************************************************************
 * 读取一行文本行
 *
 * 这个版本每读一个字节的数据就调用一次系统的read函数，非常低效
 ***************************************************************/

/* include readline */
#include	"unp.h"

/* PAINFULLY SLOW（极端的慢） VERSION -- example only */
//vptr指向存放数据的缓冲区
//maxlen：存放读取到的字节的缓存区最大长度
ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;		//rc：每次调用read读取的字符数
	char	c, *ptr;	//c：存放每次读取的字符

	ptr = vptr;
	//最多读取(maxlen-1)个字符（最后一个位置放字符串结束符）
	for (n = 1; n < maxlen; n++) {
again:
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			//读到换行符时停止
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {//已经没有数据可读（EOF）
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else {
			if (errno == EINTR)
				goto again;
			return(-1);		/* error, errno set by read() */
		}
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);	
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
