--TEST--
Test for skyray\stream\Server() simple echo server
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\Reactor;
use skyray\stream\Client;
use skyray\stream\Server;

require_once __DIR__ . '/includes/SimpleEchoProtocol.php';

$reactor = new Reactor();

$server = new Server(function () {
    return new SimpleEchoProtocol(true);
}, [
    'reactor' => $reactor
]);

$server->listen('0.0.0.0', 10000);

$reactor->addTimer(100, function () use ($reactor) {
    $client = new Client(function () {
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
[%s]: connected
[%s]: connected
[client]: received data: hello
[client]: closed
[server]: closed
