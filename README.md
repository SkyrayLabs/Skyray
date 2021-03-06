# Skyray - A networking library for PHP written in C

Skyray provides non-blocking I/O and multi-processing facilities for PHP, it is intended 
be to as flexible as possible to suit for various applications.

[![](https://travis-ci.org/SkyrayLabs/Skyray.png?branch=master)](https://travis-ci.org/SkyrayLabs/Skyray)

## Features

* Well designed OO interfaces for I/O and Process management
* Stream oriented abstraction, everything is stream, simple and clean
* Build servers with PHP, quick and easy
* Support both non-blocking and blocking interfaces 
* Support millisecond timer (both periodic and non-periodic)  
* Bring all possibilities to PHP world ... 

## Installation


```bash
git clone https://github.com/SkyrayLabs/Skyray.git --recursive
phpize
./configure --with-php-config=/path/to/php-config
make & make install
```

> **NOTES** :
> Skyray requires PHP7, please be sure your have php7 installed before building.

## Examples

### A Simple Echo Server

server.php

```php
use skyray\Reactor;
use skyray\stream\Server;
use skyray\stream\ProtocolInterface;

class SimpleEchoProtocol implements ProtocolInterface
{
    protected $debug;
    protected $stream;

    public function __construct($debug = false)
    {
        $this->debug = $debug;
    }

    public function log($info)
    {
        echo '[server]: ' . $info . PHP_EOL;
    }

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
        $this->debug && $this->log('connected');
    }

    public function dataReceived($data)
    {
        $this->log('received data: ' . $data);
        $this->stream->write($data);
    }

    public function streamClosed()
    {
        $this->debug && $this->log('closed');
    }
}


$reactor = new Reactor();

$server = new Server(function () {
    return new SimpleEchoProtocol(true);
}, $reactor);

$server->listen('0.0.0.0', 10000);

$reactor->run();
```

client.php

```php
use skyray\stream\Client;

$client = new Client();
$stream = $client->connectTCP('127.0.0.1', 10000);
$stream->write('hello world');
var_dump($stream->read());
$stream->close();
```

## Documentation

Documentation is not yet available, please refer [here](https://github.com/bixuehujin/skyray/tree/master/proto) for prototype of all classes.

## Participating

**Skyray is still in active development, your participating is very wellcome!**

You may participate in the following ways:

* [Report issues or Start a design discussion](https://github.com/bixuehujin/skyray/issues)
* Develop features or write documentation via [Pull Requests](https://github.com/bixuehujin/skyray/pulls)
 