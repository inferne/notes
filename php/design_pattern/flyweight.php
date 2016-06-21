<?php 
/**
 * @name 享元模式
 * 享元模式（Flyweight Pattern）主要用于减少创建对象的数量，以减少内存占用和提高性能。
 * 这种类型的设计模式属于结构型模式，它提供了减少对象数量从而改善应用所需的对象结构的方式。
 * 享元模式尝试重用现有的同类对象，如果未找到匹配的对象，则创建新对象。我们将通过创建 5 个对象来画出 20 个分布于不同位置的圆来演示这种模式。.
 * 由于只有 5 种可用的颜色，所以 color 属性被用来检查现有的 Circle 对象。
 * @desc 
 * 意图：运用共享技术有效地支持大量细粒度的对象。
 * 主要解决：在有大量对象时，有可能会造成内存溢出，我们把其中共同的部分抽象出来，如果有相同的业务请求，直接返回在内存中已有的对象，避免重新创建。
 * 何时使用： 
 * 1、系统中有大量对象。 
 * 2、这些对象消耗大量内存。 
 * 3、这些对象的状态大部分可以外部化。 
 * 4、这些对象可以按照内蕴状态分为很多组，当把外蕴对象从对象中剔除出来时，每一组对象都可以用一个对象来代替。 
 * 5、系统不依赖于这些对象身份，这些对象是不可分辨的。
 * 如何解决：用唯一标识码判断，如果在内存中有，则返回这个唯一标识码所标识的对象。
 * 关键代码：用 HashMap 存储这些对象。
 * 应用实例： 数据库的数据池。
 * 优点：大大减少对象的创建，降低系统的内存，使效率提高。
 * 缺点：提高了系统的负杂度，需要分离出外部状态和内部状态，而且外部状态具有固有化的性质，不应该随着内部状态的变化而变化，否则会造成系统的混乱。
 * 使用场景： 1、系统有大量相似对象。 2、需要缓冲池的场景。
 * 注意事项： 1、注意划分外部状态和内部状态，否则可能会引起线程安全问题。 2、这些类必须有一个工厂对象加以控制。
 */

/** 
 * 步骤 1
 * 创建一个接口。
 */
interface Shape
{
    public function draw();
}

/**
 * 步骤 2
 * 创建实现接口的实体类。
 */
class Circle implements Shape
{
    private $_color;
    private $_x, $_y, $_radius;
    
    public function __construct($color)
    {
        $this->_color = $color;
    }

    public function setX($x)
    {
        $this->_x = $x;
    }
    
    public function setY($y)
    {
        $this->_y = $y;
    }
    
    public function setRadius($radius)
    {
        $this->_radius = $radius;
    }
    
    public function draw()
    {
        echo "Circle: Draw() [Color : ".$this->_color.", x : ".$this->_x.", y :".$this->_y.", radius :".$this->_radius;
    }
}

/**
 * 步骤 3
 * 创建一个工厂，生成基于给定信息的实体类的对象。
 */
class ShapeFactory
{
    private static $_circleMap = array();
    
    public static function getCircle($color)
    {
        $key = sprintf("%u", crc32($color));
        $circle = @self::$_circleMap[$key];
        //var_dump($circle);
        if($circle == null){
            $circle = new Circle($color);
            self::$_circleMap[$key] = $circle;
            echo "Creating circle of color : ".$color;
        }
        return $circle;
    }
}

/**
 * 步骤 4
 * 使用该工厂，通过传递颜色信息来获取实体类的对象。
 */
$colors = array("Red", "Green", "Blue", "White", "Black");

for($i=0; $i<20; $i++){
    $color = $colors[rand(0, count($colors)-1)];
    $circle = ShapeFactory::getCircle($color);
    $circle->setX(rand(0, 100));
    $circle->setY(rand(0, 100));
    $circle->setRadius(100);
    $circle->draw();
    echo "<br/>";
}

// Creating circle of color : BlueCircle: Draw() [Color : Blue, x : 78, y :1, radius :100
// Creating circle of color : RedCircle: Draw() [Color : Red, x : 10, y :60, radius :100
// Creating circle of color : WhiteCircle: Draw() [Color : White, x : 98, y :74, radius :100
// Circle: Draw() [Color : Red, x : 76, y :44, radius :100
// Circle: Draw() [Color : White, x : 97, y :53, radius :100
// Circle: Draw() [Color : White, x : 5, y :95, radius :100
// Circle: Draw() [Color : Blue, x : 39, y :92, radius :100
// Circle: Draw() [Color : Blue, x : 84, y :80, radius :100
// Creating circle of color : GreenCircle: Draw() [Color : Green, x : 83, y :5, radius :100
// Circle: Draw() [Color : White, x : 26, y :17, radius :100
// Circle: Draw() [Color : Green, x : 11, y :94, radius :100
// Circle: Draw() [Color : Blue, x : 12, y :95, radius :100
// Circle: Draw() [Color : Blue, x : 52, y :64, radius :100
// Circle: Draw() [Color : Green, x : 1, y :95, radius :100
// Circle: Draw() [Color : Blue, x : 5, y :60, radius :100
// Circle: Draw() [Color : Green, x : 65, y :26, radius :100
// Circle: Draw() [Color : Red, x : 69, y :82, radius :100
// Circle: Draw() [Color : White, x : 7, y :42, radius :100
// Circle: Draw() [Color : Red, x : 61, y :16, radius :100
// Circle: Draw() [Color : Blue, x : 95, y :68, radius :100
