<?php 
/**
 * 省略了信号处理
 * 每次启动得手动删除server.sock文件
 * @var unknown
 */
$server_sock = __DIR__."/server.sock";

// //server 1 TCP
function server_stream($server_sock){
    $socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
    
    //socket_set_option($socket, SOL_SOCKET, SO_REUSEADDR, 1);//没作用
    
    if(!socket_bind($socket, $server_sock)){
        echo socket_last_error();
    }
    if(!socket_listen($socket)){
        echo socket_last_error();
    }
    
    while (1){
        echo "Ready to receive...\n";
        if(($cli = socket_accept($socket)) > 0){
            $length = socket_recv($cli, $buf, 1024, 0);
            echo "Receive $buf from $cli\n";
            $buf = "Response $buf";
            if(socket_send($cli, $buf, 1024, 0) == -1){
                echo socket_last_error();
            }
        }
    }
    socket_close($socket);
}

//server 2 UDP
function server_dgraw($server_sock){
    $socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
    if(!socket_bind($socket, $server_sock)){
        echo socket_last_error();
    }
    
    //socket_set_block($socket);
    
    while(1){
        echo "Ready to receive...\n";
        if(socket_recvfrom($socket, $buf, 1024, 0, $from)){
            echo "Receive $buf from $from\n";
            $buf = "Response $buf";
            if(socket_sendto($socket, $buf, 1024, 0, $from) == -1){
                echo socket_last_error();
            }
        }
    }   
    socket_close($socket);
}

server_stream($server_sock);
//server_dgraw($server_sock);
