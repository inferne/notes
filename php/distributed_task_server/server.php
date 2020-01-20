<?php

class Server
{
    public $onReInit;
    
    public $port;
    
    public $client;
    
    public $rfd;
    
    public $event;
    
    public $load;
    
    public function __construct(Epoll $event, MaxCpuId $load)
    {
        $this->event = $event; //$this->event = new Epoll()/Select();
        $this->load = $load; //$this->load = new MaxCpuId();
        global $argv;
        cli_set_process_title("spiderman: server process (php ".implode(" ", $argv).")");
    }
    
    /**
     * create server and event loop
     * @param unknown $port
     */
    public function run($port)
    {
        $this->port = $port;
        $socket = stream_socket_server("tcp://0.0.0.0:".$port, $errno, $errstr);
        if (!$socket) {
            throw new Exception($errstr, $errno);
        }
        
        stream_set_blocking($socket, 0);
        
        Logger::debug("spiderman server is running!");
        
        $this->event->setEvent($socket, EVENT::READ | EVENT::PERSIST);
        if ($this->rfd) {// set rfd callback
            socket_set_nonblock($this->rfd);
            $this->event->setEvent($this->rfd, EVENT::READ | EVENT::PERSIST, [$this, 'readChild']);
        }
        
        $this->event->onAccept = [$this, 'accept'];
        $this->event->onRead = [$this, 'read'];
        $this->event->onError = [$this, 'error'];
        $this->event->loop();
    }
    
    /**
     * accept callback
     * @param unknown $socket
     * @return number|resource
     */
    public function accept($socket)
    {
        $cli = stream_socket_accept($socket);
        if (!$cli) {
            return 0;
        }
        stream_set_blocking($cli, 0);
        
        $this->client['connections'][(int)$cli] = $cli;
        Logger::debug("accept client ".stream_socket_get_name($cli, 1)." ".(int)$cli);
        return $cli;
    }
    
    /**
     * read callback
     * @param unknown $string
     * @param unknown $fd
     * @return number
     */
    public function read($string, $fd)
    {
        $info = Protocol::parse($string);
        if ( !($info['version'] & Protocol::VERSION) ) {
            Logger::error("server read invalid ".(int)$fd);
            return 0;
        }
        Logger::debug("server read ".json_encode($info)." ".(int)$fd);
        if ($info['mtype'] & Protocol::REQUEST) {
            $this->request($info, $fd);
        } elseif ($info['mtype'] & Protocol::RESPONSE) {
            $this->response($info);
        }
    }
    
    /**
     * request msg
     * @param unknown $info
     * @param unknown $fd
     */
    public function request($info, $fd)
    {
        $remote = stream_socket_get_name($fd, 1);
        switch ($info['type']) {
            case Protocol::PING :
                if ($info['data'] != "ping") {
                    if ( ( $el = $this->load->find((int)$fd) ) != false ) {
                        if ( abs($el['id'] - $info['data']) > 10 ) {
                            $this->load->upd($info['data'], (int)$fd);
                        }
                    } else {
                        $this->load->add($info['data'], (int)$fd);
                    }
                }
                //$this->client['info'][(int)$fd] = $info['data'];
                $msg = Protocol::build(Protocol::RESPONSE, Protocol::PING, Protocol::OK);
                Logger::debug("response ping ".(int)$fd."($remote) ".Protocol::OK);
                $this->event->write($fd, $msg);
                break;
            case Protocol::ADD :
                Logger::debug("receive $remote add");
                // tell all cli
                $ip = parse_url($remote, PHP_URL_HOST);
                $msg = Protocol::build(Protocol::REQUEST, Protocol::ADD, $ip);
                foreach ($this->client['connections'] as $cli) {
                    if ($cli != $this->client['connections'][(int)$fd]) {
                        $this->event->write($cli, $msg);
                    }
                }
                Member::add($ip);
                // response cli $fd
                Logger::debug("response add ".(int)$fd."($remote) ".Protocol::OK);
                $msg = Protocol::build(Protocol::RESPONSE, Protocol::ADD, json_encode(Member::getAll()));
                $this->event->write($fd, $msg);
                break;
            case Protocol::STATUS :
                Logger::debug("receive $remote check server status");
                // check timer status
                $status = $this->checkStatus($info['data']);
                $msg = Protocol::build(Protocol::RESPONSE, Protocol::STATUS, $status);
                $this->event->write($fd, $msg);
                break;
            default:
                break;
        }
    }
    
    /**
     * response msg
     * @param unknown $info
     */
    public function response($info)
    {
        
    }
    
    /**
     * if socket error, close socket
     * @param unknown $buffer
     * @param unknown $error
     * @param unknown $fd
     */
    public function error($error, $fd)
    {
        Logger::debug("error $error ".(int)$fd);
        $this->load->del((int)$fd);
        stream_socket_shutdown($this->client['connections'][(int)$fd], STREAM_SHUT_RDWR);
        unset($this->client['connections'][(int)$fd]);
    }
    
    /**
     * read child process send's string
     */
    public function readChild()
    {
        Logger::debug("read child");
        while ( ($read = socket_read($this->rfd, 4096)) != false ) {
            Logger::debug("read child $read");
            if ($read == "timer is down!") {
                call_user_func($this->onReInit);
            } else {
                $this->distribute($read);
            }
        }
    }
    
    /**
     * check timer status
     * @return number|resource
     */
    public function checkStatus($ip)
    {
        $status = 0;
        $k = array_rand(Member::$all['timer']);
        $timer = Member::$all['timer'][$k];
        if ($timer['ip'] == $ip) {
            $client = new Client();
            $status = $client->create($timer['ip'], $this->port);
            $client->close();
        }
        return $status;
    }
    
    /**
     * distribute task
     */
    public function distribute($read)
    {
        $arrTask = explode(",", $read);
        foreach ($arrTask as $task) {
            if (!$task) {
                continue;
            }
            
            if ( $this->client['connections'] && ($el = $this->load->pop()) ) {
                if ($el['id'] < 20) {
                    Logger::info("tasker client is too busy!");
                }
                $id = $el['id'];
                $fd = $this->client['connections'][$el['dt']];
                Logger::debug("send to fd=".(int)$fd.", id=$id% (".stream_socket_get_name($fd, 1).") $task");
                $msg = Protocol::build(Protocol::REQUEST, Protocol::TASK, $task);
                if ($this->event->write($fd, $msg)) {
                    Logger::debug("write ".(int)$fd." success!");
                    $id = ($id > 10 ? $id - rand(5, 10) : $id);// rand less 5-10 cpu idle
                } else {
                    Logger::debug("write ".(int)$fd." failed!");
                }
                $this->load->upd($id, (int)$fd);
            } else {
                Logger::info("no tasker client could used!");
            }
        }
    }
    
    public function insertList()
    {
        
    }
}
