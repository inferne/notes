# 第16章 网络IPC：套接字

## 16.2 套接字描述符
套接字是通信端点的抽象。
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
* 参数ai_flags所用的标志用来指定如何处理地址和名字
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

* 参数flags指定一些转换的控制方式，如下是系统支持的标志
 * NI_DGRAM 服务基于数据报而非基于流
 * NI_NAMEREQD 如果找不到主机名字，将其作为一个错误对待
 * NI_NOFQDN 对于本地主机，近返回完全限定域名的节点名字部分
 * NI_NUMERICHOST 以数字形式而非名字返回主机地址
 * NI_NUMERICSERV 以数字形式而非名字返回服务地址（即端口号）

## 16.3.4将套接字与地址绑定
用bind函数将地址绑定到一个套接字
```c
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
//返回值：若成功则返回0，若出错则返回-1
```
* 对于所能使用的地址有一些限制：
 * 在进程所运行的机器上，指定的地址必须有效，不能指定一个其他机器的地址。
 * 地址必须和创建套接字时的地址族所支持的格式相匹配。
 * 端口号必须小于1024，除非该进程具有相应的特权（即为超级用户）。
 * 一般只有套接字端点能够与地址绑定，尽管有些协议允许多重绑定。

可以调用函数getsockname来发现绑定到一个套接字的地址。
```c
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
//返回值：若成功则返回0，若出错则返回-1
```
* alenp为一个指向整数的指针，该整数指定缓冲区sockaddr的大小。返回时，该整数会被设置成返回地址的大小。如果该地址和提供的缓冲区长度不匹配，则将其截断而不报错。如果当前没有绑定到该套接字的地址，其结果没有定义。

如果套接字已经和对方连接，调用getpeername来找到对方的地址。
```c
#include <sys/socket.h>
int getpeername(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict alenp);
//返回值：若成功则返回0，若出错则返回-1
```
## 16.4 建立连接
如果处理的是面向连接的网络服务（SOCK_STREAM或SOCK_SEQPACKET），在开始交换数据之前，需要在请求服务的进程套接字（客户端）和提供服务的进程套接字（服务端）之间建立一个连接。可以用connect建立一个连接。
```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
```
在connect中所指定的地址是想与之通信的服务器地址。如果sockfd没有绑定到一个地址，connect会给调用者绑定一个默认地址。

如果套接字描述符处于非阻塞模式下，那么在连接不能马上建立时，将会返回-1，errno设为特殊的错误码EINPROGRESS。应用程序可以使用poll或者select来判断文件描述符何时可写。如果可写，连接完成。

connect还可以用于无连接的网络服务（SOCK_DGRAM），所有发送报文的目标地址设为connect调用中指定的地址，这样每次传送报文时就不需要再提供地址。另外，仅能接收来自指定地址的报文。

服务器调用listen来声明可以接受连接请求
```c
#include <sys/socket.h>
int listen(int sockfd, int backlog); //返回值：若成功则返回0，若出错则返回-1
```
* 参数backlog提供了一个提示，用于表示进程所要入队的连接请求数量。其实际值有系统决定，但上限由<sys/socket.h>中SOMAXCONN指定。

一旦队列满，系统会拒绝多于连接请求，所有backlog的值应该基于服务器期望负载和接受连接请求与启动服务的处理能力来选择。

一旦服务器调用listen，套接字就能接收连接请求。使用函数accept获得连接请求并建立连接。
```c
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict len);
//返回值：若成功则返回文件（套接字）描述符，若出错则返回-1
```
如果服务器调用accept并且当前没有连接请求，服务器会阻塞直到一个请求到来。另外服务器可以使用poll或select来等待一个请求的到来。在这种情况下，一个待等待处理的连接请求套接字会以可读的方式出现。

## 16.5 数据传输
既然将套接字端点标示为文件描述符，那么只要建立连接，就可以使用read和write来通过套接字通信。

如果想指定选项、从多个客户端接收数据包或者发送带外数据，需要采用六个传递数据的套接字函数中的一个。
```c
#include <sys/socket.h>
ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
//返回值：若成功则返回发送的字节数，若出错则返回-1
```
* flags
 * MSG_DONTROUTE 勿将数据路由出本地网络
 * MSG_DONTWAIT 允许非阻塞操作（等价于使用O_NONBLOCK）
 * MSG_EOR 如果协议支持，此为记录结束
 * MSG_OOR 如果协议支持，发送带外数据

类似write，send时，套接字必须已经连接。如果send成功返回，并不必然表示连接另一端的进程接收数据。所保证的仅是数据已经无错误地发送到网络上。

函数sendto和send很类似。区别在于sendto允许在无连接的套接字上指定一个目标地址。
```c
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
               const struct sockaddr *destaddr, socklen_t destlen);
               //返回值：若成功则返回发送的字节数，若出错则返回-1
```
对应面向连接的套接字，目标地址是忽略的，因为目标地址蕴涵在连接中。对应无连接的套接字，不能使用send，除非在调用connect时预先设定了目标地址，或者采用sendto来提供另一种发送报文方式。

可以使用不止一个的选择来通过套接字发送数据。可以调用带有msghdr结构的sendmsg来指定多重缓冲区传输数据，这和writev很相像。
```c
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
//返回值：若成功则返回发送的字节数，若出错则返回-1

struct msghdr {
    void         *msg_name;         /* optional address */
    socklen_t     msg_namelen;      /* address size in bytes */
    struct ioves *msg_iov;          /* array of elements in array */
    int           msg_iovlen;       /* number of elements in array */
    void         *msg_control;      /* ancillary data */
    socklen_t     msg_controllen;   /* number of ancillary bytes */
    int           msg_flags;        /* flags for received message */
    ...
}
```
函数recv和read很像，但是允许指定选项来控制如何接收数据。
```c
#include <sys/socket.h>
ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
//返回值：以字节数的消息长度，若无可用消息或对方已经按序结束则返回0，若出错则返回-1
```
* flags
 * MSG_OOB 如果协议支持，接收带外数据
 * MSG_PEFK 返回报文内容而不真正取走报文
 * MSG_TRUNC 即使报文被截断，要求返回的是报文的实际长度
 * MSG_WAITALL 等待直到所有的数据可用（仅SOCK_STREAM）

如果有兴趣定位发送者，可以使用recvfrom来得到数据发送者的源地址。
```c
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *restrict buf, size_t len, int flags,
                 struct sockaddr *restrict addr,
                 socklen_t *restrict addrlen);
//返回值：以字节计数的消息长度，若无可用消息或者对方已经按序结束则返回0 ，若出错则返回-1
```
因为可以获得发送者的地址，recvfrom通常用于无连接套接字。否则，recvfrom等同于recv。

为了将接收到的数据送入多个缓冲区（类似readv），或者想接收辅助数据，可以使用recvmsg。
```c
#include <sys/socket.h>
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
//返回值：以字节计数的消息长度，若无可用消息或对方已经按序结束则返回0，若出错则返回-1
```
* 结构msghdr被recvmsg用于指定接收数据的输入缓冲区。
* 可以设置flags来改变recvmsg的默认行为

返回时，msghdr结构中的msg_flags字段被设置为所接收数据的各种特征（进入recvmsg时msg_flags被忽略）。

从recvmsg中返回的各种可能值
* MSG_CTRUNC 控制数据被截断
* MSG_DONTWAIT recvmsg处于非阻塞模式
* MSG_EOR 接收到记录结束符
* MSG_OOB 接收到带外数据
* MSG_TRUNC 一般数据被截断

## 16.6 套接字选项
套接字机制提供两个套接字来控制套接字行文。一个接口用来设置选项，另一个接口允许查询一个选项的状态。可以获取或设置三种选项：
1. 通用选项，工作在所有套接字类型上。
2. 在套接字层次管理的选项，但是依赖于下层协议的支持。
3. 特定于某协议的选项，为每个协议所独有。

可以采用setsockopt函数来设置套接字选项。
```c
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int option, const void *val, socklen_t len);
//返回值：若成功则返回0，若出错则返回-1
```
* level标识了选项应用的协议。
* option选项
 * SO_ACCEPTCONN int 返回信息指示该套接字是否能监听（仅getsockopt）
 * SO_BROADCAST int 如果*val非零，广播数据包
 * SO_DEBUG int 如果*val非零，启用网络驱动调试功能
 * SO_DONTROUTE int 如果*val非零，绕过通常路由
 * SO_ERROR int 返回挂起的套接字错误并清除（仅getsockopt）
 * SO_KEEPALIVE int 如果*val非零，启用周期性keep-alive消息
 * SO_LINGER struct linger 当有为发消息并且套接字关闭时，延迟时间
 * SO_OOBINLINE int 如果*val非零，将带外数据放在普通数据中
 * SO_RCVBUF int 以字节为单位的接收缓冲区大小
 * SO_RCVLOWAT int 接收调用中返回的以字节为单位的最小数据量
 * SO_RCVTIMEO struct timeval 套接字接收调用的超时值
 * SO_REUSEADDR int 如果*val非零，重用bind中的地址
 * SO_SNDBUF int 以字节为单位的发送缓冲区大小
 * SO_SNDLOWAT int 发送调用中以字节为单位的发送的最小数据量
 * SO_SNDTIMEO struct timeval 套接字发送调用的超时值
 * SO_TYPE int 标识套接字类型（仅getsockopt）
* val根据选项的不同指向一个数据结构或者一个整数。如果整数非零，那么选项被启用。如果整数为零，那么该选项被禁止。
* len指定了val指向的对象的大小。

可以使用getsockopt函数来发现选项的当前值。
```c
#include <sys/socket.h>
int getsockopt(int sockfd, int level, int option, void *restrict val, socklen_t *restrict lenp);
//返回值：若成功则返回0，若出错则返回-1
```
* 注意到参数lenp是一个指向整数的指针。在调用getsockopt之前，设置该整数为复制选项缓冲区的大小。如果实际的尺寸大于此值，选项会被截断而不报错；如果实际尺寸证号等于或者小于此值，那么返回时将此值更新为实际尺寸。

## 16.7 带外数据
带外数据（Out-of-band data）是一些通信协议所支持的可选特征，允许更高优先级的数据比普通数据优先传输。即使传输队列已经有数据，带外数据先行传输。TCP支持带外数据，但UDP不支持。

为帮助判断是否接收到紧急标记，可以使用函数sockatmark
```c
#include <sys/socket.h>
int sockatmark(int sockfd);
//返回值：若在标记处则返回1，如没有在标记处则返回0，若出错则返回-1
```
当带外数据出现在套接字读取队列时，select函数会返回一个文件描述符并且拥有一个异常状态挂起。TCP队列仅有一字节的紧急数据，如果在接收当前的紧急数据字节之前又有新的紧急数据到来，那么当前的字节会被丢弃。

## 16.8 非阻塞和异步I/O
在基于套接字的异步I/O中，当能够从套接字中读取数据，或者套接字写队列中的空间变得可用时，可以安排发送信号SIGIO。通过两个步骤来使用异步I/O:

1. 建立套接字与拥有者关系，信号可以被传送到合适的进程。
 1. 在fcntl使用F_SETOWN命令。
 2. 在ioctl中使用FIOSETOWN命令。
 3. 在ioctl中使用SIOCSPGRP命令。
2. 通知套接字当I/O操作不会阻塞时发信号告知。
 1. 在fcntl中使用F_SETFl命令并且启用文件标志O_ASYNC。
 2. 在ioctl中使用FIOASYNC。
