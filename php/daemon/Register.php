<?php 

/**
 * 注册类
 * @author liyunfei
 *
 */

class Register
{
    private static $array;
    
    public static function set($config, $value){
        $string = json_encode($config);
        return self::$array[md5($string)] = $value;
    }
    
    public static function get($config){
        $string = json_encode($config);
        if(!isset(self::$array[md5($string)])){
            return false;
        }else{
            return self::$array[md5($string)];
        }
    }
}