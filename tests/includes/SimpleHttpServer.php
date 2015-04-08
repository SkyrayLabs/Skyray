<?php

use skyray\core\Reactor;
use skyray\core\StreamServer;
use skyray\core\ProtocolInterface;


class SimpleHttpServer
{
    protected $server;
    protected $reactor;

    public function __construct($host, $port)
    {
        $this->reactor = new Reactor();
        $this->server = new StreamServer([$this, 'createProtocol'], $this->reactor);
        $this->server->listen($host, $port);
    }

    public function createProtocol()
    {
        return new FakeHttpProtocol();
    }

    public function start()
    {
        $this->reactor->run();
    }
}

class FakeHttpProtocol implements ProtocolInterface
{
    protected $stream;

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
    }

    public function dataReceived($data)
    {
        $this->stream->write("HTTP/1.1 404 Not Found\r\n");
        $this->stream->close();
    }

    public function streamClosed()
    {
    }
}
