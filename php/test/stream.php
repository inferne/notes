<?php

// $fp = stream_socket_client("tcp://sandbox.settlement.yongche.org:80", $errno, $errstr, 3, STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT);
// if (!$fp) {
//     echo "$errstr ($errno)<br />\n";
// } else {
//     fwrite($fp, "GET /V1/Driveraccount/getInfo HTTP/1.1\r\nHost: sandbox.settlement.yongche.org\r\nAccept: */*\r\nConnection:Keep-Alive\r\n\r\n");
//     while (!feof($fp)) {
// //         $c = fgetc($fp);
// //         if(in_array($c, array("\f", "\n", "\r", "\t", "\v"))){
// //             switch ($c){
// //                 case "\f": 
// //                     echo "\\f";
// //                     break;
// //                 case "\n": 
// //                     echo "\\n";
// //                     break;
// //                 case "\r": 
// //                     echo "\\r";
// //                     break;
// //                 case "\t": 
// //                     echo "\\t";
// //                     break;
// //                 case "\v": 
// //                     echo "\\v";
// //                     break;
// //                 default :
// //                     echo $c;
// //                     break;
// //             }
// //         }else{
// //             echo $c;
// //         }
//         $str = fread($fp, 1024);
//         //$str = fgets($fp, 1024);
//         echo $str;
//         if(substr($str, -4) == "\r\n\r\n"){
//             break;
//         }
//         //sleep(1);
//     }
//     //fclose($fp);
// }

// $fp2 = stream_socket_client("tcp://sandbox.settlement.yongche.org:80", $errno, $errstr, 3, STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT);
// if (!$fp2) {
//     echo "$errstr ($errno)<br />\n";
// } else {
//     fwrite($fp2, "GET /V1/Driveraccount/getInfo HTTP/1.1\r\nHost: sandbox.settlement.yongche.org\r\nAccept: */*\r\nConnection:Keep-Alive\r\n\r\n");
// //    $content = stream_get_contents($fp2);//一直读取直到超时才返回
// //    print_r($content);
//     $content = fread($fp2, 4096);
//     echo $content;
//     //fclose($fp);
// }

include "StreamHttpClient.php";

$url = "http://sandbox.settlement.yongche.org/V1/Driveraccount/getDriverChargeRecord";

$params = array(
    'driver_id' => 50005509,
    'service_order_id' => 6392822315957292205
);

// new Curl($url, "POST", $params);
// new Curl($url, "POST", $params);
// exit();

$http_client = new StreamHttpClient($url);
// $result = $http_client->get($params);
// echo $result;

$result = $http_client->post($params);
echo $result;

class Curl{
    private static $http_client = array();
    private $result;

    public function __construct($url,$method,$params){
        $key = md5($url);
        if(self::$http_client){
            $http_client = self::$http_client[$key];
        }else{
            $http_client = new StreamHttpClient($url);
            self::$http_client[$key] = $http_client;
        }
        $method = strtolower($method);
        $this->result = $http_client->{$method}($params);
    }

    public function execute(){
        return json_decode($this->result,true);
    }
}
