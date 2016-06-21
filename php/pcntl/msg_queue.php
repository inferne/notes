<?php 
/**
 * 进程通信-消息队列 
 */

$key = 1234;
$queue = msg_get_queue($key, 0666);

//msg_set_queue($queue, array ('msg_perm.uid'=>'80'));

//$stat = msg_stat_queue($queue);
//print_r($stat);exit();

//创建子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        $pid = posix_getpid();
        while (1){
            //只接收msgtype=$pid的消息
            if(msg_receive($queue, $pid, $msgtype, 1024, $message)){
                echo "my pid $pid receive $message ".strlen($message).PHP_EOL;
                if(trim($message) == "exit($msgtype)"){
                    echo "process $pid exit!".PHP_EOL;
                    exit();
                }
            }
            echo "$pid sleep 1";
            sleep(1);
        }
    }
}

sleep(1);//睡一觉（进程同步）

//回收子进程
foreach ($arr_pid as $pid){
    //发送消息类型为$pid的message
    if(msg_send($queue, $pid, "exit($pid)")){
        $pid = pcntl_wait($status);
        echo "recover child process $pid".PHP_EOL;
        unset($arr_pid[$pid]);
    }
}

msg_remove_queue($queue);//destory a message queue

echo posix_getpid()." end".PHP_EOL;

// my pid 4906 receive exit(4906) 10
// process 4906 exit!
// recover child process 4906
// my pid 4907 receive exit(4907) 10
// process 4907 exit!
// recover child process 4907
// my pid 4908 receive exit(4908) 10
// process 4908 exit!
// recover child process 4908
// my pid 4909 receive exit(4909) 10
// process 4909 exit!
// recover child process 4909
// my pid 4910 receive exit(4910) 10
// process 4910 exit!
// recover child process 4910
// 4905 end
