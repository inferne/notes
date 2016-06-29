#include <stdio.h>
#include <malloc.h>

typedef struct _node node;

struct _node
{
    char c;
    int freq;
    node *left;
    node *right;
};

typedef struct _queue
{
    int head;
    int tail;
    node *data;
    int length;
} queue;

print_queue(queue *Q)
{
    int i;
    for(i = Q->head; i < Q->tail; i++){
        printf("c=%c,freq=%-4d", Q->data[i].c, Q->data[i].freq);
    }
    printf("\n");
}
//取出最小值
node * extract_min(queue *Q)
{
    int i;
    node *x = NULL;
    if(Q->tail != Q->head){//表示空队列
        x = Q->data + Q->head;
        Q->head++;
    }
    return x;
}
//插入
void insert(queue *Q, node *z)
{
    int i,j;
    node *head = Q->data;
    node *x = (node *)malloc(sizeof(node));
    head[Q->tail] = *z;//新节点添加到队尾
    //把新节点移动到合适的位置
    i = Q->tail;
    while(head[i].freq < head[(i-1)].freq){
        *x = head[i];
        head[i] = head[(i-1)];
        head[(i-1)] = *x;
        i--;
    }
    Q->tail++;
}

void huffman(queue *Q)
{
    int i;
    int n = Q->length;
    node *x = NULL,*y = NULL;
    while(Q->tail - Q->head > 1){
        node *z;
        z->left = x = extract_min(Q);
        z->right = y = extract_min(Q);
        z->freq = x->freq + y->freq;
        insert(Q, z);
    }
}
//打印树 其中A为一个全为0的辅助矩阵，n是矩阵的行数，m是矩阵的行数列数
void print_tree(node *x, int *A, int n, int m, int i, int j)
{
    *(A + (i * m + j - 1)) = (int)x->c+1000;//+1000与在输出时方便与freq区别
    *(A + (i * m + j + 0)) = (int)':'+1000;
    *(A + (i * m + j + 1)) = x->freq;
    if(x->left != NULL){
        *(A + ((i + 1) * m + j - 1)) = -3;
        print_tree(x->left, A, n, m, i+2, j - m/(2 << (i+2)/2));
    }
    if(x->right != NULL){
        *(A + ((i + 1) * m + j + 2)) = -2;
        print_tree(x->right, A, n, m, i+2, j + m/(2 << (i+2)/2));
    }
    if(i == 0){//遍历输出
        int k;
        int c = 0;
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                k = *(A + (i * m + j)); 
                if(k == 0){
                    printf(" ");
                }else if(k == -3){
                    printf("/");
                }else if(k == -2){
                    printf("\\");
                }else if(k >= 1000){
                    printf("%c", k-1000);
                }else{
                    printf("%d", k);
                }
            }
            printf("\n");
        }
    }
}

int main()
{
    queue *Q = NULL;
    node C[32] = {{'f',5},{'e',9},{'c',12},{'b',13},{'d',16},{'a',45}};
    Q = (queue *)malloc(sizeof(queue));
    Q->head = 0;
    Q->tail = 6;
    Q->data = C;
    Q->length = 32;
    huffman(Q);
    
    int n = 10;
    int m = 64;
    int *A = (int *)malloc(sizeof(int) * n * m);//打印时使用
    node *x = (node *)malloc(sizeof(node));
    x = extract_min(Q);
    print_tree(x, A, n, m, 0, m/2);
    return 0;
}

//                                :100                              
//                                /  \                             
//                a:45                             :55              
//                                                /  \             
//                                        :25             :30      
//                                        /  \            /  \     
//                                    c:12     b:13     :14     d:16  
//                                                    /  \         
//                                                  f:5 e:9        
