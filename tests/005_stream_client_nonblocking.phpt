--TEST--
Test for StreamClient::connectTCP() non-blocking mode
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\Reactor;
use skyray\core\StreamClient;
use skyray\core\StreamProtocolInterface;

$server = require_once __DIR__ . '/includes/ServerProcess.php';
register_shutdown_function(function () use ($server) {
    $server->stop();
});

set_exception_handler(function ($e) {
    echo '[error]: ' . $e->getMessage();
});

class MyProtocol implements StreamProtocolInterface
{
    protected $stream;
    protected $data = '';
    protected $closeOnConnect;

    public function __construct($closeOnConnect = false)
    {
        $this->closeOnConnect = $closeOnConnect;
    }

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
        var_dump('connected');
        if ($this->closeOnConnect) {
            $this->stream->close();
        }
        $this->stream->write("GET / HTTP/1.1\r\nConnection: keep-alive\r\n\r\n");
    }

    public function dataReceived($data)
    {
        $this->data .= $data;
    }

    public function streamClosed()
    {
        if ($this->data) {
            echo explode("\r\n", $this->data)[0] . PHP_EOL;
        }
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


echo "====================\n";

$reactor = new Reactor();

$client = new StreamClient(function () {
    return new MyProtocol(true);
}, $reactor);


$s = $client->connectTCP('127.0.0.1', 2333);

echo get_class($s) . PHP_EOL;
$reactor->addTimer(200, function () {
    var_dump('timer');
});
$reactor->run();
?>
--EXPECTF--
skyray\core\Stream
string(9) "connected"
HTTP/1.1 404 Not Found
closed
====================
skyray\core\Stream
string(9) "connected"
[error]: Unable to write to stream, the stream may already closed
closed
string(5) "timer"
