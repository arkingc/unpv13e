/*                           
 *                       内存地址增大的方向
 *                  <------------------------
 *                       
 *                   地址A+1           地址A
 *               --------------------------------
 *     小端字接续 |    高序字节    |     低序字节    |
 *               --------------------------------
 *                      |                |
 *                     \|/              \|/
 *               --------------------------------
 *              | MSB         16位值         LSB |
 *               --------------------------------
 *                     /|\              /|\
 *                      |                |
 *               --------------------------------
 *     大端字节序 |    高序字节    |     低序字节   |
 *               -------------------------------- 
 *                     地址A           地址A+1 
 *                       
 *                  ------------------------>
 *                       内存地址增大的方向      
 */
#include	"unp.h"

int
main(int argc, char **argv)
{
	union {
	  short  s;
      char   c[sizeof(short)];
    } un;

	un.s = 0x0102;
	printf("%s: ", CPU_VENDOR_OS);
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			printf("big-endian\n");
		else if (un.c[0] == 2 && un.c[1] == 1)
			printf("little-endian\n");
		else
			printf("unknown\n");
	} else
		printf("sizeof(short) = %d\n", sizeof(short));

	exit(0);
}
