#include <stdio.h>
#include <queue.h>

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
    int   π;
};

struct _list
{
    node *x;
    node *next;
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
    node *u;
    //顶点初始化
    while(i < c){
        u = G->V + i;
        u->color = WHITE;
        u->d = -1;
        u->π = NULL;
        i++;
    }
    s->color = GRAY;
    s->d = 0;
    s->π = NULL;
    //使用先进先出队列搜索
    queue *Q;
    init_queue(Q);
    enqueue(Q, s);
    while(Q != NULL){
        u = dequeue(Q);
        v = (G->Adj + u->k)->next;//在Adj链表中的u
        while((v = v->next) != NULL){
            if(v->color == WHITE){
                v->color = GRAY;
                v->d = u->d + 1;
                v->π = u;
                enqueue(Q, v);
            }
        }
        u->color = BLACK;
    }
}

print_path(graph *G, node *s, node *v)
{
    if(v == s)
        printf("%d\n", s->k);
    else if (v->π == NULL)
        printf("no path from s to v exists\n");
    else{
        print_path(G, s, v->π);
        printf("%d\n", v->k);
    }
}