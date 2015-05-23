<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-23
 */

namespace skyray\stream;

/**
 * Class FdWatcherHandler
 *
 * @package skyray\stream
 * @since 1.0
 */
abstract class FdWatcherHandler
{
    public $watcher;

    /**
     * Handler that will be called when watched file descriptor becomes readable.
     *
     * @param mixed $fd
     */
    abstract public function onReadable($fd);

    /**
     * Handler that will be called when watched file descriptor becomes writable.
     *
     * @param mixed $fd
     */
    abstract public function onWritable($fd);

    /**
     * Handler that will be called when error occurs on watched file descriptor.
     *
     * @param mixed $fd
     * @param string $error
     */
    abstract public function onError($fd, $error);
}
