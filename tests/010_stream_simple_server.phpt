--TEST--
Test for StreamServer() simple echo server
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\Reactor;
use skyray\core\StreamClient;
use skyray\core\StreamServer;

require_once __DIR__ . '/includes/SimpleEchoProtocol.php';

$reactor = new Reactor();

$server = new StreamServer(function () {
    return new SimpleEchoProtocol(true);
}, $reactor);

$server->listen('0.0.0.0', 10000);

$reactor->addTimer(100, function () use ($reactor) {
    $client = new StreamClient(function () {
        return new SimpleEchoProtocol(true, true);
    }, $reactor);

    $client->connectTCP('127.0.0.1', 10000);
});

$reactor->addTimer(400, function () use ($reactor) {
    $reactor->stop();
});
$reactor->run();

?>
--EXPECTF--
[server]: connected
[client]: connected
[client]: received data: hello
[client]: closed
[server]: closed
