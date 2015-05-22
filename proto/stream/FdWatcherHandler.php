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
     */
    abstract public function onReadable();

    /**
     * Handler that will be called when watched file descriptor becomes writable.
     */
    abstract public function onWritable();

    /**
     * Handler that will be called when error occurs on watched file descriptor.
     */
    abstract public function onError();
}
