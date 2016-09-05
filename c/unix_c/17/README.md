# 第17章 高级进程间通信

## 17.2 基于STREAMS的管道
基于STREAMS的管道（简称为STREAMS管道，STREAMS pipe）是一个双向（全双工管道）。单个STREAMS管道就能向父、子进程提供双向的数据流。

### 17.2.1 命名的STREAMS管道
通常，管道仅在相关进程之间使用：子进程继承父进程的管道。STREAMS机制提供了一种途径，使得进程可以给予管道一个文件系统中的名字。这就避免了面向FIFO的问题。

我们可以用fattach函数给STREAMS管道一个文件系统中的名字。
```c
#include <stropts.h>
int fattach(int filedes, const char *path);
```
