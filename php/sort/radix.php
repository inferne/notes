<?php 
/**
 * 基数排序
 * 有n个d为k进制数，从这个n个数的最低位采用计数排序开始排序，然后再排次高位，一直到d位
 */
function counting($array, $digit){
    //找出最大的数
    $max = 0;
    foreach($array as $value){
        if($value > $max) 
            $max = $value;
    }
    //在数组c中记录从0-max之间每个数的数量
    $c = array();
    for($i = 0; $i <= $max; $i++){
        $c[$i] =0;
    }
    //print_r($c);
    foreach($array as $value){
        $c[($value/$digit)%10] +=1;
    }
    //print_r($c);
    //整理数组c中的数据，另每个元素记录的数据为小于等于他的数的个数
    foreach($c as $key => $value){
        if($key > 0)
            $c[$key] += $c[$key-1];
    }
    //print_r($c);
    //输出排序数组B
    $b = array();
    for($i = count($array)-1; $i >= 0; $i--){
        $b[$c[($array[$i]/$digit)%10]-1] = $array[$i];
        $c[($array[$i]/$digit)%10]--;
    }
    return $b;
}

function radix($array, $d, $k=10){
    $digit=1;
    for($i = 1; $i <= $d; $i++){
        $array = counting($array, $digit);
        $digit *= $k;
    }
    return $array;
}

$array = [150,333,467,223,793,977,823,613,128,529];
$array = radix($array, 3, 10);

for ($i = 0; $i < count($array); $i++){
    printf("%4d", $array[$i]);
}
