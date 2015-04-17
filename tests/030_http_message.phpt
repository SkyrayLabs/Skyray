--TEST--
Test for http message
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Message;

$message = new Message();
var_dump(get_class($message));
var_dump($message->getProtocolVersion());

$newVersion = $message->setProtocolVersion("1.1")->getProtocolVersion();
var_dump($newVersion);

$message->addHeader('HOST', 'http://php.net')->addHeader("Host", "http://php.net");
var_dump($message->getHeader('host'));

$message->removeHeader('host');
var_dump($message->getHeaders());

$message->addHeader('name', "value")->addHeader('Name', 'value2');
var_dump($message->getHeaders());
var_dump($message->hasHeader('NAME'));
?>
--EXPECTF--
string(19) "skyray\http\Message"
NULL
string(3) "1.1"
array(2) {
  [0]=>
  string(14) "http://php.net"
  [1]=>
  string(14) "http://php.net"
}
array(0) {
}
array(1) {
  ["Name"]=>
  array(2) {
    [0]=>
    string(5) "value"
    [1]=>
    string(6) "value2"
  }
}
bool(true)
