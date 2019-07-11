<?php

/**
 * max cpu idle percentage
 * @author liyunfei
 *
 */
class MaxCpuId
{
    public $sl;
    
    public $head = 0;
    
    public function __construct()
    {
        // 哨兵
        $this->head = [
            'lv' => 1, // level
            'id' => -1, // number
            'dt' => -1, // data
            'next' => [], // next elements
        ];
    }
    
    public function add($id, int $dt)
    {
        $elv = 0;
        do {
            $elv++;
        } while ( rand(0, 99) < 50 );
        
        $el = [
            'lv' => $elv, // level
            'id' => $id, // number
            'dt' => $dt, // data
            'next' => [], // next elements
        ];
        
        $hd = &$this->head;
        $hlv = $hd['lv'];
        if ($hlv <= $elv) {
            $hlv = $elv+1;
            $hd['lv'] = $hlv;
        }
        do {
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
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
        } while (1);
        
        $this->sl[$dt] = &$el;
    }
    
    public function del(int $dt)
    {
        if (!isset($this->sl[$dt])) {
            return 0;
        }
        $id = $this->sl[$dt]['id'];
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
                if ($next['dt'] == $dt) {
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
    
    public function find(int $dt)
    {
        if (!isset($this->sl[$dt])) {
            return 0;
        }
        $id = $this->sl[$dt]['id'];
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
                if ($next['dt'] == $dt) {
                    return $next;//$hd['next'][$hlv-1];
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
        
        return false;
    }
    
    /**
     * why write this? because could less memory opeation
     * @param int $nid
     * @param int $dt
     */
    public function upd($nid, int $dt)
    {
        if (!isset($this->sl[$dt]) || $nid == $this->sl[$dt]['id']) {
            return 0;
        }
        
        /* delete from old position */
        $id = $this->sl[$dt]['id'];
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
                if ($next['dt'] == $dt) {
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
        /* add to new position */
        $el = &$this->sl[$dt];
        $el['id'] = $id = $nid;
        $elv = $el['lv'];
        $hd = &$this->head;
        $hlv = $hd['lv'];
        do {
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
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
        } while (1);
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
            $next = &$hd['next'][$hlv-1];
            if ($next == null) {
                if ($hlv > 1) {
                    $hlv--;
                } else {
                    break;
                }
                continue;
            }
            
            if ($next != null ) {
                $hd = &$next;
                continue;
            }
        } while (1);
        
        if ($hd != $this->head) {
            $this->del($hd['dt']);
            return $hd;
        }
        return [];
    }
    
    public function print()
    {
        $cnt = count($this->sl);
        $hd = &$this->head;
        $hlv = $hd['lv'];
        for ($j = 0; $j <= $cnt; $j++) {
            printf("%4d", $hd['dt']);
            $hd = &$hd['next'][0];
        }
        printf("\n");
        $hd = &$this->head;
        for ($j = 0; $j <= $cnt; $j++) {
            printf("----");
        }
        printf("\n");
        for ($i = 0; $i < $hlv; $i++) {
            $hd = &$this->head;
            for ($j = 0; $j <= $cnt; $j++) {
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
