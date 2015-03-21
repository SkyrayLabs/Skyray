--TEST--
Test for StreamClient::connectTCP()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\StreamClient;

$client = new StreamClient(null);
$stream = $client->connectTCP('122.0.0.1', 8080);
var_dump($stream);
?>
--EXPECTF--
object(skyray\core\Stream)#2 (0) {
}
