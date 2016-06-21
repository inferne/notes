1 ques_push目录是用pthreads写的多线程推送程序<br/>

2 SwooleProcess是一个封装的swoole_process类<br/>
示例:<br/>
//初始化类<br/>
$swoole_process = new SwooleProcess();<br/>
//调用多进程方法<br/>
$swoole_process->robot($api_conduct, $api_answer, $value);<br/>
//回收所有进程<br/>
$swoole_process->wait_quit_all();<br/>

2 redis_server.php是用swoole写的一个分布式redis服务，不过性能比较差，还需改进<br/>
分布式redis服务测试<br/>
redis-benchmark -p 6379 -t set -n 100000 -q<br/>
SET: 5628.73 requests per second<br/>

redis单机测试<br/>
redis-benchmark -p 6380 -t set -n 100000 -q<br/>
SET: 87873.46 requests per second<br/>

性能分布式性能为单机的1/15，太菜了我<br/>
