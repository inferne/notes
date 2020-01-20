<?php

class Member
{
    public static $config = ROOT_DIR."/spiderman.json";
    
    public static $all;
    
    public static function getAll()
    {
        if (!self::$all) {
            self::$all = json_decode(@file_get_contents(self::$config), 1);
        }
        return self::$all;
    }
    
    public static function setAll($member)
    {
        file_put_contents(self::$config, json_encode($member), LOCK_EX);
        self::$all = $member;
    }
    
    public static function add($ip = 0)
    {
        if (!$ip) {
            $ip = self::getLocalIp();
        }
        
        $ipi = ip2long($ip);
        $info = ['ip' => $ip, 'role' => '', 'status' => 1];
        //init clusters, first meachine is timer
        if (!self::$all || !self::$all['timer']) {
            $info['role'] = 'timer';
            self::$all["timer"][$ipi] = $info;
            file_put_contents(self::$config, json_encode(self::$all));
        } elseif (isset(self::$all["timer"][$ipi])) {
            $info = self::$all["timer"][$ipi];
        } elseif (isset(self::$all["tasker"][$ipi])) {
            $info = self::$all["tasker"][$ipi];
        } else {
            $info['role'] = 'tasker';
            self::$all["tasker"][$ipi] = $info;
            file_put_contents(self::$config, json_encode(self::$all));
        }
        
        return $info;
    }
    
    public static function getTimerAll()
    {
        if (!self::$all) {
            self::$all = json_decode(@file_get_contents(self::$config), 1);
        }
        return self::$all['timer'];
    }
    
    public static function getTaskerAll()
    {
        if (!self::$all) {
            self::$all = json_decode(@file_get_contents(self::$config), 1);
        }
        return self::$all['tasker'];
    }
    
    public static function setTimer($ip)
    {
        $ipi = ip2long($ip);
        unset(self::$all['tasker'][$ipi]);
        array_pop(self::$all['timer']);
        self::add($ip);
    }
    
    public static function getLocalIp()
    {
        return gethostbyname($_SERVER['HOSTNAME']);
    }
}
