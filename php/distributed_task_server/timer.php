<?php

class Timer
{
    public $wfd;
    
    public $arrTask = [];
    
    public $lastTime = 0;
    
    public $shmop;
    
    public function __construct()
    {
        cli_set_process_title("spiderman: timer process");
    }
    
    /**
     * init php env
     */
    public function init()
    {
//         $this->shmop = new Shmop();
//         $this->shmop->isMaster = 0;
    }
    
    /**
     * run all task
     */
    public function run()
    {
        Logger::info("spiderman timer is running!");
        $timer = new TimerWheel();
        $timer->setLogFile(ROOT_DIR."/logs/timer.log");
//         $this->shmop->isMaster = 1;
        while (1) {
            //Logger::info("-----------------------start manager------------------------");
            // 热加载
            $taskConfig = $this->parseConfig();
            //print_r($taskConfig);exit();
            foreach ($taskConfig as $task) {
                // 从计时器添加/删除任务
                $md5 = md5(json_encode($task));
                if (!isset($this->arrTask[$task['id']]) || $md5 != $this->arrTask[$task['id']]['md5']) {
                    if ($task['status'] == 1) {
                        if (isset($this->arrTask[$task['id']])) {
                            $timer->delTask($task['id']);
                        }
                        Logger::debug("create task: ".json_encode($task));
                        $timer->createTask($task['frequency'], $task['id']);
                        $task['md5'] = $md5;
                        $this->arrTask[$task['id']] = $task;
                    } else {
                        if (isset($this->arrTask[$task['id']])) {
                            Logger::debug("del task: ".json_encode($task));
                            $timer->delTask($task['id']);
                            unset($this->arrTask[$task['id']]);
                        }
                    }
                }
                $this->lastTime = $task['edit_time'];
            }
            
            $taskList = $timer->getTask();
            if ($taskList) {
                $i = 0;
                $ids = '';
                foreach ($taskList as $k => $task) {
                    $ids .= $task['id'].",";
                    if ( $i == 100 || $k == count($taskList) - 1 ) {
                        Logger::debug("send $ids to server");
                        socket_write($this->wfd, $ids);
                    }
                }
            }
            //Logger::info("-----------------------stop manager------------------------");
            sleep(1);
            if (posix_getppid() == 1) { // server process down ?!
                // restart
                Spiderman::reStart();
            }
        }
    }
    
    /**
     * parse config
     * @param unknown $conffile
     * @return array|mixed
     */
    public function parseConfig()
    {
        $task_config = [];
        $result = shell_exec("php src/run/task/index.php collectTask list ".$this->lastTime);
        //var_dump($result);
        $ret = json_decode($result, 1);
        if ($ret && isset($ret['data'])) {
            $task_config = $ret['data'];
        } else {
            Logger::error($result);
        }
        
        return $task_config;
    }
}
