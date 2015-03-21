<?php

namespace skyray\core;

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
 * @package skyray\core
 * @since 0.0.1
 */
class StreamClient {

    protected $factory;
    protected $reactor;

    public function __construct($factory, $reactor = null)
    {
        $this->factory = $factory;
        $this->reactor = $reactor;
    }

    /**
     * @param $host
     * @param $port
     * @return Stream|Deferred
     */
    public function connectTCP($host, $port)
    {
        $protocol = $this->factory->createProtocol();
        return new Stream($protocol, $this->reactor);
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
