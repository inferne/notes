<?php

class Skiplist
{
    public $head = 0;
    
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
        if ($hd['lv'] <= $el['lv']) {
            $hd['lv'] = $el['lv']+1;
        }
        $hlv = $hd['lv'];
        do {
            if ($hd['next'][$hlv-1] == null) {
                if ( $hlv <= $el['lv'] ) {
                    $el['next'][$hlv-1] = &$hd['next'][$hlv-1];
                    $hd['next'][$hlv-1] = &$el;
                }
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] < $id ) {
                $hd = &$hd['next'][$hlv-1];
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] >= $id) {
                if ( $hlv <= $el['lv'] ) {
                    $el['next'][$hlv-1] = &$hd['next'][$hlv-1];
                    $hd['next'][$hlv-1] = &$el;
                }
                
                if ($hlv > 1) {
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
            if ($hd['next'][$hlv-1] == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] < $id ) {
                $hd = &$hd['next'][$hlv-1];
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] >= $id) {
                if ($hd['next'][$hlv-1]['id'] == $id) {
                    $tp = $hd['next'][$hlv-1];
                    unset($hd['next'][$hlv-1]);
                    $hd['next'][$hlv-1] = &$tp['next'][$hlv-1];
                }
                if ($hlv > 1) {
                    $hlv--;
                } elseif ($hd['next'][$hlv-1]['id'] == $hd['next'][$hlv-1]['next'][$hlv-1]['id']) {
                    $hd = &$hd['next'][$hlv-1];
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
            if ($hd['next'][$hlv-1] == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] < $id ) {
                $hd = &$hd['next'][$hlv-1];
                continue;
            }
            
            if ($hd['next'][$hlv-1]['id'] >= $id) {
                if ($hd['next'][$hlv-1]['id'] == $id) {
                    return true;//$hd['next'][$hlv-1];
                }
                if ($hlv > 1) {
                    $hlv--;
                } elseif ($hd['next'][$hlv-1]['id'] == $hd['next'][$hlv-1]['next'][$hlv-1]['id']) {
                    $hd = &$hd['next'][$hlv-1];
                } else {
                    break;
                }
                continue;
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
        
        $this->del($hd['id']);
        
        return $hd;
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

for ($i = 10; $i > 0; $i--) {
    var_dump($sl->find($i));
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

// -1  27  31  35  36  46  50  53  53  53  64  67  67  70  76  85  90  92  92  93  95  96  null
// -1  27          36  46      53      53      67  67  70  76  85              93          null
// -1                  46      53      53      67          76                              null
// -1                                  53                                                  null
// -1                                  53                                                  null
// -1                                                                                      null
