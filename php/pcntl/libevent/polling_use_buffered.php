<?php

function print_line($fd, $arg)
{
    static $max_requests = 0;
    
    $max_requests++;
    
    if ($max_requests == 10){
        // exit loop after 10 writes
        event_base_loopexit($arg);
    }
    
    // print the line
    echo event_buffer_read($fd, 4096);
}

function error_func($buf, $what, $arg)
{
    // handle errors
}

// create base and event
$base = event_base_new();
$eb = event_buffer_new(STDIN, "print_line", NULL, "error_func", $base);

// set event buffer base
event_buffer_base_set($eb, $base);
event_buffer_enable($eb, EV_READ);

// start event loop
event_base_loop($base);

//end