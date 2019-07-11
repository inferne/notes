<?php

class Protocol
{
    const VERSION = 0x10000000;
    
    // message type
    const REQUEST   = 0x1000;
    const RESPONSE  = 0x2000;
    const EXCEPTION = 0x3000;
    
    //data type
    const STRING   = 0x10;
    const JSON     = 0x20;
    const PROTOBUF = 0x30;
    const MSGPACK  = 0x40;
    
    //servers type
    const PING   = 0x1000; //心跳(ping)
    const ADD    = 0x2000; //加入集群
    const STATUS = 0x3000; //timer 状态
    
    //task
    const TASK   = 0x8000;
    
    const OK     = 'ok';
    
    const EOF    = "\r\n\r\n";
    
    public static $header = [
        'version' => 'N',
        'type' => 'n',
        'number' => 'N',
        'length' => 'N',
    ];
    
    public static function getType($string)
    {
        return unpack("n", substr($string, 4, 2));
    }
    
    public static function parse($msg)
    {
        $d = unpack("Nversion/ntype/Nnumber/Nlength", $msg);
        $d['mtype'] = $d['version'] & 0xff00;
        $d['dtype'] = $d['version'] & 0xff;
        $d['version'] = $d['version'] & 0xffff0000;
        $d['data'] = substr($msg, 14, $d['length']);
        return $d;
    }
    
    public static function build($msgType = 0x1000, $type = 0x1000, $data = 'hello world!')
    {
        $string = '';
        $string .= pack("N", self::VERSION | $msgType | self::STRING);
        $string .= pack("n", $type);
        $string .= pack("N", 123456789);
        $string .= pack("N", strlen($data));
        $string .= $data;
        return $string.self::EOF;
    }
}
