<?php 
/**
 * 进程通讯-共享内存 
 */

$shmop_key = ftok(__FILE__, 't');

/**
 * 创建计数器
*/
function create_counter($shmop_key){
    $shmid = @shmop_open($shmop_key, 'c', 0644, 20);
    if(!$shmid){
        echo json_encode(error_get_last())."\n";
        exit();
    }
    
    echo "init:".shmop_read($shmid, 0, 20)."\n";
    shmop_write($shmid, 0, 0);
    shmop_close($shmid);
}

/**
 * 计数器加一
 */
function counter($shmop_key, $start){
    $shmid = shmop_open($shmop_key, "c", 0644, 20);
    //echo $shmid."\n";
    $count = shmop_read($shmid, $start, 4);
    $count += 1;
    shmop_write($shmid, $count, $start);
    echo $count."\n";
    shmop_close($shmid);
}

/**
 * 等待计数完成
 * @param unknown $count
 * @return boolean
 */
function wait_count_over($shmop_key, $count){
    while (1){
        $shmid = shmop_open($shmop_key, "c", 0644, 20);
        $string = shmop_read($shmid, 0, 20)."\n";
        $arr_count = str_split($string, 4);
        if(array_sum($arr_count) >= $count){
            shmop_write($shmid, 0, 0);
            break;
        }
        sleep(1);
    }
    return true;
}

function remove_counter($shmop_key){
    $shmid = shmop_open($shmop_key, "w", 0644, 20);
    shmop_delete($shmid);
    shmop_close($shmid);
}

create_counter($shmop_key);

//创建子进程
$arr_pid = array();
for ($i = 1; $i <= 5; $i++){
    $pid = pcntl_fork();
    if($pid){
        $arr_pid[$pid] = $pid;
    }else{
        $pid = posix_getpid();
        $n = 4;
        while ($n){
            counter($shmop_key, ($i-1)*4);
            $n--;
        }
        echo "process $pid exit!".PHP_EOL;
        exit(0);
    }
}

sleep(1);

wait_count_over($shmop_key, 20);
remove_counter($shmop_key);

//回收子进程
while ($arr_pid){
    $pid = pcntl_wait($status);
    echo "recover child process $pid".PHP_EOL;
    unset($arr_pid[$pid]);
}

echo posix_getpid()." end".PHP_EOL;
