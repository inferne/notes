#include <stdio.h>

strongly_connected_components(graph *G)
{
    dfs(G);
    GT = compute(G);
    dfs(GT);
    dfs_tree(GT);
}
