#include	"unp.h"

/***************************************************
 * 该函数的问题在于： 直到客户键入EOF，才会回射所有文本行：
 *
 * hello,world
 * and hi
 * hello??
 * ^D
 * hello,world
 * and hi
 * hello??
 ***************************************************/

void
str_echo(int sockfd)
{
	char		line[MAXLINE];
	FILE		*fpin, *fpout;

	fpin = Fdopen(sockfd, "r");
	fpout = Fdopen(sockfd, "w");

	while (Fgets(line, MAXLINE, fpin) != NULL)
		/**************************************************
		 * 问题在于缓冲：
		 *   1）标准I/O流被标准I/O函数库完全缓冲，意味着该函数库把回射行
		 *      复制到输出流的标准I/O缓冲区，但是不把该缓冲区中的内容写到
		 *      描述符，因为该缓冲区未满
		 *   2）当客户键入EOF时，致使str_cli函数调用shutdown，从而发送
		 *      一个FIN到服务器
		 *   3）服务器接收到FIN，被fgets读入，致使fgets返回一个空指针。
		 *      str_echo函数返回到服务器的main函数，子进程通过调用exit终止
		 *   4）C库函数exit调用标准I/O清理函数。之前由fputs调用填入缓冲区中
		 *      的未满内容现被输出
		 *   ...
		 * 
		 * 如何解决？
		 *   1）通过调用setvbuf迫使输出流变为“行缓冲”
		 *   2）在每次调用fputs之后调用fflush强制输出每个回射行
		 * 
		 * 然而在现实使用中，这两个办法都易于犯错，与Nagle算法的交互也成问题。
		 * 大多数情况下，最好的解决办法是彻底避免在套接字上使用标准I/O库函数
		 * 当标准I/O流的便利性大过对缓冲带来的bug的担忧时，在套接字上使用标准
		 * I/O流也可能可行，但这种情况很罕见
		 * *************************************************/
		Fputs(line, fpout);
}
