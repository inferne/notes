<?php 

class A{
    protected $i = 'bbb';
    protected static $j = 2;
    public function test() {
        echo $this->i;
        echo self::$j;
        echo static::$j;
    }
}

class B extends A{
    protected $i = 'aaa';
    protected static $j = 1;
    public function test2(){
        $this->test();
    }
}

$b = new B();
$b->test();