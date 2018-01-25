<?php 

$yac = new yac();

$yac->set('test', 'test');

echo $yac->get('test');

$yac->set(array('ta'=>'ta','tb'=>'tb'));

print_r($yac->get(array('ta','tb')));

echo $yac->get('ta');
echo $yac->get('tb');