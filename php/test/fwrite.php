<?php

$fp1 = fopen("aaa", 'a');

$fp2 = fopen("aaa", 'a');

flock($fp1, LOCK_EX);
fwrite($fp1, "hello a!\n");
flock($fp1, LOCK_UN);

flock($fp2, LOCK_EX);
fwrite($fp2, "hello b!\n");
flock($fp2, LOCK_UN);
fclose($fp2);

$txt = "368756435";
$n = 1000;
$time1 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    flock($fp1, LOCK_EX);
	fwrite($fp1, "hello aaa!\n");
	flock($fp1, LOCK_UN);
}
fclose($fp1);
$time2 = microtime(true);
for ($i=0; $i < $n; $i++) { 
    file_put_contents("bbb", "hello bbb!\n", FILE_APPEND | LOCK_EX);
}
$time3 = microtime(true);

echo "old encode $n time ".($time2 - $time1)."\n";
echo "new encode $n time ".($time3 - $time2)."\n";