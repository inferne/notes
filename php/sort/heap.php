<?php 
/**
 * 堆排序
 * 通过最大（小）堆实现排序，算法复杂度为O(nlgn)
 */

//使$i的子树为最大堆
function max_heapify($array, $i)
{
    $l = ($i+1)*2-1;
    $r = ($i+1)*2;
    $largest = null;
    printf("%4d:%4d%4d%4d\n", $i, $array[$i], $l, $r);
    if(isset($array[$l]) && $array[$l] > $array[$i]){
        $largest = $l;
    }else{
        $largest = $i;
    }
    if(isset($array[$r]) && $array[$r] > $array[$largest]){
        $largest = $r;
    }
    //判断$i是否已经最大
    if($largest != $i){
        //交换$i和$largest
        $array[$i] = $array[$i] ^ $array[$largest];
        $array[$largest] = $array[$i] ^ $array[$largest];
        $array[$i] = $array[$i] ^ $array[$largest];
        $array = max_heapify($array, $largest);//递归
    }
    return $array;
}

//构造最大堆
function build_max_heap($array)
{
    //从第一个非叶子节点开始循环
    for($i = count($array)/2-1; $i > 0; $i--){
        $array = max_heapify($array, $i);
        printf("%s\n", json_encode($array));
    }
    printf("bulid success:%s\n", json_encode($array));
    return $array;
}

function heap_sort($array)
{
    $result = array();
    $array = build_max_heap($array);
    $i = count($array);
    while ( $i > 0 ){
        if($i > 1){
            //交换树根和最后一个叶子元素
            $last = count($array)-1;
            $array[0] = $array[0] ^ $array[$last];
            $array[$last] = $array[0] ^ $array[$last];
            $array[0] = $array[0] ^ $array[$last];
        }else 
            $last = 0;
        $result[] = $array[$last];
        //printf("last:%d\n", $array[$last]);
        unset($array[$last]);
        $i--;
        if($i > 1){
            $array = max_heapify($array, 0);
            printf("%s\n", json_encode($array));
        }
    }
    return $result;
}

$array = [10,3,4,2,7,9,8,6,1,5];
print_r(heap_sort($array));