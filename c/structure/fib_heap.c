#include <stdio.h>
#include <malloc.h>

#define exchange(x, y)    x = x ^ y;y = x ^ y;x = x ^ y;

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
    node min;
    int n;
} heap;

heap * make_fib_heap()
{
    heap *H;
    H = (heap *)malloc(sizeof(heap));
    H->min = NULL;
    H->n = 0;
    return H;
}

void fib_heap_insert(heap *H, node *x)
{
    x->degree = 0;
    x->p = NULL;
    x->child = NULL;
    x->mark = FALSE;
    if(H->min == NULL){
        //create a root list fot H containing just x
        x->left = x;
        x->right = x;
        H->min = x;
    }else{
        //insert x into H's root list
        H->min->left->right = x;
        x->right = H->min;
        x->left = H->min->left;
        H->min->left = x;
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
    if(H1->min == NULL or (H2.min != NULL and H2->min->key < H1->min->key)){
        H->min = H2->min;
    }
    H->n = H1->n + H2->n;
    return H;
}

void fib_heap_link(heap *H, node *x, node *y)
{
    //
}
//合并根链表
void consolidate(heap *H)
{
    //let A[0, D(H.n) be a new array]
    node *A, *x = NULL, *y = NULL, *w = NULL;
    int i = 0, d = D(H->n);
    A = (node *)malloc(sizeof(node) * d);
    while(i < d){
        A[i] = NULL;
    }
    //for each node w in the root list of H
    w = H->root;
    do{
        x = w;
        d = x->drgree;
        while(A[d] != NULL){
            y = A[d]; //another node with the same drgree as x
            if(x->key > y->key){
                exchange(x, y);
            }
            fib_heap_link(H, x, y);
            A[d] = NULL;
            d = d + 1;
        }
        A[d] = x;
        w = w->right;
    } while();
}
//抽取最小节点
node * fib_heap_extract_min(heap *H)
{
    int i;
    node *z = H->min, node *x = NULL;
    if(z != NULL){
        //for each child x of z
        if(z->child != NULL){
            x = z->child;
            do{
                //add x to root list of H
                fib_heap_insert(H, x);
                x = x->left;
            }while(z->child != x);
        }
        //remove z from the root list of H
        fib_heap_delete(H, z);
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

int main()
{
    heap *H = make_fib_heap();
    return 0;
}