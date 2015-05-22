<?php

namespace skyray\stream;

/**
 * Interface ProtocolInterface
 *
 * @package skyray\stream
 * @since 0.1
 */
interface ProtocolInterface
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
