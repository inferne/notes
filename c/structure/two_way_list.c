#include <stdio.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0

typedef struct _list
{
    int key;
    struct _list *prev;//上一个
    struct _list *next;//下一个
} list;

list * new_node()
{
    list *new;
    new = (list *)malloc(sizeof(list));
    new->prev = NULL;
    new->next = NULL;
    return new;
}

list * list_search(list *L, int k)
{
    list *h = L;
    while(h != NULL && h->key != k){
        h = h->next;
    }
    return h;
}

list * list_insert(list *L, list *x)
{
    x->next = L;
    if(L != NULL){//L是非空链表
        L->prev = x;
    }
    return x;
}

void list_delete(list *L, list *x)
{
    if(x->prev != NULL){
        (x->prev)->next = x->next;
    }else{
        L = x->next;
    }
    if(x->next != NULL){
        (x->next)->prev = x->prev;
    }
    free(x);
}

list * list_create(int n)
{
    int i;
    list *head=NULL,*new,*tail;
    //创建连表
    for(i = 0; i < n; i++){
        //新建一个节点
        new = new_node();
        new->key = i;
        if(i == 0){//头结点
            head = new;
        }else{//其他节点
            tail->next = new;
            new->prev = tail;
        }
        tail = new;//更新tail
    }
    return head;
}
//逆转单链表
list * reverse(list *L)
{
    list *n = L,*i=NULL,*j=NULL;
    j = n->next;
    n->next = NULL;
    while(n != NULL && j != NULL){
        i = j;//保存n节点的next指针
        j = i->next;//保存n节点的下下个节点指针
        i->next = n;//让n的下一个节点的next指针指向n
        n = i;
    }
    return n;
}

void print_list(list *L)
{
    list *n = L;
    while(n != NULL){
        printf("%4d", n->key);
        n = n->next;
    }
    printf("\n-----------------------------------------------------------\n");
}

int main()
{
    list *L,*x,*y;
    L = list_create(10);
    //print_list(L);
    y = new_node();
    y->key = 12;
    L = list_insert(L, y);
    //print_list(L);
    x = list_search(L, 6);
    list_delete(L, x);
    //print_list(L);
    L = reverse(L);
    //print_list(L);
    return 0;
}

// 输出
//    0   1   2   3   4   5   6   7   8   9
// -----------------------------------------------------------
//   12   0   1   2   3   4   5   6   7   8   9
// -----------------------------------------------------------
//   12   0   1   2   3   4   5   7   8   9
// -----------------------------------------------------------
//   9   8   7   5   4   3   2   1   0  12
// -----------------------------------------------------------
