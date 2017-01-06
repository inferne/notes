<?php 

define("∞", 99);
define("N", NULL);

/**
 * O(n^3)
 * @param array $W
 * D_k[i][j]为从节点i到节点j的所有中间节点全部取自集合{1,2,...,k}的一条最短路径的权重
 */
function floyd_warshall(array $W){
    $n = $W['rows'];
    $D_0 = $W;
    $P_0 = array();
    for($i = 1; $i <= $n; $i++){
        for($j = 1; $j <= $n; $j++){
            if($W[$i][$j] != ∞ && $i != $j){
                $P_0[$i][$j] = $i;
            }else{
                $P_0[$i][$j] = -1;
            }
        }
    }
    echo "D_0\n";
    print_square_matrix($D_0);
    echo "P_0\n";
    print_square_matrix($P_0);
    for($k = 1; $k <= $n; $k++){
        ${"D_".$k} = array();
        for($i = 1; $i <= $n; $i++){
            for($j = 1; $j <= $n; $j++){
                ${"P_$k"}[$i][$j] = ${"P_".($k-1)}[$i][$j];
                if(${"D_".($k-1)}[$i][$k] != ∞ && ${"D_".($k-1)}[$k][$j] != ∞ && ${"D_".($k-1)}[$i][$k] + ${"D_".($k-1)}[$k][$j] < ${"D_".($k-1)}[$i][$j]){
                    if($k != $j){
                        ${"P_$k"}[$i][$j] = ${"P_".($k-1)}[$k][$j];
                    }else{
                        ${"P_$k"}[$i][$j] = ${"P_".($k-1)}[$i][$k];
                    }
                }
                ${"D_".$k}[$i][$j] = min(${"D_".($k-1)}[$i][$j], ${"D_".($k-1)}[$i][$k] + ${"D_".($k-1)}[$k][$j]);
            }
        }
        echo "D_$k\n";
        print_square_matrix(${"D_$k"});
        echo "P_$k\n";
        print_square_matrix(${"P_$k"});
    }
    return ${"D_$n"};
}

/**
 * 有向图的传递闭包
 * @param array $G
 * 如果图中存在一条从节点i到节点j的所有中间节点都取自集合{1,2,...,k}的路径,则T_k[i][j]=1
 */
function transitive_closure(array $G){
    $n = $G['V'];
    $T_0 = array();
    for($i = 1; $i <= $n; $i++){
        for($j = 1; $j <= $n; $j++){
            if($i == $j || $G['E'][$i][$j] > 0){
                $T_0[$i][$j] = 1;
            }else{
                $T_0[$i][$j] = 0;
            }
        }
    }
    echo "T_0\n";
    print_square_matrix(${"T_0"});
    for($k = 1; $k <= $n; $k++){
        ${"T_$k"} = [];
        for($i = 1; $i <= $n; $i++){
            for($j = 1; $j <= $n; $j++){
                ${"T_$k"}[$i][$j] = ${"T_".($k-1)}[$i][$j] || (${"T_".($k-1)}[$i][$k] && ${"T_".($k-1)}[$k][$j]);
            }
        }
        echo "T_$k\n";
        print_square_matrix(${"T_$k"});
    }
    return ${"T_$n"};
}

function print_square_matrix(array $A){
    $n = isset($A['rows']) ? $A['rows'] : count($A);
    for ($i = 1; $i <= $n; $i++){
        for ($j = 1; $j <= $n; $j++){
            printf("%4d", $A[$i][$j]);
        }
        echo "\r\n";
    }
    echo "-------------------------------\r\n";
}

$W = [
    [N,  N,  N,  N,  N,  N],
    [N,  0,  3,  8,  ∞, -4],
    [N,  ∞,  0,  ∞,  1,  7],
    [N,  ∞,  4,  0,  ∞,  ∞],
    [N,  2,  ∞, -5,  0,  ∞],
    [N,  ∞,  ∞,  ∞,  6,  0],
];

$W['rows'] = 5;

floyd_warshall($W);

echo "************************************************\r\n";

$G = [
    'V' => 4,
    'E' => [
        [N,  N,  N,  N,  N],
        [N,  1,  N,  N,  N],
        [N,  N,  1,  1,  1],
        [N,  N,  1,  1,  N],
        [N,  1,  N,  1,  1],
    ]
];

transitive_closure($G);

//end