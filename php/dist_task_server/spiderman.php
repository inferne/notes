<?php

class Spiderman
{
    public $config = ROOT_DIR."/src/run/server/spiderman.json";
    
    public $logFile;
    
    public $member;
    
    public $daemonize;
    
    public $port = 8989;
    
    public $cpid;
    public $tpid;
    
    public $sockets = array();
    
    /**
     * init enveriament
     */
    public function init()
    {
        $php = explode(" ", exec("whereis -b php"));
        
        if (!$php[1]) {
            echo "php is not found!";
        } else {
            $path = getenv("PATH");
            putenv("PATH=".$path.":".dirname($php[1]));
        }
        
        $options = getopt("d::");
        if (isset($options['d'])) { // php collect -d could into daemonize
            $this->daemonize = true;
            
            if (!$this->logFile) {
                $this->logFile = ROOT_DIR."/logs/".basename(__FILE__, ".php").".log";
            }
            /* redirect stdout, stderr */
            $outlog = $this->logFile;
            $errlog = $this->logFile.".error";
            global $STDOUT,$STDERR;
            fclose(STDOUT);
            fclose(STDERR);
            $STDOUT = fopen($outlog, "a");
            $STDERR = fopen($errlog, "a");
        }
    }
    
    /**
     * run server, client, timer and tasker
     * @throws Exception
     */
    public function run()
    {
        $this->init();
        $this->daemonize();
        
        Member::$all = Member::getAll();
        //add member
        $info = Member::add();
        
        /* On Windows we need to use AF_INET */
        $domain = (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN' ? AF_INET : AF_UNIX);
        /* Setup socket pair */
        if (socket_create_pair($domain, SOCK_STREAM, 0, $this->sockets) === false) {
            Logger::error("socket_create_pair failed. Reason: ".socket_strerror(socket_last_error()));
        }
        
        $pid = pcntl_fork();
        if ( $pid > 0 ) { // server manger
            $this->tpid = $pid;
            
            if ( $info['role'] == 'tasker' ) {
                if ( ($ppid = pcntl_fork()) == 0 ) {
                    $this->runClient();
                } elseif ($ppid < 0) {
                    throw new Exception("fork error!");
                }
                $this->cpid = $ppid;
            }
            
            $this->runServer();
        } elseif ($pid == 0) {
            if ($info['role'] == 'timer') { 
                $this->runTimer();
            } else {
                $this->runTasker();
            }
            exit();
        } else {
            throw new Exception("fork error!");
        }
    }
    
    /**
     * run server
     */
    public function runServer()
    {
        $server = new Server(new Epoll(), new MaxCpuId());
        $server->onReInit = [$this, 'reInit'];
        $server->rfd = $this->sockets[1];
        $server->run($this->port);
    }
    
    /**
     * run client
     */
    public function runClient()
    {
        $key = array_rand(Member::$all['timer']);
        $timer = Member::$all['timer'][$key];
        $client = new Client();
        $client->wsfd = $this->sockets[0];
        $client->wtfd = $this->sockets[1];
        if ( $client->create($timer['ip'], $this->port) ) {
            $client->run();
        }
        exit();
    }
    
    /**
     * run timer
     */
    public function runTimer()
    {
        $timer = new Timer();
        $timer->wfd = $this->sockets[0];
        $timer->run();
        exit();
    }
    
    /**
     * run tasker
     */
    public function runTasker()
    {
        $tasker = new Tasker();
        $tasker->rfd = $this->sockets[0];
        $tasker->run();
        exit();
    }
    
    /**
     * re init all process role
     * @return number
     */
    public function reInit()
    {
        pcntl_waitpid($this->cpid, $status);
        Logger::info("client process=".$this->cpid." status=".$status);
        
        $ip = Member::getLocalIp();
        $tasker = array_pop(Member::getTaskerAll());
        Member::setTimer($tasker['ip']);
        if ( $ip == $tasker['ip'] ) {
            posix_kill($this->tpid, SIGINT);
            pcntl_waitpid($this->tpid, $status);
            Logger::info("tasker process=".$this->tpid." status=".$status);
            
            $pid = pcntl_fork();
            if ($pid == 0) {
                $this->runTimer();
            }
        } else {
            sleep(3);
            $pid = pcntl_fork();
            if ($pid == 0) {
                $this->runClient();
            }
            $this->cpid = $pid;
        }
        
        return 1;
    }
    
    /**
     * Run as deamon mode.
     * copy from workerman
     * @throws Exception
     */
    public function daemonize()
    {
        if(!$this->daemonize){
            return 1;
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
    
    public function printLog($msg){
        if (is_array($msg)) {
            $msg = json_encode($msg);
        }
        $msg = date("Y-m-d H:i:s")." ".posix_getpid()." ".$msg."\n";
        if ($this->logFile && $this->daemonize) { // 后台执行时写文件
            file_put_contents($this->logFile, $msg, FILE_APPEND | LOCK_EX);
        } else { // 前台执行时输出，方便debug
            echo $msg;
        }
    }
}
