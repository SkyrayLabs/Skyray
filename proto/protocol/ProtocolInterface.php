<?php

namespace skyray\protocol;

/**
 * Interface ProtocolInterface
 *
 * @package skyray\protocol
 */
interface ProtocolInterface
{
    public function makeConnection($stream); //???

    /**
     * Called when underlying connection established successfully.
     *
     * @return mixed
     */
    public function connectionMade();

    /**
     * @param $data
     * @return mixed
     */
    public function dataReceived($data);

    public function connectionLost();
}
