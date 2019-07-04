<?php

class Skiplist
{
    public $head = 0;
    
    public $i = 0;
    public $j = 0;
    
    public function __construct()
    {
        // 哨兵
        $this->head = [
            'lv' => 1, // level
            'id' => -1, // number
            'next' => [], // next elements
        ];
    }
    
    public function add(int $id)
    {
        $elv = 0;
        do {
            $elv++;
        } while ( rand(0, 99) < 50 );
        
        $el = [
            'lv' => $elv, // level
            'id' => $id, // number
            'next' => [], // next elements
        ];
        
        $hd = &$this->head;
        $hlv = $hd['lv'];
        if ($hlv <= $elv) {
            $hlv = $elv+1;
            $hd['lv'] = $hlv;
        }
        do {
            $this->i++;
            $next = &$hd['next'][$hlv-1];
            if ($next == null) {
                if ( $hlv <= $elv ) {
                    $el['next'][$hlv-1] = &$next;
                    $hd['next'][$hlv-1] = &$el;
                }
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($next['id'] < $id ) {
                $hd = &$next;
                continue;
            }
            
            if ($next['id'] >= $id) {
                if ( $hlv <= $elv ) {
                    $el['next'][$hlv-1] = &$next;
                    $hd['next'][$hlv-1] = &$el;
                }
                if ($hlv > 1) {
//                     if ($hd['next'][0] == $hd['next'][$hlv-1]) {
//                         $next = &$hd['next'][0];
//                         while ($next['lv'] > 1) {
//                             $hd['next'][$next['lv']] = &$hd['next'][$next['lv']]['next'][$next['lv']];
//                             unset($hd['next'][$next['lv']]);
//                             $next['lv']--;
//                         }
//                     }
//                     if ($hd['next'][$hlv-2]['lv'] == $hd['next'][$hlv-2]['next'][$hlv-2]['lv'] && $hd['next'][$hlv-2]['lv'] < $hlv) {
//                         $this->print();
//                         $nn = &$hd['next'][$hlv-2]['next'][$hlv-2];
//                         $nn['lv']++;
//                         $nn['next'][$hlv-1] = &$hd['next'][$hlv-1];
//                         $hd['next'][$hlv-1] = &$nn;
//                         $this->print();
//                         sleep(1);
//                     }
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
        } while (1);
    }
    
    public function del(int $id)
    {
        $hd = &$this->head;
        $hlv = $hd['lv'];
        do {
            $next = &$hd['next'][$hlv-1];
            if ($next == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($next['id'] < $id ) {
                $hd = &$next;
                continue;
            }
            
            if ($next['id'] >= $id) {
                if ($next['id'] == $id) {
                    unset($hd['next'][$hlv-1]);
                    $hd['next'][$hlv-1] = &$next['next'][$hlv-1];
                }
                if ($hlv > 1) {
                    $hlv--;
                } elseif ($next['id'] == $next['next'][$hlv-1]['id']) {
                    $hd = &$next;
                } else {
                    break;
                }
                continue;
            }
        } while (1);
        unset($this->sl[$dt]);
    }
    
    public function find(int $id)
    {
        $hd = &$this->head;
        $hlv = $hd['lv'];
        do {
            $this->j++;
            $next = &$hd['next'][$hlv-1];
            if ($next == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($next['id'] < $id ) {
                $hd = &$next;
                continue;
            }
            
            if ($next['id'] > $id) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            if ($next['id'] == $id) {
                return true;//$hd['next'][$hlv-1];
            }
            
        } while (1);
        
        return false;
    }
    
    /**
     * pop last element
     * @return unknown|number|number[]|NULL[]|array[]
     */
    public function pop()
    {
        $hd = &$this->head;
        $hlv = $hd['lv'];
        do {
            if ($hd['next'][$hlv-1] == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($hd['next'][$hlv-1] != null ) {
                $hd = &$hd['next'][$hlv-1];
                continue;
            }
        } while (1);
        
        if ($hd != $this->head) {
            $this->del($hd['id']);
            return $hd;
        }
        return [];
    }
    
    public function print()
    {
        $cnt = count($this->sl);
        $hd = &$this->head;
        $hlv = $hd['lv'];
        for ($i = 0; $i < $hlv; $i++) {
            $hd = &$this->head;
            while ($hd != null) {
                if ($hd['lv'] > $i) {
                    printf("%4d", $hd['id']);
                } else {
                    printf("    ");
                }
                $hd = &$hd['next'][0];
            }
            printf("  null\n");
        }
    }
}


$sl = new Skiplist();

for ($i = 40; $i >= 0; $i--) {
    $sl->add(rand(10,99), $i);
}

$sl->print();

echo memory_get_usage()."\n";
// exit();
for ($i = 10; $i > 0; $i--) {
    $sl->find($i);
}

echo memory_get_usage()."\n";

for ($i = 10; $i >= 0; $i--) {
    $n = rand(10,99);
    echo "$n ";
    $sl->del($n);
}

echo "\n";
$sl->print();

echo memory_get_usage()."\n";
/* test 10w's add */
$time1 = microtime(true);
for ($i = 10000; $i >= 0; $i--) {
    $sl->add(rand(10000000,99999999), $i);
}

$time2 = microtime(true);

echo memory_get_usage()."\n";

echo ($time2 - $time1)."s\n";
/* test 10w's find */
for ($i = 10000; $i > 0; $i--) {
    $sl->find(rand(10000000,99999999));
}

$time3 = microtime(true);

echo ($time3 - $time2)."s\n";

echo $sl->i." ".$sl->j."\n";

$i = 10000;
$j = 10000;
echo (int)($i*log($i, 2))." ".(int)($j*log($i, 2))."\n";

// -1  27  31  35  36  46  50  53  53  53  64  67  67  70  76  85  90  92  92  93  95  96  null
// -1  27          36  46      53      53      67  67  70  76  85              93          null
// -1                  46      53      53      67          76                              null
// -1                                  53                                                  null
// -1                                  53                                                  null
// -1                                                                                      null
