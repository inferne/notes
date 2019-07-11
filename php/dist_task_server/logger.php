<?php

class Logger
{
    public static function debug($msg){
        if (is_array($msg)) {
            $msg = json_encode($msg);
        }
        $title = explode(" ", cli_get_process_title());
        $msg = date("Y-m-d H:i:s")." ".posix_getpid()." ".$title[1]." debug ".$msg."\n";
        echo $msg;
    }
    
    public static function info($msg){
        if (is_array($msg)) {
            $msg = json_encode($msg);
        }
        $title = explode(" ", cli_get_process_title());
        $msg = date("Y-m-d H:i:s")." ".posix_getpid()." ".$title[1]." info ".$msg."\n";
        echo $msg;
    }
    
    public static function error($msg){
        if (is_array($msg)) {
            $msg = json_encode($msg);
        }
        $title = explode(" ", cli_get_process_title());
        $msg = date("Y-m-d H:i:s")." ".posix_getpid()." ".$title[1]." error ".$msg."\n";
        echo $msg;
    }
}
