#include <stdio.h>

void kruskal(G, w)
{
    A = kong;
    for each vertex v属于G.v
        make_set(v);
    sort the edges of G.E into nondecreasing order by weight w
    for each edge(u,v)属于G.E，taken in nondecreasing order by weight
        if(find_set(u) != find_set(v))
            A = A交{(u,v)}
            union(u,v)
    return A
}

void prim(G, w, r)
{
    for each u 属于 G.V
        u.key = 无穷大
        u.p = NIL;
    r.key = 0;
    Q = G.V;
    while Q != NULL
        u = extract_min(Q);
        for each v 属于 G.Adj[u]
            if v 属于 Q and w(u, v) < v.key
                v.p = u;
                v.key = w(u, v);//decrease_key(Q, v, w(u, v));
}