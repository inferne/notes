/* single source shortest path */

#include <stdio.h>

initialize_single_source(G, s)
{
    for each vertex v ∈ G.V
        v.d = ∞
        v.π = NIL
    s.d = 0
}

relax(u, v, w)
{
    if(v.d > u.d + w(u,v))
        v.d = u.d + w(u,v)
        v.π = u
}

bellman_ford(G, w, s)
{
    initialize_single_source(G, s);
    for i=1 to |G.V|-1
        for each edge(u,v) ∈ G.E
            relax(u,v,w)
    for each edge(u,v) ∈ G.E
        if v.d > u.d + w(u,v)
            return false
    return true
}

dag_shortest_paths(G, w, s)
{
    topologically sort the vertices of G
    initialize_single_source(G, s)
    for each vertex u, taken in topologically sorted order
        for each vertex v ∈ G.Adj[u]
            relax(u,v,w)
}

dijkstra(G, w, s)
{
    initialize_single_source(G, s);
    S = Ø
    Q = G.V
    while Q != Ø
        u = extract_min(Q)
        S = S∪{u}
        for each vertrx v ∈ G.Adj[u]
            relax(u,v,w)
}

yen_bellman_ford(G, w, s)
{
    initialize_single_source(G, s);
    Gf = Ø;
    Gb = Ø;
    for each u ∈ G.V
        for each v ∈ G.Adj[u]
            if(u.i < v.i)
                Gf.Adj[u] = Gf.Adj[u]∪{v}
            else
                Gb.Adj[u] = Gb.Adj[u]∪{v}
    for each u ∈ G.V
        for each v ∈ Gf.Adj[u]
            relax(u,v,w);
    for each u ∈ G.V
        for each v ∈ Gb.Adj[u]
            relax(u,v,w);
}
