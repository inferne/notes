
PHP_MINIT_FUNCTION(myphpextension)
{
    //注册常量或者类初始化操作
    return SUCCESS;
}

PHP_RINIT_FUNCTION(myphpextension)
{
    //例如记录请求开始时间
    //随后在请求结束的时候记录请求结束时间。这样我们就能记录下处理请求所花费的时间了
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(myphpextension)
{
    //例如记录请求结束时间，并把相应的信息写入到日志文件中。
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(myphpextension)
{
    return SUCCESS;
}