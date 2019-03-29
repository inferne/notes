<?php

class Shmop 
{
    public $shm_id;
    
    public $size = 1 << 24; // 64k
    
    public function __construct()
    {
        $shm_key = ftok(__FILE__, 't');
        $this->shm_id = shmop_open($shm_key, "c", 0644, $this->size);
    }
    
    public function readByte($offset)
    {
        return shmop_read($this->shm_id, $offset, 1);
    }
    
    public function writeByte($offset, $data)
    {
        return shmop_write($this->shm_id, $data, $offset);
    }
    
    public function __destruct()
    {
        shmop_delete($this->shm_id);
        shmop_close($this->shm_id);
    }
}

$shmop = new Shmop();

for ($i = 0; $i < 10; $i++) {
    $shmop->writeByte($i, $i%2);
    echo "write $i ".($i%2)."\n";
}

for ($i = 0; $i < 15; $i++) {
    $ret = $shmop->readByte($i);
    echo "read $i $ret\n";
}

