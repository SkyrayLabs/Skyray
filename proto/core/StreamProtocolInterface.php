<?php

namespace skyray\core;

/**
 * Interface ProtocolInterface
 *
 * @package skyray\protocol
 * @since 0.1
 */
interface StreamProtocolInterface
{
    /**
     * Called when stream created successfully but the connection is not established yet.
     *
     * @param $stream
     */
    public function connectStream($stream);

    /**
     * Called when underlying connection established successfully.
     */
    public function streamConnected();

    /**
     * Called when new data received.
     *
     * @param $data
     */
    public function dataReceived($data);

    /**
     * Called when the related stream closed.
     */
    public function streamClosed();
}
