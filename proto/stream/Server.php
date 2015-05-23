<?php

namespace skyray\stream;

use skyray\Object;

/**
 * Class StreamServer
 *
 * @property \skyray\Reactor $reactor The reactor object the server current running on, if the property was absent,
 * a default reactor will be automatically created.
 *
 * @package skyray\stream
 * @since 0.1
 */
class Server extends Object
{
    /**
     * Constructor
     *
     * @param callable $factory Factory function to create protocol for streams, the returned object of this callable
     * should be instance of \skyray\stream\ProtocolInterface.
     * @param array $config The configuration to initialize the object.
     * @throws \skyray\InvalidParamException if $factory is not a valid callable.
     */
    public function __construct($factory, $config = [])
    {

    }

    /**
     * Returns the reactor object used by the server,
     *
     * @return \skyray\Reactor
     */
    public function getReactor()
    {

    }

    /**
     * Sets the reactor object used by the server
     *
     * @param \skyray\Reactor $reactor
     */
    public function setReactor($reactor)
    {

    }

    public function setLogger() //???? how to
    {

    }

    public function getLogger()
    {

    }

    /**
     * Starts to listen, ready to accept connections.
     *
     * @param $host
     * @param $port
     * @param int $backlog
     * @throws \skyray\SkyrayException throws if the server can not be started.
     */
    public function listen($host, $port, $backlog = 10)
    {

    }

    /**
     * Starts the underlying reactor, simply a shortcut of Reactor::run().
     */
    public function start()
    {

    }

    /**
     * This method is a shortcut for Reactor::stop().
     */
    public function stop()
    {

    }
}
