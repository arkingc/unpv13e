/* include my_lock_init */
#include	"unpthread.h"
#include	<sys/mman.h>

static pthread_mutex_t	*mptr;	/* actual mutex will be in shared memory */

void
my_lock_init(char *pathname)
{
	int		fd;
	pthread_mutexattr_t	mattr;

	fd = Open("/dev/zero", O_RDWR, 0);

	//映射的字节数是一个pthread_mutex_t类型变量的大小
	mptr = Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	//关闭描述符是可行的，因为该描述符已被内存映射
	Close(fd);

	Pthread_mutexattr_init(&mattr);
	Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	//全局或静态互斥锁变量可以使用常值PTHREAD_MUTEX_INITIALIZER初始化
	//，但是对于一个存放在共享内存区中的互斥锁，必须调用一个Pthread库函数
	//告知该函数库：这是一个位于共享内存区中的互斥锁，将用于不同进程之间的上锁
	Pthread_mutex_init(mptr, &mattr);
}
/* end my_lock_init */

/* include my_lock_wait */
void
my_lock_wait()
{
	Pthread_mutex_lock(mptr);
}

void
my_lock_release()
{
	Pthread_mutex_unlock(mptr);
}
/* end my_lock_wait */
