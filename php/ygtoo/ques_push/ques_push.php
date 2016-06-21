<?php
/**
 * 推送问题，格式遵循接口文档刷题数据格式
 */

$debug = false;//debug模式

//debug模式的老师id只能是内部人员
if($debug) $test_uid = array(125, 135, 127305, 136633);

define('PATH', dirname(dirname(__DIR__)));

spl_autoload_register(function($class){
    require_once PATH.'/library/'.$class.'.php';
});

require_once PATH.'/config.php';
require_once PATH.'/library/RedisDB.php';
require_once PATH.'/library/database.php';
require_once 'worker.php';

$push_queue = 'question_push_queue';
$redis = new RedisDB();

$filename = isset($argv[1]) ? $argv[1] : 'question_push.txt';
$file_end = '==END==';

//读取文件，添加任务到redis（共享内存队列）
//读取文件指针位置记录
$push_seek = 'push_seek';

//if(!file_exists($push_seek)) file_put_contents($push_seek, $data);

$fpseek = fopen($push_seek, 'a+');

if(flock($fpseek, LOCK_EX)){//加锁阻塞
    
    $file_content = fgets($fpseek);
    //echo $file_content,'<br/>';
    if(!$file_content) {
        $file_content = array('index'=>1, 'seek'=>0);
    }else{
        $file_content = json_decode($file_content, true);
    }
    
    $seek = isset($file_content['seek']) ? $file_content['seek'] : 0;
    $index = isset($file_content['index']) ? $file_content['index'] : 1;
    
    //打开任务文件
    $task_file = $filename.'.'.$index;
    if(file_exists($task_file)){
        $fppush = fopen($task_file, 'r');
        
        fseek($fppush, $seek);
        
        while (!feof($fppush)){
            $row = fgets($fppush);
            //检查文件结束符,检测到后文件index+1,读取下一个文件
            if(trim($row) == $file_end){
                $index++;//文件名加1
                $seek = 0;//初始化文件偏移量
                break;
            }

            if($row){//内容非空
                $seek += strlen($row);//更新读取位置偏移量
                if( ($row = trim($row)) != '' ){//过滤空白符
                    echo $row,chr(10);
                    $redis->push($push_queue, $row);
                }
            }
        }
        
        fclose($fppush);
    }

    fseek($fpseek, 0);//文件指针倒回起始
    
    $file_content = json_encode(array('index'=>$index, 'seek'=>$seek));
    //echo $file_content,'<br/>';
    ftruncate($fpseek, 0);//清空文件
    
    fwrite($fpseek, $file_content);//记录读取的偏移量
}

fclose($fpseek);

$db = Database::init($config['db']);

$max_pthread = isset($argv[2]) ? $argv[2] : 5;//最大线程数量
$max_exec = isset($argv[3]) ? $argv[3] : 10;//最多执行次数
$timeout = isset($argv[4]) ? $argv[4] : 1;//线程超时时间

$pool_worker = new PoolWorker($max_pthread, $timeout);

//$pool = array();//线程池

if($redis->exists($push_queue)){
    while(($content = $redis->pop($push_queue)) != null){
        if(!$content) continue;
        
        //根据推送任务查询推送的消息内容（问题信息和老师信息）
        $content = trim($content);
        $arr_cont = explode(" ", $content);
        $q_id = $arr_cont[0];
        $system_credit = $arr_cont[1];
        $t_id = explode("_", $arr_cont[2]);
        
        if($debug){//debug模式只给内部人员推送
            foreach ($t_id as $k => $v){
                if(!in_array($v, $test_uid)) unset($t_id[$k]);
            }
        }
        
        if(count($t_id) == 0) continue;
        
        $fields = 'q_id, ask_id, grade, subject, content, create_time, update_time';
        $ques_info = $db::getInfo('question', $fields, array('q_id'=>$q_id));
        
        if($ques_info){
            $grade = Tools::config('config.grade');
            $subject = Tools::config('config.subject');

            $ques_info['grade']   = isset($grade[$ques_info['grade']])     ? $grade[$ques_info['grade']]     : '其他';
            $ques_info['subject'] = isset($subject[$ques_info['subject']]) ? $subject[$ques_info['subject']] : '其他';
            
            $cont = json_decode($ques_info['content'], true);
            if (isset($cont['photo']) && $cont['photo'])
                $cont['photo_thumb'] = Ygtoo::format_url($cont['photo'], 1, 3);
            $ques_info['content'] = $cont;
            
            $user_info = $db::getInfo('user', 'name, header', array('u_id'=>$ques_info['ask_id']));
            if(!$user_info['header']) $user_info['header'] = Ygtoo::format_url($user_info['header'], 1);
            $user_info['header_thumb'] = Ygtoo::format_url($user_info['header'], 1, 2);
            
            $stu_info = $db::getInfo('student', 'star', array('s_id'=>$ques_info['ask_id']));
            
            $mark_info = $db::getInfo('mark_up', 'credit', array('q_id'=>$q_id));
            //学生加价学豆优先抵消系统加价
            if($mark_info && $mark_info['credit'] > 0){
                if($mark_info['credit'] < $system_credit){
                    $system_credit = $system_credit-$mark_info['credit'];
                }else
                    $system_credit = 0;
            }else
                $mark_info['credit'] = 0;
            
            $ques_info = array_merge($ques_info, $user_info, $stu_info);
            $ques_info['credit'] = $mark_info['credit']+$system_credit;
            $param['toUserId'] = $t_id;
            $param['extra'] = $ques_info;
            
            //写入问题发送记录
            $fields = '`q_id`, `t_id`, `system_credit`, `create_time`';
            $value = '';
            $now = date("Y-m-d H:i:s");
            foreach ($t_id as $key => $val){
                $value .= ($value ? ','.chr(10) : '' ).'('.$q_id.', '.$val.', '.$system_credit.', "'.$now.'")';
            }
            $sql = 'insert into qt_question_send ('.$fields.') values '.$value;
            $db::query($sql);
            
            $info = RongCloud::push_question($param);
            $info['content'] = $content;//推送原始内容
            
            //直接推送
            //$ret = Curl::request($info['api'], $info['query'], 'POST', true, $info['header']);
            
            //多线程推送
            $pool_worker->push($info);
        }
        
        //$redis->push($push_queue, $content);
        
        $max_exec--;//允许执行次数减一
        Ygtoo::output_log('max_exec '.$max_exec);
        if($max_exec == 0) break;
        usleep(1000);
    }
}

//等待线程释放
while (count($pool_worker->pool)){
    //遍历检查线程组运行结束
    foreach ($pool_worker->pool as $key => $worker){
        if($worker->param == '' || !$worker->isRunning()) {
            $worker->runing = false;
            unset($pool_worker->pool[$key]);
        }
    }
    Ygtoo::output_log("等待线程结束...");
    sleep(1);
}

Ygtoo::output_log("任务执行完成");

posix_kill(posix_getpid(), 15);

//end
