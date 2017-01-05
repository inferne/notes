#include <stdio.h>

print_all_pairs_shortest_path(∏, i, j)
{
    if i == j
        print i
    else if π_ij == NIL
        print "no path from" i "to" j "exists"
    else print_all_pairs_shortest_path(∏, i, π_ij);
        print j
}

extend_shortest_paths(L, W)
{
    n = L.rows
    let LL = (ll_ij) be a new n * n matrix
    for i = 1 to n
        for j = 1 to n
            ll_ij = ∞
            for k = 1 to n
                ll_ij = min(ll_ij, l_ik+w_kj)
    return LL;
}

square_matrix_multiply(A,B)
{
    n = A.rows
    let C be a new n * n matrix
    for i = 1 to n
        for j = 1 to n
            c_ij = 0
            for k = 1 to n
                c_ij = c_ij + a_ik*b_kj
    return C
}

slow_all_pairs_shortest_paths(W)
{
    n = W.rows
    L^(1) = W
    for m = 2 to n-1
        let L^(m) be a new n*n matrix
        L^(m) = extend_shortest_paths(L^(m-1),W)
    return L^(n-1)
}

faster_all_pairs_shortest_paths(W)
{
    n = W.rows
    L^(1) = W
    m = 1
    while m < n-1
        let L^(2m) be a new n*n matrix
        L^(2m) = extend_shortest_paths(L^(m),L^(m))
        m = 2m
    return L^(m)
}

floyd_warshall(W)
{
    n = W.rows
    D^(0) = W
    for k = 1 to n
        let D^(k) = (d^(k)_ij) be a new n*n matrix
        for i = 1 to n
            for j = 1 to n
                d^(k)_ij = min(d^(k-1)_ij, d^(k-1)_ik+d^(k-1)_kj)
    return D^(n)
}

transitive_closure(G)
{
    n = |G.V|
    let T^(0) = (t^(0)_ij) be a new n*n matrix
    for i = 1 to n
        for j = 1 to n
            if i == j or (i,j)∈G.E
                t^(0)_ij = 1
            else
                t^(0)_ij = 0
    for k = 1 to n
        let T^(k) = (t^(k)_ij) be a new n*n matrix
        for i = 1 to n
            for j = 1 to n
                t^(k)_ij = t^(k-1)_ij || (t^(k-1)_ik && t^(k-1)_kj)
    retrun T^(n)
}