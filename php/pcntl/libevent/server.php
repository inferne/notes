<?php 
/**
 * 省略了信号处理
 * 每次启动得手动删除server.sock文件
 * @var unknown
 */

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

// //server 1 TCP
$sock = socket_create(AF_UNIX, SOCK_STREAM, 0);

socket_set_option($sock, SOL_SOCKET, SO_REUSEADDR, 1);//没作用

if(!socket_bind($sock, $server_sock)){
    echo socket_last_error();
}
if(!socket_listen($sock)){
    echo socket_last_error();
}

$base = event_base_new();
event_base_add($sock, EV_READ | EV_REASIST, $base, "accept");

function accept($sock, $flag, $base)
{
    $cli = socket_accept($sock);
    event_base_add($cli, EV_READ, $base, "read");
}

function read($sock, $flag, $base)
{
    static $max_requests = 0;
    $max_requests++;
    $data = socket_read($sock, 1024, PHP_NORMAL_READ);
    var_dump($data);
    socket_write($sock, "Hello world! $sock\n", 1024);
    if($max_requests == 30){
        event_base_loopexit($base);
    }
}

// close the listening socket
socket_close($sock);
unlink($server_sock);
