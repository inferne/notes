<?php


namespace Wotte\Interfaces
{
    interface Log
    {
        public function write();
    }
}

namespace Wotte\Contracts
{
    use Wotte\Interfaces\Log as LogInterface;
    
    abstract class Log implements LogInterface
    {
        abstract public function write();
    }
}

namespace Wotte\Foundation
{
    use Wotte\Contracts\Log as LogAbstract;
    
    class Log extends LogAbstract
    {
        public function write()
        {
            echo 1;
        }
    }
}

namespace
{
    require __DIR__ . '/vendor/autoload.php';

    $app = new Illuminate\Foundation\Application();
    $app->bind('Wotte\Interfaces\Log', 'Wotte\Contracts\Log'); 
    $app->bind('Wotte\Contracts\Log', 'Wotte\Foundation\Log');
    $obj = $app->make('Wotte\Interfaces\Log');
    $obj->write();
}

