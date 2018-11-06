/* 
 * 今天看php-fpm源码看到了zend_try zend_catch, 然后学习了一下
 * c语言实现try {} catch {} 
 * 原理：
 * 1.setjmp(j)设置“jump”点，用正确的程序上下文填充jmp_buf对象j。这个上下文包括程序存放位置、栈和框架指针，
 * 其它重要的寄存器和内存数据。当初始化完jump的上下文，setjmp()返回0值。
 * 2.以后调用longjmp(j,r)的效果就是一个非局部的goto或“长跳转”到由j描述的上下文处（也就是到那原来设置j的setjmp()处）。
 * 当作为长跳转的目标而被调用时，setjmp()返回r或1（如果r设为0的话）。（记住，setjmp()不能在这种情况时返回0。）
 * 通常有两类返回值，setjmp()让你知道它正在被怎么使用。当设置j时，setjmp()如你期望地执行；但当作为长跳转的目标时，
 * setjmp()就从外面“唤醒”它的上下文。你可以用longjmp()来终止异常，用setjmp()标记相应的异常处理程序。
 */

#include <stdio.h>
#include <setjmp.h>

//异常标记
typedef struct _execption
{
	jmp_buf stack_info; //保存异常处理入口的堆栈信息
	int type; //异常类型, 0 表示无异常 异常类型号取小于0的数
} execption;

// 获取异常类型号
#define EXECPTION_TYPE(ex) ex.type

// 可能抛出异常的代码块
#define try(ex) if((ex.type = setjmp((ex.stack_info))) == 0) 

// 捕获异常
#define catch(ex, t) else if(ex.type == t) 

// 抛出异常
#define throw(ex, t) longjmp(ex.stack_info, t) 

void test(int type)
{
	execption ex;

	// 异常类型号用负值表示
	type = type > 0 ? -type : type;

	try (ex) {
		if (type == 0) {
			printf("没有异常\n");
		} else {
			throw(ex, type);
		}
	} catch (ex, -1) {
		printf("异常类型:%d\n", ex.type);
	} catch (ex, -2) {
		printf("异常类型:%d\n", ex.type);
	} catch (ex, -3) {
		printf("异常类型:%d\n", ex.type);
	}
}

void main()
{
	test(0);
	test(1);
	test(2);
	test(3);
}

// 输出
// 没有异常
// 异常类型:-1
// 异常类型:-2
// 异常类型:-3


// try {} catch {}

// try {

// } catch (ex, type) {

// } catch (ex, type) {

// } catch(ex, type) {

// }

// 宏展开

// if (/* condition */) {
	
// } else if (/* condition */) {

// } else if (/* condition */) {

// } else if (/* condition */) {

// }

