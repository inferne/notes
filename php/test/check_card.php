<?php 

$host = 'https://auth.usolv.com';

$api3 = '/api/v1/certificate/bankcard/3';

$arr_user = array();
$arr_user[0] = array(
    'name' => '钟治宇',//错误时200,result为0
    'card' => '6230580000038178211',//错误时直接500
    'eid'  => '110105198802118334'//错误时200,result为0
);

$arr_user[1] = array(
    'name' => '李云飞',//错误时200,result为0
    'card' => '6212260200046018491',//错误时直接500
    'eid'  => '610528199008048155'//错误时200,result为0
);
$user = $arr_user[rand(0, count($arr_user)-1)];

$query = '?'.http_build_query($user);

$url = $host.$api3.$query;

$api_key = 'c3bf1bdb454b7e9fabe348ab';
list($sec, $usec) = explode(".", microtime(true));
$trans_id = 'YC'.date("Ymdhis",$sec).$usec.rand(1000, 9999);

$ch = curl_init($url);
curl_setopt($ch, CURLOPT_HEADER, false);
curl_setopt($ch, CURLOPT_HTTPHEADER, array(
    'Api-Key:'.$api_key,
    'X-Trans-ID:'.$trans_id
));
curl_setopt($ch, CURLOPT_POST, 1);
//curl_setopt($ch, CURLOPT_PROTOCOLS, CURLPROTO_HTTPS);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
curl_setopt($ch, CURLOPT_TIMEOUT, 10);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$result = curl_exec($ch);
//echo $result.PHP_EOL;

$error = '';
if(curl_errno($ch) > 0){
    $error = 'errno:'.curl_errno($ch).' error:'.curl_error($ch);
}

$info = array();
$info['url'] = curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
$info['http_code'] = curl_getinfo($ch, CURLINFO_HTTP_CODE);
$info['total_time'] = curl_getinfo($ch, CURLINFO_TOTAL_TIME);
$info['namelookup_time'] = curl_getinfo($ch, CURLINFO_NAMELOOKUP_TIME);
$info['connect_time'] = curl_getinfo($ch, CURLINFO_CONNECT_TIME);
//print_r($curl_info);

curl_close($ch);

$message = $trans_id .' '. $result.' '.$error.' '.json_encode($info).chr(10);
//echo $message;
error_log($message, 3, '/tmp/usolvlog');
