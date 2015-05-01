--TEST--
Test for skyray\http\Request class
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Request;

$request = new Request();
var_dump($request->getMethod()); // GET method by default
var_dump($request->getCookieParams()); // default to []
var_dump($request->getQueryParams()); // default to []

$request->setMethod('POST')
    ->setCookieParams([1])
    ->setQueryParams([2]);

var_dump($request->getMethod());
var_dump($request->getCookieParams());
var_dump($request->getQueryParams());

echo '=== Testing pull query params from request uri ===' . PHP_EOL;
$request = new Request();
$request->setUri('/foo?foo=bar&bar=foo');
var_dump($request->getQueryParams());

echo '=== Testing pull cookie params from request header ===' . PHP_EOL;
$request = new Request();
$request->addHeader('Cookie', 'aaa=bbb; bbb=ccc');
$request->addHeader('Cookie', 'eee=fff');
var_dump($request->getCookieParams());
?>
--EXPECTF--
string(3) "GET"
array(0) {
}
array(0) {
}
string(4) "POST"
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
=== Testing pull query params from request uri ===
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "foo"
}
=== Testing pull cookie params from request header ===
array(3) {
  ["aaa"]=>
  string(3) "bbb"
  ["bbb"]=>
  string(3) "ccc"
  ["eee"]=>
  string(3) "fff"
}
