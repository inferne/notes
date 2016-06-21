<?php 
/**
 * @name 适配器模式
 * 适配器模式（Adapter Pattern）是作为两个不兼容的接口之间的桥梁。这种类型的设计模式属于结构型模式，它结合了两个独立接口的功能。
 * 这种模式涉及到一个单一的类，该类负责加入独立的或不兼容的接口功能。举个真实的例子，读卡器是作为内存卡和笔记本之间的适配器。
 * 您将内存卡插入读卡器，再将读卡器插入笔记本，这样就可以通过笔记本来读取内存卡。
 * 我们通过下面的实例来演示适配器模式的使用。其中，音频播放器设备只能播放 mp3 文件，通过使用一个更高级的音频播放器来播放 vlc 和 mp4 文件。
 * @desc 
 * 意图：将一个类的接口转换成客户希望的另外一个接口。适配器模式使得原本由于接口不兼容而不能一起工作的那些类可以一起工作。
 * 主要解决：主要解决在软件系统中，常常要将一些"现存的对象"放到新的环境中，而新环境要求的接口是现对象不能满足的。
 * 何时使用： 1、系统需要使用现有的类，而此类的接口不符合系统的需要。 2、想要建立一个可以重复使用的类，用于与一些彼此之间没有太大关联的一些类，
 * 包括一些可能在将来引进的类一起工作，这些源类不一定有一致的接口。 3、通过接口转换，将一个类插入另一个类系中。（比如老虎和飞禽，现在多了一个
 * 飞虎，在不增加实体的需求下，增加一个适配器，在里面包容一个虎对象，实现飞的接口。）
 * 如何解决：继承或依赖（推荐）。
 * 关键代码：适配器继承或依赖已有的对象，实现想要的目标接口。
 * 应用实例： 
 * 1、美国电器 110V，中国 220V，就要有一个适配器将 110V 转化为 220V。 
 * 2、在 LINUX 上运行 WINDOWS 程序。
 */

/**
 * 步骤 1
 * 为媒体播放器和更高级的媒体播放器创建接口。
 */
interface MediaPlayer
{
    public function play($type, $filename);
}
 
interface AdvancedMediaPlayer
{
     public function playVlc($filename);
     public function playMp4($filename);
}
 
 /**
  * 步骤 2
  * 创建实现了 AdvancedMediaPlayer 接口的实体类。
  */
class VlcPlayer implements AdvancedMediaPlayer
{
    public function playVlc($filename)
    {
        echo "Playing vlc file. Name: ".$filename;
    }
    
    public function playMp4($filename){}
}

class Mp4Player implements AdvancedMediaPlayer
{
    public function playVlc($filename){}
    
    public function playMp4($filename)
    {
        echo "Playing vlc file. Name: ".$filename;
    }
}

/**
 * 步骤 3
 * 创建实现了 MediaPlayer 接口的适配器类。
 */
class MediaAdapter implements MediaPlayer
{
    public $_advancedMusicPlayer;
    
    public function __construct($type)
    {
        $player = $type."Player";
        $this->_advancedMusicPlayer = new $player();
    }
    
    public function play($type, $filename)
    {
        $play = "play".$type;
        $this->_advancedMusicPlayer->{$play}($filename);
    }
}

/**
 * 步骤4
 * 创建实现了MediaPlayer接口的实体类
 */
class AudioPlayer implements MediaPlayer
{
    public $_media = array("Vlc", "Mp4");
    
    public $_mediaAdaper;
    
    public function play($type, $filename)
    {
        $type = ucfirst($type);
        if($type == "Mp3")
            echo "Playing $type file. Name: ".$filename;
        elseif(in_array($type, $this->_media)){
            $this->_mediaAdaper = new MediaAdapter($type);
            $this->_mediaAdaper->play($type, $filename);
        }else
            echo "Invalid media. " . $type . " format not supported";
    }
}

/**
 * 步骤 5
 * 使用 AudioPlayer 来播放不同类型的音频格式。
 */
$audioPlayer = new AudioPlayer();
$audioPlayer->play("mp3", "beyond the horizon.mp3");//Playing Mp3 file. Name: beyond the horizon.mp3
$audioPlayer->play("mp4", "alone.mp4");//Playing vlc file. Name: alone.mp4
$audioPlayer->play("vlc", "far far away.vlc");//Playing vlc file. Name: far far away.vlc
$audioPlayer->play("avi", "mind me.avi");//Invalid media. Avi format not supported
