<?php
use skyray\core\StreamProtocolInterface;


class SimpleEchoProtocol implements StreamProtocolInterface
{
    protected $debug;
    protected $clientMode;
    protected $stream;

    public function __construct($debug = false, $clientMode = false)
    {
        $this->debug = $debug;
        $this->clientMode = $clientMode;
    }

    public function log($info)
    {
        echo ($this->clientMode ? '[client]: ' : '[server]: ') . $info . PHP_EOL;
    }

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
        $this->debug && $this->log('connected');
        if ($this->clientMode) {
            $this->stream->write("hello");
        }
    }

    public function dataReceived($data)
    {
        if ($this->clientMode) {
            $this->log('received data: ' . $data);
            $this->stream->close();
        } else {
            $this->stream->write($data);
        }
    }

    public function streamClosed()
    {
        $this->debug && $this->log('closed');
    }
}
