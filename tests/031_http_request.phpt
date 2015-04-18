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
