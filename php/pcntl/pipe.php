<?php 
/**
 * 进程通信-管道 
 * 管道分为pipe（匿名管道）和fifo（有名管道）
 */

//创建子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        $pid = posix_getpid();
        //命名管道
        $fifo_path = "/dev/shm/pcntl_fifo_".$pid;
        if( !file_exists($fifo_path) ){
            //创建管道
            posix_mkfifo($fifo_path, 0600);
            echo "process mkfifo $fifo_path".PHP_EOL;
        }
        //打开管道
        $fifo = fopen($fifo_path, "r");
        
        while (1){
            //读取管道数据
            $data = fread($fifo, 1024);
            if($data == "exit"){//读到exit则删除管道并退出
                echo "process ".$pid." read $data".PHP_EOL;
                //关闭管道
                fclose($fifo);
                //删除管道
                unlink($fifo_path);
                exit(0);
            }
            sleep(1);
        }
    }
}

sleep(1);

//利用管道回收子进程
foreach ($arr_pid as $pid){
    $fifo_path = "/dev/shm/pcntl_fifo_".$pid;
    //打开管道
    $fifo = fopen($fifo_path, "w");
    //向管道写入数据
    fwrite($fifo, "exit");
    //关闭管道
    fclose($fifo);
    $pid = pcntl_wait($status);
    echo "recover child process $pid".PHP_EOL;
    unset($arr_pid[$pid]);
    sleep(1);
}

echo posix_getpid()." end".PHP_EOL;
