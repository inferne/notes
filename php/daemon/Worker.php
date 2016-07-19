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
    //log file
    public $log_file;
    //max message size
    public $max_size = 1024;
    
    public function __construct() {
        $this->queue = msg_get_queue(ftok(__FILE__, 'R'), 0666);
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
            $pid = pcntl_fork();
            if( $pid > 0 ){
                $this->arr_worker[$i] = $pid;
            }else if($pid == 0){
                $this->loop();
            }else{
                throw new Exception("fork error!");
            }
        }
        sleep(1);//睡一觉（进程同步）
    }

    /**
     * parse user options
     */
    public function options(){
        $short = "c:d::";
        $longopts = array('help');
        $options = getopt($short, $longopts);
        if(isset($options['c']) && $options['c'] > 0){
            $this->worker_num = $options['c'];
        }
        if(isset($options['d'])){
            $this->daemonize = true;
        }
        if(isset($options['help'])){
            echo "if user set options and set config, options is first vaild!".PHP_EOL;
            echo "-c <number>    worker number".PHP_EOL;
            echo "-d             daemonize".PHP_EOL;
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
    
    /**
     * 轮询
     */
    public function loop(){
        $pid = posix_getpid();
        while (1){
            $msgtype = '';
            $message = '';
            //只接收msgtype=$pid的消息
            if(msg_receive($this->queue, $pid, $msgtype, $this->max_size, $message)){
                //$this->log("receive $message strlen[".strlen($message).']');
                if(trim($message) == "exit()"){
                    $this->log("process exit!");
                    exit();
                }else{
                    call_user_func($this->onRecive, $this, $message);
                }
            }
            usleep(100);
        }
    }
    
    /**
     * 发送数据
     * @param string $message
     * @param int    $id
     */
    public function send($message, $id = -1){
        if($id == -1){
            $i = sprintf("%u", crc32($message)) % $this->worker_num;
        }else{
            $i = $id % $this->worker_num;
        }
        msg_send($this->queue, $this->arr_worker[$i], $message);
    }

    /**
     * 停止
     */
    public function stop(){
        foreach ($this->arr_worker as $pid){
            //发送消息类型为$pid的message
            if(msg_send($this->queue, $pid, "exit()")){
                $pid = pcntl_wait($status);
                $this->log("recover child process $pid");
                unset($this->arr_worker[$pid]);
            }
        }
        msg_remove_queue($this->queue);//destory a message queue
    }

    /**
     * log
     * @param unknown $message
     */
    public function log($message){
        $message = date("Y-m-d H:i:s").' '.posix_getpid().' '.$message.PHP_EOL;
        if(!$this->daemonize || !$this->log_file){
            echo $message;
        }else{
            error_log($message, 3, $this->log_file);
        }
    }
}