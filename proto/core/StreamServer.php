<?php

namespace skyray\core;

/**
 * Class StreamServer
 *
 * @package skyray\core
 * @since 0.1
 */
class StreamServer
{
    /**
     * Constructor
     *
     * @param callable $protocolCreator Factory function to create protocol for streams, the returned object of this callable
     * should be instance of \skyray\core\ProtocolInterface.
     * @param null $reactor The Reactor object, if null passed, a new reactor object will be automatically created by the server.
     */
    public function __construct($protocolCreator, $reactor = null)
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
