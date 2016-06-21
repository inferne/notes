<?php 

class Consistent_Hashing
{
    private static $_virtual_num = 1024;//虚拟节点数量
    
    private static $_interval;
    
    private static $_virtual_node = array();//虚拟节点
    
    //物理节点配置
    private static $_physical_config = array(
        array('host'=>'host1'), 
        array('host'=>'host2'), 
        array('host'=>'host3'), 
        array('host'=>'host4')
    );

    //物理节点hash table
    private static $_physical_hash_table = array();
    
    public function __construct()
    {
        self::$_interval = 2 << 21;
        $this->virtual_node();
        //print_r(self::$_virtual_node);
        $this->physical_node();
    }
    
    /**
     * 虚拟节点分布
     * 等间隔分布各个节点到0 - 2^32-1的范围上
     * 有兴趣的也可实现其他分布方式
     */
    public function virtual_node()
    {
        $arr_virtual = array();
        for ($i=1; $i <= self::$_virtual_num; $i++){
            $arr_virtual[$i] = ($i > 1 ? $arr_virtual[$i-1] : 0) + self::$_interval;
        }
        self::$_virtual_node = $arr_virtual;
    }
    
    /**
     * 物理节点分布
     */
    public function physical_node()
    {
        foreach (self::$_physical_config as $key => $config){
            self::$_physical_hash_table[sprintf("%u", crc32($config['host']))] = $config;
        }
        ksort(self::$_physical_hash_table);
    }
    
    /**
     * 保存key
     * @param unknown $key
     */
    public function save($key)
    {
        $hash = sprintf("%u", crc32($key));
        //echo "hash:".$hash.chr(10);
        /* 对应的虚拟节点(顺时针旋转到最近的的虚拟节点上) */
        foreach (self::$_virtual_node as $node => $value){
            if($value >= $hash) break;
        }
        //echo "node:".$node.chr(10);
        /* 虚拟节点映射到物理节点 */
        foreach (self::$_physical_hash_table as $hash => $physical){
            if($hash >= self::$_virtual_node[$node]) break;
        }
        
        echo "physical:".json_encode($physical).chr(10);
        /* --------数据保存-------- */
        //exit();
    }
    
    /**
     * 快速保存key
     * @param unknown $key
     */
    public function fast_save($key)
    {
        $hash = sprintf("%u", crc32($key));
        //echo "hash:".$hash.chr(10);
        //按虚拟节点分布规则快速定位节点
        $virtual = ceil($hash/(self::$_interval));
        //echo "node:".$virtual.chr(10);
        /* 虚拟节点映射到物理节点(简单实现-可以维护一张对照表) */
        $physical = $virtual%count(self::$_physical_config);
        $physical = self::$_physical_config[$physical];
        echo "physical:".json_encode($physical).chr(10);
        /* --------数据保存-------- */
        
    }
}

$a = new Consistent_Hashing();
echo "save".PHP_EOL;
$a->save("xxxxxx");
echo "--------------------------------------------------".PHP_EOL;
echo "fast_save".PHP_EOL;
$a->fast_save("xxxxxx");
