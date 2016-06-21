<?php 
/**
 * 进程通信-信号
 */

//设置信号处理函数
function sig_handler($signo){
    echo "$signo process ".posix_getpid()." will exit".PHP_EOL;
    exit();
}

//创建5个子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        pcntl_signal(SIGTERM, 'sig_handler');//安装信号处理器
        while (1){
            pcntl_signal_dispatch();//调用等待信号的处理器
            echo 'my pid '.posix_getpid().PHP_EOL;
            sleep(1);
        }
    }
}

sleep(1);

//通过信号停止所有子进程
foreach ($arr_pid as $pid){
    $result = posix_kill($pid, SIGTERM);//发送SIGTERM信号
    if($result){
        $pid = pcntl_wait($status);//等待回收
        echo "recover child process $pid".PHP_EOL;
        unset($arr_pid[$pid]);
    }
}

echo posix_getpid()." end".PHP_EOL;
