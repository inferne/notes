<?php 
/**
 * 省略了信号处理
 * 每次启动得手动删除server.sock文件
 * @var unknown
 */
$server_sock = __DIR__."/server.sock";

// //server 1 TCP
$sock = socket_create(AF_UNIX, SOCK_STREAM, 0);

socket_set_option($sock, SOL_SOCKET, SO_REUSEADDR, 1);//没作用

if(!socket_bind($sock, $server_sock)){
    echo socket_last_error();
}
if(!socket_listen($sock)){
    echo socket_last_error();
}

$clients = array($sock);

while (1){
    $read = $clients;
    // if there are no clients with data, go to next iteration
    if(socket_select($read, $write, $except, 0) < 1){
        continue;
    }
    echo "--------------------------------------------------------------------------------------\n";
    echo "all socket\n";
    print_r($clients);
    echo "read socket\n";
    print_r($read);
    // check if there is a client trying to connect
    if(in_array($sock, $read)){
        // accept the client, and add him to the $clients array
        $clients[] = $newsock = socket_accept($sock);
        
        // send the client a welcome message
        socket_write($newsock, "no noobs, but ill make an exception :) There are ".(count($clients) - 1)." client(s) connected to the server\n");
        
        socket_getpeername($newsock, $ip);
        echo "New client connected: {$ip}\n";
        
        // remove the listening socket from the clients-with-data array
        $key = array_search($sock, $read);
        unset($read[$key]);
    }
    
    // loop through all the clients that have data to read from
    foreach ($read as $read_sock){
        // read until newline or 1024 bytes
        // socket_read while show errors when the client is disconnected, so silence the error messages
        $data = @socket_read($read_sock, 1024, PHP_NORMAL_READ);
        // check if the client is disconnected
        if ($data === false) {
            // remove client for $clients array
            $key = array_search($read_sock, $clients);
            unset($clients[$key]);
            echo "client disconnected.\n";
            // continue to the next client to read from, if any
            continue;
        }
        echo "Read $data from $read_sock\n";
        
        // trim off the trailing/beginning white spaces
        $data = trim($data);
        
        // check if there is any data after trimming off the spaces
        if (!empty($data)) {
        
            // send this to all the clients in the $clients array (except the first one, which is a listening socket)
            foreach ($clients as $send_sock) {
        
                // if its the listening sock or the client that we got the message from, go to the next one in the list
                if ($send_sock == $sock || $send_sock == $read_sock)
                    continue;
        
                // write the message to the client -- add a newline character to the end of the message
                socket_write($send_sock, $data."\n");
                echo "Send $data to $send_sock\n";
            } // end of broadcast foreach
        }
    } // end of reading foreach
}

// close the listening socket
socket_close($sock);
