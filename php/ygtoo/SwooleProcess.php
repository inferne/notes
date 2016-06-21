<?php
/**
 * SwooleProcess
 */

class SwooleProcess{
    
    private $_worker = array();//工作进程
    private $_number;//可用进程数量
    private $_mpid;//监控进程id
    
    public function __construct($number = 100){
        $this->_number = $number;
    }
    
    /**
     * 请求进程
     * @param unknown $api
     * @param unknown $params
     */
    public function request($api, $params){
        while(1){//检查存在可用进程,进程被耗光则等待释放
            if($this->_number > 0){
                $process = new swoole_process(function(swoole_process $worker) use ($api, $params){
                    $ret = Curl::request($api, $params, 'POST');
                    self::request_log($worker->pid, $api, $params, $ret);
                }, false);
                $pid = $process->start();
                $this->_worker[$pid] = $pid;
                $this->_number--;
                self::log("start request process ".$pid." surplus process ".$this->_number);
                break;
            }else {
                $this->wait_quit_one();
                usleep(1000);
            }
        }
    }

    /**
     * 机器人答题进程
     * @param unknown $api
     * @param unknown $params
     */
    public function robot($api_conduct, $api_answer, $value){
        while(1){//检查存在可用进程,进程被耗光则等待释放
            if($this->_number > 0){
                $process = new swoole_process(function(swoole_process $worker) use ($api_conduct, $api_answer, $value){
                    $u_id = $value['u_id'];
                    $params = array();
                    $params['uid'] = $u_id;
                    $params['bat_id'] = $value['bat_id'];
                    while (1){
                        //请求进行挑战接口
                        $ret = Curl::request($api_conduct, $params, 'POST');
                        self::request_log($worker->pid, $api_conduct, $params, $ret);
                        if($ret && $ret['code'] == 0){
                            $params2 = array();
                            $params2['uid'] = $u_id;
                            $params2['bp_id'] = $ret['msg']['bp_id'];
                            $params2['select'] = 1;
                            //挑战回答
                            $ret = Curl::request($api_answer, $params2, 'POST');
                            self::request_log($worker->pid, $api_answer, $params2, $ret);
                            if($ret['code'] != 0 || $ret['msg'] == 2) break;//判断出错或者返回结果为挑战完成
                        }else 
                            break;//出错跳出
                    }
                }, false);
                $pid = $process->start();
                $this->_worker[$pid] = $pid;
                $this->_number--;
                self::log("start request process ".$pid." surplus process ".$this->_number);
                break;
            }else {
                $this->wait_quit_one();
                usleep(1000);
            }
        }
    }
    
    public function wait_quit(){
        $res = swoole_process::wait(false);
        if($res){
            $pid = $res['pid'];
            self::log("unset request process ".$pid);
            unset($this->_worker[$pid]);
            $this->_number++;
            return true;
        }else 
            return false;
    }
    
    /**
     * 等待退出至少一个进程，尽可能多回收
     */
    public function wait_quit_one(){
        while (1){
            $bind = false;
            $count = count($this->_worker);
            for($i=0; $i<$count; $i++){
                $ret = $this->wait_quit();
                if( !$bind && $ret )
                    $bind = true;
            }
            if( $bind ) break;
            usleep(1000);
        }
    }

    /**
     * 等待退出所有进程
     */
    public function wait_quit_all(){
        while (count($this->_worker) > 0){
            $this->wait_quit();
            usleep(1000);
        }
    }
    
    private static function log($content){
        echo $content.chr(10);
    }
    
    private static function request_log($pid, $api, $params, $ret){
        echo "process: $pid request: $api ".json_encode($params).' receive: '.json_encode($ret).chr(10);
    }
}
