<?php 

$name = "好好学习天天向上";

$name = preg_split("/(?<!^)(?!$)/u", $name);
//$name = mb_split("//", $name);

print_r($name);

echo implode("%", $name);