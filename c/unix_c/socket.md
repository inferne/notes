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

## 16.3 地址查询
通过调用gethostent，可以找到给定计算机的主机信息
```c
#include <netdb.h>
struct hostent *gethostent(void); //返回值：若成功则返回指针，若出错则返回NULL
void sethostent(int stayopen);
void endhostent(void);
```

获得网络名字和网络号
```c
#include <netdb.h>
struct netent *getnetbyaddr(uint32_t net, int type);
struct netent *getnetbyname(const char *name);
struct netent *getnetent(void);
//以上三个函数的返回值：若成功则返回指针，若出错则返回NULL
void setnetent(int stayopen);
void endnetent(void);
```
```c
struct netent {
    char    *n_name; /* network name */
    char   **n_aliases; /* alternate network name array pointer */
    int      n_addrtype; /* address type */
    uint32_t n_net; /* network number */
    ...
};
```
网络好按照网络字节序返回。地址类型是一个地址族常量（例如AF_INET）。
可以将协议名字和协议号采用一下函数映射。
```c
#include <netdb.h>
struct protoent *getprotobyname(const char *name);
struct protoent *getprotobynimber(int proto);
struct protoent *getprotoent(void);
//以上所有函数的返回值：若成功则返回指针，若出粗则返回NULL
void setprotoent(int stayopen);
void endprotoent(void);
```
POSIX.1定义的结构protoent至少包含如下成员
```c
struct protoent {
    char  *p_name /* protocol name */
    char **p_aliases; /* pointer to alternate protocol name array */
    int    p_proto; /* protocol number */
    ...
};
```

服务是由地址的端口号部分表示的。每个服务由一个唯一的、熟知的端口号来提供。采用函数getservbyname可以将一个服务名字映射到一个端口号，函数getservbyport将一个端口号映射到一个服务名，或者采用函数getservent顺序扫描服务数据库。
```c
#include <netdb.h>
struct servent *getservbyname(const char *name, const char *proto);
struct servent *getservbyport(int port, const char *proto);
struct servent *getservent(void);
//以上所有函数的返回值：若成功则返回指针，若错粗则返回NULL
void setservent(int stayopen);
void endservent(void);
```
servent至少包含如下成员
```c
struct servent {
    char   *s_name;    /* service name */
    char  **s_aliases; /* pointer to alternate service name array */
    int     s_port;    /* port number */
    char   *s_proto;   /* name of protocol */
    ...
};
```
POSIX.1定义了若干新的函数，允许应用程序将一个主机名字和服务名字映射到一个地址，或者相反。这些函数代替老的函数gethostbyname和gethostbyaddr。
函数getaddrinfo允许将一个主机名字和服务名字映射到一个地址。
```c
#include <sys/socket.h>
#include <netdb.h>
int getaddrinfo(const char *restrict host,const char *restrict service, 
                const struct addrinfo *restrict hint, struct addrinfo **restrict res);
                //返回值：若成功则返回0，若出错则返回非0错误码
void freeaddrinfo(struct addrinfo *ai);
```
需要提供主机名字、服务名字，或者两者都提供。如果只提供一个名字，另一个则必须是一个空指针。主机名字可以是一个节点名或点分十进制表示的主机地址。
函数getaddrinfo返回一个结构addrinfo的链表。可以用freeaddrinfo来释放一个或多个这种结构，这取决于ai_next字段链接起来的结构有多少。
结构addrinfo的定义：
```c
struct addrinfo {
    int             *ai_flags;     /* customize behavior */
    int             *ai_family;    /* address family */
    int             *ai_socktype;  /* socket type */
    int             *ai_protocol;  /* protocol */
    socklen_t       *ai_addrlen;   /* length in bytes of address */
    struct sockaddr *ai_addr;      /* address */
    char            *ai_canonname; /* canonical name of host */
    struct addrinfo *ai_next;      /* next in list */
    ...
};
```
参数ai_flags所用的标志用来指定如何处理地址和名字
* AI_ADDRCONFIG 查询配置的地址类型（IPv4和IPv6）
* AI_ALL 查询IPv4和IPv6地址（仅用于AI_V4MAPPED）
* AI_CANONNAME 需要一个规范名（而不是别名）
* AI_NUMERICHOST 以数字格式返回主机地址
* AI_NUMERICSERV 以端口号返回服务
* AI_PASSIVE 套接字地址用于监听绑定
* AI_V4MAPPED 如果没有找到IPv6地址，则返回映射到IPv6格式的IPv4地址
如果getaddrinfo失败，不能使用perror或strerror来生成错误消息。而是调用gai_strerror将返回的错误码转换成错误消息。
```c
#include <netdb.h>
const char *gai_strerror(int error); //返回值：指向描述符错误的字符串指针
```
函数getnameinfo将地址转换成主机名或者服务名。
```c
#include <sys/socket.h>
#include <netdb.h>
int getnameinfo(const struct sockaddr *restrict addr, 
                socklen_t alen, char *restrict host, 
                socklen_t hostlen, char *restrict service, 
                socklen_t servlen, unsigned int flags); 
                //返回值：若成功则返回0，若出错则返回非0值
```
套接字地址（addr）被转换成主机名或服务名。如果host非空，它指向一个长度为hostlen字节的缓冲区用于存储返回的主机名。同样，如果service非空，它只想一个长度为servlen字节的缓冲区用于存储返回的服务名。

参数flags指定一些转换的控制方式，如下是系统支持的标志
* NI_DGRAM 服务基于数据报而非基于流
* NI_NAMEREQD 如果找不到主机名字，将其作为一个错误对待
* NI_NOFQDN 对于本地主机，近返回完全限定域名的节点名字部分
* NI_NUMERICHOST 以数字形式而非名字返回主机地址
* NI_NUMERICSERV 以数字形式而非名字返回服务地址（即端口号）

