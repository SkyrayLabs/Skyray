<?php

namespace skyray;

use skyray\stream\FdWatcher;

/**
 * Class Reactor
 *
 * @package skyray
 * @since 0.1
 */
class Reactor
{
    /**
     * Attach a stream to reactor, the stream will be set as non-blocking and Reactor will process dataReceived and streamClosed events on its protocol.
     *
     * @param \skyray\stream\Stream $stream The stream to add
     */
    public function attach($stream)
    {

    }

    /**
     * Detach the stream from reactor, no future events will get notified.
     *
     * @param \skyray\stream\Stream $stream The stream to add
     */
    public function detach($stream)
    {

    }

    /**
     * Add a raw file descriptor to reactor for monitoring readable and writable events.
     *
     * @param mixed $fd The fd the watch, integer file descriptor or php socket resources.
     * @param \skyray\stream\FdWatcherHandler $handler
     * @param int $events The interested events to watch, must be combination of FdWatcher::READABLE and
     * FdWatcher::WRITABLE.
     * @return \skyray\stream\FdWatcher
     */
    public function watch($fd, $handler, $events = FdWatcher::READABLE | FdWatcher::WRITABLE)
    {

    }

    /**
     * Monitoring filesystem changes.
     *
     * @param string $filename
     * @param \skyray\fs\FileEventHandler $handler
     * @return \skyray\fs\FileWatcher
     */
    public function watchFile($filename, $handler)
    {

    }

    /**
     * Monitoring process state changes.
     *
     * @param \skyray\processing\Process $process
     * @param \skyray\processing\ProcessWatcherHandler $handler
     * @return Watcher
     */
    public function watchProcess($process, $handler)
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
