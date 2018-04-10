/***************************************************************
 * 套接字上调用write输出的字节数可能比请求的数量少，原因在于内核中用于
 * 套接字的缓冲区可能已经达到了极限。需要调用者再次调用write，以输出剩
 * 余的字节。writen函数保证返回时所有请求数量的字节都被写入了套接字缓存区
 ***************************************************************/

/* include writen */
#include	"unp.h"

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		err_sys("writen error");
}
