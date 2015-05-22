<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-23
 */

namespace skyray\processing;

/**
 * Class ProcessWatcherHandler
 *
 * @package skyray\processing
 * @since 1.0
 */
abstract class ProcessWatcherHandler
{
    public $watcher;

    /**
     * Handler that will be called when watched process exited.
     *
     * @param integer $status
     */
    abstract public function onExited($status);

    /**
     * Handler that will be called when watched process ended.
     *
     * @param integer $status
     */
    abstract public function onEnded($status);
}
