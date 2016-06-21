<?php 
/**
 * @name 解释器模式 
 * 解释器模式（Interpreter Pattern）提供了评估语言的语法或表达式的方式，它属于行为型模式。
 * 这种模式实现了一个表达式接口，该接口解释一个特定的上下文。这种模式被用在 SQL 解析、符号处理引擎等。
 * @desc 
 * 意图：给定一个语言，定义它的文法表示，并定义一个解释器，这个解释器使用该标识来解释语言中的句子。
 * 主要解决：对于一些固定文法构建一个解释句子的解释器。
 * 何时使用：如果一种特定类型的问题发生的频率足够高，那么可能就值得将该问题的各个实例表述为一个简单语言中的句子。
 * 这样就可以构建一个解释器，该解释器通过解释这些句子来解决该问题。
 * 如何解决：构件语法树，定义终结符与非终结符。
 * 关键代码：构件环境类，包含解释器之外的一些全局信息，一般是 HashMap。
 * 应用实例：编译器、运算表达式计算。
 * 优点： 1、可扩展性比较好，灵活。 2、增加了新的解释表达式的方式。 3、易于实现简单文法。
 * 缺点： 1、可利用场景比较少。 2、对于复杂的文法比较难维护。 3、解释器模式会引起类膨胀。 4、解释器模式采用递归调用方法。
 * 使用场景： 
 * 1、可以将一个需要解释执行的语言中的句子表示为一个抽象语法树。 
 * 2、一些重复出现的问题可以用一种简单的语言来进行表达。 
 * 3、一个简单语法需要解释的场景。
 */

interface Expression
{
    public function interpret($string);
}

class TerminalExpression implements Expression
{
    private $_data;
    
    public function __construct($data)
    {
        $this->_data = $data;
    }
    
    public function interpret($string)
    {
        if(strpos($string, $this->_data) !== false){
            return true;
        }
        return false;
    }
}

class OrExpression implements Expression
{
    private $_exp1;
    private $_exp2;
    
    public function __construct(Expression $exp1, Expression $exp2){
        $this->_exp1 = $exp1;
        $this->_exp2 = $exp2;
    }
    
    public function interpret($string)
    {
        return $this->_exp1->interpret($string) || $this->_exp2->interpret($string);
    }
}

class AndExpression implements Expression
{
    private $_exp1;
    private $_exp2;
    
    public function __construct(Expression $exp1, Expression $exp2){
        $this->_exp1 = $exp1;
        $this->_exp2 = $exp2;
    }
    
    public function interpret($string)
    {
        return $this->_exp1->interpret($string) && $this->_exp2->interpret($string);
    }
}

class Interpreter
{
    //规则：Robert 和 John 是男性
    public function getMaleExpression()
    {
        $robert = new TerminalExpression('Robert');
        $john = new TerminalExpression('John');
        return new OrExpression($robert, $john);
    }
    
    //规则：Julie 是一个已婚的女性
    public function getMarriedWommanExpression()
    {
        $julie = new TerminalExpression('Julie');
        $married = new TerminalExpression('Married');
        return new AndExpression($julie, $married);
    }
}

$interpreter = new Interpreter();
$isMale = $interpreter->getMaleExpression()->interpret('John');
$isMarried = $interpreter->getMarriedWommanExpression()->interpret('Married Julie');

echo "John is male? ".printf("%s", $isMale);
echo "Julie is a married women? ".printf("%s", $isMarried);