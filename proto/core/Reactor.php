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
     * Add a reader stream to reactor.
     *
     * @param \skyray\core\Stream $stream The stream to add
     * @param callable $callback The callback is only useful when adding streams that without protocol associated.
     */
    public function addReader($stream, $callback = null)
    {

    }

    /**
     * Add a writer stream to reactor.
     *
     * @param \skyray\core\Stream $stream The stream to add
     * @param callable $callback The callback is only useful when adding streams that without protocol associated.
     */
    public function addWriter($stream, $callback = null)
    {

    }

    /**
     * Add a stream to reactor for both read and write.
     *
     * @param $stream
     * @param null $callback
     */
    public function addBoth($stream, $callback = null)
    {

    }

    /**
     * Remove the specified reader stream from reactor.
     *
     * @param $stream
     */
    public function removeReader($stream)
    {

    }

    /**
     * Remove the specified writer stream from reactor.
     *
     * @param $stream
     */
    public function removeWriter($stream)
    {

    }

    /**
     * Remove a stream from reactor for both read and write.
     *
     * @param $stream
     */
    public function removeBoth($stream)
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
}
