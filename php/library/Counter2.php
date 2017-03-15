<?php 

/**
 * Counter
 * @author liyunfei
 * 1000000 times incr
 * real	0m32.758s
 * user	0m14.384s
 * sys	0m17.650s
 */

class Counter2
{
    private $size = 8;
    private $fp;
    private $lock_file = "counter.lock";
    
    public function __construct(){
        if (decoct(fileperms("/dev/shm/") & 0666) == 666){
            $this->lock_file = "/dev/shm/".$this->lock_file;
        }
        $this->fp = fopen($this->lock_file, "a+");
    }
    
    /**
     * when first new Counter must do init
     */
    public function init(){
        ftruncate($this->fp, 0);
    }
    
    public function incr(){
        if ( flock($this->fp, LOCK_EX) ) {
            fseek($this->fp, 0);
            $count = fread($this->fp, $this->size);
            //echo $count."\n";
            ftruncate($this->fp, 0);
            $count += 1;
            fwrite($this->fp, $count);
            flock($this->fp, LOCK_UN);
            //sleep(1);
        }
        return $count;
    }

    public function get(){
        fseek($this->fp, 0);
        return fread($this->fp, $this->size);
    }
    
    public function remove(){
        fclose($this->fp);
        unlink($this->lock_file);
    }
}