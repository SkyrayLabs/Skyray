<?php

namespace skyray\protocol;

interface FactoryInterface
{
    public function __construct($params = []);

    /**
     * @return mixed
     */
    public function createProtocol();
}
