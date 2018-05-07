#include	"web.h"

void
start_connect(struct file *fptr)
{
	int				fd, flags, n;
	struct addrinfo	*ai;

	//调用host_serv(封装了getaddrinfo)函数查找并转换主机名和服务名
	//返回指向某个addrinfo结构数组的一个指针
	ai = Host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);

	//只使用第一个结构，创建一个TCP套接字
	fd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	fptr->f_fd = fd;
	printf("start_connect for %s, fd %d\n", fptr->f_name, fd);

	//将套接字设置为非阻塞
	flags = Fcntl(fd, F_GETFL, 0);
	Fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	//发起非阻塞connect，并把相应文件的标志设置为F_CONNECTING
	if ( (n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0) {
		if (errno != EINPROGRESS)
			err_sys("nonblocking connect error");
		fptr->f_flags = F_CONNECTING;
		//将套接字添加到全局的读写描述符集中
		FD_SET(fd, &rset);			/* select for reading and writing */
		FD_SET(fd, &wset);
		if (fd > maxfd)
			maxfd = fd;

	} 
	//如果连接已经建立，调用write_get_cmd发送一个命令到服务器
	else if (n >= 0)				/* connect is already done */
		write_get_cmd(fptr);	/* write() the GET command */
}
