<?php

namespace skyray\core;

/**
 * Stream represents a connection to a remote or local endpoint/socket. A stream can't be created directly,
 * all streams can only be created via StreamClient.
 *
 * @package skyray\core
 * @since 0.1
 */
class Stream
{
    /**
     * A stream can not be created directly, using StreamClient or StreamServer instead.
     */
    private function __construct()
    {

    }

    /**
     * Returns whether the stream is a readable stream.
     *
     * @return boolean
     */
    public function isReadable()
    {

    }

    /**
     * Returns whether the stream is a writable stream.
     *
     * @return boolean
     */
    public function isWritable()
    {

    }

    /**
     * Returns whether the stream is blocking or not.
     *
     * @return boolean
     */
    public function isBlocking()
    {

    }

    /**
     * Read data from stream, for non-blocking streams, a \skyray\core\SkyrayException will be thrown.
     *
     * @return string
     */
    public function read()
    {

    }

    /**
     * Write data to the stream.
     *
     * @param string $data
     * @return false|integer Returns false on failure, otherwise bytes sent or queued by underlying implementation successfully.
     */
    public function write($data)
    {

    }

    /**
     * Close the stream.
     */
    public function close()
    {

    }

    /**
     * Returns the protocol used by the stream.
     *
     * @return \skyray\core\ProtocolInterface|null
     */
    public function getProtocol()
    {

    }

    /**
     * The the socket name of the peer endpoint of the stream.
     *
     * @return array
     *
     * For tcp streams:
     * ```php
     * [
     *     'type' => 'tcp'
     *     'host' => '211.189.121.89',
     *     'port' => 9090,
     * ]
     * ```
     *
     * For unix socket streams:
     * ```php
     * [
     *     'type' => 'unix',
     *     'path' => '/path/to/socket.sock'
     * ]
     * ```
     *
     */
    public function getPeerName()
    {

    }

    /**
     * The the socket name of the stream.
     *
     * @return array The return array is the same as static::getPeerName().
     */
    public function getSockName()
    {

    }
}
