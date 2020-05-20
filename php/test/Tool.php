<?php

class OldTool
{/*{{{*/

     const ID_CRYPT_KEY = 'asfdhpiadaoasjfpowjhp';
     const ID_CRYPT_CHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

    public static function htmlEncode($content)
    {/*{{{*/
        if (empty($content)) {
            return '';
        }

        return htmlspecialchars($content, ENT_QUOTES);
    }/*}}}*/

    public static function pullIndex(array $arr, $key, $vk = '')                                 
    {/*{{{*/                                                                               
        $ret = array();                                                                    
        array_walk($arr, function($info) use (&$ret, $key, $vk) {                          
            if(isset($info[$key])) {                                                       
                $ret[$info[$key]] = strlen($vk) && isset($info[$vk]) ? $info[$vk] : $info; 
            }                                                                              
        });                                                                                
        return $ret;                                                                       
    }/*}}}*/

   /**
     * 替换规则
     * (http:)//s(0~9).qhres.com/static/ => https://s(0~1).ssl.qhres.com/ssl/
     * (http:)//p(0~9).qhimg.com/ => https://p(0~1).ssl.qhimg.com/
     * (http:)//i(0~9).qhimg.com/ => https://i.ssl.qhimg.com/
     * 
     * 原静床地址：(http:)//s(0~9).qhres.com 其中http:可能省略
     * 静床css和js静床支持内部内容替换：
     * 普通https地址：https://s.ssl.qhres.com/static/5139cb917395c769/xxx.js
     * 支持内部替换的https地址：https://s.ssl.qhres.com/ssl/5139cb917395c769/xxx.js
     */
    public static function convertHttpToHttps($content)
    {/*{{{*/
        $pattern = '#(http:)?(//|\\\/\\\/)([psi])(\d{1,2})\.(qhimg|qhres|qhmsg)\.com(/|\\\/)?(static)?#';
        $result  = preg_replace_callback($pattern,
            function($matches) {
                $no = $matches[3] == "i" ? '' : $matches[4] % 2;
                return 'https:'.$matches[2].$matches[3].$no.'.ssl.'.$matches[5].'.com'.(isset($matches[6]) ? $matches[6] : '').(isset($matches[7]) ? 'ssl' : '');
            },$content);
        return $result;
    }/*}}}*/ 

    //以下两个函数对ID进行加解密：只支持数字的加密已测试 0-10000000有效
    public static function idEncode($txt)
    {/*{{{*/
        if (empty($txt) || !is_numeric($txt)) {
            return '';
        }

        $arrayDigit  = array(0,1,2,3,4,5,6,7,8,9);
        $arrayLetter = array('a','b','c','d','e','f','g','h','i','j');
        $len      = strlen($txt);
        if ($len < 10) {
            $buquan = substr(md5($txt.self::ID_CRYPT_KEY), 0, (10-$len));
            $buquan = str_replace($arrayDigit,$arrayLetter,$buquan);
        } else {
            $buquan = '';
        }
        $txt   = $buquan.$txt;
        $nh    = 11;
        $ch    = self::ID_CRYPT_CHARS[$nh];
        $mdKey = md5(self::ID_CRYPT_KEY.$ch);
        $mdKey = substr($mdKey,$nh%8, $nh%8+7);

        $txt = str_replace('=','',base64_encode($txt));
        $tmp = '';
        $i   = 0;
        $j   = 0;
        $k   = 0;
        for ($i=0; $i<strlen($txt); $i++) {
            $k = $k == strlen($mdKey) ? 0 : $k;
            $j = ($nh+strpos(self::ID_CRYPT_CHARS,$txt[$i])+ord($mdKey[$k++]))%61;
            $tmp .= self::ID_CRYPT_CHARS[$j];
        }
        return $tmp;
    }/*}}}*/

    public static function idDecode($txt)
    {/*{{{*/
        if (empty($txt)) {
            return 0;
        }

        $tTxt  = $txt;
        $txt   = 'L'.$txt;
        $ch    = $txt[0];
        $nh    = strpos(self::ID_CRYPT_CHARS,$ch);
        $mdKey = md5(self::ID_CRYPT_KEY.$ch);
        $mdKey = substr($mdKey,$nh%8, $nh%8+7);
        $txt   = substr($txt,1);
        $tmp   = '';
        $i     = 0;
        $j     = 0;
        $k     = 0;
        for ($i=0; $i<strlen($txt); $i++) {
            $k = $k == strlen($mdKey) ? 0 : $k;
            $j = strpos(self::ID_CRYPT_CHARS,$txt[$i])-$nh - ord($mdKey[$k++]);
            while ($j<0) $j+=61;
            $tmp .= self::ID_CRYPT_CHARS[$j];
        }
        $code =  intval(preg_replace('/[a-z]/i','',base64_decode($tmp)));
        if (self::idEncode($code) == $tTxt ) {
            return $code;
        }
        return 0;
    }/*}}}*/

    public static function verifyToken($token)
    {
        $str = \Pumpkin\Lib\AES::decode($token);
        parse_str($str, $params);
        return isset($params['time']) && (time() - $params['time'] < 300);
    }

    public static function createToken($info = [])
    {
        $info['time'] = time();
        ksort($info);
        $str = http_build_query($info);
        return \Pumpkin\Lib\AES::encode($str);
    }

     /**
     * 导出excel(csv)
     * @data 导出数据
     * @headlist 第一行,列名
     * @fileName 输出Excel文件名
     */
    public static function csvExport($data = array(), $headlist = array(), $fileName) {
      
        header('Content-Type: application/vnd.ms-excel');
        header('Content-Disposition: attachment;filename="'.$fileName.'.csv"');
        header('Cache-Control: max-age=0');
      
        //打开PHP文件句柄,php://output 表示直接输出到浏览器
        $fp = fopen('php://output', 'a');
        
        //输出Excel列名信息
        foreach ($headlist as $key => $value) {
            //CSV的Excel支持GBK编码，一定要转换，否则乱码
            $headlist[$key] = iconv('utf-8', 'gbk', $value);
        }
      
        //将数据通过fputcsv写到文件句柄
        fputcsv($fp, $headlist);
        
        //计数器
        $num = 0;
        
        //每隔$limit行，刷新一下输出buffer，不要太大，也不要太小
        $limit = 100000;
        
        //逐行取出数据，不浪费内存
        $count = count($data);
        for ($i = 0; $i < $count; $i++) {
        
            $num++;
            
            //刷新一下输出buffer，防止由于数据过多造成问题
            if ($limit == $num) { 
                ob_flush();
                flush();
                $num = 0;
            }
            
            $row = $data[$i];
            foreach ($row as $key => $value) {
                $row[$key] = iconv('utf-8', 'gbk', $value);
            }

            fputcsv($fp, $row);
        }
    }

}/*}}}*/


class NewTool
{/*{{{*/

     const ID_CRYPT_KEY = 'asfdhpiadaoasjfpowjhp';
     const ID_CRYPT_CHARS = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

    public static function htmlEncode($content)
    {/*{{{*/
        if (empty($content)) {
            return '';
        }

        return htmlspecialchars($content, ENT_QUOTES);
    }/*}}}*/

    public static function pullIndex(array $arr, $key, $vk = '')                                 
    {/*{{{*/                                                                               
        $ret = array();                                                                    
        array_walk($arr, function($info) use (&$ret, $key, $vk) {                          
            if(isset($info[$key])) {                                                       
                $ret[$info[$key]] = strlen($vk) && isset($info[$vk]) ? $info[$vk] : $info; 
            }                                                                              
        });                                                                                
        return $ret;                                                                       
    }/*}}}*/

   /**
     * 替换规则
     * (http:)//s(0~9).qhres.com/static/ => https://s(0~1).ssl.qhres.com/ssl/
     * (http:)//p(0~9).qhimg.com/ => https://p(0~1).ssl.qhimg.com/
     * (http:)//i(0~9).qhimg.com/ => https://i.ssl.qhimg.com/
     * 
     * 原静床地址：(http:)//s(0~9).qhres.com 其中http:可能省略
     * 静床css和js静床支持内部内容替换：
     * 普通https地址：https://s.ssl.qhres.com/static/5139cb917395c769/xxx.js
     * 支持内部替换的https地址：https://s.ssl.qhres.com/ssl/5139cb917395c769/xxx.js
     */
    public static function convertHttpToHttps($content)
    {/*{{{*/
        $pattern = '#(http:)?(//|\\\/\\\/)([psi])(\d{1,2})\.(qhimg|qhres|qhmsg)\.com(/|\\\/)?(static)?#';
        $result  = preg_replace_callback($pattern,
            function($matches) {
                $no = $matches[3] == "i" ? '' : $matches[4] % 2;
                return 'https:'.$matches[2].$matches[3].$no.'.ssl.'.$matches[5].'.com'.(isset($matches[6]) ? $matches[6] : '').(isset($matches[7]) ? 'ssl' : '');
            },$content);
        return $result;
    }/*}}}*/ 

    public static $ord_crypt_chars;

    public static $mdKey;
    public static $ord_mdKey;

    //以下两个函数对ID进行加解密：只支持数字的加密已测试 0-10000000有效
    public static function idEncode($txt)
    {/*{{{*/
        if (empty($txt) || !is_numeric($txt)) {
            return '';
        }

        $arrayDigit  = array(0,1,2,3,4,5,6,7,8,9);
        $arrayLetter = array('a','b','c','d','e','f','g','h','i','j');
        $len      = strlen($txt);
        if ($len < 10) {
            $buquan = substr(md5($txt.self::ID_CRYPT_KEY), 0, (10-$len));
            $buquan = str_replace($arrayDigit,$arrayLetter,$buquan);
        } else {
            $buquan = '';
        }
        $txt   = $buquan.$txt;
        $nh    = 11;
        $ch    = self::ID_CRYPT_CHARS[$nh];
        if (!self::$mdKey) {
            $mdKey = md5(self::ID_CRYPT_KEY.$ch);
            self::$mdKey = substr($mdKey,$nh%8, $nh%8+7);
        }
        if (!self::$ord_crypt_chars) {
            for ($i=0; $i < strlen(self::ID_CRYPT_CHARS); $i++) { 
                self::$ord_crypt_chars[self::ID_CRYPT_CHARS[$i]] = ord(self::ID_CRYPT_CHARS[$i]) >= 65 ? ord(self::ID_CRYPT_CHARS[$i]) - 65 : ord(self::ID_CRYPT_CHARS[$i]) + 10;
            }
        }

        $txt = str_replace('=','',base64_encode($txt));
        $tmp = '';
        $i   = 0;
        $j   = 0;
        $k   = 0;
        $txtLen = strlen($txt);
        $mdkeyLen = strlen(self::$mdKey);
        for ($i=0; $i<$txtLen; $i++) {
            $k = $k%$mdkeyLen;
            //echo $txt[$i]." ".ord($txt[$i])." ".(ord($txt[$i]) - 65)." ".strpos(self::ID_CRYPT_CHARS,$txt[$i])."\n";
            //$j = ($nh+self::$ord_crypt_chars[$txt[$i]]+self::$ord_crypt_chars[self::$mdKey[$k++]])%61;
            $j = ($nh+strpos(self::ID_CRYPT_CHARS,$txt[$i])+ord(self::$mdKey[$k++]))%61;
            $tmp .= self::ID_CRYPT_CHARS[$j];
        }
        return $tmp;
    }/*}}}*/

    public static function idDecode($txt)
    {/*{{{*/
        if (empty($txt)) {
            return 0;
        }

        $tTxt  = $txt;
        $txt   = 'L'.$txt;
        $ch    = $txt[0];
        $nh    = strpos(self::ID_CRYPT_CHARS,$ch);
        $mdKey = md5(self::ID_CRYPT_KEY.$ch);
        $mdKey = substr($mdKey,$nh%8, $nh%8+7);
        $txt   = substr($txt,1);
        $tmp   = '';
        $i     = 0;
        $j     = 0;
        $k     = 0;
        for ($i=0; $i<strlen($txt); $i++) {
            $k = $k == strlen($mdKey) ? 0 : $k;
            $j = strpos(self::ID_CRYPT_CHARS,$txt[$i])-$nh - ord($mdKey[$k++]);
            while ($j<0) $j+=61;
            $tmp .= self::ID_CRYPT_CHARS[$j];
        }
        $code =  intval(preg_replace('/[a-z]/i','',base64_decode($tmp)));
        if (self::idEncode($code) == $tTxt ) {
            return $code;
        }
        return 0;
    }/*}}}*/

    public static function verifyToken($token)
    {
        $str = \Pumpkin\Lib\AES::decode($token);
        parse_str($str, $params);
        return isset($params['time']) && (time() - $params['time'] < 300);
    }

    public static function createToken($info = [])
    {
        $info['time'] = time();
        ksort($info);
        $str = http_build_query($info);
        return \Pumpkin\Lib\AES::encode($str);
    }

     /**
     * 导出excel(csv)
     * @data 导出数据
     * @headlist 第一行,列名
     * @fileName 输出Excel文件名
     */
    public static function csvExport($data = array(), $headlist = array(), $fileName) {
      
        header('Content-Type: application/vnd.ms-excel');
        header('Content-Disposition: attachment;filename="'.$fileName.'.csv"');
        header('Cache-Control: max-age=0');
      
        //打开PHP文件句柄,php://output 表示直接输出到浏览器
        $fp = fopen('php://output', 'a');
        
        //输出Excel列名信息
        foreach ($headlist as $key => $value) {
            //CSV的Excel支持GBK编码，一定要转换，否则乱码
            $headlist[$key] = iconv('utf-8', 'gbk', $value);
        }
      
        //将数据通过fputcsv写到文件句柄
        fputcsv($fp, $headlist);
        
        //计数器
        $num = 0;
        
        //每隔$limit行，刷新一下输出buffer，不要太大，也不要太小
        $limit = 100000;
        
        //逐行取出数据，不浪费内存
        $count = count($data);
        for ($i = 0; $i < $count; $i++) {
        
            $num++;
            
            //刷新一下输出buffer，防止由于数据过多造成问题
            if ($limit == $num) { 
                ob_flush();
                flush();
                $num = 0;
            }
            
            $row = $data[$i];
            foreach ($row as $key => $value) {
                $row[$key] = iconv('utf-8', 'gbk', $value);
            }

            fputcsv($fp, $row);
        }
    }

}/*}}}*/

function create_ascii_table()
{
    $ascii_table = [];
    for ($i=32; $i < 127; $i++) { 
        $ascii_table[chr($i)] = $i;
    }

    return $ascii_table;
}

$key = 13131313;

function encode($int)
{
    return $int ^ $key;
}

$txt = "368756435";
$n = 10000;
$time1 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    OldTool::idEncode($txt);
}
$time2 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    NewTool::idEncode($txt);
}
$time3 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    encode($txt);
}
$time4 = microtime(true);

echo "old encode $n time ".($time2 - $time1)."\n";
echo "new encode $n time ".($time3 - $time2)."\n";
echo "simple encode $n time ".($time4 - $time3)."\n";

$n = 10000;
$variable = range(0, $n);
foreach ($variable as $key => $value) {
    $variable[$key] = ord($value);
}

$time1 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    ord($i);
}
$time2 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    $variable[$i];
}
$time3 = microtime(true);

echo "old encode $n time ".($time2 - $time1)."\n";
echo "new encode $n time ".($time3 - $time2)."\n";

echo ord('A')." ".ord('A' | 'B')."\n";

$yac = new Yac();
$time1 = microtime(true);
for ($i=0; $i < $n; $i++) {
    $yac->set("foo", "bar");
}
$time2 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    $yac->get("foo");
}
$time3 = microtime(true);

echo "yac set $n time ".($time2 - $time1)."\n";
echo "yac get $n time ".($time3 - $time2)."\n";