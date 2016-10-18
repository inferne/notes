<?php 

$client_sock = __DIR__."/client.sock";
$server_sock = __DIR__."/server.sock";

//client 1 TCP
function client_stream($server_sock){
    $socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
    if(!socket_connect($socket, $server_sock)){
        echo socket_last_error($socket);
    }
    
    $buf = "hello kity";
    socket_send($socket, $buf, 1024, 0);
    if(!socket_recv($socket, $buf, 1024, 0)){
        echo socket_last_error($socket);
    }
    echo "Received $buf from $socket\n";
    socket_close($socket);
}

//client 2 UDP
function client_dgraw($server_sock, $client_sock){
    $socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
    if(!socket_bind($socket, $client_sock)){
        echo socket_last_error();
    }
    
    $buf = "hello kity";
    socket_sendto($socket, $buf, 1024, 0, $server_sock);
    socket_recvfrom($socket, $buf, 1024, 0, $from);
    echo "Received $buf from $from\n";
    
    socket_close($socket);
    unlink($client_sock);
}

client_stream($server_sock);
//client_dgraw($server_sock, $client_sock);

echo "Client exit!\n";
