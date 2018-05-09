/* include web1 */
#include	"web.h"

int
main(int argc, char **argv)
{
	int		i, fd, n, maxnconn, flags, error;
	char	buf[MAXLINE];
	fd_set	rs, ws;

	if (argc < 5)
		err_quit("usage: web <#conns> <hostname> <homepage> <file1> ...");
	maxnconn = atoi(argv[1]);	//由命令行参数给出最大并发连接数

	//根据命令行指定的文件数量和最大文件数量的较小者决定nfiles
	nfiles = min(argc - 4, MAXFILES);
	//填写file结构数组
	for (i = 0; i < nfiles; i++) {
		file[i].f_name = argv[i + 4];
		file[i].f_host = argv[2];
		file[i].f_flags = 0;
	}
	printf("nfiles = %d\n", nfiles);

	//创建一个TCP连接，发出一个命令到服务器，然后读取主页
	//这是第一个连接，需要在开始并行建立多个连接前独自完成
	home_page(argv[2], argv[3]);

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	maxfd = -1;
	//nlefttoread：仍待读取的文件数，到达0时程序任务完成
	//nlefttoconn：尚无TCP连接的文件数
	nlefttoread = nlefttoconn = nfiles;
	//nconn：当前打开着的连接数（不能超过第一个命令行参数）
	nconn = 0;
/* end web1 */
/* include web2 */
	while (nlefttoread > 0) {
		//如果没有到达最大并行连接数而且另有连接需要建立，就找到一个尚未处理的文件
		//(由值为0的f_flags指示)，然后调用start_connect发起另一个连接。活跃连接
		//(nconn)增1，仍待建立连接数(nlefttoconn)减1
		while (nconn < maxnconn && nlefttoconn > 0) {
				/* 4find a file to read */
			for (i = 0 ; i < nfiles; i++)
				//找到了一个
				if (file[i].f_flags == 0)
					break;
			if (i == nfiles)
				err_quit("nlefttoconn = %d but nothing found", nlefttoconn);
			start_connect(&file[i]);
			nconn++;
			nlefttoconn--;
		}

		//在所有活跃的描述符上使用select
		//（如果有一些连接在connect后，select之前完成3路握手会怎么样？）
		// 1）有一个非阻塞connect正在进展的描述符会同时开启2个描述符集
		// 2）连接建立完毕并正在等待来自服务器的数据的描述符只会开启读描述符集
		rs = rset;
		ws = wset;
		n = Select(maxfd+1, &rs, &ws, NULL, NULL);

		//遍历file结构数组中的每个元素，确定哪些描述符需要处理
		for (i = 0; i < nfiles; i++) {
			flags = file[i].f_flags;
			//跳过已完成和未连接的文件
			if (flags == 0 || flags & F_DONE)
				continue;
			fd = file[i].f_fd;
			//对于设置了F_CONNECT标志的一个描述符，如果它在读描述符集或写描述符集中对应的
			//位已打开，那么非阻塞connect已经完成
			if (flags & F_CONNECTING &&
				(FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws))) {
				n = sizeof(error);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &n) < 0 ||
					error != 0) {
					err_ret("nonblocking connect failed for %s",
							file[i].f_name);
				}
					/* 4connection established */
				printf("connection established for %s\n", file[i].f_name);
				//连接建立、发送GET命令到服务器后只需读取，所以清除写描述符集中的位
				FD_CLR(fd, &wset);		/* no more writeability test */
				//发送GET请求获取文件
				write_get_cmd(&file[i]);/* write() the GET command */
			} 
			//对于设置了F_READING标志的一个描述符，如果它在读描述符集中对应的位已打开，
			//就调用read。如果相应连接被对端关辉，就关闭该套接字并设置F_DONE标志，
			//然后关闭该描述符在读描述符集中对应的位
			else if (flags & F_READING && FD_ISSET(fd, &rs)) {
				if ( (n = Read(fd, buf, sizeof(buf))) == 0) {
					printf("end-of-file on %s\n", file[i].f_name);
					Close(fd);
					file[i].f_flags = F_DONE;	/* clears F_READING */
					FD_CLR(fd, &rset);
					nconn--;
					nlefttoread--;
				} else {
					printf("read %d bytes from %s\n", n, file[i].f_name);
				}
			}
		}
	}
	exit(0);
}
/* end web2 */
