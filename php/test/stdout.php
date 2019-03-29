<?php

function test(){
    $file = '/tmp/std.log';
    global $STDOUT, $STDERR;
    $handle = fopen($file, "a");
    if ($handle) {
        unset($handle);
        @fclose(STDOUT);
        @fclose(STDERR);
        $STDOUT = fopen($file, "a");
        $STDERR = fopen($file, "a");
    } else {
        throw new Exception('can not open stdoutFile ' . $file);
    }
}

test();
print_r($_SERVER);  