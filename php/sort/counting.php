<?php 
/**
 * 计数排序
 * 找出数组A中的最大值k，通过数组C记录每个0-k之间每个数<=它的数有多少个，直接记录到在数组B中对应的位置
 */
function counting($array){
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
        $c[$value] +=1;
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
        $b[$c[$array[$i]]-1] = $array[$i];
        $c[$array[$i]]--;
    }
    return $b;
}

$array = [10,3,4,2,7,9,8,6,1,5];
$array = counting($array, 0, 9);
for ($i = 0; $i < count($array); $i++){
    echo $array[$i];
}
