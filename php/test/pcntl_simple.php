<?php 

pcntl_signal(SIGCHLD, function(){
    $pid = pcntl_wait();
    echo "child process $pid exit".PHP_EOL;
});

$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
//         echo "wait child pid $pid".PHP_EOL;
//         pcntl_waitpid($pid, $status);
//         echo pcntl_wstopsig($status);
    }else{
        echo 'my pid '.posix_getpid().PHP_EOL;
        sleep(1);
        exit();
    }
}

while ($arr_pid){
    $pid = pcntl_wait($status);
    echo "recover child process $pid".PHP_EOL;
    unset($arr_pid[$pid]);
}

echo posix_getpid()." end".PHP_EOL;
