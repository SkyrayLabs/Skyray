<?php

namespace skyray\core;

/**
 * Class Reactor
 *
 * @package skyray\core
 * @since 0.1
 */
class Reactor
{
    /**
     * Attach a stream to reactor, the stream will be set as non-blocking and Reactor will process dataReceived and streamClosed events on its protocol.
     *
     * @param \skyray\core\Stream $stream The stream to add
     */
    public function attach($stream)
    {

    }

    /**
     * Detach the stream from reactor, no future events will get notified.
     *
     * @param \skyray\core\Stream $stream The stream to add
     */
    public function detach($stream)
    {

    }

    /**
     * Add a raw file descriptor to reactor for monitoring readable and writable events.
     *
     * @param $stream
     */
    public function watch($fd, $onReadable = null, $onWriteable = null)
    {

    }

    /**
     * Remove raw file descriptor from reactor.
     *
     * @param $stream
     */
    public function unwatch($fd)
    {

    }

    /**
     * Add to timer to reactor
     *
     * @param $interval
     * @param $callback
     */
    public function addTimer($interval, $callback)
    {

    }

    /**
     * Add a timer that will run periodically.
     *
     * @param $interval
     * @param $callback
     */
    public function addPeriodicTimer($interval, $callback)
    {

    }

    /**
     * Cancel the timer
     *
     * @param $timer
     */
    public function cancelTimer($timer)
    {

    }

    /**
     * Start the reactor, the method will block until no stream or timer needs monitoring.
     */
    public function run()
    {

    }

    /**
     * Stop the reactor, causing Reactor::run() to end as soon as possible.
     */
    public function stop()
    {

    }
}
