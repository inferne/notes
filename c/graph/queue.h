#include <stdio.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

typedef struct _queue
{
    int   head;//头
    int   tail;//尾
    void *A[17];//指针数组
    int   length;
    int   cnt;
} queue;

void enqueue(queue *Q, void *x);

void * dequeue(queue *Q);

void init_queue(queue *Q, int l);