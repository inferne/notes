<?php 
/**
 * @name 过滤器模式 
 * 过滤器模式（Filter Pattern）或标准模式（Criteria Pattern）是一种设计模式，这种模式允许开发人员使用不同的标准来过滤一组对象，
 * 通过逻辑运算以解耦的方式把它们连接起来。这种类型的设计模式属于结构型模式，它结合多个标准来获得单一标准。
 * @desc 
 * 实现
 * 我们将创建一个 Person 对象、Criteria 接口和实现了该接口的实体类，来过滤 Person 对象的列表。
 */

/** 
 * 步骤 1
 * 创建一个类，在该类上应用标准。
 */
class Person
{
    private $_name;
    private $_gender;
    private $_maritalStatus;
    
    public function __construct($name, $gender, $maritalStatus)
    {
        $this->_name = $name;
        $this->_gender = $gender;
        $this->_maritalStatus = $maritalStatus;
    }

    public function getName()
    {
        return $this->_name;
    }
    
    public function getGender()
    {
        return $this->_gender;
    }
    
    public function getMaritalStatus()
    {
        return $this->_maritalStatus;
    }
}

/**
 * 步骤 2
 * 为标准（Criteria）创建一个接口。
 */
interface Criteria
{
    public function meetCriteria($persons);//标准满足条件
}

/**
 * 步骤3
 * 创建实现了Criteria接口的实体类
 */
class CriteriaMale implements Criteria
{
    public function meetCriteria($persons)
    {
        $malePersons = array();
        foreach ($persons as $person){
            if($person->getGender() == "Male"){
                $malePersons[] = $person;
            }
        }
        return $malePersons;
    }
}

class CriteriaFemale implements Criteria
{
    public function meetCriteria($persons)
    {
        $femalePersons = array();
        foreach ($persons as $person){
            if($person->getGender() == "Female"){
                $femalePersons[] = $person;
            }
        }
        return $femalePersons;
    }
}

class CriteriaSingle implements Criteria
{
    public function meetCriteria($persons)
    {
        $singlePersions = array();
        foreach ($persons as $person){
            if($person->getMaritalStatus() == "Single"){
                $singlePersions[] = $person;
            }
        }
        return $singlePersions;
    }
}

/**
 * 两个标准类与的集合
 */
class AndCriteria implements Criteria
{
    private $_criteria;
    private $_otherCriteria;
    
    public function __construct(Criteria $criteria, Criteria $otherCriteria)
    {
        $this->_criteria = $criteria;
        $this->_otherCriteria = $otherCriteria;
    }
    
    public function meetCriteria($persons)
    {
        $firstCriteriaPersons = $this->_criteria->meetCriteria($persons);
        return $this->_otherCriteria->meetCriteria($firstCriteriaPersons);
    }
}

/**
 * 两个标准类或的集合
 */
class OrCriteria implements Criteria
{
    private $_criteria;
    private $_otherCriteria;
    
    public function __construct(Criteria $criteria, Criteria $otherCriteria)
    {
        $this->_criteria = $criteria;
        $this->_otherCriteria = $otherCriteria;
    }
    
    public function meetCriteria($persons)
    {
        $firstCriteriaItems = $this->_criteria->meetCriteria($persons);
        $otherCriteriaItems = $this->_otherCriteria->meetCriteria($persons);
        
        foreach ($persons as $person){
            if(!in_array($person, $firstCriteriaItems)){
                $firstCriteriaItems[] = $person;
            }
        }
        return $firstCriteriaItems;
    }
}

/**
 * 步骤4
 * 使用不同的标准（Criteria）和它们的结合来过滤 Person 对象的列表。
 */
function printPersons($persons)
{
    foreach ($persons as $person){
        echo "Person : [ Name : ".$person->getName()
        .", Gender : ".$person->getGender()
        .", Marital Status : ".$person->getMaritalStatus()." ]<br/>";
    }
}

$persons = array();
$persons[] = new Person("Robert","Male", "Single");
$persons[] = new Person("John","Male", "Married");
$persons[] = new Person("Laura","Female", "Married");
$persons[] = new Person("Diana","Female", "Single");
$persons[] = new Person("Mike","Male", "Single");
$persons[] = new Person("Bobby","Male", "Single");

$male = new CriteriaMale();
$female = new CriteriaFemale();
$single = new CriteriaSingle();
$singleMale = new AndCriteria($single, $male);
$singleOrFemale = new OrCriteria($single, $female);

echo "Males:<br/>";
printPersons($male->meetCriteria($persons));

echo "Females:<br/>";
printPersons($female->meetCriteria($persons));

echo "Single:<br/>";
printPersons($single->meetCriteria($persons));

echo "Single Male:<br/>";
printPersons($singleMale->meetCriteria($persons));

echo "Single Or Females:<br/>";
printPersons($singleOrFemale->meetCriteria($persons));

/**
 * 输出
 */
// Males:
// Person : [ Name : Robert, Gender : Male, Marital Status : Single ]
// Person : [ Name : John, Gender : Male, Marital Status : Married ]
// Person : [ Name : Mike, Gender : Male, Marital Status : Single ]
// Person : [ Name : Bobby, Gender : Male, Marital Status : Single ]
// Females:
// Person : [ Name : Laura, Gender : Female, Marital Status : Married ]
// Person : [ Name : Diana, Gender : Female, Marital Status : Single ]
// Single:
// Person : [ Name : Robert, Gender : Male, Marital Status : Single ]
// Person : [ Name : Diana, Gender : Female, Marital Status : Single ]
// Person : [ Name : Mike, Gender : Male, Marital Status : Single ]
// Person : [ Name : Bobby, Gender : Male, Marital Status : Single ]
// Single Male:
// Person : [ Name : Robert, Gender : Male, Marital Status : Single ]
// Person : [ Name : Mike, Gender : Male, Marital Status : Single ]
// Person : [ Name : Bobby, Gender : Male, Marital Status : Single ]
// Single Or Females:
// Person : [ Name : Robert, Gender : Male, Marital Status : Single ]
// Person : [ Name : Diana, Gender : Female, Marital Status : Single ]
// Person : [ Name : Mike, Gender : Male, Marital Status : Single ]
// Person : [ Name : Bobby, Gender : Male, Marital Status : Single ]
// Person : [ Name : John, Gender : Male, Marital Status : Married ]
// Person : [ Name : Laura, Gender : Female, Marital Status : Married ]
