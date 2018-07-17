<?php 

/**
 * 
 * @author liyunfei
 * @version 1.0
 * @desc this class support http long connect
 */
class Http
{
    public function print_string($string){
        for($i = 0; $i < strlen($string); $i++){
            $c = $string[$i];
            switch ($c){
                case "\f":
                    echo "\\f";
                    break;
                case "\n":
                    echo "\\n";
                    break;
                case "\r":
                    echo "\\r";
                    break;
                case "\t":
                    echo "\\t";
                    break;
                case "\v":
                    echo "\\v";
                    break;
                default :
                    echo $c;
                    break;
            }
        }
    }
    
    /**
     * 检查http数据结束符
     * @param unknown $request
     * @return mixed
     */
    public static function check($request){
        if(substr($request, -5) == "0\r\n\r\n"){
            return 1;
        }
        $request = explode("\r\n\r\n", $request);
        //var_dump($request);
        $rqt = explode("\r\n", $request[0]);
        
        $request_line = $rqt[0];//请求行
        $rl = explode(" ", $request_line);
        $method = $rl[0];
        if($method == "GET" && count($request) == 2){
            return 1;
        }else{
            //请求头
            $header = [];
            for ($i = 1; $i < count($rqt); $i++) {
                $h = explode(":", $rqt[$i]);
                $header[$h[0]] = $h[1];
            }
            
            if(isset($header['Content-Length'])){
                if($header['Content-Length'] == strlen($request[1])){
                    return 1;
                }
            }
        }
        return 0;
    }
    
    /**
     * 解析请求数据
     * @param unknown $request
     * @return mixed
     */
    public static function resolve($request){
        //echo $request."\n";
        $request = explode("\r\n\r\n", $request);
        
        $rqt = explode("\r\n", $request[0]);
        
        $request_line = $rqt[0];//请求行
        $rl = explode(" ", $request_line);
        $method = $rl[0];
        //请求头
        $header = [];
        for ($i = 1; $i < count($rqt); $i++) {
            $h = explode(":", $rqt[$i]);
            $header[$h[0]] = $h[1];
        }
        
        $path = parse_url($rl[1], PHP_URL_PATH);
        switch ($method){
            case 'GET' :
                $query = parse_url($rl[1], PHP_URL_QUERY);
                break;
            case 'PUT' :
            case 'POST' :
                //数据
                if(isset($header['Content-Length'])){
                    $data = $request[1];
                }else{
                    $package = explode("\r\n", $request[1]);
                    $data = "";
                    for($i = 0; $i < count($package); $i+=2){
                        if(hexdec($package[$i]) > 0){
                            $data .= $package[$i+1];
                        }
                    }
                }
                $query = $data;
                break;
            default:
                $data = "invalid http method!";
                break;
        }
        
        $params = [];
        parse_str($query, $params);
        
        $result['request'] = $request_line;
        $result['header'] = $header;
        $result['data'] = ['path' => $path, 'params' => $params];
        
        return $result;
    }
    
    /**
     * 生成相应数据
     * @param unknown $data
     * @return string
     */
    public static function response($data){
        $response = "HTTP/1.1 200 OK\r\n"
            . "Connection: close\r\n"
            . "Date: ".date("D M j Y G:i:s T")."\r\n"
            . "Content-Type: text/json\r\n"
            . "\r\n"
            . json_encode($data);
        return $response;
    }
}