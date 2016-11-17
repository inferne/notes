#include <stdio.h>

#define WHITE 1
#define GRAY  2
#define BLACK 3

typedef struct _graph graph;
typedef struct _node  node;
typedef struct _list  list;

int time = 0;

struct _node
{
    int   k;
    int   color;
    int   d;
    int   f;
    int   π;
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

//深度优先搜索
dfs(graph *G, int c)
{
    int i = 0;
    node *u;
    //顶点初始化
    while(i < c){
        u = G->V + i;
        u->color = WHITE;
        u->π = NULL;
        i++;
    }
    time = 0;
    for(i = 1; i <= c; i++){
        u = G->V + i;
        if(u->color == WHITE){
            dfs_visit(G, u);
        }
    }
}

dfs_visit(graph *G, node *u)
{
    node *v;
    time = time + 1;
    u->d = time;
    u->color = GRAY;
    while((v = (G->Adj + u->k)->next->x) != NULL){
        if(v->color == WHITE){
            v->π = u;
            dfs_visit(G, v);
        }
    }
    u->color = BLACK;
    time = time + 1;
    u->f = time;
}