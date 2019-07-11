<?php

define("ROOT_DIR", dirname(dirname(dirname(__DIR__))));

chdir(ROOT_DIR);

spl_autoload_register(function($class){
    //echo strtolower($class).".php"."\n";
    if (is_file(ROOT_DIR."/src/app/Lib/".$class.".php")) {
        include ROOT_DIR."/src/app/Lib/".$class.".php";
    }
    if (is_file(ROOT_DIR."/src/run/server/".strtolower($class).".php")) {
        include ROOT_DIR."/src/run/server/".strtolower($class).".php";
    }
});

try {
    $spiderman = new Spiderman();
    $spiderman->run();
} catch (Exception $e) {
    echo $e->__toString();
    exit();
}