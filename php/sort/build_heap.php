<?php
/**
 * 构造最大堆-插入法 
 */
 
//修改$i节点的值，重新生成堆
function heap_increase_key(&$heap, $i, $key)
{
    $parent = ceil($i/2)-1;
    $heap[$i] = $key;
    while ($i > 1 && $heap[$parent] < $heap[$i]){
        $heap[$i] = $heap[$i] ^ $heap[$parent];
        $heap[$parent] = $heap[$i] ^ $heap[$parent];
        $heap[$i] = $heap[$i] ^ $heap[$parent];
        $i = $parent;
        $parent = ceil($i/2)-1;
    }
    return $heap;
}

//插入元素
function max_heap_insert(&$heap, $key)
{
    $heap[] = 0;
    heap_increase_key($heap, count($heap)-1, $key);
}

//构造最大堆
function build_max_heap($array)
{
    $heap = array();
    foreach($array as $value){
        max_heap_insert($heap, $value);
    }
    return $heap;
}
$array = [10,3,4,2,7,9,8,6,1,5];
print_r(build_max_heap($array));