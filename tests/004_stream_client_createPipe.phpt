--TEST--
Test for StreamClient::createPipe()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\StreamClient;

$client = new StreamClient(null);
$streams = $client->createPipe(false);
var_dump($streams[1]->send("data"));
var_dump($streams[0]->recv());
?>
--EXPECTF--
bool(true)
string(4) "data"
