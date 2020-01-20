<?php

class Epoll implements Event
{
    public $base;
    
    public $event; // event must persist, because php will free resource, loop could error
    
    public $bevent;
    
    public $onAccept;
    
    public $onRead;
    
    public $onError;
    
    const EOF = "\r\n\r\n";
    
    /**
     * init event base
     */
    public function __construct()
    {
        $this->base = event_base_new();
    }
    
    /**
     * set event base 
     * @param unknown $socket
     * @param unknown $events
     */
    public function setEvent($socket, $events, $callback = NULL)
    {
        if (!$callback) {
            $callback = [$this, "accept"];
        }
        
        $event = event_new();
        event_set($event, $socket, $events, $callback, $this->base);
        event_base_set($event, $this->base);
        event_add($event);
        $this->event[(int)$socket] = $event;
    }
    
    public function setBufferEvent($fd, $events, $callback)
    {
        $bevent = event_buffer_new($fd, $callback, NULL, [$this, "error"], $fd);
        event_buffer_base_set($bevent, $this->base);
        event_buffer_timeout_set($bevent, 30, 30);
        event_buffer_watermark_set($bevent, EV_READ, 0, 0xffffff);
        event_buffer_priority_set($bevent, 10);
        event_buffer_enable($bevent, $events);
        
        $this->bevent[(int)$fd] = $bevent;
    }
    
    /**
     * when listen socket EV_READ arrival, let's accept
     * @param stream $socket
     * @param unknown $flag
     * @param unknown $base
     * @return number
     */
    public function accept($socket, $flag, $base)
    {
        $fd = call_user_func($this->onAccept, $socket);
        if (!$fd) {
            return 0;
        }
        stream_set_blocking($fd, 0);
        $bevent = event_buffer_new($fd, [$this, "read"], NULL, [$this, "error"], $fd);
        event_buffer_base_set($bevent, $base);
        event_buffer_timeout_set($bevent, 30, 30);
        event_buffer_watermark_set($bevent, EV_READ, 0, 0xffffff);
        event_buffer_priority_set($bevent, 10);
        event_buffer_enable($bevent, EV_READ | EV_PERSIST);
        
        $this->bevent[(int)$fd] = $bevent;
    }
    
    /**
     * when other socket EV_READ arrival, let's read
     * @param unknown $bevent
     * @param unknown $fd
     */
    public function read($bevent, $fd)
    {
        $string = '';
        while (($read = event_buffer_read($bevent, 1024)) != '') {
            $string .= $read;
            if (substr($read, -4) == self::EOF) {
                break;
            }
        }
        call_user_func_array($this->onRead, [$string, $fd]);
    }
    
    public function write($fd, $msg)
    {
        $bevent = $this->bevent[(int)$fd];
        return event_buffer_write($bevent, $msg);
    }
    
    /**
     * if socket error, close buffer and onerror
     * @param unknown $bevent
     * @param unknown $error
     * @param unknown $fd
     */
    public function error($bevent, $error, $fd)
    {
        call_user_func_array($this->onError, [$error, $fd]);
        event_buffer_disable($bevent, EV_READ | EV_WRITE);
        event_buffer_free($bevent);
        unset($this->bevent[(int)$fd]);
    }
    
    /**
     * loop until a read event arrivals
     */
    public function loop()
    {
        event_base_loop($this->base);
    }
}
