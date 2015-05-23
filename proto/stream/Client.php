<?php

namespace skyray\stream;

use skyray\protocol\HttpProtocol;
use skyray\protocol\ProtocolInterface;

/**
 * StreamClient
 *
 * This is an example to create an asynchronous stream:
 *
 * ```php
 * $factory = new FooFactory();
 * $client = new StreamClient($factory, $reactor);
 * $stream = $client->connectIpv4('127.0.0.1', 80);
 * ```
 *
 * ```php
 * $client = new StreamClient(null);
 * $stream = $client->connectIpv4('127.0.0.1', 80);
 * $stream->read(); // ...
 * ```
 *
 * How reconnecting should be implemented???
 *
 * @package skyray\stream
 * @since 0.0.1
 */
class Client {

    protected $protocolCreator;
    protected $reactor;

    public function __construct($protocolCreator, $reactor = null)
    {
        $this->protocolCreator = $protocolCreator;
        $this->reactor = $reactor;
    }

    protected function makeConnection()
    {

    }

    /**
     * @param $host
     * @param $port
     * @return ProtocolInterface|Deferred|Stream
     */
    public function connectTCP($host, $port)
    {
        $creator = $this->protocolCreator;
        /** @var ProtocolInterface $protocol */
        $protocol = $creator();

        $stream = $this->makeConnection();
        $protocol->streamConnected($stream);
        while($data = $stream->read()) {
            $protocol->dataReceived($data);
        }
        $protocol->streamClosed();
        return $protocol;
    }


    public function connectUNIX($path)
    {

    }

    public function connectSSL($path)
    {

    }

    public function createPipe($dulex)
    {

    }
}
