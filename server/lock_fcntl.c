/* include my_lock_init */
#include	"unp.h"

static struct flock	lock_it, unlock_it;
static int			lock_fd = -1;
					/* fcntl() will fail if my_lock_init() not called */

void
my_lock_init(char *pathname)
{
    char	lock_file[1024];

		/* 4must copy caller's string, in case it's a constant */
    strncpy(lock_file, pathname, sizeof(lock_file));
    //Mkstemp根据路径名模板（如“/tmp/lock.XXXX”）创建一个唯一的路径名
    //然后创建一个具备该路径名的文件
    lock_fd = Mkstemp(lock_file);

    //unlink上面创建的文件，通过从文件系统目录中删除该路径名，
    //以后即使程序崩溃，这个临时文件也完全消失。
    //然而只要有一个或多个进程打开着这个文件（它的引用计数大于0），该文件
    //本身就不会被删除
    //这也是从目录中删除一个路径名与关闭一个打开着的文件的本质差别
    Unlink(lock_file);			/* but lock_fd remains open */

    /*
     * 初始化两个flock结构：一个用于上锁文件，一个用于解锁文件
     */

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = 0;			//上锁范围起自字节0
	lock_it.l_len = 0;				//跨越整个文件

	unlock_it.l_type = F_UNLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = 0;
	unlock_it.l_len = 0;
}
/* end my_lock_init */

/* include my_lock_wait */
void
my_lock_wait()
{
    int		rc;
    
    while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
		if (errno == EINTR)
			continue;
    	else
			err_sys("fcntl error for my_lock_wait");
	}
}

void
my_lock_release()
{
    if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0)
		err_sys("fcntl error for my_lock_release");
}
/* end my_lock_wait */
