<?php

use skyray\Reactor;
use skyray\stream\Server;
use skyray\stream\ProtocolInterface;

/**
 * A simple http server implementation only relies on StreamServer.
 */
class SimpleHttpServer
{
    /**
     * @var StreamServer
     */
    protected $server;

    public function __construct($host, $port)
    {
        $this->reactor = new Reactor();
        $this->server = new Server([$this, 'createProtocol']);
        $this->server->listen($host, $port);
    }

    public function createProtocol()
    {
        return new FakeHttpProtocol();
    }

    public function start()
    {
        $this->server->reactor->run();
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
