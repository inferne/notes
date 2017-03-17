<?php 

include 'Worker.php';

$worker = new Worker();

$worker->onReceive = function(Worker $fd, $message){
    //处理业务逻辑
    $fd->log("recive $message");
    //更新数据库
};

$worker->run();

$start_time = time();

for($i =0; $i < 100000; $i++){
    $worker->send(time().rand(10000, 99999));
}

$worker->stop();

$end_time = time();

echo "100000 use ".($start_time - $end_time)."s";
