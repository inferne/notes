<?php 

/**
 * 注册类
 * @author liyunfei
 *
 */
define("ROOT_PATH", __DIR__);

class Autoload
{
    public static function init($class){
        if(strpos("\\", $class) === 0){
            $class = str_replace("\\", "//", $class);
            if(is_file(ROOT_PATH.$class.".php")){
                require_once ROOT_PATH.$class.".php";
            }
        }else{
            if(is_file($class.".php")){
                require_once $class.".php";
            }
        }
    }
}