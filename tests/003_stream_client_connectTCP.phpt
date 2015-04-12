--TEST--
Test for StreamClient::connectTCP() blocking mode
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\StreamClient;
use skyray\processing\Process;
use skyray\core\ProtocolInterface;

$server = require_once __DIR__ . '/includes/ServerProcess.php';

echo "==== test without protocol ====\n";
$client = new StreamClient(null);
$stream = $client->connectTCP('127.0.0.1', 2333);
var_dump(get_class($stream));
$stream->write("GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n");
$data = $stream->read();
echo explode("\r\n", $data)[0] . PHP_EOL;
echo "==== done ====\n\n";

echo "==== test with protocol ====\n";


class MyProtocol implements ProtocolInterface
{
    protected $stream;
    protected $data = '';

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
        $this->stream->write("GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n");
    }

    public function dataReceived($data)
    {
        $this->data .= $data;
    }

    public function streamClosed()
    {
        echo explode("\r\n", $this->data)[0] . PHP_EOL;
        echo "closed\n";
    }
}

$creator = function () {
    return new MyProtocol();
};

$client = new StreamClient($creator);
$protocol = $client->connectTCP('127.0.0.1', 2333);
var_dump(get_class($protocol));
echo "==== done ====\n";

$server->stop();
?>

--EXPECTF--
==== test without protocol ====
string(18) "skyray\core\Stream"
HTTP/1.1 404 Not Found
==== done ====

==== test with protocol ====
HTTP/1.1 404 Not Found
closed
string(10) "MyProtocol"
==== done ====
