--TEST--
Test for skyray\http\Message class
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Message;

$message = new Message();
var_dump(get_class($message));
var_dump($message instanceof \skyray\Object);
var_dump($message->getProtocolVersion());

$newVersion = $message->setProtocolVersion("1.1")->getProtocolVersion();
var_dump($newVersion);

$message->addHeader('HOST', 'http://php.net')->addHeader("Host", "http://php.net");
var_dump($message->getHeader('host'));
var_dump($message->getHeader('Host', true));

$message->removeHeader('host');
var_dump($message->getHeaders());

$message->addHeader('name', "value")->addHeader('Name', 'value2');
var_dump($message->getHeaders());
var_dump($message->hasHeader('NAME'));

var_dump($message->getBody()); // default to NULL
var_dump($message->getRawBody()); // default to NULL

$message->setBody([])->setRawBody('');

var_dump($message->getBody());
var_dump($message->getRawBody());

echo "=== parsing raw body json ===\n";
$message = new Message();
$message->setHeader('Content-Type', 'application/json; charset=UTF-8');
$message->setRawBody(json_encode(['foo' => 'bar']));
var_dump($message->getBody());

echo "=== parsing raw body url-encoded ==\n";
$message = new Message();
$message->setHeader('Content-Type', 'application/x-www-form-urlencoded');
$message->setRawBody('foo=bar&bar=foo');
var_dump($message->getBody());

echo "=== parsing raw body unknown ==\n";
$message = new Message();
$message->setHeader('Content-Type', 'application/unknown');
$message->setRawBody('foo=bar&bar=foo');
var_dump($message->getBody());
?>
--EXPECTF--
string(19) "skyray\http\Message"
bool(true)
NULL
string(3) "1.1"
array(2) {
  [0]=>
  string(14) "http://php.net"
  [1]=>
  string(14) "http://php.net"
}
string(14) "http://php.net"
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
NULL
NULL
array(0) {
}
string(0) ""
=== parsing raw body json ===
array(1) {
  ["foo"]=>
  string(3) "bar"
}
=== parsing raw body url-encoded ==
array(2) {
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "foo"
}
=== parsing raw body unknown ==
string(15) "foo=bar&bar=foo"
