<?php

class Server
{
    public $serv;
    
    public $onReceive;
    
    public function __construct(){
        /**
         * 创建服务器
         * @var Ambiguous $this->serv
         */
        $this->serv = stream_socket_server("tcp://0.0.0.0:1234", $errno, $errstr);
        
        if (!$this->serv) {
            echo "$errstr ($errno)<br />\n";
        }
    }
    
    public function accept()
    {
        while ( ($cli = stream_socket_accept($this->serv)) > 0 ) {
            //stream_set_blocking($cli, 0);
            $request = '';
            while ( 1 ) {
                echo "$cli get remote name ".stream_socket_get_name($cli, 1)."\n";
                echo "$cli get local name ".stream_socket_get_name($cli, 0)."\n";
                $request .= stream_socket_recvfrom($cli, 1500);
                if(Http::check($request)){
                    break;
                }
                if (!$request) {
                    sleep(1);
                }
            }
            $response = call_user_func($this->onReceive, $cli, $request);
            stream_socket_sendto($cli, $response);
            stream_socket_shutdown($cli, STREAM_SHUT_RDWR);
            //echo "$cli exit!";
        }
    }
    
    public function close(){
        stream_socket_shutdown($this->serv, STREAM_SHUT_RDWR);
    }
}

