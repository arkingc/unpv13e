/***************************************************************
 * writen函数保证返回时所有请求数量的字节都被写入了套接字缓存区
 ***************************************************************/

/* include writen */
#include	"unp.h"

//vptr指向存有用于写的数据缓冲区
ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;		//剩余需要写的字节数
	ssize_t		nwritten;	//已经写完成的字节数
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

		nleft -= nwritten;	//更新剩余需要写的字节数
		ptr   += nwritten;	//更新指向写缓冲区的指针
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
