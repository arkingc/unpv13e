#include	"unp.h"

#define	MAXFILES	20 		//最多读20个来自web服务器的文件
#define	SERV		"80"	/* port number or service name */

struct file {
  char	*f_name;			/* filename */
  char	*f_host;			/* hostname or IPv4/IPv6 address */
  int    f_fd;				/* descriptor */
  int	 f_flags;			/* F_xxx below */
} file[MAXFILES];			//file结构数组

#define	F_CONNECTING	1	/* connect() in progress */
#define	F_READING		2	/* connect() complete; now reading */
#define	F_DONE			4	/* all done */

#define	GET_CMD		"GET %s HTTP/1.0\r\n\r\n"

			/* globals */
int		nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
fd_set	rset, wset;			//读描述符集和写描述符集为全局变量

			/* function prototypes */
void	home_page(const char *, const char *);
void	start_connect(struct file *);
void	write_get_cmd(struct file *);
