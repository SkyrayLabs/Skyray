<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-3-2
 */

namespace skyray\http;

use skyray\core\StreamServer;

/**
 * Http Server
 *
 * @package skyray\http
 * @since 0.1
 */
class Server extends StreamServer
{
    /**
     * Sets a handler to decide how should http protocol upgrade to another protocol such as websocket.
     *
     * The parameter $upgrader should be a valid callable with the following prototype:
     *
     * ```php
     * function_name(Request $request): Response
     * ```
     *
     * The handler should returns a valid http Response to indicate a successful upgrading, otherwise the upgrade request
     * will be ignored, and the HttpProtocol::onRequest() will be called.
     *
     * @param callable $upgrader
     */
    public function setUpgrader(callable $upgrader)
    {

    }

    /**
     * Returns the upgrader handler used by this server.
     *
     * @return callable
     */
    public function getUpgrader()
    {

    }

    /**
     * Sets websocket factory used to create websocket protocol instances.
     *
     * @param callable $wsFactory
     */
    public function setWsFactory(callable $wsFactory)
    {

    }

    /**
     * Returns the websocket factory used by this server.
     *
     * @return callable
     */
    public function getWsFactory()
    {

    }
}
