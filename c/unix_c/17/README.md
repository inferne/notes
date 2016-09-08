# 第17章 高级进程间通信

## 17.2 基于STREAMS的管道
基于STREAMS的管道（简称为STREAMS管道，STREAMS pipe）是一个双向（全双工管道）。单个STREAMS管道就能向父、子进程提供双向的数据流。

### 17.2.1 命名的STREAMS管道
通常，管道仅在相关进程之间使用：子进程继承父进程的管道。STREAMS机制提供了一种途径，使得进程可以给予管道一个文件系统中的名字。这就避免了面向FIFO的问题。

我们可以用fattach函数给STREAMS管道一个文件系统中的名字。
```c
#include <stropts.h>
int fattach(int filedes, const char *path);
//返回值：若成功则返回0，若出错则返回-1
```
path参数必须引用一个现存的文件，调用进程应该拥有该文件并且对它具有写权限，或者正在以超级用户特权运行。

一旦STREAMS管道连接到文件系统名字空间，那么原来使用该名字的底层文件就不再是可访问的。打开该名字的任一进程将能访问相应管道，而不是访问原先的文件。在调用fattach之前打开底层文件的任一进程可以继续访问该文件。

一个进程可以调用fdetach函数撤销STREAMS管道文件与文件系统中名字的关联关系。
```c
#include <stropts.h>
int fdetach(const char *path);
```
在调用fdetach函数之后，先前依靠打开path而能访问STREAMS管道的进程可继续访问该管道，但是在此之后打开path的进程将访问驻留在文件系统的底层文件。

### 17.2.2 唯一连接
如果多个进程都想要用命名STREAMS管道与服务器进程通信，那么仍然存在问题。若几个客户进程同时将数据写至一管道，那么这些数据就会混合交错。即使我们保证客户进程写的字节数小于PIPE_BUF，使得写操作是原子性的，但是仍无法保证服务器进程将数据送回所期望的某个客户进程，也无法保证该客户进程一定会读此消息。当多个客户进程同时读一管道时，我们无法调度具体哪一个客户进程去读我们所发送的消息。

connld STREAMS模块解决了这一问题。客户进程打开pipe绝不会接收到它所打开的文件描述符。作为替代，操作系统创建了一个新管道，对客户进程返回其一端，作为它打开pipe的结果。系统将次新管道另一端的文件描述符经由已存在的连接管道发送给服务器进程。这样在客户进程和服务器进程之间构成了唯一连接。

我们将开发三个函数，使用这些函数可以创建在无关进程之间的唯一连接。
```c
#include "apue.h"
int serv_listen(const char *name);
//返回值：若成功则返回要侦听的文件描述符，若出错则返回负值
int serv_accept(int listenfd, uid_t *uidptr);
//返回值：若成功则返回新文件描述符，若出错则返回负值
int cli_conn(const char *name);
//返回值：若成功则返回文件描述，若出错则返回负值
```

## 17.3 UNIX域套接字
UNIX域套接字用于在同一台机器上运行的进程间的通信。虽然因特网域套接字可以用于同一目的，但UNIX域套接字的效率更高。UNIX域套接字仅仅复制数据，它们并不执行协议处理，不需要添加或删除网络报头，无需计算检验和，不要产生顺序号，无需发送确认报文。

UNIX域套接字提供流和数据报两种接口。UNIX域数据报服务是可靠的，既不会丢失消息，也不会传递出错。UNIX域套接字是套接字和管道之间的混合物。为了创建一对非命名的、相互连接的UNIX域套接字，用户可以使用它们面向网络的与套接字接口，也可以使用socketpair函数。
```c
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int sockfd[2]);
//返回值：若成功则返回0，若出错则返回-1
```

### 17.3.1 命名UNIX域套接字
UNIX域套接字的地址由sockaddr_un结构表示。套接字的地址格式可能随实现而变。
```c
#include <sys/un.h>
struct sockaddr_un {
    sa_family_t sun_family; /* AF_UNIX */
    char        sun_path[108]; /* pathname */
}
```

### 17.3.2 唯一连接
服务器进程可以使用标准的bind、listen和accept函数，为客户进程安排一个唯一UNIX域连接（unique UNIX domain connection）。

## 17.4 传送文件描述符
使用三个函数以发送和接收文件描述符。
```c
#include "apue.h"
int send_fd(int fd, int status, const char *errmsg);
int send_err(int fd, int status, const char *errmsg);
//以上两个函数返回值：若成功则返回0，若出错则返回-1
int recv_fd(int fd, ssize_t (*userfunc)(int, const void *, size_t));
//返回值：若成功则返回文件描述符，若出错则返回负值
```
### 17.4.1 经由基于STREAMS的管道传送文件描述符
文件描述符用两个ioctl命令经由STREAMS管道交换，这两个命令是：I_SENDFD和I_RECVFD。为了发送一个描述符，将ioctl的第三个参数设置为实际描述符。

### 17.4.2 经由UNIX域套接字传送文件描述符
为了用UNIX域套接字交换文件描述符，调用sendmsg(2)和recvmsg(2)函数（16.5节）。这两个函数的参数都有一个指向msghdr结构的指针，该结构包含了所有有关收发内容的信息。该结构的定义大致如下：
```c
struct msghdr {
    void          *msg_name;       /* optional address */
    socklen_t      msg_namelen;    /* address size in bytes */
    struct iovec  *msg_iov;        /* array of I/O buffers */
    int            msg_iovlen;     /* number of elements in array */
    void          *msg_control;    /* ancillary data */
    socklen_t      msg_controllen; /* number of ancillary bytes */
    int            msg_flags;      /* flags for received message */
}
```
msg_control字段指向cmsghdr（控制信息首部）结构，msg_controllen字段包含控制信息的字节数。
```c
struct cmsghdr {
    socklen_t  cmsg_len;    /* data byte count, including header */
    int        cmsg_level;  /* originating protocol */
    int        cmsg_type;   /* protocol-specific type */
    /* followed by the actual control message data */
}
```
为了发送文件描述符，将cmsg_len设置为cmsghdr结构的长度加一个整形（描述符）的长度，cmsg_level字段设置为SOL_SOCKET，cmsg_type字段设置为SCM_RIGHTS，用以指明我们在传送访问权。（SCM指的是套接字级控制消息，socket_level control message。）访问全仅能通过UNIX域套接字传送。描述符紧随cmsg_type字段之后存放，用CMSG_DATA宏获得该整形量的指针。

三个宏用于访问控制数据，一个宏用于帮助计算cmsg_len所使用的值。
```c
#include <sys/socket.h>
unsigned char * CMSG_DATA(struct cmsghdr *cp);
//返回值：指向与cmsghdr结构相关联的数据的指针
struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *mp);
//返回值：指向域msghdr结构相关联的第一个cmsghdr结构的指针，若无这样的结构则返回NULL
struct cmsghdr *CMSG_NXTHDR(struct msghdr *mp, struct cmsghdr *cp);
//返回值：指向与msghdr结构相关联的下一个cmsghdr结构的指针，该msghdr结构给出了当前cmsghdr结构，若当前cmsghdr结构已是最后一个则返回NULL
unsigned int CMSG_LEN(unsigned int nbytes);
//返回值：为nbytes大小的数据对象分配的长度
```
在传送文件描述符方面，UNIX域套接字和STREAMS管道之间的一个区别是，用STREAMS管道时我们得到发送进程的身份。某些UNIX域套接字版本提供类似的功能，但他们的接口不同。

## 17.5 open服务器版本1
使用文件描述符传送技术，我们开发了一个open服务器；一个由一个进程执行以打开一个或几个文件的程序。该服务器不是将文件内容送回调用进程，而是送回一个打开文件描述符。这使该服务器对任何类型的文件（例如一个设备或套接字）都能起作用。这意味着，用IPC交换了最小量的信息--从客户端进程到服务器进程传送文件名和打开模式，而从服务器进程到客户进程返回描述符。文件内容不需用IPC传送。
