--TEST--
Test for skyray\stream\Stream::getPeerName() & Stream::getSockName()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\stream\Client;

$server = require_once __DIR__ . '/includes/ServerProcess.php';
register_shutdown_function(function () use ($server) {
    $server->stop();
});

$client = new Client(null);
$stream = $client->connectTCP('127.0.0.1', 2333);
echo 'peer name:' . PHP_EOL;
var_dump($stream->getPeerName());
echo 'sock name:' . PHP_EOL;
var_dump($stream->getSockName());
$stream->close();
echo "==== done ====\n\n";
?>
--EXPECTF--
peer name:
array(3) {
  ["type"]=>
  string(3) "tcp"
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  int(%d)
}
sock name:
array(3) {
  ["type"]=>
  string(3) "tcp"
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  int(%d)
}
==== done ====
