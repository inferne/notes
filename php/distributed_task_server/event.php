<?php

interface Event
{
    const TIMEOUT = 1;
    
    const READ = 2;
    
    const WRITE = 4;
    
    const SIGNAL = 8;
    
    const PERSIST = 16;
    
    public function setEvent($socket, $events);
    
    public function setBufferEvent($fd, $events, $callback);
    
    public function accept($socket, $flag, $base);
    
    public function read($buffer, $fd);
    
    public function write($fd, $msg);
    
    public function error($buffer, $error, $fd);
    
    public function loop();
}
