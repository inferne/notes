<?php

class Client
{
    public $cfd;
    
    public $wtfd;
    public $wsfd;
    
    public $ip;
    public $port;
    
    public function __construct()
    {
        $title = cli_get_process_title();
        if (strpos($title, "spiderman") === false) {
            cli_set_process_title("spiderman: client process");
        }
    }
    
    /**
     * create a socket client
     * @param unknown $ip
     * @param unknown $port
     */
    public function create($ip, $port)
    {
        $this->ip = $ip;
        $this->port = $port;
        $this->cfd = stream_socket_client("tcp://$ip:$port", $errno, $errstr, 3);
        if (!$this->cfd) {
            Logger::error("$errstr $errno");
            return 0;
        }
        stream_set_blocking($this->cfd, 0);
        Logger::info("client connect server[".$ip."] is ok!");
        return $this->cfd;
    }
    
    /**
     * add this ip to server
     * @param unknown $ipint
     * @return number
     */
    public function add()
    {   
        $msg = Protocol::build(Protocol::REQUEST, Protocol::ADD);
        if ( ! ($l = fwrite($this->cfd, $msg)) ) {
            throw new Exception("add client ip failed!");
        }
        Logger::info("add client ip is ok!");
        return $l;
    }
    
    public function run()
    {
        $this->add();
        Logger::info("spiderman client is running!");
        while (($string = $this->read()) != '') {
            $result = Protocol::parse($string);
            Logger::debug($result);
            if ($result['mtype'] & Protocol::REQUEST) {
                $this->request($result);
            } elseif ($result['mtype'] & Protocol::RESPONSE) {
                $this->response($result);
            }
        }
    }
    
    public function request($result)
    {
        switch ($result['type']) {
            case Protocol::TASK :
                $l = socket_write($this->wtfd, $result['data']);
                if (!$l) {
                    Logger::error("translate task failed!");
                }
                break;
            case Protocol::ADD :
                Member::add($result['data']);
                break;
            default:
                break;
        }
    }
    
    public function response($result)
    {
        switch ($result['type']) {
            case Protocol::PING :
                Logger::debug("ping server is ok!");
                break;
            case Protocol::ADD :
                Logger::debug("add tasker is ok!");
                Member::setAll(json_decode($result['data'], 1));
                break;
            case Protocol::STATUS :
                return $result['data'];
                break;
            default:
                break;
        }
    }
    
    /**
     * read some message from server
     * @return string
     */
    public function read()
    {
        do {
            $read = [$this->cfd];
            $except = [$this->cfd];
            $write = [];
            if (stream_select($read, $write, $except, 5) > 0) {
                $string = '';
                do {
                    $string .= fgets($this->cfd, 1024);
                } while (substr($string, -4) != Protocol::EOF && $string != '');
                //timer shutdown ?
                if ($string == '') { // RST ?
                    // re connect
                    do {
                        $this->checkTimer();
                        Logger::info("re connect timer server!");
                        sleep(3);
                    } while ( !$this->create($this->ip, $this->port) ) ;
                    $this->run();
                }
                return $string;
            } else {
                $l = $this->ping($this->cpuId());
                if (!$l) {
                    Logger::error("ping server failed!");
                }
            }
            if (posix_getppid() == 1) { // server process down ?!
                // restart
                sleep(3);
                Spiderman::reStart();
            }
        } while (1) ;
    }
    
    public function write($msg)
    {
        return fwrite($this->cfd, $msg);
    }
    
    public function ping($info = 'ping')
    {
        $msg = Protocol::build(Protocol::REQUEST, Protocol::PING, $info);
        return fwrite($this->cfd, $msg);
    }
    
    public $n = 5;
    
    /**
     * check timer status
     * @return number
     */
    public function checkTimer()
    {
        $tasker = Member::getTaskerAll();
        $tip = ip2long(Member::getLocalIp());
        unset($tasker[$tip]);
        $i = count($tasker) > $this->n ? $this->n : count($tasker);
        $arrFlags = [];
        for ( ; $i > 0; $i--) {
            $arrFlags[$i] = 0;
            $k = array_rand($tasker); // rand
            $client = new Client();
            $r = $client->create($tasker[$k]['ip'], $this->port);
            unset($tasker[$k]);
            if (!$r) {
                continue;
            }
            $l = $client->write(Protocol::build(Protocol::REQUEST, Protocol::STATUS, $this->ip));
            if (!$l) {
                $client->close();
                continue;
            }
            $s = $client->read();
            if (!$s) {
                $client->close();
                continue;
            }
            $result = Protocol::parse($s);
            Logger::debug($result);
            if ($result['mtype'] & Protocol::RESPONSE) {
                if ( $this->response($result) ) {
                    $arrFlags[$i] = 1;
                    $client->close();
                    break;
                } else {
                    $arrFlags[$i] = 2;
                }
            }
            $client->close();
        }
        $ac = array_count_values($arrFlags);
        if ($ac[1] > 0) { // timer is up!
            return 1;
        } elseif ($ac[2] > count($arrFlags)/2) { // timer is down!
            socket_write($this->wsfd, "timer is down!");
            exit(112); // host is down
        } else {
            //all members down ?
        }
    }
    
    public function close()
    {
        if ($this->cfd) {
            stream_socket_shutdown($this->cfd, STREAM_SHUT_RDWR);
        }
    }
    
    public $prev;
    
    public function cpuId()
    {
        $prev = $this->prev;
        if (!$prev) {
            $stat = exec("head -n 1 /proc/stat");
            $prev = explode(" ", $stat);
            sleep(1);
        }
        $stat = exec("head -n 1 /proc/stat");
        $next = explode(" ", $stat);
        
        $total = 0;
        for ($i = 1; $i < count($next); $i++) {
            $total += (int)$next[$i] - (int)$prev[$i];
        }
        $idle = $next[5] - $prev[5];
        
        $this->prev = $next; 
        
        return 100*round($idle/$total, 6);
    }
}
