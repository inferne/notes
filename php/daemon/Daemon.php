<?php 
/**
 * daemon
 * @author liyunfei
 *
 */
 
include 'Autoload.php';
spl_autoload_register("Autoload::init");

class Daemon
{
    private $_worker;
    public $page     = 1;
    public $pagesize = 1000;
    
    /**
     * init
     */
    public function __construct(){
        $this->_worker = new Worker();
        $this->_worker->set(array(
            'worker_num' => 2,
            'daemonize'  => false,
        ));
        
        $this->_worker->onStart = function(Worker $fd){
            echo "child worker start";
        };
        
        $this->_worker->onRecive = function(Worker $fd, $message){
            //子进程业务逻辑
        };
        
        $this->_worker->run();
    }
    
    /**
     * work
     * @param unknown $config
     * @param unknown $table
     * @param unknown $arr_fields
     */
    public function work($config, $table, $arr_fields, $arr_where=array(), $func=null){
        try{
            //任务分发逻辑
        } catch (Exception $e){
            
        }
        $this->_worker->stop();
    }
}