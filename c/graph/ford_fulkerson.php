<?php 

define("N", NULL);

function bfs(array $G, $s, $t){}

function ford_fulkerson(array $G, $s, $t){
    //init array f
    $f = [];
    foreach ($G['E'] as $u => $value){
        foreach ($value as $v => $val){
            $f[$u][$v] = 0;
        }
    }
    $Gf = $G;
    while( ($p = bfs(& $Gf, $s, $t)) != NULL ){
        $cfp = min($p);
        foreach ($p as $u => $v){
            if(in_array($v, $G['E']['u'])){
                $f[$u][$v] += $cfp;
            }else{
                $f[$v][$u] -= $cfp;
            }
        }
    }
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

johnson($W);

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

//end