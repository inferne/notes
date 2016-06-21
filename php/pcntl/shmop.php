<?php 
/**
 * 进程通讯-共享内存
 */

$key = 1234;
$size = 2048;
//创建共享内存
$shmid = shmop_open($key, "c", 0644, $size);
if(!$shmid)
    exit("shmop_open failed!");

//创建子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        $pid = posix_getpid();
        while (1){
            //读取共享内存
            $shmop_data = shmop_read($shmid, 0, $size);
            if($shmop_data){
                echo "my pid $pid read $shmop_data ".strlen($shmop_data).PHP_EOL;
                //读到exit时退出进程
                if(trim($shmop_data) == "exit") {
                    echo "process $pid exit!".PHP_EOL;
                    exit(0);
                }
            }
            sleep(1);
        }
    }
}

sleep(1);

//回收子进程
while ($arr_pid){
    //写入exit到共享内存
    $written = shmop_write($shmid, "exit", 0);
    if($written){
        $pid = pcntl_wait($status);
        echo "recover child process $pid".PHP_EOL;
        unset($arr_pid[$pid]);
    }else 
        sleep(1);
}

shmop_delete($shmid);
shmop_close($shmid);

echo posix_getpid()." end".PHP_EOL;
