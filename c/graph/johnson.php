<?php 

define("∞", 99);
define("N", NULL);

function bellman_ford($G, $w, $s) {}
function dijkstra($G, $w, $u) {}

function johnson(array $G, array $w){
    //G'.V增加s顶点
    $G1 = $G;
    $G1['V'][] = 's';
    //G'.E增加s顶点相关的边
    $G1['E']['s'] = $G['V'];
    //w(s,v) = 0
    foreach ($G['V'] as $k => $v){
        $w1['s'][$k] = 0;
    }
    
    if(bellman_ford($G1, $w, 's') == FALSE){
        print "the input graph contains a negative-weight cycle";
    }else{
        foreach ($G1['V'] as $k => $v){
            $h['v'] = $w['s'][$v];
        }
        //translation w->w1
        foreach ($G1['E'] as $k => $v){
            $w1['u']['v'] = $w['u']['v'] + $h['u'] - $h['v'];
        }
        $D = $w;
        foreach ($G['V'] as $k => $u){
            $d = dijkstra($G, $w1, $u);
            foreach ($G['V'] as $m => $v){
                $D['u']['v'] = $d['u']['v'] + $h['v'] - $h['u'];
            }
        }
    }
    return $D;
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