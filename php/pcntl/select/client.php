<?php 

$server_sock = __DIR__."/server.sock";

//client 1 TCP
$socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
if(!socket_connect($socket, $server_sock)){
    echo socket_last_error($socket);
}

if(!socket_recv($socket, $buf, 1024, 0)){
    if(socket_last_error($socket) == 0){
        echo "Connection closed by server!\n";
    }else{
        echo socket_last_error($socket);
    }
}else{
    echo "Received $buf from $socket\n";
}

$buf = "Hello kity!\n";
socket_send($socket, $buf, 1024, 0);
echo "Send to server $buf";

if(!socket_recv($socket, $buf, 1024, 0)){
    if(socket_last_error($socket) == 0){
        echo "Connection closed by server!\n";
    }else{
        echo socket_last_error($socket);
    }
}else{
    echo "Received $buf from $socket\n";
}

socket_close($socket);
echo "Client exit!\n";
