#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define FALSE 0
#define TRUE  1
#define or    ||
#define and   &&

typedef struct _node node;

struct _node
{
    int   key;
    node *p;
    node *left;
    node *right;
    node *child;
    int   degree;
    int   mark;
};

typedef struct _heap
{
    node *min;
    int   n;
} heap;

void print_fib_node(node *x);
void print_fib_heap(node *x, int *A, int n, int m, int i, int j);

int D(int n)
{
    int i = 0;
    while(n >> i+1 > 0){
        i++;
    }
    return i;
}

heap * make_fib_heap()
{
    heap *H;
    H = (heap *)malloc(sizeof(heap));
    H->min = NULL;
    H->n = 0;
    return H;
}
//create a root list fot H containing just x
void fib_create_root(heap *H, node *x)
{
    x->left = x;
    x->right = x;
    H->min = x;
}
//add y to x left
void fib_heap_add(node *x, node *y)
{
    x->left->right = y;
    y->right = x;
    y->left = x->left;
    x->left = y;
}

void fib_heap_insert(heap *H, node *x)
{
    x->degree = 0;
    x->p = NULL;
    x->child = NULL;
    x->mark = FALSE;
    if(H->min == NULL){
        //create a root list fot H containing just x
        fib_create_root(H, x);
    }else{
        //insert x into H's root list
        fib_heap_add(H->min, x);
        if(x->key < H->min->key){
            H->min = x;
        }
    }
    H->n += 1;
}
//合并
heap * fib_heap_union(heap *H1, heap *H2)
{
    heap *H = make_fib_heap();
    H->min = H1->min;
    /* concatenate the root list of H2 with the root list of H */
    H->min->right->left = H2->min->left;
    H2->min->left->right = H->min->right;
    H->min->right = H2->min;
    H2->min->left = H->min;
    if(H1->min == NULL or (H2->min != NULL and H2->min->key < H1->min->key)){
        H->min = H2->min;
    }
    H->n = H1->n + H2->n;
    return H;
}
//移除y的子节点x
void fib_heap_remove(node *x)
{
    x->left->right = x->right;
    x->right->left = x->left;
    if(x->p != NULL){
        x->p->degree -= 1;
    }
}
//把y链接到x的child上
void fib_heap_link(heap *H, node *x, node *y)
{
    fib_heap_remove(y);

    if(x->child == NULL){
        y->left = y->right = y;
        x->child = y;
    }else{
        fib_heap_add(x->child, y);
    }
    y->p = x;
    x->degree += 1;
    y->mark = FALSE;
}
//合并根链表
void consolidate(heap *H)
{
    int i = 0, d = D(H->n);
    //let A[0, D(H.n) be a new array]
    node *A[d+1], *x = NULL, *y = NULL, *z = NULL, *w = NULL;
    //A = (node *)malloc(sizeof(node) * d);
    while(i <= d){
        A[i] = NULL;
        i++;
    }
    //for each node w in the root list of H
    w = H->min;
    do{
        x = w;
        w = w->right;//move w
        d = x->degree;
        while(A[d] != NULL){
            y = A[d]; //another node with the same drgree as x
            //sure x->key < y->key
            if(x->key > y->key){
                z = x;
                x = y;
                y = z;
            }
            fib_heap_link(H, x, y);
            A[d] = NULL;
            d = d + 1;
        }
        A[d] = x;
    } while(w != H->min);
    H->min = NULL;
    for(i = 0; i < sizeof(A)/sizeof(node *); i++){
        if(A[i] != NULL){
            if(H->min == NULL){
                //create a root list for H containing just A[i]
                fib_create_root(H, A[i]);
            }else{
                //insert A[i] into H's root list
                fib_heap_add(H->min, A[i]);
                if(A[i]->key < H->min->key){
                    H->min = A[i];
                }
            }
        }
    }
}
//抽取最小节点
node * fib_heap_extract_min(heap *H)
{
    int i;
    node *z = H->min, *x = NULL, *y = NULL;
    if(z != NULL){
        //for each child x of z
        if(z->child != NULL){
            y = z->child;
            do{
                x = y;
                y = y->right;
                //add x to root list of H
                x->p = NULL;
                fib_heap_add(H->min, x);
            }while(z->child != y);
        }
        //remove z from the root list of H
        fib_heap_remove(z);
        if(z == z->right){
            H->min = NULL;
        }else{
            H->min = z->right;
            consolidate(H);
        }
        H->n -= 1;
    }
    return z;
}
//切断
void cut(heap *H, node *x, node *y)
{
    //remove x from the child list of y,desrementing y->degree
    fib_heap_remove(x);
    //add x to the root list of H
    fib_heap_insert(H, x);
    x->p = NULL;
    x->mark = FALSE;
}
//级联切断
void cascading_cut(heap *H, node *y)
{
    node *z = y->p;
    if(z != NULL){
        if(y->mark == FALSE){
            y->mark = TRUE;
        }else{
            cut(H, y, z);
            cascading_cut(H, z);
        }
    }
}
//关键字减值
void fib_heap_decrease_key(heap *H, node *x, int k)
{
    if(k > x->key){
        printf("new key is greater than current key\n");
        exit(1);
    }
    x->key = k;
    node *y = x->p;
    if(y != NULL and x->key < y->key){
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if(x->key < H->min->key){
        H->min = x;
    }
}
//删除一个节点
void fib_heap_delete(heap *H, node *x)
{
    fib_heap_decrease_key(H, x, -1);
    fib_heap_extract_min(H);
}

void print_fib_node(node *x)
{
    printf("x=%p,key=%d,p=%p,left=%p,right=%p,child=%p,degree=%d,mark=%d\n",
        x, x->key, x->p, x->left, x->right, x->child, x->degree, x->mark);
}

// void print_fib_heap(node *x)
// {
//     int k = 0;
//     node *y = x;
//     while(k == 0 || y != x){
//         printf("%4d", y->key);;
//         if(y->child != NULL){
//             printf("\n");
//             print_fib_heap(y->child);
//         }
//         y = y->right;
//         k++;
//     }
//     printf("\n");
// }

void print_fib_heap(node *x, int *A, int n, int m, int i, int j)
{
    int b = 0, p = 0;
    int width = 0;
    node *y = x;
    do{
        *(A+i*m+j) = x->key;//节点位置
        if(x->child != NULL){
            *(A+(i+1)*m+j) = -1;//|位置
            print_fib_heap(x->child, A, n, m, i+2, j);
        }
        width = 1 << x->degree;
        x = x->right;
        if(x != y){
            for(p = 1; p <= width; p++){
                *(A+i*m+j+p) = -2;//-位置
            }
        }
        j = j + width + 1;//下一个节点开始位置
    } while(x != y);

    if(i == 0){//遍历输出
        int k;
        for(i = 0; i < n; i++){
            for(j = 0; j < m; j++){
                k = *(A + (i * m + j)); 
                if(k == 0){
                    printf("  ");
                }else if(k == -1){
                    printf(" |");
                }else if(k == -2){
                    printf("--");
                }else{
                    printf("%2d", k);
                }
            }
            printf("\n");
        }
    }
}

int main()
{
    int A[15] = {23,7,21,3,18,52,38,39,41,17,30,24,26,46,35};
    heap *H = make_fib_heap();
    
    int i = 0;
    node *x = NULL;
    for(i = 0; i < 15; i++){
        x = (node *)malloc(sizeof(node));
        x->key = A[i];
        fib_heap_insert(H, x);
    }

    int *B = NULL, n = 10, m = 64;
    B = (int *)malloc(sizeof(int) * n * m);

    printf("print_fib_heap\n");
    printf("------------------------------------------------------------\n");
    print_fib_heap(H->min, B, 1, m, 0, 1);

    printf("fib_heap_extract_min\n");
    printf("------------------------------------------------------------\n");
    x = fib_heap_extract_min(H);
    print_fib_node(x);
    B = (int *)malloc(sizeof(int) * n * m);
    print_fib_heap(H->min, B, 7, m, 0, 1);

    printf("fib_heap_delete(%d)\n", H->min->key);
    printf("------------------------------------------------------------\n");
    fib_heap_delete(H, H->min);
    B = (int *)malloc(sizeof(int) * n * m);
    print_fib_heap(H->min, B, 7, m, 0, 1);

    return 0;
}

// print_fib_heap
// ------------------------------------------------------------
//    3-- 7--23--21--18--52--38--39--41--17--30--24--26--46--35
// fib_heap_extract_min
// ------------------------------------------------------------
// x=0x7cf0f0,key=3,p=(nil),left=0x7cf3b0,right=0x7cf070,child=(nil),degree=0,mark=0
//    7----------------17--------35                            
//    |                 |         |                            
//   23--18----38      30--24    46                            
//        |     |           |                                  
//       21    52--39      26                                  
//                  |                                          
//                 41                                          
// fib_heap_delete(7)
// ------------------------------------------------------------
//   17----------------23--38                                  
//    |                     |                                  
//   30--24----18          52--39                              
//        |     |               |                              
//       26    21--35          41                              
//                  |                                          
//                 46                                          