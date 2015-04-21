<?php

namespace skyray\core;

use skyray\Object;

/**
 * Class StreamServer
 *
 * @property Reactor $reactor The reactor object the server current running on, if the property was absent,
 * a default reactor will be automatically created.
 *
 * @package skyray\core
 * @since 0.1
 */
class StreamServer extends Object
{
    /**
     * Constructor
     *
     * @param callable $factory Factory function to create protocol for streams, the returned object of this callable
     * should be instance of \skyray\core\ProtocolInterface.
     * @param array $config The configuration to initialize the object.
     * @throws \skyray\InvalidParamException if $factory is not a valid callable.
     */
    public function __construct($factory, $config = [])
    {

    }

    /**
     * Returns the reactor object used by the server,
     *
     * @return \skyray\core\Reactor
     */
    public function getReactor()
    {

    }

    /**
     * Sets the reactor object used by the server
     *
     * @param \skyray\core\Reactor $reactor
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
     * @throws \skyray\core\SkyrayException throws if the server can not be started.
     */
    public function listen($host, $port, $backlog = 10)
    {

    }

    /**
     * Returns the stats of the server
     *
     * @return array Array of stats information.
     */
    public function stats()
    {

    }
}
