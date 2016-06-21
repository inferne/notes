<?php

$config = array(
    array('ip'=>'127.0.0.1', 'port'=>6380, 'timeout'=>0.5),
    array('ip'=>'127.0.0.1', 'port'=>6381, 'timeout'=>0.5),
    array('ip'=>'127.0.0.1', 'port'=>6382, 'timeout'=>0.5),
    array('ip'=>'127.0.0.1', 'port'=>6383, 'timeout'=>0.5)
);

$server = new swoole_server('0.0.0.0', 6379);

$server->set(array(
    'worker_num' => 20,
    'daemonize' => true,
//    'log_file' => 'log/swoole.log',
//    'heartbeat_check_interval' => 60,
//    'heartbeat_idle_time' => 1800
));

//连接事件
$server->on('connect', function($serv, $fd){
    echo "Client:Connect.\n";
});

//接收到数据
$server->on('receive', function($serv, $fd, $from_id, $data) use ($config){
    echo "\r\n---------------------------------------------------------------\r\n".$data;
    if( strpos($data, "QUIT") === false ){
        $arr_data = explode("\r\n", $data);
        //print_r($arr_data);
        $k = crc32($arr_data[4])%count($config);
        $server = $config[$k];
        
        $client = new swoole_client(SWOOLE_SOCK_TCP, SWOOLE_SOCK_ASYNC);
        
        $client->on("connect", function($cli) use ($data){
            echo "cli send {$data}\n";
            $cli->send($data);
        });
        
        $client->on("receive", function($cli, $data) use ($serv, $fd){
            echo "cli receive {$data}\n";
            echo "serv send {$data}\n";
            $serv->send($fd, $data);
            $cli->close();
            //$serv->close($fd);
        });
        
        $client->on("close", function($cli){
            echo "cli close\n";
        });
        
        $client->on("error", function($cli){
            echo "cli Connect failed\n";
        });
        
        echo "cli connect ".json_encode($server).PHP_EOL;
        $client->connect($server['ip'], $server['port'], $server['timeout']);
    }else{
        $serv->close($fd);
    }
});

//关闭
$server->on('close', function($serv, $fd){
    echo "Client:Close.\n";
});

$server->start();
