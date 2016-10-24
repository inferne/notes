<?php 

/**
 * add an event
 * @param unknown $sock
 * @param unknown $events
 * @param unknown $callback
 * @param string $base
 */
function event_base_add($sock, $events, $base, $callback)
{
    $event = event_new();
    // set event flags
    event_set($event, $sock, $events, $callback, $base);
    // set event base
    event_base_set($event, $base);
    // enable event
    event_add($event);
    // start event loop
    event_base_loop($base);
}

$server_sock = __DIR__."/server.sock";

//client 1 TCP
$socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
if(!socket_connect($socket, $server_sock)){
    echo socket_last_error($socket);
}

$base = event_base_new();
event_base_add($socket, EV_READ, $base, "read");

function read($socket, $flags, $base)
{
    static $max = 0;
    $max++;
    if(!socket_recv($socket, $buf, 1024, 0)){
        if(socket_last_error($socket) == 0){
            echo "Connection closed by server!\n";
        }else{
            echo socket_last_error($socket);
        }
    }else{
        echo "Received $buf from $socket\n";
    }
    
    $buf = "Hello kity! $max\n";
    socket_send($socket, $buf, 1024, 0);
    echo "Send to server $buf";
    if($max == 10){
        event_base_loopexit($base);
    }
}

socket_close($socket);
echo "Client exit!\n";
