#include	"unp.h"

//前3个参数和connect一样，第4个参数是等待连接完成的秒数
int
connect_nonb(int sockfd, const SA *saptr, socklen_t salen, int nsec)
{
	int				flags, n, error;
	socklen_t		len;
	fd_set			rset, wset;
	struct timeval	tval;

	//将sockfd设置为非阻塞
	flags = Fcntl(sockfd, F_GETFL, 0);
	Fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	//期望的错误是EINPROGRESS，表示连接建立已经启动，但是尚未完成
	//任何其它的错误返回给该函数的调用者
	if ( (n = connect(sockfd, saptr, salen)) < 0)
		if (errno != EINPROGRESS)
			return(-1);

	/* 这里可以编写在等待连接完成期间想执行的代码 */

	//处理第一个细节，即connect立即完成
	if (n == 0)
		goto done;	/* connect completed immediately */

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);	//将sockfd添加到读描述符集
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	//调用select等待连接建立完成
	//
	//注意：调用connect之后、select之前，有可能连接已经建立并有来自对端的数据
	//     到达。这种情况下即使套接字上不发生错误，套接字也是既可读又可写，这和
	//     连接建立失败(即调用connect时连接不能立即建立）情况下套接字的读写条件
	//     一样。这里通过调用getsockopt检查套接字上是否存在待处理错误来处理该情况
	//
	//不能假设套接字的可写(而不可读)条件是select返回套接字操作成功条件的唯一方法
	//（这个条件是Berkeley对于select和非阻塞connect的规则之一，但是其它的实现
	//不一定有这个规则 ），因此一个移植性问题是怎样判断连接建立是否成功：
	//     1）调用getpeername代替getsockopt，如果getpeername以ENOTCONN错误
	//        失败返回，那么连接建立已经失败，必须接着以SO_ERROR调用getsockopt
	//        取得套接字上待处理的错误
	//     2）以值为0的长度参数调用read。如果read失败，那么connect已经失败，read
	//        返回的errno给出了连接失败的原因。如果连接建立成功，那么read应该返回0
	//     3）再调用connect一次。它应该失败，如果错误是EISCONN，那么套接字已经连接，
	//        也就是说第一次连接已经成功
	if ( (n = Select(sockfd+1, &rset, &wset, NULL,
					 nsec ? &tval : NULL)) == 0) {
		close(sockfd);		/* timeout */
		errno = ETIMEDOUT;
		return(-1);
	}

	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		//调用getsockopt取得套接字的待处理错误。如果连接建立成功，该值将为0
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return(-1);			/* Solaris pending error */
	} else
		err_quit("select error: sockfd not set");

done:
	//恢复sockfd的标志
	Fcntl(sockfd, F_SETFL, flags);	/* restore file status flags */

	if (error) {
		close(sockfd);		/* just in case */
		errno = error;
		return(-1);
	}
	return(0);
}
