<?php

$val = "hello world";
$path = "/tmp/ftok_1.key";

// Get the file token key
$key = ftok($path, 'a');
echo $key."\n";
$memsize = strlen($val) +1024; //php default 10000
// 将共享内存挂在当前内存

$shm_id = shm_attach($key, $memsize, 0777);
var_dump($shm_id);
if ($shm_id === false) {
    die('Unable to create the shared memory segment');
}

//判断是否有某个键
$has = shm_has_var($shm_id, 1);
var_dump($has);
if (!$has) {
    shm_put_var($shm_id, 1, $val);
}
//获取值
$ret = shm_get_var($shm_id, 1);
var_dump($ret);
//删除某个键
//$ret = shm_remove($shm_id);
//将共享内存从当前进程释放
$ret = shm_detach($shm_id);