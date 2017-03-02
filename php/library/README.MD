基于stream_socket_client的http client</br>
支持http长连接</br>
自动根据服务端连接类型选择合适的连接类型</br>

## StreamHttpClient.php
1. 支持的方法
   * get()
      * send a get request
   * post()
      * send a post request
      
## demo
```php
$url = "http://http://xiaoyblog.sinaapp.com/";

$http_client = new StreamHttpClient($url);
$result = $http_client->get($params);
echo $result;

$result = $http_client->post($params);
echo $result;
```
