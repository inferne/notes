# 第18章 终端I/O

## 18.1 引言
所有操作系统的终端I/O处理都是非常繁琐的，UNIX也不例外。在大多数版本的UNIX手册中，终端I/O手册常常是最长的部分。

## 18.2 综述
终端I/O有两种不同的工作模式：
1. 规范模式输入处理（Canonical mode input prosessing）。在这种模式中，终端输入以行为单位进行处理。对于每个读要求，终端驱动程序最多返回一行。
2. 非规范模式处理（Noncanonical mode input processing）。输入字符并不组成行。
