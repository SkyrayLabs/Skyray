--TEST--
Test for skyray\http\ServerProtocol basic
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Request;
use skyray\http\ServerProtocol;

class MyProtocol extends ServerProtocol
{
    public function onRequest($request)
    {
        echo "====== request =======" . PHP_EOL;
        echo "{$request->getMethod()} {$request->getUri()} HTTP/{$request->getProtocolVersion()}" . PHP_EOL;
        echo "====== headers =======" . PHP_EOL;
        var_dump($request->getHeaders());
        echo "====== body =======" . PHP_EOL;
        echo $request->getRawBody() . PHP_EOL;
    }
}
$protocol = new MyProtocol();
/* Test for partial body */
$protocol->dataReceived("POST /index.php HTTP/1.1\r\n");
$protocol->dataReceived("Content-Length: 8\r\n\r\n");
$protocol->dataReceived("data");
$protocol->dataReceived("data");
$protocol->dataReceived("\r\n\r\n");

/* Test for partial uri and headers */
$protocol->dataReceived("GET /index");
$protocol->dataReceived(".php HTTP/1.1\r\n");
$protocol->dataReceived("Hos");
$protocol->dataReceived("t: php");
$protocol->dataReceived(".net\r\n");
$protocol->dataReceived("\r\n");

?>
--EXPECTF--
====== request =======
POST /index.php HTTP/1.1
====== headers =======
array(1) {
  ["Content-Length"]=>
  array(1) {
    [0]=>
    string(1) "8"
  }
}
====== body =======
datadata
====== request =======
GET /index.php HTTP/1.1
====== headers =======
array(1) {
  ["Host"]=>
  array(1) {
    [0]=>
    string(7) "php.net"
  }
}
====== body =======


