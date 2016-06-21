<?php 
/**
 * 攻击的原理很简单, 目前很多语言, 使用hash来存储k-v数据, 包括常用的来自用户的POST数据, 
 * 攻击者可以通过构造请求头, 并伴随POST大量的特殊的”k”值(根据每个语言的Hash算法不同而定制), 
 * 使得语言底层保存POST数据的Hash表因为”冲突”(碰撞)而退化成链表.
 * 
 * 在PHP中,如果键值是数字, 那么Hash的时候就是数字本身, 一般的时候都是, index & tableMask. 
 * 而tableMask是用来保证数字索引不会超出数组可容纳的元素个数值, 也就是数组个数-1.
 * PHP的Hashtable的大小都是2的指数, 比如如果你存入10个元素的数组, 那么数组实际大小是16, 如果存入20个, 
 * 则实际大小为32, 而63个话, 实际大小为64. 当你的存入的元素个数大于了数组目前的最多元素个数的时候, PHP会对这个数组进行扩容, 并且从新Hash.
 * 现在, 我们假设要存入64个元素(中间可能会经过扩容, 但是我们只需要知道, 最后的数组大小是64, 并且对应的tableMask为63:0111111), 
 * 那么如果第一次我们存入的元素的键值为0, 则hash后的值为0, 第二次我们存入64, hash(1000000 & 0111111)的值也为0, 
 * 第三次我们用128, 第四次用192… 就可以使得底层的PHP数组把所有的元素都Hash到0号bucket上, 从而使得Hash表退化成链表了.
 * 
 * 具体参见 http://www.laruence.com/2011/12/30/2435.html
 */

$size = pow(2, 16);
$startTime = microtime(true);
$array = array();
for ($key = 0, $maxKey = ($size - 1) * $size; $key <= $maxKey; $key += $size) {
    //echo $size,' ',$maxKey,' ',$key,'<br/>';
    $array[$key] = 0;
}
$endTime = microtime(true);
echo '插入 ', $size, ' 个恶意的元素需要 ', $endTime - $startTime, ' 秒', "\n";

$startTime = microtime(true);
$array = array();
for ($key = 0, $maxKey = $size - 1; $key <= $maxKey; ++$key) {
    $array[$key] = 0;
}
$endTime = microtime(true);
echo '插入 ', $size, ' 个普通元素需要 ', $endTime - $startTime, ' 秒', "\n";
