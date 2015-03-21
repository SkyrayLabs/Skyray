<?php

namespace skyray\core;


class StreamServer {
    public function __construct($reactor);
    public function listen($port);
}
