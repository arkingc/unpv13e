> UNIX网络编程 卷1：套接字联网API（第3版）源代码

* 函数
    - [sock_ntop](lib/sock_ntop.c)
    - [readn](lib/readn.c)
    - [writen](lib/writen.c)
    - [readline](test/readline1.c)
    - [readline](lib/readline.c)\(改进版\)
* 程序
    * [测试主机字节序](intro/byteorder.c)
    * [TCP回射程序——服务器](tcpcliserv/tcpserv01.c)
    * [TCP回射程序——客户端](tcpcliserv/tcpcli01.c)

<br>

宏值与头文件表：

|常量|值|所在头文件|
|:--:|:--:|:--:|
|SERV_PORT|9877|unp.h|
|MAXLINE|4096|unp.h|

<br>

POSIX规范要求的数据类型：

|数据类型|说明|头文件|
|:--:|:--:|:--:|
|int8_t|带符号的8位整数|<sys/types.h>|
|uint8_t|无符号的8位整数|<sys/types.h>|
|int16_t|带符号的16位整数|<sys/types.h>|
|uint16_t|无符号的16位整数|<sys/types.h>|
|int32_t|带符号的32位整数|<sys/types.h>|
|uint32_t|无符号的32位整数|<sys/types.h>|
|sa_family_t|套接字地址结构的地址族|<sys/socket.h>|
|socklen_t|套接字地址结构的长度，一般为uint32_t|<sys/socket.h>|
|in_addr_t|IPv4地址，一般为uint32_t|<netinet/in.h>|
|in_port_t|TCP或UDP端口，一般为uint16_t|<netinet/in.h>|