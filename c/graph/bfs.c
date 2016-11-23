#include <stdio.h>
#include "queue.h"

#define WHITE 1
#define GRAY  2
#define BLACK 3

typedef struct _graph graph;
typedef struct _node  node;
typedef struct _list  list;

struct _node
{
    int   k;
    int   color;
    int   d;
    node *f;
};

struct _list
{
    node *x;
    list *next;
};

struct _graph
{
    node *V;
    list *Adj;
};

//广度优先搜索
bfs(graph *G, node *s, int c)
{
    int i = 0;
    node *u, *v;
    list *ls;
    //顶点初始化
    while(i < c){
        u = G->V + i;
        u->color = WHITE;
        u->d = -1;
        u->f = NULL;
        i++;
    }
    s->color = GRAY;
    s->d = 0;
    s->f = NULL;
    //使用先进先出队列搜索
    queue *Q;
    init_queue(Q, 16);
    enqueue(Q, s);
    while(Q->cnt != 0){
        u = (node *)dequeue(Q);
        printf("u k=%d,color=%d,d=%d,f=%p\n", u->k, u->color, u->d, u->f);
        ls = (G->Adj + u->k)->next;
        while(ls != NULL){
            v = ls->x;//在Adj[u]链表中的v
            printf("v k=%d,color=%d,d=%d,f=%p\n", v->k, v->color, v->d, v->f);
            if(v->color == WHITE){
                v->color = GRAY;
                v->d = u->d + 1;
                v->f = u;
                enqueue(Q, v);
            }
            ls = ls->next;
        }
        u->color = BLACK;
    }
}

print_graph(graph *G, node *s)
{
    // if(v == s)
    //     printf("%d\n", s->k);
    // else if (v->f == NULL)
    //     printf("no path from s to v exists\n");
    // else{
    //     print_path(G, s, v->f);
    //     printf("%d\n", v->k);
    // }
}

void list_add(list *l, node *x)
{
    list *y = (list *)malloc(sizeof(list));
    y->x = x;
    y->next = l->next;
    l->next = y;
}

int main()
{
    node V[8];
    list Adj[8];
    graph *G;
    int i = 0, n = 8;
    G->V = V;
    G->Adj = Adj;

    for (i = 0; i < n; i++){
        V[i].k = i;
    }

    for (i = 0; i < n; i++){
        Adj[i].x = &V[i];
        Adj[i].next = NULL;
    }

    list_add(&Adj[0], &V[1]);
    list_add(&Adj[0], &V[4]);
    list_add(&Adj[1], &V[0]);
    list_add(&Adj[1], &V[5]);
    list_add(&Adj[2], &V[3]);
    list_add(&Adj[2], &V[5]);
    list_add(&Adj[2], &V[6]);
    list_add(&Adj[3], &V[2]);
    list_add(&Adj[3], &V[6]);
    list_add(&Adj[3], &V[7]);
    list_add(&Adj[4], &V[0]);
    list_add(&Adj[5], &V[1]);
    list_add(&Adj[5], &V[2]);
    list_add(&Adj[5], &V[6]);
    list_add(&Adj[6], &V[2]);
    list_add(&Adj[6], &V[3]);
    list_add(&Adj[6], &V[5]);
    list_add(&Adj[6], &V[2]);
    list_add(&Adj[6], &V[3]);
    list_add(&Adj[6], &V[5]);
    list_add(&Adj[6], &V[7]);
    list_add(&Adj[7], &V[3]);
    list_add(&Adj[7], &V[6]);

    bfs(G, V, n);
    print_graph(G, V);

    return 0;
}