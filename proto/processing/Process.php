<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-3-21
 * Time: 下午11:07
 */

namespace skyray\processing;

/**
 * Class Process
 * @package skyray\processing
 * @since 0.1.0
 */
class Process
{
    /**
     * Constructor
     *
     * @param callable $callable  The callable that will be invoked in child process.
     * @param array $args Array of arguments that will pass to the callable
     */
    public function __construct($callable, $args = [])
    {

    }

    /**
     * Replace the process with an executable, this method can only be invoked in child process.
     *
     * @param $executable
     * @param array $args
     */
    public function exec($executable, $args = [])
    {

    }

    /**
     * This method will be invoked in child process.
     */
    public function run()
    {

    }

    /**
     * Start the process
     */
    public function start()
    {

    }

    /**
     * Wait the process to finish.
     */
    public function join()
    {

    }

    /**
     * Returns whether the process is alive,
     */
    public function isAlive()
    {

    }

    /**
     * Send a signal to the process
     *
     * @param $signal
     */
    public function kill($signal)
    {

    }

    /**
     * Returns the exit code of the process, if the process is not yet started or finished, null weill be returned.
     *
     * @return integer|null
     */
    public function getExitCode()
    {

    }

    /**
     * Bind signal handlers for this process, this method can only be used in child process.
     *
     * @param $signal
     * @param $callback
     */
    public function signal($signal, $callback)
    {

    }
}
