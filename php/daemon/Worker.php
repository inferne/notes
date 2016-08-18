<?php 
/**
 * worker
 * @author liyunfei
 *
 */
 
class Worker
{
    //worker process number
    public $worker_num = 4;
    //if daemonize
    public $daemonize = false;
    //worker array
    public $arr_worker = array();
    //sysvmsg queue
    public $queue;

    public $onRecive;
    
    public $onStart;
    //log file
    public $log_file;
    //max message size
    public $max_size = 4096;
    
    public function __construct() {
        $this->init();
    }
    
    public function init(){
        $key = ftok(__FILE__, 'R');
        if(msg_queue_exists($key)){
            $this->queue = msg_get_queue($key, 0666);
            msg_remove_queue($this->queue);
        }
        $this->queue = msg_get_queue($key, 0666);
        //$this->log_file = 'Worker.log';
    }
    
    /**
     * 设置
     * @param unknown $config
     */
    public function set($config){
        foreach ($config as $key => $value){
            $this->{$key} = $value;
        }
    }
    
    /**
     * 运行
     */
    public function run(){
        $this->options();//parse options
        $this->daemonize();
        for ($i = 0; $i < $this->worker_num; $i++){
            $this->fork_child($i);
        }
        //$this->monitor();
        sleep(1);//睡一觉（进程同步）
    }

    /**
     * parse user options
     */
    public function options(){
        $short = "c:d::";
        $longopts = array('stop', 'help');
        $options = getopt($short, $longopts);
        if(isset($options['c']) && $options['c'] > 0){
            $this->worker_num = $options['c'];
        }
        if(isset($options['d'])){
            $this->daemonize = true;
        }
        if(isset($options['stop'])){
            global $argv;
            exec("kill `ps -ef|grep ".$argv[0]."|grep -v grep|grep -v vi|awk '{print $2}'`");
            exit();
        }
        if(isset($options['help'])){
            echo "if user set options and set config, options is first vaild!".PHP_EOL;
            echo "-c <number>    worker number".PHP_EOL;
            echo "-d             daemonize".PHP_EOL;
            echo "--stop         stop all worker".PHP_EOL;
            echo "--help         help".PHP_EOL;
            exit();
        }
    }
    
    /**
     * Run as deamon mode.
     * copy from workerman
     * @throws Exception
     */
    public function daemonize(){
        if(!$this->daemonize){
            return ;
        }
        umask(0);
        $pid = pcntl_fork();
        if($pid === -1){
            throw new Exception("fork error!");
        }else if($pid > 0){
            exit(0);
        }
        if (-1 === posix_setsid()) {
            throw new Exception("setsid fail");
        }
        // Fork again avoid SVR4 system regain the control of terminal.
        if($pid === -1){
            throw new Exception("fork error!");
        }else if($pid > 0){
            exit(0);
        }
    }
    /* 
    public function monitor(){
        while (1){
            $pid = pcntl_wait($status);
            $this->arr_worker[$pid];
            $this->fork_child();
        }
    }
     */
    
    /**
     * fork a child process
     * @throws Exception
     */
    public function fork_child($i){
        $pid = pcntl_fork();
        if( $pid > 0 ){
            $this->arr_worker[$i] = $pid;
        }else if($pid == 0){
            if($this->onStart){
                call_user_func($this->onStart, $this);
            }
            $this->loop();
        }else{
            throw new Exception("fork error!");
        }
    }
    
    /**
     * 轮询
     */
    public function loop(){
        $pid = posix_getpid();
        while (1){
            $msgtype = '';
            $message = '';
            $errcode = 0;
            //只接收msgtype=$pid的消息
            if(msg_receive($this->queue, $pid, $msgtype, $this->max_size, $message, false, 0, $errcode)){
                //$this->log("receive $message strlen[".strlen($message).'] msgtype:'.$msgtype);
                if(trim($message) == "exit()"){
                    $this->log("process exit!");
                    exit();
                }else{
                    call_user_func($this->onRecive, $this, $message);
                }
            }
            if($errcode > 0){
                $this->log("receive errcode:".$errcode);
                sleep(1);
            }
            usleep(100);
        }
    }
    
    public $i = 0;
    
    /**
     * 发送数据
     * @param string $message
     * @param int    $id
     */
    public function send($message, $id = -1){
        //if($id == -1){
        //    $i = sprintf("%u", crc32($message)) % $this->worker_num;
        //}else{
        //    $i = $id % $this->worker_num;
        //}
        $i = $this->i % $this->worker_num;
        $this->i++;
        $j = 100;//最大重试次数
        do{
            //$this->log("send ".$message);
            $errcode = 0;
            $result = @msg_send($this->queue, $this->arr_worker[$i], $message, false, true, $errcode);
            //send failed retry
            if(!$result){
                $this->log("send errcode:".$errcode);
                sleep(1);
                $j--;
            }
        } while(!$result && $j);
        return $result;
    }

    /**
     * 停止
     */
    public function stop(){
        foreach ($this->arr_worker as $pid){
            //发送消息类型为$pid的message
            if(msg_send($this->queue, $pid, "exit()", false)){
                $pid = pcntl_wait($status);
                $this->log("recover child process $pid");
                unset($this->arr_worker[array_search($pid, $this->arr_worker)]);
            }
        }
        msg_remove_queue($this->queue);//destory a message queue
    }

    private $index = 0;
    
    /**
     * log
     * @param unknown $message
     */
    public function log($message){
        $message = date("Y-m-d H:i:s").' '.posix_getpid().' '.$message.PHP_EOL;
        if(!$this->daemonize || !$this->log_file){
            echo $message;
        }else{
            $filename = $this->log_file.sprintf("%04d", $this->index);
            $stat = stat($filename);
            //日志文件大小大于2G则更换文件
            while($stat['size'] >= 2 * 1024 * 1024 * 1024){
                $this->index++;
                $filename = $this->log_file.sprintf("%04d", $this->index);
                $stat = stat($filename);
            }
            error_log($message, 3, $filename);
        }
    }
}