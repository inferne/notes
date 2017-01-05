<?php 

define("∞", 99);

function print_all_pairs_shortest_path(array $P, $i, $j){
    if ($i == $j){
        print $i;
    }elseif ($P[$i][$j] == NULL){
        print "no path from i to j exists";
    }else{
        print_all_pairs_shortest_path($P, $i, $j);
        print $j;
    }
}

/**
 *
 * @param array $L 包含最短路径的实际权重
 * @param array $W 输入矩阵
 * @return Ambigous <multitype:, number, mixed>
 */
function extend_shortest_paths(array $L, array $W){
    $n = isset($L['rows']) ? $L['rows'] : count($L);
    $L1 = array();
    for($i = 0; $i < $n; $i++){
        for($j = 0; $j < $n; $j++){
            $L1[$i][$j] = ∞;
            for($k = 0; $k < $n; $k++){
                $L1[$i][$j] = min($L1[$i][$j], $L[$i][$k] + $W[$k][$j]);
            }
        }
    }
    return $L1;
}

/**
 *
 * @param array $L 包含最短路径的实际权重矩阵
 * @param array $W 输入矩阵
 * @param array $P 前驱节点的矩阵
 * @return Ambigous <multitype:, number, mixed>
 */
function extend_shortest_paths_2(array $L, array $W, array $P){
    $n = isset($L['rows']) ? $L['rows'] : count($L);
    $L1 = array();
    $P1 = array();
    for($i = 0; $i < $n; $i++){
        for($j = 0; $j < $n; $j++){
            $L1[$i][$j] = ∞;
            $P1[$i][$j] = -1;
            for($k = 0; $k < $n; $k++){
                //echo "i = $i, k = $k, j = $j, ",$L[$i][$k],' ',$W[$k][$j],' ',$L1[$i][$j],' ',$P[$k][$j],"\n";
                if($W[$k][$j] != ∞ && $L[$i][$k] != ∞ && $L[$i][$k] + $W[$k][$j] < $L1[$i][$j]){
                    if($k == $j){
                        $P1[$i][$j] = $P[$i][$k];
                    }else{
                        $P1[$i][$j] = $P[$k][$j];
                    }
                }
                $L1[$i][$j] = min($L1[$i][$j], $L[$i][$k] + $W[$k][$j]);
            }
        }
    }
    return array("L"=>$L1, "P"=>$P1);
}

/**
 * 对extend_shortest_paths替换后的矩阵乘法
 * @param array $A
 * @param array $B
 */
function square_matrix_multiply(array $A, array $B){
    $n = isset($A['rows']) ? $A['rows'] : count($A);
    $C = array();
    for($i = 0; $i < $n; $i++){
        for($j = 0; $j < $n; $j++){
            $C[$i][$j] = 0;
            for($k = 0; $k < $n; $k++){
                if($A[$i][$k] == ∞ || $B[$k][$j] == ∞){
                    $C[$i][$j] = ∞;
                }else{
                    $C[$i][$j] = $C[$i][$j] + $A[$i][$k] * $B[$k][$j];
                }
            }
        }
    }
    return $C;
}

/**
 * 计算出最短路径的矩阵序列
 * @param array $W
 * @return Ambigous <Ambigous <multitype:, number>>
 */
function slow_all_pairs_shortest_paths(array $W){
    $n = $W['rows'];
    $L_1 = $W;
    for($m = 2; $m <= $n-1; $m++){
        ${"L_".$m} = array();
        ${"L_".$m} = extend_shortest_paths(${"L_".($m-1)}, $W);
        print_square_matrix(${"L_".$m});
    }
    return ${"L_".($n-1)};
}

/**
 * 计算出最短路径的矩阵序列和前驱节点的矩阵序列
 * @param array $W
 * @return Ambigous <Ambigous <multitype:, number>>
 */
function slow_all_pairs_shortest_paths_2(array $W){
    $n = $W['rows'];
    $L_1 = $W;
    $P_1 = array();
    for($i = 0; $i < $n; $i++){
        for($j = 0; $j < $n; $j++){
            if($W[$i][$j] != ∞){
                $P_1[$i][$j] = $i;
            }else{
                $P_1[$i][$j] = -1;
            }
        }
    }
    print_square_matrix($P_1);
    for($m = 2; $m <= $n-1; $m++){
        ${"L_".$m} = array();
        $result = extend_shortest_paths_2(${"L_".($m-1)}, $W, ${"P_".($m-1)});
        ${"L_".$m} = $result['L'];
        ${"P_".$m} = $result['P'];
        echo "L_$m\r\n";
        print_square_matrix(${"L_".$m});
        echo "P_$m\r\n";
        print_square_matrix(${"P_".$m});
    }
    return ${"L_".($n-1)};
}

function faster_all_pairs_shortest_paths(array $W){
    $n = $W['rows'];
    $L_1 = $W;
    $m = 1;
    while($m < $n - 1){
        ${"L_".(2 * $m)} = array();
        ${"L_".(2 * $m)} = extend_shortest_paths(${"L_".$m}, ${"L_".$m});
        print_square_matrix(${"L_".(2 * $m)});
        $m = 2 * $m;
    }
    return ${"L_".$m};
}

function print_square_matrix(array $A){
    $n = isset($A['rows']) ? $A['rows'] : count($A);
    for ($i = 0; $i < $n; $i++){
        for ($j = 0; $j < $n; $j++){
            printf("%4d", $A[$i][$j]);
        }
        echo "\r\n";
    }
    echo "-------------------------------\r\n";
}

$W = [
    [0,  3,  8,  ∞, -4],
    [∞,  0,  ∞,  1,  7],
    [∞,  4,  0,  ∞,  ∞],
    [2,  ∞, -5,  0,  ∞],
    [∞,  ∞,  ∞,  6,  0],
];

$W['rows'] = 5;

print_square_matrix($W);
slow_all_pairs_shortest_paths($W);
echo "**************************************************\n";
faster_all_pairs_shortest_paths($W);
echo "**************************************************\n";
echo "L_1\r\n";
print_square_matrix($W);
echo "P_1\r\n";
slow_all_pairs_shortest_paths_2($W);
