<?php 
/**
 * @name 建造者模式 
 * 建造者模式（Builder Pattern）使用多个简单的对象一步一步构建成一个复杂的对象。
 * 这种类型的设计模式属于创建型模式，它提供了一种创建对象的最佳方式。
 * 一个 Builder 类会一步一步构造最终的对象。该 Builder 类是独立于其他对象的。
 * @desc 
 * 意图：将一个复杂的构建与其表示相分离，使得同样的构建过程可以创建不同的表示。
 * 主要解决：主要解决在软件系统中，有时候面临着"一个复杂对象"的创建工作，其通常由各个部分的子对象用一定的算法构成；
 * 由于需求的变化，这个复杂对象的各个部分经常面临着剧烈的变化，但是将它们组合在一起的算法却相对稳定。
 * 何时使用：一些基本部件不会变，而其组合经常变化的时候。
 * 如何解决：将变与不变分离开。
 * 关键代码：建造者：创建和提供实例，导演：管理建造出来的实例的依赖关系。
 * 实现：
 * 我们假设一个快餐店的商业案例，其中，一个典型的套餐可以是一个汉堡（Burger）和一杯冷饮（Cold drink）。
 * 汉堡（Burger）可以是素食汉堡（Veg Burger）或鸡肉汉堡（Chicken Burger），它们是包在纸盒中。
 * 冷饮（Cold drink）可以是可口可乐（coke）或百事可乐（pepsi），它们是装在瓶子中。
 * 我们将创建一个表示食物条目（比如汉堡和冷饮）的 Item 接口和实现 Item 接口的实体类，以及一个表示食物包装的 Packing 接口和实现 Packing 接口
 * 的实体类，汉堡是包在纸盒中，冷饮是装在瓶子中。
 * 然后我们创建一个 Meal 类，带有 Item 的 ArrayList 和一个通过结合 Item 来创建不同类型的 Meal 对象的 MealBuilder。BuilderPatternDemo，
 * 我们的演示类使用 MealBuilder 来创建一个 Meal。
 */

/** 
 * 步骤 1
 * 创建一个表示事物条目和食物包装的接口
 */
interface Item
{
    public function name();
    public function packing();
    public function price();
}

interface Packing
{
    public function pack();
}

/**
 * 步骤2
 * 创建实现Packing接口的实例类
 */
class Wrapper implements Packing
{
    public function pack()
    {
        return "Wrapper";
    }
}

class Bottle implements Packing
{
    public function pack()
    {
        return "Bottle";
    }
}

/**
 * 步骤3
 * 创建实现Item接口的抽象类，该类提供了默认的功能
 */
abstract class Burger implements Item
{
    public function packing()
    {
        return new Wrapper();
    }
}

abstract class ColdDrink implements Item
{
    public function packing()
    {
        return new Bottle();
    }
}

/**
 * 步骤4
 * 创建扩展了Burger个ColdDrink的实体类
 */
class VegBurger extends Burger
{
    public function price()
    {
        return "25.0";
    }
    
    public function name()
    {
        return "Ver Burger";
    }
}

class ChickenBurger extends Burger
{
    public function price()
    {
        return "50.0";
    }

    public function name()
    {
        return "Chicken Burger";
    }
}

class Coke extends ColdDrink
{
    public function price()
    {
        return "30.0";
    }
    
    public function name()
    {
        return "Coke";
    }
}

class Pepsi extends ColdDrink
{
    public function price()
    {
        return "35.0";
    }

    public function name()
    {
        return "Pepsi";
    }
}

/**
 * 步骤5
 * 创建一个Meal类，带有上面定义的Item对象
 */
class Meal
{
    private $_item = array();
    
    public function addItem(Item $item)
    {
        $this->_item[] = $item;
    }
    
    public function getCost()
    {
        $cost = 0;
        foreach ($this->_item as $item){
            $cost += $item->price();
        }
        return $cost;
    }
    
    public function showItems()
    {
        foreach ($this->_item as $item){
            echo "Item : ".$item->name();
            echo ", Packing : ".$item->packing()->pack();
            echo ", Price : ".$item->price();
        }
    }
}

/**
 * 步骤6
 * 创建一个MealBuilder类，实际的builder类复杂创建Meal对象
 */
class MealBuilder
{
    public function prepareVegMeal()
    {
        $meal = new Meal();
        $meal->addItem(new VegBurger());
        $meal->addItem(new Coke());
        return $meal;
    }

    public function prepareNonVegMeal()
    {
        $meal = new Meal();
        $meal->addItem(new ChickenBurger());
        $meal->addItem(new Pepsi());
        return $meal;
    }
}

/**
 * 步骤7
 * 使用MealBuilder演示建造者模式
 */
$mealBuilder = new MealBuilder();
$vegMeal = $mealBuilder->prepareVegMeal();
echo "Ver Meal";//Ver Meal
$vegMeal->showItems();//Item : Ver Burger, Packing : Wrapper, Price : 25.0Item : Coke, Packing : Bottle, Price : 30.0
echo "Total Cost: ".$vegMeal->getCost();//Total Cost: 55

$nonVegMeal = $mealBuilder->prepareNonVegMeal();
echo "Non-Veg Meal";//Non-Veg Meal
$nonVegMeal->showItems();//Item : Chicken Burger, Packing : Wrapper, Price : 50.0Item : Pepsi, Packing : Bottle, Price : 35.0
echo "Total Cost: ".$nonVegMeal->getCost();//Total Cost: 85
