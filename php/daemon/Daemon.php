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
    public $amount   = 256;
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
        $amount = $this->amount;
        $this->_worker->onRecive = function(Worker $fd, $message) use ($amount){
            //处理业务逻辑
            $fd->log("recive $message");
            //更新数据库
            $data = json_decode($message, true);
            $model = ucwords(str_replace("_", " ", $data['table']));
            $model = str_replace(" ", "", $model);
            unset($data['table']);
            //echo $model;
            if($model == "OrderTransactionHistory") $model = 'OrderSettlementHistory';
            require_once '../../application/bootstrap.php';
            $model = "\\Settlement\\Model\\".$model;
            $config = array('model'=>$model, 'number'=>$data['driver_id'] % 256);
            $db_model = Register::get($config);
            if(!$db_model){
                $db_model = new $model($data['driver_id']);
                Register::set($config, $db_model);
            }
            $child = array();
            if(isset($data['child'])){
                $child = $data['child'];
                unset($data['child']);
            }
            $id = $db_model->insertRecord($data);
            //$id = 1;
            if($child){
                $child[$db_model->_primaryKey] = $id;
                //递归调用
                call_user_func($fd->onRecive, $fd, json_encode($child));
            }
            $fd->log("result $id");
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
            $db = new Database($config);
            
            $primary = $arr_fields[0];
            $id = 0;
            $flag = true;
            $data = array();
            while($flag){
                $arr_where[$primary] = array('>'=>$id);
                $orderby = "order by $primary asc";
                $data = $db->queryList($table, implode(",", $arr_fields), $arr_where, $this->page, $this->pagesize, $orderby);
                //print_r($data);
                if($func){
                    $data = $func($data, $db);
                }
                //print_r($data);exit();
                if(count($data) < $this->pagesize){
                    $flag = false;
                }else{
                    $id = $data[count($data)-1][$primary];
                }
                foreach ($data as $key => $value){
                    unset($value[$primary]);//删除主键id
                    $value['table'] = $table;
                    $this->_worker->send(json_encode($value), $value['driver_id'] % $this->amount);//发送到子进程
                }
                usleep(100);
            }
        } catch (Exception $e){
            
        }
        $this->_worker->stop();
    }
}