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
void print_fib_heap(node *x);

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
    node *A[d], *x = NULL, *y = NULL, *w = NULL;
    //A = (node *)malloc(sizeof(node) * d);
    while(i <= d){
        A[i] = NULL;
        i++;
    }
    //for each node x in the root list of H
    x = H->min;
    do{
        d = x->degree;
        while(A[d] != NULL){
            y = A[d]; //another node with the same drgree as x
            //sure x->key < y->key
            if(x->key > y->key){
                w = x;
                x = y;
                y = w;
            }
            fib_heap_link(H, x, y);
            A[d] = NULL;
            d = d + 1;
        }
        A[d] = x;
        x = x->right;//move x
    } while(x != H->min);
    H->min = NULL;
    for(i = 0; i <= sizeof(A)/sizeof(node *); i++){
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
    node *z = H->min, *x = NULL;
    if(z != NULL){
        //for each child x of z
        if(z->child != NULL){
            x = z->child;
            do{
                printf("---------------------------------------\n");
                x = x->left;
                //add x to root list of H
                print_fib_node(H->min);
                print_fib_node(x->right);
                fib_heap_add(H->min, x->right);
                print_fib_node(H->min);
            }while(z->child != x);
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
    printf("-------------------------------------------------\n");
    print_fib_heap(H->min);
    fib_heap_extract_min(H);
}

void print_fib_node(node *x)
{
    printf("x=%p,key=%d,p=%p,left=%p,right=%p,child=%p,degree=%d,mark=%d\n",
        x, x->key, x->p, x->left, x->right, x->child, x->degree, x->mark);
}

void print_fib_heap(node *x)
{
    // int k = 0;
    // node *y = x;
    // while(k == 0 || y != x){
    //     *(A + i*n + j) = y->key;
    //     if(y->child != NULL){
    //         *(A + (i+1)*n + j+1) = -1;
    //         print_fib_heap(y->child, A, m, n, i+2, j);
    //     }
    //     y = y->right;
    //     k++;
    //     j += 2;
    // }

    int k = 0;
    node *y = x;
    while(k == 0 || y != x){
        printf("%4d", y->key);;
        if(y->child != NULL){
            printf("\n");
            print_fib_heap(y->child);
        }
        y = y->right;
        k++;
    }
    printf("\n");
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

    print_fib_heap(H->min);

    x = fib_heap_extract_min(H);
    printf("------------------------------------------------------------\n");
    print_fib_node(x);
    print_fib_heap(H->min);

    fib_heap_delete(H, H->min);
    printf("------------------------------------------------------------\n");
    print_fib_heap(H->min);

    return 0;
}