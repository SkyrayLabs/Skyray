<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-4-13
 */

namespace skyray\http;

use skyray\core\StreamProtocolInterface;

/**
 * A server side http protocol implementation.
 *
 * @package skyray\http
 * @since 0.1
 */
abstract class ServerProtocol implements StreamProtocolInterface
{
    /**
     * Callback method to handle http requests, all subclasses should implement this method to handle
     * request, and return a Response stands the result of the request.
     *
     * If the method throws exception, a 500(server error) response will be issued.
     *
     * @param Request $request The current received request
     * @return Response The response that returns to client
     */
    abstract public function onRequest(Request $request);

    /**
     * @inheritdoc
     */
    public function connectStream($stream)
    {

    }

    /**
     * @inheritdoc
     */
    public function streamConnected()
    {

    }

    /**
     * @inheritdoc
     */
    public function dataReceived($data)
    {

    }

    /**
     * @inheritdoc
     */
    public function streamClosed()
    {

    }
}
