<?php 

/**
 * Counter
 * @author liyunfei
 * 1000000 times incr
 * real	0m8.636s
 * user	0m6.868s
 * sys	0m1.771s
 */

class Counter
{
    private $shmop_key;
    
    private $shmop_size = 8;
    
    private $shmid;
    
    private $fp;
    private $lock_file = "counter.lock";
    
    public function __construct(){
        $this->shmop_key = ftok(__FILE__, 't');
        
        $this->shmid = @shmop_open($this->shmop_key, 'c', 0644, $this->shmop_size);
        if(!$this->shmid){
            echo json_encode(error_get_last())."\n";
            exit();
        }
        if (decoct(fileperms("/dev/shm/") & 0666) == 666){
            $this->lock_file = "/dev/shm/".$this->lock_file;
        }
        $this->fp = fopen($this->lock_file, "a+");
    }
    
    /**
     * when first new Counter must do init
     */
    public function init(){
        shmop_write($this->shmid, str_pad(0, $this->shmop_size), 0);
    }
    
    public function incr(){
        if ( flock($this->fp, LOCK_EX) ) {
            $count = shmop_read($this->shmid, 0, $this->shmop_size);
            //echo posix_getpid()." ".$count."\n";
            $count += 1;
            shmop_write($this->shmid, $count, 0);
            flock($this->fp, LOCK_UN);
            //sleep(1);
        }
        return $count;
    }
    
    public function get(){
        return shmop_read($this->shmid, 0, $this->shmop_size);
    }
    
    public function remove(){
        fclose($this->fp);
        unlink($this->lock_file);
        shmop_delete($this->shmid);
        shmop_close($this->shmid);
    }
}