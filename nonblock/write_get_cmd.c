#include	"web.h"

void
write_get_cmd(struct file *fptr)
{
	int		n;
	char	line[MAXLINE];

	//发送一个HTTP GET命令到服务器
	n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);
	Writen(fptr->f_fd, line, n);
	printf("wrote %d bytes for %s\n", n, fptr->f_name);

	//设置F_READING标志，因为不是|=，所以同时会清除F_CONNECTING标志
	//该标志向main函数主循环指出，本描述符已经准备好提供输入
	fptr->f_flags = F_READING;			/* clears F_CONNECTING */

	//在读描述符集中打开与本描述符对应的位，根据需要更新maxfd
	FD_SET(fptr->f_fd, &rset);			/* will read server's reply */
	if (fptr->f_fd > maxfd)
		maxfd = fptr->f_fd;
}
