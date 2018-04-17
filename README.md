> UNIX网络编程 卷1：套接字联网API（第3版）源代码

* 函数
    - [sock_ntop](lib/sock_ntop.c)
    - 套接字读写
        + [readn](lib/readn.c)
        + [writen](lib/writen.c)
        + [readline](test/readline1.c)
        + [readline](lib/readline.c)\(改进版\)
* 程序
    * [测试主机字节序](intro/byteorder.c)

<table>
<tr>
    <td rowspan="5" align="center"> TCP回射服务器 </td>
    <td rowspan="2" align="center"> v1 </td>
    <td align="center"> <a href = "tcpcliserv/tcpcli01.c">客户端</a> </td>
    <td align="center"> <a href = "lib/str_cli.c">str_cli函数</a> </td>
</tr>
<tr>
    <td align="center"> <a href = "tcpcliserv/tcpserv01.c">服务器</a> </td>
    <td align="center">  </td>
</tr>
<tr>
    <td rowspan="3" align="center"> select </td>
    <td align="center"> <a href = "select/tcpcli01.c">客户端</a> </td>
    <td align="center"> <a href = "select/strcliselect01.c">str_cli函数</a> </td>
</tr>
<tr>
    <td align="center"> <a href = "select/tcpcli02.c">客户端</a> </td>
    <td align="center"> <a href = "select/strcliselect02.c">str_cli函数</a> </td>
</tr>
<tr>
    <td align="center"> <a href = "https://github.com/arkingc/unpv13e/blob/master/znote/select.md#%E4%BB%A3%E7%A0%81">服务器</a> </td>
    <td align="center">  </td>
</tr>
</table>

<br>

宏值与头文件表：

|常量|值|所在头文件|
|:--:|:--:|:--:|
|SERV_PORT|9877|unp.h|
|MAXLINE|4096|unp.h|

<br>

Linux环境下开发经常会碰到很多错误(设置errno)，常见错误：

|  | 描述 |
|:--:|:--:|
|EAGAIN|通常发生在非阻塞I/O中，如果数据未准备好，I/O操作会返回这个错误，提示再试一次|
|EINTR|表示系统调用被一个捕获的信号中断，发生该错误可以继续读写套接字|