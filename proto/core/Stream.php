<?php

namespace skyray\core;

/**
 * Stream represents a connection to a remote or local endpoint/socket. A stream can't be created directly,
 * all streams only can be created via StreamClient.
 *
 * @package skyray\core
 * @since 0.0.1
 */
class Stream {

    private function __construct()
    {

    }
    /**
     * Returns whether the stream is ready to read or write data.
     *
     * @return boolean
     */
    public function isReady()
    {

    }

    public function isReadable()
    {

    }

    public function isWritable()
    {

    }

    public function setBlocking($blocking) //???
    {

    }

    public function isBlocking()
    {

    }

    public function read()
    {

    }

    public function write()
    {

    }

    public function close()
    {

    }

    public function setProtocol($protocol)
    {

    }

    public function getProtocol()
    {

    }

    public function getRemoteAddress()
    {

    }
}



$factory = new HttpFactory();
$protocol = $factory->createProtocol();



$client = new StreamClient($factory, $reactor);
$stream = $client->connectIpv4('127.0.0.1', 9000);
$stream->write();
$stream->close();

class HttpFactory extends Factory {

    public function __construct($stream);
    public function connectionMade();
    public function loseConnection();
    public function receivedData();
}
