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
        $write_list = [];
        
        //stream_set_blocking($this->serv, 0);
        $read_fds = [(int)$this->serv => $this->serv];
        $write_fds = [];
        $except_fds = [];
        while (1) {
            $read = $read_fds;
            $write = $write_fds;
            $except = $read_fds;
            $tv_sec = 60;
            echo "\n-------------------------------------------------------------------------------------\n";
            if ( stream_select($read, $write, $except, $tv_sec) > 0 ) {
                if ($read) {//有可读fd返回时
                    foreach ($read as $fd) {
                        if ($fd == $this->serv) {
                            $cli = stream_socket_accept($fd);
                            $read_fds[(int)$cli] = $cli;
                            echo "accept cli:$cli\n";
                            echo "add read fds $cli\n";
                        } else {
                            $request = '';
                            do {
                                $request .= stream_socket_recvfrom($fd, 1500);
                                if (!$request) {
                                    echo "$fd get remote name ".stream_socket_get_name($fd, 1)."\n";
                                    echo "$fd get local name ".stream_socket_get_name($fd, 0)."\n";
                                    sleep(1);
                                }
                            } while ($request && !Http::check($request));
                            //echo "receive $cli $request\n";
                            if ($request) {
                                $response = call_user_func($this->onReceive, $fd, $request);
                                stream_socket_sendto($fd, $response);
                                stream_socket_shutdown($fd, STREAM_SHUT_RDWR);
                            } else {
                                //读取不到数据的连接关闭掉
                                stream_socket_shutdown($fd, STREAM_SHUT_RDWR);
                            }
                            unset($read_fds[(int)$fd]);
                            echo "unset read fds $fd\n";
                            //$write[] = $fd;
                        }
                    }
                }
            }
        }
    }
    
    public function close(){
        stream_socket_shutdown($this->serv, STREAM_SHUT_RDWR);
    }
}

