<?php 
/**
 * 合并排序 
 * 通过按顺序合并两个有序数组来实现排序，算法复杂度为O(nlgn)
 */
function merge($array, $start, $end){
    if($start == $end) return $array;
    $middle = (int)(($start+$end)/2);
    //echo $middle;
    $array = merge($array, $start, $middle);
    $array = merge($array, $middle+1, $end);
    return child_merge($array, $start, $middle, $end);
}

function child_merge($array, $start, $middle, $end){
    $left = array();
    $right = array();
    printf("start=%d middle=%d end=%d\n", $start, $middle, $end);
    
    for ($i = 0; $i < $middle-$start+1; $i++) {
        $left[$i] = $array[$start+$i];
    }
    for ($j = 0; $j < $end-$middle; $j++) {
        $right[$j] = $array[$middle+1+$j];
    }
    
    $left[] = 999999;
    $right[] = 999999;
    $i = 0;
    $j = 0;
    for ($k = $start; $k <= $end; $k++) {
        if ($left[$i] <= $right[$j]) {
            $array[$k] = $left[$i];
            $i++;
        } else {
            $array[$k] = $right[$j];
            $j++;
        }
        printf("%4d", $array[$k]);
    }
    printf("\n");
    return $array;
}

$array = [10,3,4,2,7,9,8,6,1,5];
print_r(merge($array, 0, 9));