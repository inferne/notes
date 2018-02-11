<?php
/**
 * Redis option class
 * 
 * @copyright shangque.com Inc
 * 
 */

class Redis{
   
    private $redis; //redis对象
   
    public $static;//链接状态
    
    /**
     * 初始化Redis
     * $config = array(
     *  'server' => '127.0.0.1' 服务器
     *  'port'   => '6379' 端口号
     * )
     * @param array $config
     */
    function __construct($config = array()){
		if( empty( $config ) ){
			$config = array('server'=>'127.0.0.1', 'port'=>'6379', 'timeout'=>3);
		}
		
		$config['timeout'] = isset($config['timeout']) ? $config['timeout'] : 3;
		
        $this->redis = new Redis();
        $this->static = $this->redis->pconnect($config['server'], $config['port'], $config['timeout']);
    }
   
    /**
     * 设置值
     * @param string $key KEY名称
     * @param string|array $value 获取得到的数据
     * @param int $timeOut 时间
     */
    public function set($key, $value, $timeOut = 0,$type='json') {
        if($type=='serialize')
        {
            $value = serialize($value);
        }
        else
        {
            $value = json_encode($value);
        }
           
        $retRes = $this->redis->set($key, $value);
        if ($timeOut > 0) $this->redis->setTimeout($key, $timeOut);
        return $retRes;
    }
   
    /**
     * 通过KEY获取数据
     * @param string $key KEY名称
     */
    public function get($key,$type='json') {
        $result = $this->redis->get($key);
           
        if($type=='serialize')
        {
            return unserialize($result);
        }
        else
        {
            return json_decode($result);
        }
    }
   
    /**
     * 删除一条数据
     * @param string $key KEY名称
     */
    public function delete($key) {
        return $this->redis->delete($key);
    }
   
    /**
     * 清空数据
     */
    public function flushAll() {
        return $this->redis->flushAll();
    }
   
    /**
     * 数据入队列
     * @param string $key KEY名称
     * @param string|array $value 获取得到的数据
     * @param bool $right 是否从右边开始入
     */
    public function push($key, $value ,$right = true) {
        //$value = json_encode($value);
        return $right ? $this->redis->rPush($key, $value) : $this->redis->lPush($key, $value);
    }
   
    /**
     * 数据出队列
     * @param string $key KEY名称
     * @param bool $left 是否从左边开始出数据
     */
    public function pop($key , $left = true) {
        return $left ? $this->redis->lPop($key) : $this->redis->rPop($key);
//         $val = $left ? $this->redis->lPop($key) : $this->redis->rPop($key);
//         return json_decode($val);
    }
	
    /**
     * 返回名称为key的list中start至end之间的元素（end为 -1 ，返回所有）
     * @param string $key KEY名称
     * @param intval $strat 启始位置
	 * @paraam intval $end 结束位置
     */	
	public function range($key, $start = 0, $end = -1) {
		return $this->redis->lRange($key, $start, $end);
	}
	
    /**
     * 截取名称为key的list，保留start至end之间的元素
     * @param string $key KEY名称
     * @param intval $strat 启始位置
	 * @paraam intval $end 结束位置
     */	
	public function trim($key, $start = 0, $end = -1) {
		return $this->redis->lTrim($key, $start, $end);
	}
	
    /**
     * 返回指定键存储在列表中指定的元素。 0第一个元素，1第二个… -1最后一个元素，-2的倒数第二…错误的索引或键不指向列表则返回FALSE。
     * @param string $key KEY名称
     * @param intval $strat 启始位置
	 * @paraam intval $end 结束位置
     */	
	public function lGet($key, $start = 0, $type='json') {
	
		$result = $this->redis->lGet($key, $start);
        return json_decode($result);

	}
	
	/**
	 * 设置值
	 * @param string $key KEY名称
	 * @param string|array $value 获取得到的数据
	 * @param int $timeOut 时间
	 */
	public function hset($hash, $key, $value, $type='json') {
		if($type=='serialize'){
			$value = serialize($value);
		}else{
			$value = json_encode($value, true);
		}	
		$retRes = $this->redis->hset($hash, $key, $value);
		return $retRes;
	}
	 
	/**
	 * 通过KEY获取数据
	 * @param string $key KEY名称
	 */
	public function hget($hash, $key, $type='json') {
		$result = $this->redis->hget($hash, $key);		 
		if($type=='serialize'){
			return unserialize($result);
		}else{
			return json_decode($result, true);
		}
	}
   
    /**
     * 数据自增
     * @param string $key KEY名称
     */
    public function increment($key) {
        return $this->redis->incr($key);
    }
   
    /**
     * 数据自减
     * @param string $key KEY名称
     */
    public function decrement($key) {
        return $this->redis->decr($key);
    }
   
    /**
     * key是否存在，存在返回ture
     * @param string $key KEY名称
     */
    public function exists($key) {
        return $this->redis->exists($key);
    }
   
    /**
     * 返回redis对象
     * redis有非常多的操作方法，我们只封装了一部分
     * 拿着这个对象就可以直接调用redis自身方法
     */
    public function redis() {
        return $this->redis;
    }

    public function __destruct(){
        if($this->static) $this->redis->close();
    }
}
