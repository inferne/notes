<?php 
/**
 * @name 抽象工厂模式 
 * 抽象工厂模式（Abstract Factory Pattern）是围绕一个超级工厂创建其他工厂。该超级工厂又称为其他工厂的工厂。这种类型的
 * 设计模式属于创建型模式，它提供了一种创建对象的最佳方式。
 * 在抽象工厂模式中，接口是负责创建一个相关对象的工厂，不需要显式指定它们的类。每个生成的工厂都能按照工厂模式提供对象。
 * @desc 
 * 意图：提供一个创建一系列相关或相互依赖对象的接口，而无需指定它们具体的类。
 * 主要解决：主要解决接口选择的问题。
 * 何时使用：系统的产品有多于一个的产品族，而系统只消费其中某一族的产品。
 * 如何解决：在一个产品族里面，定义多个产品。
 * 关键代码：在一个工厂里聚合多个同类产品。
 */

/** 
 * 步骤 1
 * 创建一个接口
 */
interface Shape
{
    public function draw();
}

/** 
 * 步骤 2
 * 创建实现接口的实体类。
 */
class Rectangle implements Shape
{
    public function draw()
    {
        echo "this is Rectangle::draw method.";
    }
}

class Square implements Shape
{
    public function draw()
    {
        echo "this is Square::draw method.";
    }
}

class Circle implements Shape
{
    public function draw()
    {
        echo "this is Circle::draw method.";
    }
}

/**
 * 步骤 3
 * 创建另一个接口。
 */
interface Color
{
    public function fill();
}

/**
 * 步骤 4
 * 实现颜色接口
 */
class Red implements Color
{
    public function fill()
    {
        echo "this is Red::fill() method.";
    }
}

class Blue implements Color
{
    public function fill()
    {
        echo "this is Blue::fill() method.";
    }
}

class Green implements Color
{
    public function fill()
    {
        echo "this is Green::fill() method.";
    }
}

/**
 * 步骤 5
 * 创建Shape和Color的工厂类。
 */
class ShapeFactory
{
    public function get($class)
    {
        return new $class();
    }
}

class ColorFactory
{
    public function get($class)
    {
        return new $class();
    }
}

/**
 * 步骤 6
 * 创建AbstractFactory抽象工厂类
 */
class AbstractFactory
{
    public function create($factory)
    {
        $factory .= "Factory";
        return new $factory();
    }
}

$factory = new AbstractFactory();
$factory->create("Shape")->get("Rectangle")->draw();          //this is Rectangle::draw method.
$factory->create("Shape")->get("Square")->draw();             //this is Square::draw method.
$factory->create("Shape")->get("Circle")->draw();             //this is Circle::draw method.
$factory->create("Color")->get("Red")->fill();                //this is Red::fill method.
$factory->create("Color")->get("Blue")->fill();               //this is Blue::fill method.
$factory->create("Color")->get("Green")->fill();              //this is Green::fill method.
