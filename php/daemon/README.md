# 多进程类

依赖拓展<br/>
pcntl<br/>
posix<br/>
sysvmsg<br/>

进程间使用sysvmsg进行通讯<br/>
## Worker.php
1. 支持的参数
   * -c <number>    worker number
   * -d             daemonize
   * --stop         stop all worker
   * --help         help
2. 支持设置的属性
   * worker_num
      * worker process number from 0 to 99999
   * daemonize
      * bool true or false
   * onReceive
      * when your child receive message execute this function
   * onStart
      * when your child start execute this function
   * log_file
      * define log filename
   * max_size
      * define a message max size(byte)
   * block
      * bool true or false,set send message and receive message block
3. 支持的方法
   * set()
      * set your config valid
   * run()
      * run worker and create child process
   * send()
      * worker send message to child process
   * stop()
      * worker recyce child process

## demo1

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
## demo2

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
