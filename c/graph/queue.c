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

void enqueue(queue *Q, void *x)
{
    Q->A[Q->tail] = x;
    //printf("tail %4d ", Q->tail);
    if(Q->tail == Q->length){
        Q->tail = 1;//到队列尾时重置tail为1
    }else{
        Q->tail += 1;
    }
    Q->cnt++;
    //printf("en %4d next tail %4d\n", x, Q->tail);
}

void * dequeue(queue *Q)
{
    void *x = Q->A[Q->head];
    //printf("head %4d ", Q->head);
    if(Q->head == Q->length){
        Q->head = 1;//到队列尾时重置head为1
    }else{
        Q->head += 1;
    }
    Q->cnt--;
    //printf("de %4d next head %4d\n", x, Q->head);
    return x;
}

void init_queue(queue *Q, int l)
{
    int i;
    Q->head = 1;
    Q->tail = 1;
    for(i = 0; i <= l; i++){
        Q->A[i] = NULL;
    }
    Q->length = l;
    Q->cnt = 0;
}