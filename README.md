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

<table>
<tr>
    <td rowspan="3"> TCP回射服务器 </td>
    <td rowspan="2"> v1 </td>
    <td> <a href = "tcpcliserv/tcpcli01.c">客户端</a> </td>
    <td> <a href = "lib/str_cli.c">str_cli函数</a> </td>
</tr>
<tr>
    <td> <a href = "tcpcliserv/tcpserv01.c">服务器</a> </td>
    <td>  </td>
</tr>
<tr>
    <td rowspan="1"> select </td>
    <td> 客户端 </td>
    <td> <a href = "select/strcliselect01.c">str_cli函数(select版)</a> </td>
</tr>
</table>

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

<br>

Linux环境下开发经常会碰到很多错误(设置errno)，常见错误：

|  | 描述 |
|:--:|:--:|
|EAGAIN|通常发生在非阻塞I/O中，如果数据未准备好，I/O操作会返回这个错误，提示再试一次|