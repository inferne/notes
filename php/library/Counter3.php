<?php 

/**
 * Counter
 * @author liyunfei
 * 1000000 times incr
 * real	0m14.316s
 * user	0m11.740s
 * sys	0m1.562s
 */

class Counter3
{
    private $key;
    
    private $shmop_size = 8;
    
    private $sem;
    private $shmid;
    
    private $fp;
    private $lock_file = "counter.lock";
    
    public function __construct(){
        $this->key = ftok(__FILE__, 't');
        
        $this->shmid = @shmop_open($this->key, 'c', 0644, $this->shmop_size);
        if(!$this->shmid){
            echo json_encode(error_get_last())."\n";
            exit();
        }
        $this->sem = sem_get($this->key);
    }
    
    /**
     * when first new Counter must do init
     */
    public function init(){
        shmop_write($this->shmid, str_pad(0, $this->shmop_size), 0);
    }
    
    public function incr(){
        if ( sem_acquire($this->sem) ) {
            $count = shmop_read($this->shmid, 0, $this->shmop_size);
            //echo posix_getpid()." ".$count."\n";
            $count += 1;
            shmop_write($this->shmid, $count, 0);
            sem_release($this->sem);
            //sleep(1);
        }
        return $count;
    }
    
    public function get(){
        return shmop_read($this->shmid, 0, $this->shmop_size);
    }
    
    public function remove(){
        sem_remove($this->sem);
        shmop_delete($this->shmid);
        shmop_close($this->shmid);
    }
}
