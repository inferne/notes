#!/bin/bash

source ~/.bash_profile

PATH=/opt/service/php/bin:$PATH

cd `dirname $0`

PH=`pwd`

#check allocation start_ask_qsa
PN=`ps -ef|grep start_ques_push.sh|grep -v "#"|grep -v "vi"|grep -v "grep"|awk "BEGIN {a=0} {a++} END {print a}"`
if [ "$PN" -eq "0" ]
then
    chmod +x start_ques_push.sh
    nohup ./start_ques_push.sh &>/dev/null &
fi
