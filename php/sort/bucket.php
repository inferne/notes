<?php 
/**
 * 桶排序
 * 把n个数分不到n个桶中，然后对每个桶中的元素进行排序，合并所有桶即得排序结果，O(n)，worst case O(n^2)
 */
 
function insert($array){
    //找出最大的数
    foreach ($array as $i => $value){
        for($j = $i-1; $j >= 0; $j--){
            if($array[$i] < $array[$j]){
                $array[$i] ^= $array[$j];
                $array[$j] ^= $array[$i];
                $array[$i] ^= $array[$j];
            }
        }
    }
    return $array;
}

function bucket($array){
    $bucket = array();
    //所有元素放入桶中
    foreach($array as $value){
        $bucket[$value][] = $value;
    }
    //每个桶中的数排序
    foreach($bucket as $key => $value){
        $bucket[$key] = insert($value);
    }
    //print_r($bucket);
    //合并所有桶
    $array = array();
    for($i = 0; $i < count($bucket); $i++){
        foreach($bucket[$i] as $k => $v){
            $array[] = $v;
        }
    }
    return $array;
}

$array = [0,3,4,2,7,9,8,6,1,5];
//print_r(insert($array));
print_r(bucket($array));
// Array
// (
//     [0] => 0
//     [1] => 1
//     [2] => 2
//     [3] => 3
//     [4] => 4
//     [5] => 5
//     [6] => 6
//     [7] => 7
//     [8] => 8
//     [9] => 9
// )