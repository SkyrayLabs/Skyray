<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-10
 */

namespace skyray\defer;

/**
 * Class FulfilledPromise
 *
 * @package skyray\defer
 * @since 0.1
 */
class FulfilledPromise extends Promise
{
    /**
     * Constructor
     *
     * @param mixed $value The value of the fulfilled promise.
     */
    public function __construct($value)
    {

    }

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
}
