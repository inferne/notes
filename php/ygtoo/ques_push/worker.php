<?php

/**
 * 推送工作线程
 * 实现线程长驻、重复使用线程，线程超时、避免空线程占用资源
 * @author yunfei
 *
 */
class PushWorker extends Thread{
    
    public $runing = false;
    public $name;
    public $param;
    public $interval = 10000;//线程初始等待间隔10ms
    public $timeout = 10;//线程超时时间
    public $retry = 3;//重试次数
    
    public function __construct($name, $timeout){
        $this->runing = true;
        $this->name   = $name;
        $this->param = '';
        $this->timeout = $timeout;
    }
    
    /**
     * 执行线程
     * @see Threaded::run()
     */
    public function run(){
        require_once dirname(dirname(__DIR__)).'/library/Ygtoo.php';
        $interval = $this->interval;
        $wait_time = 0;
        $retry = $this->retry;
        while ($this->runing){//线程长驻、重复使用线程
            if ($this->param) {//线程执行的参数
                //重试功能
                while($retry){
                    $info = $this->param;
                    Ygtoo::output_log("线程[{$this->name}] 任务参数:[".$info['content']."] 开始执行");
                    //推送逻辑
                    require_once dirname(dirname(__DIR__)).'/library/Curl.php';
                    Ygtoo::output_log("线程[{$this->name}] 任务参数:[".$info['content']."] 开始推送:");
                    $ret = Curl::request($info['api'], $info['query'], 'POST', true, $info['header']);
                    if(!$ret) $ret = Curl::$request_error;
                    Ygtoo::output_log("线程[{$this->name}] 任务参数:[".$info['content']."] 执行结果:".json_encode($ret)."");
                    if($ret['code'] == 200)
                        $retry = 0;
                    else {
                        $retry--;
                        if($retry == 0){//线程停止
                            $this->runing = false;
                            $this->repush($info['content']);//失败任务回收
                        }
                    }
                }
                
                //初始化线程
                $wait_time = 0;
                $interval = $this->interval;
                $this->param = '';//释放线程
                $retry = $this->retry;
            } else {
                $interval = $interval << 1;//等待时间翻倍
            }

            usleep($interval);//线程等待
            $wait_time += $interval;
            Ygtoo::output_log("线程[{$this->name}] 等待任务.. interval:[".($interval/1000)."ms] time:[".($wait_time/1000)."ms]");
            //echo $this->interval;
            //检查线程等待超时、线程退出
            if($wait_time >= $this->timeout*1000000) $this->runing = false;
        }
    }
    
    /**
     * 重新推进redis
     * @param unknown $content
     */
    public function repush($content){
        $push_queue = 'question_push_queue';
        require_once dirname(dirname(__DIR__)).'/library/RedisDB.php';
        
        $redis = new RedisDB();
        $redis->push($push_queue, $content);
        Ygtoo::output_log("线程[{$this->name}] 任务参数:[".$content."] 执行回收");
    }
}

/**
 * 线程池管理
 * 实现线程池，任务分配功能
 * @author yunfei
 *
 */
class PoolWorker{

    public $max_pthread = 100;
    public $timeout = 10;
    
    public $pool;
    
    public function __construct($max_pthread, $timeout){
        $this->max_pthread = $max_pthread;
        $this->timeout = $timeout;
    }
    
    public function push($param){
        $wait_time = 0;
        $flag = false;//任务分发标识
        //任务分配，若无可用线程则循环等待
        while (!$flag){
            $flag = true;
            $bind = false;
            //释放死掉的线程
            if($this->pool){
                foreach ($this->pool as $key => $worker){
                    if(!$worker->isRunning() && !$worker->runing){
                        unset($this->pool[$key]);
                        $this->max_pthread++;
                    }
                }
            }

            //从线程池找一个空闲线程来执行任务
            if($this->pool){
                foreach ($this->pool as $worker){
                    if($worker->runing && $worker->param == ''){
                        $worker->param = $param;
                        $bind = true;
                        break;
                    }
                }
            }
            
            //线程池没有可用线程时创建线程
            if(!$bind){
                if($this->max_pthread > 0){
                    $name = rand(10000, 65536);
                    $push_worker = new PushWorker($name, $this->timeout);//新建线程
                    $push_worker->start();
                    $push_worker->param = $param;
                    $this->pool[$name] = $push_worker;
                    $this->max_pthread--;
                }else{
                    $flag = false;//标识任务未分出去
                    Ygtoo::output_log("任务[".$param['content']."] 等待空闲线程...");
                    usleep(100000);//等待空闲线程100ms
                    $wait_time += 100000;
                    if($wait_time/1000000 >= $this->timeout) {
                        Ygtoo::output_log("任务[".$param['content']."] 等待超时");
                        return false;
                    }
                }
            }
        }
    }
}
