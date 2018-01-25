<?php 

if(trim("    "))
    echo 1;
else 
    echo 2;
exit();

//namespace test;

define("AAA", "/");
define('BBB', "/");

echo AAA;
echo BBB;

echo DIRECTORY_SEPARATOR;

echo strtotime(date("Y-m-d"));
echo "<br>";
echo strtotime(date("Y-m-d 00:00:00"));

$a = '';

var_dump(json_encode($a, true));

function luhn($card) {
    $total = 0;
    for ($i=0; $i<strlen($card); $i++){
        if($i%2 == 1){
            $n = $card[$i]*2;
            $total += $n > 10 ? $n - 9 : $n;
        } else
            $total += $card[$i];
    }
    if($total%10 == 0)
        return true;
    else
        return false;
}

var_dump(luhn("6212260200046018491"));
var_dump(luhn("6230580000038178211"));

list($sec, $usec) = explode(".", microtime(true));
//echo $usec;
echo 'YC'.date("Ymdhis",$sec).$usec.rand(1000, 9999);

try {
    throw new \Exception();
}catch (\Exception $e){
    echo "Exception1\r\n";
} finally {
    echo "finally\r\n";
}

print_r(explode(',', 'aaa'));

echo ucwords('driver charge record');
echo ucwords('driver_charge_record');
echo ucwords('driverchargerecord');

echo sprintf("%04d", 0);

echo date("Y-m-d", strtotime("-1 months"));

echo '----------------------------------<br/>';

echo ini_get("yac.enable_cli");
