<?php

$n = $argv[1];

class A {
    public function __construct(){
    	//echo 1;
    }

	public function b(){
		echo "b";
	}
	public function c(){
		echo "c";
	}
}

$t1 = microtime(true);

for ($i=0; $i < $n; $i++) { 
	$b = new A();
}

$t2 = microtime(true);
$a = new A();
echo "  new $n use time:".($t2 - $t1)."\n";

for ($i=0; $i < $n; $i++) { 
	$b = clone $a;
}

$t3 = microtime(true);
echo "clone $n use time:".($t3 - $t2)."\n";

//end