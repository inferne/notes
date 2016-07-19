<?php 
/**
 * database
 * @author liyunfei
 *
 */
 
class Database
{
    public $_last_query;
    
    public $_db;
    
    public function __construct($config){
        $dsn = "mysql:host=localhost;_dbname=test";
        try {
            $this->_db = new PDO($config['dsn'], $config['username'], $config['password'], array(
                PDO::ATTR_TIMEOUT => 1,
                PDO::ATTR_CASE   => PDO::CASE_LOWER,
                PDO::MYSQL_ATTR_INIT_COMMAND   => 'set names utf8',
            ));
        } catch (PDOException $e){
            echo "Connection failed ".$e->getMessage().PHP_EOL;
            throw new Exception($e->getMessage());
        }
    }
    
    /**
     * 查询列表
     * @param unknown $fields
     * @param unknown $arr_where
     * @param number $page
     * @param number $pagesize
     * @param unknown $orderby
     * @return Ambigous <mixed, unknown>
     */
    public function queryList($table, $fields, $arr_where, $page=1, $pagesize=1, $orderby=''){
        $where = '';
        if($arr_where){
            $where = ' where '.$this->where($arr_where);
        }
        $orderby = self::orderby($orderby);
        $limit = '';
        if($page){
            $limit = "limit ".($page-1)*$pagesize.",".$pagesize;
        }
        $sql = "select $fields from ".$table . $where." ".$orderby." $limit";
        echo $sql.PHP_EOL;
        $this->_last_query = $sql;
        $sth = $this->_db->prepare($sql);
        $sth->execute();
        return $sth->fetchAll(PDO::FETCH_ASSOC);
    }
    
    /**
     * 获取一条信息
     * @param string $fields
     * @param array $arr_where
     * @param string $orderby
     */
    public function getInfo($table, $fields, $arr_where, $orderby=''){
        $result = $this->queryList($table, $fields, $arr_where, 1, 1, $orderby);
        if(empty($result)){
            return array();
        }else{
            return $result[0];
        }
    }
    
    /**
     * 组合where条件
     * @param array $arr_where
     * @return Ambigous <string, unknown>
     */
    public function where($arr_where){
        $where = '';
        if(is_string($arr_where)){
            $where = $arr_where;
        }else{
            foreach ($arr_where as $key => $value){
                if(in_array($key, array('and', 'or', 'xor'))){
                    $where .= " $key (".$this->where($value).")";
                }elseif(is_int($value)){
                    $where .= ($where ? ' and ' : '')."`$key` = {$value}";
                }elseif (is_array($value)){
                    if(self::is_assoc($value)){
                        foreach ($value as $k => $v){
                            if(in_array($k, array(">","<",">=","<=","<>","!=","like"))){
                                $where .= ($where ? ' and ' : '')."`$key` $k {$v}";
                            }
                        }
                    }else
                        $where .= ($where ? ' and ' : '')."`$key` in ('".implode("','", $value)."')";
                }else
                    $where .= ($where ? ' and ' : '')."`$key` = '{$value}'";
            }
        }
        return $where;
    }
    
    /**
     * 判断数组是否为关联数组
     * @param array $array
     */
    public static function is_assoc($array){
        return array_keys($array) !== range(0, count($array)-1);
    }
    
    /**
     * order by
     * @param array $orderby
     * @return string
     */
    public static function orderby($orderby){
        if(is_string($orderby)) return $orderby;
    
        $result = '';
        foreach ($orderby as $fileds => $sort){
            $result .= ($result ? ',' : '' )."$fileds $sort";
        }
        return 'order by '.$result;
    }
    
    /**
     * 通过多条件更新
     * @param unknown $arr_data
     * @param unknown $arr_where
     * @return mixed
     */
    public function updateByCondition($table, $arr_data, $arr_where){
        $set = "";
        foreach ($arr_data as $key => $value){
            $set .= ($set ? ',': '')."`$key` = ".$value;
        }
        $where = $this->where($arr_where);
        $sql = "update ".$table." set $set where $where";
        $this->_last_query = $sql;
        return $this->_db->query($sql);
    }
    
    /**
     * 数组选择性重组
     * @param unknown $index
     * @param unknown $fields
     * @param unknown $array
     * @return array
     */
    public function selectToArray($index, $fields, $array){
        $result = array();
        if($fields != "*"){
            $arr_field = explode(',', $fields);
        }
        foreach ($array as $key => $value){
            if($fields == "*"){
                $result[$value[$index]] = $value;
            }elseif (count($arr_field) == 1){
                $result[$value[$index]] = $value[$fields];
            }else{
                foreach ($arr_field as $field){
                    $result[$value[$index]][$field] = $value[$field];
                }
            }
        }
        return $result;
    }
    
    /**
     * 查询选择性重组的数组
     * @param unknown $index
     * @param unknown $fields
     * @param unknown $array
     * @return array
     */
    public function getSelectList($table, $index, $fields, $arr_where=array(), $orderby=''){
        if($index != $fields){
            $result = $this->queryList($table, $index.($index ? ',' : '').$fields, $arr_where, 0, 0, $orderby);
        }else{
            $result = $this->queryList($table, $fields, $arr_where, 0, 0, $orderby);
        }
        $result = $this->selectToArray($index, $fields, $result);
        return $result;
    }
}