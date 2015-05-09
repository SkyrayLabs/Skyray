<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-5-10
 */

namespace skyray\defer;

/**
 * Interface PromiseInterface
 *
 * @package skyray\defer
 * @since 0.1
 */
interface PromiseInterface
{
    /**
     * @param callable $onFulfilled
     * @param callable $onRejected
     * @param callable $onProgress
     * @return static
     */
    public function then($onFulfilled = null, $onRejected = null, $onProgress = null);

    /**
     * @param callable $onFulfilled
     * @param callable $onRejected
     * @param callable $onProgress
     */
    public function done($onFulfilled = null, $onRejected = null, $onProgress = null);
}
