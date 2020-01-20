<?php

class Tasker
{
    public $rfd;
    
    public $arrWorker = [];
    
    public function __construct()
    {
        cli_set_process_title("spiderman: tasker process");
    }
    
    public function run()
    {
        Logger::info("spiderman tasker is running!");
        socket_set_nonblock($this->rfd);
        while (1) {
            $task = socket_read($this->rfd, 1024);
            if ($task) {
                $this->exec($task);
            }
            sleep(1);
            $this->wait();
        }
    }
    
    public function exec($task)
    {
        if (($pid = pcntl_fork()) == 0) {
            cli_set_process_title("spiderman: exec process(" . $task . ")");
            Logger::info("exec task $task");
            $_SERVER['argv'][0] = 'src/run/task/index.php';
            $_SERVER['argv'][1] = 'collectTask';
            $_SERVER['argv'][2] = 'exec';
            $_SERVER['argv'][3] = $task;
            include $_SERVER['argv'][0];
            //pcntl_exec("php", $_SERVER['argv']);
            exit();
        }
        $this->arrWorker[$pid] = $pid;
    }
    
    /**
     * 回收任务
     */
    public function wait()
    {
        while ( count($this->arrWorker) > 0 && ($pid = pcntl_wait($status, WNOHANG | WUNTRACED)) > 0 ) {
            if (isset($this->arrWorker[$pid])) {
                Logger::info("wait & recover process $pid");
                unset($this->arrWorker[$pid]);
            }
        }
    }
}

