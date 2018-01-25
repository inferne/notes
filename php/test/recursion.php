<?php 

ini_set('xdebug.max_nesting_level', 999999);

$n = 400;
$time1 = microtime(true);
function test($n){
    if($n == 1) return 1;
    return $n+test($n-1);
}
$result = test($n);
$time2 = microtime(true);
echo "recursion $n:".($time2-$time1)." result:$result<br/>";
$result = 0;
while ($n){
    $result += $n--;
}
$time3 = microtime(true);
echo "while $n:".($time3-$time2)." result:$result<br/>";
