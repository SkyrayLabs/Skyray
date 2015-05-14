<?php

namespace skyray\defer;

/**
 * Class Promise
 *
 * @package skyray\defer
 * @since 0.1
 *
 * @method finally() see _finally()
 * @method catch() see _catch()
 */
class Promise implements PromiseInterface
{
    /**
     * @param callable $onFulfilled
     * @param callable $onRejected
     * @param callable $onProgress
     * @return static
     */
    public function then($onFulfilled = null, $onRejected = null, $onProgress = null)
    {

    }

    /**
     * @param callable $onFulfilled
     * @param callable $onRejected
     * @param callable $onProgress
     */
    public function done($onFulfilled = null, $onRejected = null, $onProgress = null)
    {

    }

    public function _catch($errorHandler = null)
    {

    }

    public function _finally($handler = null, $notifyHandler = null)
    {

    }

    public static function resolve($value)
    {

    }

    public static function reject($value)
    {

    }

    public static function all()
    {

    }
}
