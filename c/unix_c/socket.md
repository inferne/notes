# 第16章 网络IPC：套接字

## 16.2 套接字描述符
### 要创建一个套接字，可以调用socket函数
```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```
#### 参数
* domain（域）确定通信的特性，包括地址格式，各个域有自己的格式表示地址，而表示各个域的场数都以AF_开头，意指地址族
  * AF_INET-------------------------IPv4因特网域
  * AF_INET6------------------------IPv6因特网域
  * AF_UNIX/AF_LOCAL----------------UNIX域
  * AF_UNSPEC-----------------------未指定
* type确定套接字的类型，进一步确定通信特征，在实现中可以自由增加对其它类型的支持
  * SOCK_DGRAM----------------------长度固定的、无连接的不可靠报文传递
  * SOCK_RAW------------------------IP协议的数据报接口（POSIX.1中为可选）
  * SOCK_SEQPACKET------------------长度固定，有序、可靠的面向连接报文传递
  * SOCK_STREAM---------------------有序、可靠、双向的面向连接字节流
* protocol通常是0，表示给定的域和套接字类型选择默认协议，当对同一域和套接字类型支持多个协议时，可以使用protocol选择一个特定协议。在AF_INET通信域中套接字类型SOCK_STREAM的默认协议是TCP（传输控制协议），套接字类型SOCK_DGRAM的默认协议是UDP（用户数据报协议）。

数据报是一种自包含报文

对于SOCK_STREAM套接字，应用程序意识不到报文界限，因为套接字提供的是字节流服务。这意味着当从套接字读取数据时，他也许不会返回所有由发送进程所写的字节数。最终可以获得发送过来的所有数据，但也许要通过若干次函数调用得到。

SOCK_SEQPACKET套接字和SOCK_STREAM套接字很类似，但从该套接字得到的是基于报文的服务而不是字节流服务。这意味这从SOCK_SEQPACKET套接字接收的数据量与对方所发送的一致。流控制传输协议（Stream Control Transmission Protocol，SCTP）提供了因特网域上的顺序数据包服务。

SOCK_RAW套接字提供一个数据报接口用于直接访问下面的网络层（在因特网域中为IP）。使用这个接口时，应用程序负责构造自己的协议首部，这是因为传输协议（TCP和UDP等）被绕过了。当创建一个原始套接字时需要有超级用户权限，用以防止恶意程序绕过内建安全机制来创建报文。

### 套接字通信是双向的。可以采用函数shutdown来禁止套接字上的输入/输出
```c
#include <sys/socket.h>
int shutdown(int sockfd, int how);
```
#### 参数
* how
 * SHUT_RD关闭读端，那么无法从套接字读取数据
 * SHUT_WR关闭写端，那么无法使用套接字发送数据
 * SHUT_RDWR则将无法读取和发送数据
shutdown允许使一个套接字处于不活动状态

## 16.3 寻址
字节序是一个处理器架构特性，用于指示像整数这样的大数据类型的内部字节顺序。
四个实施在处理器字节序和网络字节序之间的转换函数
```c
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostint32); //返回值：以网络字节序表示的32位整形数
uint16_t htons(uint16_t hostint16); //返回值：以网络字节序表示的16位整形数
uint32_t ntohl(uint32_t netint32);  //返回值：以主机字节序表示的32位整形数
uint16_t ntohs(uint16_t netint16);  //返回值：以主机字节序表示的16位整形数
```
h表示“主机（host）”字节序，n表示“网络（network）”字节序。l表示“长（long）”整数（即4个字节），s表示“短（short）”整数（即2个字节）。

有时，需要打印出能被人所理解的地址格式。
```c
#include <arpa/inet.h>
const char *inet_ntop(int domain, const void *restrict addr, char *restrict str, socklen_t size);
//返回值：若成功则返回地址字符串指针，弱出错则返回NULL

int inet_pton(int domain, const char *restrict str, void *restrict addr);
//返回值：若成功则返回1，若格式无效则返回0，若出错则返回-1
```
* inet_ntop将网络字节序的二进制地址转换成文本字符串格式
* inet_pton将文本字符串格式转换成网络字节序的二进制地址
* 参数domain仅支持两个值：AF_INET和AF_INET6
* 参数size指定了用以保存文本字符串的缓冲区大小。
 * INET_ADDRSTRLEN定义了足够大的空间来存放表示IPv4地址的文本字符串
 * INET6_ADDRSTRLEN定义了足够大的空间来存放表示IPv6地址的文本字符串
