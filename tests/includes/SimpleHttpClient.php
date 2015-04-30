<?php

use skyray\Object;
use skyray\core\Reactor;
use skyray\core\StreamClient;
use skyray\core\StreamProtocolInterface;

/**
 * A simple http client implementation only relies on StreamClient.
 */
class SimpleHttpClient
{
    protected $reactor;
    protected $callback;
    protected $clients = [];

    public function __construct(Reactor $reactor, $callback)
    {
        $this->reactor = $reactor;
        $this->callback = $callback;
    }


    public function request($method, $url, $headers = [], $body = null)
    {
        $method = strtoupper($method);

        $parts = parse_url($url);
        //$key = $parts['host'] . ':' . $parts['port'];
        if (!isset($headers['Host'])) {
            $headers['Host'] = $parts['host'];
        }

        if (!isset($headers['Content-Type'])) {
            if (is_string($body) || is_null($body)) {
                $headers['Content-Type'] = 'text/plain';
            } else {
                $headers['Content-Type'] = 'application/json';
            }
        }

        if ($headers['Content-Type'] == 'text/plain') {
            $rawBody = (string)$body;
        } else {
            $rawBody = json_encode($body, JSON_UNESCAPED_UNICODE);
        }
        $headers['Content-Length'] = strlen($rawBody);

        $data = "{$method} {$parts['path']} HTTP/1.1\r\n";
        $headerLines = [];
        foreach ($headers as $name => $value) {
            $headerLines[] = $name . ': ' . $value;
        }
        $data .= implode("\r\n", $headerLines) . "\r\n";
        $data .= $rawBody . "\r\n";

        $factory = function () use ($data) {
            return new ClientHttpProtocol($data, $this->callback, $this->reactor);
        };

        $client = new StreamClient($factory, $this->reactor);
        $client->connectTCP($parts['host'], $parts['port']);
    }
}

class SimpleHttpResponse extends Object
{
    public $protocolVersion;
    public $statusCode;
    public $statusPhase;
    public $headers;
    public $body;

    public function hasHeader($name)
    {
        return isset($this->headers[$name]);
    }

    public function getHeader($name)
    {
        return isset($this->headers[$name]) ? $this->headers[$name] : null;
    }
}

class ClientHttpProtocol implements StreamProtocolInterface
{
    protected $stream;

    protected $data;
    protected $callback;
    protected $reactor;

    public function __construct($data, $callback, $reactor)
    {
        $this->data = $data;
        $this->callback = $callback;
        $this->reactor = $reactor;
    }

    public function connectStream($stream)
    {
        $this->stream = $stream;
    }

    public function streamConnected()
    {
        if ($this->data != null) {
            $this->stream->write($this->data);
            $this->data = null;
        }
    }

    public function dataReceived($data)
    {
        $lines = explode("\r\n\r\n", $data);

        $header = array_shift($lines);
        $headerLines = explode("\r\n", $header);

        $response = new SimpleHttpResponse();

        $responseParts = explode(' ', array_shift($headerLines), 3);
        $response->protocolVersion = explode('/', $responseParts[0])[1];
        $response->statusCode = $responseParts[1];
        $response->statusPhase = $responseParts[2];

        $response->headers = [];
        foreach ($headerLines as $line) {
            $parts = explode(': ', $line);
            $response->headers[$parts[0]] = $parts[1];
        }

        if ($body = array_shift($lines)) {
            $body = trim($body);
            if ($response->getHeader('Transfer-Encoding') != 'chunked') {
                $response->body = $body;
            } else {
                $bodyParts = explode("\r\n", $body);
                foreach ($bodyParts as $key => $part) {
                    if ($key % 2 == 1) {
                        $response->body = $part;
                    }
                }
            }
        }

        if ($this->callback) {
            call_user_func($this->callback, $response);
        }

        $this->stream->close();
    }

    public function streamClosed()
    {
        @$GLOBALS['close_count'] += 1;
    }

    public function __destruct()
    {
        @$GLOBALS['client_destruct_count'] += 1;
    }
}
