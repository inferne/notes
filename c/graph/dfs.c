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
    node *p;
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
        u->p = NULL;
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
    list *ls = (G->Adj + u->k)->next;
    while(ls != NULL){
        v = ls->x;
        if(v->color == WHITE){
            v->p = u;
            dfs_visit(G, v);
        }
        ls = ls->next;
    }
    u->color = BLACK;
    time = time + 1;
    u->f = time;
}
