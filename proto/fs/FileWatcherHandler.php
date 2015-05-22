<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-23
 */

namespace skyray\fs;

/**
 * Class FileWatcherHandler
 *
 * @package skyray\stream
 * @since 1.0
 */
abstract class FileWatcherHandler
{
    public $watcher;

    /**
     * Handler that will be called when watched file been modified
     *
     * @param string $path
     */
    abstract public function onModified($path);

    /**
     * Handler that will be called when watched file been moved.
     *
     * @param string $path
     */
    abstract public function onMoved($path);

    /**
     * Handler that will be called when watched file been deleted.
     *
     * @param string $path
     */
    abstract public function onDeleted($path);
}
