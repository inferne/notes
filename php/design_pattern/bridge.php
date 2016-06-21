<?php 
/**
 * @name 桥接模式
 * 桥接（Bridge）是用于把抽象化与实现化解耦，使得二者可以独立变化。这种类型的设计模式属于结构型模式，
 * 它通过提供抽象化和实现化之间的桥接结构，来实现二者的解耦。
 * 这种模式涉及到一个作为桥接的接口，使得实体类的功能独立于接口实现类。这两种类型的类可被结构化改变而互不影响。
 * 我们通过下面的实例来演示桥接模式（Bridge Pattern）的用法。其中，可以使用相同的抽象类方法但是不同的桥接实现类，来画出不同颜色的圆。
 * @desc 
 * 意图：将抽象部分与实现部分分离，使它们都可以独立的变化。
 * 主要解决：在有多种可能会变化的情况下，用继承会造成类爆炸问题，扩展起来不灵活。
 * 何时使用：实现系统可能有多个角度分类，每一种角度都可能变化。
 * 如何解决：把这种多角度分类分离出来，让它们独立变化，减少它们之间耦合。
 * 关键代码：抽象类依赖实现类。
 * 应用实例： 1、猪八戒从天蓬元帅转世投胎到猪，转世投胎的机制将尘世划分为两个等级，即：灵魂和肉体，前者相当于抽象化，后者相当于实现化。
 * 生灵通过功能的委派，调用肉体对象的功能，使得生灵可以动态地选择。 
 * 2、墙上的开关，可以看到的开关是抽象的，不用管里面具体怎么实现的。
 * 优点： 1、抽象和实现的分离。 2、优秀的扩展能力。 3、实现细节对客户透明。
 * 缺点：桥接模式的引入会增加系统的理解与设计难度，由于聚合关联关系建立在抽象层，要求开发者针对抽象进行设计与编程。
 */

/** 
 * 步骤 1
 * 创建桥接实现接口
 */
interface DrawAPI
{
    public function drawCircle($radius, $x, $y);
}

/**
 * 步骤2
 * 创建实现了DrawAPI接口的实体桥接实现类
 */
class RedCircle implements DrawAPI
{
    public function drawCircle($radius, $x, $y)
    {
        echo "Drawing Circle[ color: red, radius: " . $radius . ", x: " . $x . ", " . $y . "]";
    }
}

class GreenCircle implements DrawAPI
{
    public function drawCircle($radius, $x, $y)
    {
        echo "Drawing Circle[ color: green, radius: " . $radius . ", x: " . $x . ", " . $y . "]";
    }
}

/**
 * 步骤3
 * 使用DrawAPI接口创建抽象类Shape
 */
abstract class Shape
{
    protected $_drawAPI;
    
    protected function __construct(DrawAPI $drawAPI)
    {
        $this->_drawAPI = $drawAPI;
    }
    
    public function draw(){}
}

/**
 * 步骤4
 * 创建实现了Shape接口的实体类
 */
class Circle extends Shape
{
    private $_x, $_y, $_radius;
    
    public function __construct($x, $y, $radius, DrawAPI $drawAPI)
    {
        parent::__construct($drawAPI);
        $this->_x = $x;
        $this->_y = $y;
        $this->_radius = $radius;
    }
    
    public function draw()
    {
        $this->_drawAPI->drawCircle($this->_radius, $this->_x, $this->_y);
    }
}

/**
 * 步骤 5
 * 使用 Shape 和 DrawAPI 类画出不同颜色的圆。
 */
$redCircle = new Circle(100,100, 10, new RedCircle());
$redCircle->draw();//Drawing Circle[ color: red, radius: 10, x: 100, 100]

$greenCircle = new Circle(100,100, 10, new GreenCircle());
$greenCircle->draw();//Drawing Circle[ color: green, radius: 10, x: 100, 100]
