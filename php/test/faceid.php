<?php 

class Faceid
{
    private $api_key = 'CWNQnGuN_-y8Gtk9kKtlU8Dy-BKsNVUN';
    private $api_secret = 'VN6zL67-X5nNVB5EE-444jeA3OPvu-0s';
    
    private $host = 'https://api.faceid.com/';
    private $verify = 'faceid/v1/verify';
    private $detect = 'faceid/v1/detect';
    private $ocridcard = 'faceid/v1/ocridcard';
    
    public function verify($face_token='')
    {
        $name = '李云飞';
        $idcard = '610528199008048155';
        $delta = 'delta';
        $filename = 'image/20140711171118_FxyTW.jpeg';
        $url = $this->host.$this->verify;
        $params = array(
            'api_key'=>$this->api_key,
            'api_secret'=>$this->api_secret,
            'name'=>$name,
            'idcard'=>$idcard,
        );
        if($face_token){
            $params['face_token'] = $face_token;
        }else{
            $params['delta'] = $delta;
            //'image_action[x]'=>$name,
            //'image_idcard[Y]'=>$name,
            $filename = realpath($filename);
            if(version_compare("5.5", PHP_VERSION, "<=")){
                //CURLFile好像需要绝对路径
                $params['image_best'] = new CURLFile($filename, 'image/jpeg', 'image_best');
                //var_dump($params['image_best']);
            }else{
                $params['image_best'] = "@$filename";
            }
        }
        $this->curl_file($url, $params);
    }
    
    public function detect()
    {
        $filename = 'image/34766ef5e.jpg';
        $url = $this->host.$this->detect;
        $params = array(
            'api_key'=>$this->api_key,
            'api_secret'=>$this->api_secret,
        );
        $filename = realpath($filename);
        
        if(version_compare("5.5", PHP_VERSION, "<=")){
            //CURLFile好像需要绝对路径
            $params['image'] = new CURLFile($filename, 'image/jpeg', 'image');
            //var_dump($params['image_best']);
        }else{
            $params['image'] = "@$filename";
        }
        return $this->curl_file($url, $params);
    }
    
    public function ocridcard()
    {
        $filename = 'image/a50f4bfbfbedab642fd5b086f736afc378311ed4.jpg';
        $url = $this->host.$this->ocridcard;
        $params = array(
            'api_key'=>$this->api_key,
            'api_secret'=>$this->api_secret,
        );
        $filename = realpath($filename);
        
        if(version_compare("5.5", PHP_VERSION, "<=")){
            //CURLFile好像需要绝对路径
            $params['image'] = new CURLFile($filename, 'image/jpeg', 'image');
            //var_dump($params['image_best']);
        }else{
            $params['image'] = "@$filename";
        }
        $this->curl_file($url, $params);
    }
    
    public function curl_file($url, $params)
    {
        $ch = curl_init($url);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $params);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false);
        curl_setopt($ch, CURLOPT_TIMEOUT, 10);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        $result = curl_exec($ch);
        echo $result.PHP_EOL;
        
        $error = '';
        if(curl_errno($ch) > 0){
            $error = 'errno:'.curl_errno($ch).' error:'.curl_error($ch);
            echo $error;
        }
        
        $info = array();
        $info['url'] = curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);
        $info['http_code'] = curl_getinfo($ch, CURLINFO_HTTP_CODE);
        $info['total_time'] = curl_getinfo($ch, CURLINFO_TOTAL_TIME);
        $info['namelookup_time'] = curl_getinfo($ch, CURLINFO_NAMELOOKUP_TIME);
        $info['connect_time'] = curl_getinfo($ch, CURLINFO_CONNECT_TIME);
        print_r($info);
        
        curl_close($ch);
        return json_decode($result, true);
    }
}

$faceid = new Faceid();
echo "<br>---------------------------------------<br>";
$faceid->verify();
echo "<br>---------------------------------------<br>";
$result = $faceid->detect();
$face_token = $result['faces'][0]['token'];
echo "<br>---------------------------------------<br>";
$faceid->verify($face_token);
echo "<br>---------------------------------------<br>";
$faceid->ocridcard();