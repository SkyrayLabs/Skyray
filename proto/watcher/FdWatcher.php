<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-23
 */

namespace skyray\watcher;

use skyray\Watcher;

/**
 * Class FdWatcher
 *
 * @package skyray\watcher
 * @since 1.0
 */
class FdWatcher extends Watcher
{
    const READABLE = 1;
    const WRITABLE = 2;

    /**
     * Returns the watched file descriptor.
     *
     * @return mixed
     */
    public function getFd()
    {

    }

    /**
     * Watch the underling file descriptor again for the specified events.
     *
     * @param int $events The interested events to watch, must be combination of FdWatcher::READABLE and
     * FdWatcher::WRITABLE.
     */
    public function watch($events = self::READABLE | self::WRITABLE)
    {

    }

    /**
     * Stop watch specified events, default to all.
     *
     * @param int $events The events to stop watching.
     */
    public function stop($events = self::READABLE | self::WRITABLE)
    {

    }

    /**
     * Returns whether specified events is watched.
     *
     * @param $events
     * @return boolean
     */
    public function isWatched($events)
    {

    }
}
