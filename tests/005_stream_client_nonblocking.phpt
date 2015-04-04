--TEST--
Test for StreamClient::connectTCP() non-blocking mode
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\Reactor;
use skyray\core\StreamClient;
use skyray\core\SkyrayException;
use skyray\core\ProtocolInterface;

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
        var_dump('connected');
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

$reactor = new Reactor();

$client = new StreamClient(function () {
    return new MyProtocol();
}, $reactor);

$s = $client->connectTCP('127.0.0.1', 2333);
echo get_class($s) . PHP_EOL;
$reactor->run();
?>
--EXPECTF--
skyray\core\Stream
string(9) "connected"
HTTP/1.1 404 Not Found
closed
