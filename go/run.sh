#!/usr/bin

path=`pwd`
pid_file="${path##*/}.pid"

pid=`cat $pid_file`
if [[ $pid != "" ]]; then
	exit 0
fi
echo $$ > $pid_file

exec_file="bin/${path##*/}"
echo $exec_file

log_dir="/tmp/${path##*/}"
mkdir -p $log_dir
ln -s $log_dir "logs"
log_file="logs/request.log"
last_time="0"

while :
do
	fpid=`pgrep -f $exec_file`
	echo $fpid
	modify_time=`stat -t $exec_file|cut -d ' ' -f 13`
	if [[ $fpid == ""  ||  $last_time != $modify_time ]]; then
		if [[ $fpid != "" ]]; then
			echo "kill $fpid"
			kill $fpid
		fi
		echo "$exec_file >> $log_file &"
		$exec_file >> $log_file &
		last_time=$modify_time
	fi  
	sleep 1
done
