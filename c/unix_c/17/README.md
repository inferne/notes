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
