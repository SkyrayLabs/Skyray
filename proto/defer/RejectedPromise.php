<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-10
 */

namespace skyray\defer;

/**
 * Class RejectedPromise
 *
 * @package skyray\defer
 * @since 0.1
 */
class RejectedPromise extends Promise
{
    /**
     * Constructor
     *
     * @param mixed $reason The reason of the rejected promise.
     */
    public function __construct($reason)
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
