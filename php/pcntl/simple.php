<?php 

//创建子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        echo 'my pid '.posix_getpid().PHP_EOL;
        sleep(1);
        exit();
    }
}

//回收子进程
while ($arr_pid){
    $pid = pcntl_wait($status);
    echo "recover child process $pid".PHP_EOL;
    unset($arr_pid[$pid]);
}

echo posix_getpid()." end".PHP_EOL;
