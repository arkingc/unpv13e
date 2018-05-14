typedef struct {
  pid_t		child_pid;		/* process ID */
  int		child_pipefd;	/* 父进程中连接到该子进程的字节流管道描述符(父进程端) */
  int		child_status;	/* 子进程状态，0表示空闲，1表示忙碌 */
  long		child_count;	/* # connections handled */
} Child;

Child	*cptr;		/* array of Child structures; calloc'ed */
