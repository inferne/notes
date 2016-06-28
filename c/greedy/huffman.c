#include <stdio.h>
#include <malloc.h>

typedef struct _node
{
    char c;
    int freq;
    struct _node *left;
    struct _node *right;
} node;

typedef struct _queue
{
    int head;
    int tail;
    node *data;
    int length;
} queue;

//取出最小值
node * extract_min(queue *Q)
{
    int i;
    node *x = &Q->data[Q->head];
    Q->head++;
    return x;
}

//插入
void insert(queue *Q, node *z)
{
    int i,j;
    int n = Q->tail - Q->head;
    node *head = &Q->data[Q->head];
    for(i = 0; i < n; i++){
        if(head[i].freq > z->freq){
            for(j = n-1; j >= i; j--){
                head[j] = head[j-1];
            }
            head[i] = *z;
        }
    }
    Q->tail++;
}

void huffman(queue *Q)
{
    int i;
    int n = Q->length;
    node *x,*y;
    for(i = 0; i < n; i++){
        node *z;
        z->left = x = extract_min(Q);
        z->right = y = extract_min(Q);
        z->freq = x->freq + y->freq;
        insert(Q, z);
    }
}
//未完待续
int main()
{
    queue *Q;
    node C[] = {{'f',5},{'e',9},{'c',12},{'b',13},{'d',16},{'a',45}};
    Q = (queue *)malloc(sizeof(queue));
    Q->head = 0;
    Q->tail = 0;
    Q->data = C;
    Q->length = 6;
    huffman(Q);
    printf("\n");
    return 0;
}

// 4   7  11  16