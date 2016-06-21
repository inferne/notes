#include <stdio.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

typedef struct _queue
{
    int head;//头
    int tail;//尾
    int A[16];//数组
    int length;
} queue;

void enqueue(queue *Q, int x)
{
    Q->A[Q->tail] = x;
    printf("tail %4d ", Q->tail);
    if(Q->tail == Q->length){
        Q->tail = 1;//到队列尾时重置tail为1
    }else{
        Q->tail += 1;
    }
    printf("en %4d next tail %4d\n", x, Q->tail);
}

int dequeue(queue *Q)
{
    int x = Q->A[Q->head];
    printf("head %4d ", Q->head);
    if(Q->head == Q->length){
        Q->head = 1;//到队列尾时重置head为1
    }else{
        Q->head += 1;
    }
    printf("de %4d next head %4d\n", x, Q->head);
    return x;
}

int main()
{
    queue *Q;
    Q = (queue *)malloc(sizeof(queue));
    //init
    Q->head = 1;
    Q->tail = 1;
    Q->length = 16;

    enqueue(Q, 3);
    enqueue(Q, 2);
    dequeue(Q);
    dequeue(Q);
    return 0;
}