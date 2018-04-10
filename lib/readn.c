/***************************************************************
 * 套接字上调用read输入的字节数可能比请求的数量少，原因在于内核中用于
 * 套接字的缓冲区可能已经达到了极限。需要调用者再次调用read，以输入剩
 * 余的字节。readn函数保证返回时所有请求数量的字节都已从套接字缓存区读出
 ***************************************************************/

/* include readn */
#include	"unp.h"

ssize_t						/* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/* end readn */

ssize_t
Readn(int fd, void *ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = readn(fd, ptr, nbytes)) < 0)
		err_sys("readn error");
	return(n);
}
