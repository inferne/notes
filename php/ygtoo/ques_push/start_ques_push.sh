#!/bin/bash

source ~/.bash_profile

cd `dirname $0`

while((1))
do
    eval `cat ../data.conf | grep -v '#'`
    log_file="$log_dir/"`date +%Y`
    mkdir -p -m 755 $log_file
    log_file="$log_file/ques"`date +%m%d`
    PN=`ps -ef | grep "php ques_push.php" | grep -v "grep" | grep -v "vi" | awk "BEGIN {a=0} {a++} END {print a}"`
    if [ "$PN" -lt "$ques_process" ]
    then
        php ques_push.php $ques_push_file $ques_threads $ques_max_task $ques_threads_timeout &>> $log_file &
    fi
    sleep 1
done

wait
