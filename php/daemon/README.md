# 多进程类Worker
==============

依赖拓展
pcntl
posix
sysvmsg
进程间使用sysvmsg进行通讯

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

$worker->onRecive = function(Worker $fd, $message){
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
假如你又一个消费脚本来消费队列里面的任务，此示例可以简单的把你的脚本变成多进程版，当前脚本无需任何修改
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