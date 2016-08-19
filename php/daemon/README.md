# 多进程类<br/>
Worker.php
==============

依赖拓展<br/>
pcntl<br/>
posix<br/>
sysvmsg<br/>
进程间使用sysvmsg进行通讯<br/>
Worker.php
* worker_num-----------------worker process number
* daemonize------------------daemonize;
* onReceive------------------when your child receive message execute function
* onStart--------------------when your child start execute function
* log_file-------------------define log file
* max_size-------------------define one message max size
* block----------------------set send and receive block
* set()----------------------set config
* run()----------------------run worker and create child
* send()---------------------worker send message to child
* stop()---------------------worker recyce child

--------------------------
## 1 demo1
-------
假如你有一个队列，此示例适用于快速消费掉一个队列
```php
include 'Autoload.php';
spl_autoload_register("Autoload::load");

$worker = new Worker();
$worker->set(array(
    'worker_num' => 32,
    'daemonize'  => false,
    'max_size'   => 128,
));

$worker->onReceive = function(Worker $fd, $message){
    echo $message;
};
$worker->run();

foreach ($data as $key => $value){
    $result = $worker->send(json_encode($value));//发送到子进程
}

$worker->stop();
```
## 2 demo2
-------
假如你有一个消费脚本来消费队列里面的任务，此示例可以简单的把你的脚本变成多进程版，当前脚本无需任何修改
```php
include 'Autoload.php';
spl_autoload_register("Autoload::load");

$worker = new Worker();
$worker->set(array(
    'worker_num' => 32,
    'daemonize'  => false,
));

$worker->onStart = function(Worker $fd){
    include 'your_file.php';
};
$worker->run();

$worker->stop();
```
