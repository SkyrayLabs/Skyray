--TEST--
Test for skyray\http\ServerProtocol basic
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Request;
use skyray\http\Response;
use skyray\http\ServerProtocol;
use skyray\http\Server;

$GLOBALS['request_count'] = 0;
$GLOBALS['close_count'] = 0;
$GLOBALS['client_destruct_count'] = 0;
$GLOBALS['server_close_count'] = 0;
$GLOBALS['server_destruct_count'] = 0;

register_shutdown_function(function () {
    printf("client requests: %d\n", $GLOBALS['request_count']);
    printf("client closed: %d\n", $GLOBALS['close_count']);
    printf("client destruct count: %d\n", $GLOBALS['client_destruct_count']);
    printf("server closed: %d\n", $GLOBALS['server_close_count']);
    printf("server destruct count: %d\n", $GLOBALS['server_destruct_count']);
});

set_exception_handler(function ($e) {
    var_dump($e);
});

ini_set('memory_limit', '3M');

require __DIR__ . '/includes/SimpleHttpClient.php';

$responses = [
    '/get_json_body' => [
        'headers' => [
            'Connection' => 'close',
        ],
        'body' => [],
    ],
    '/get_json_body2' => [
        'headers' => [], // with without Connection: close
        'body' => [],
    ],
    '/get_text_body' => [
        'headers' => [
            'Connection' => 'close',
        ],
        'rawBody' => 'this is ray body',
    ],
    '/get_text_body2' => [
        'headers' => [],
        'rawBody' => 'this is ray body',
    ],
];

class MyProtocol extends ServerProtocol
{
    public function onRequest(Request $request)
    {
        global $responses;
        //print "[Request] {$request->getMethod()} {$request->getUri()}" . PHP_EOL;

        $response = new Response();

        if (!isset ($responses[$request->getUri()])) {
            return $response;
        }

        $expected = $responses[$request->getUri()];

        foreach ($expected['headers'] as $name => $value) {
            $response->addHeader($name, $value);
        }
        if (isset($expected['body'])) {
            $response->setBody($expected['body']);
        }
        if (isset($expected['rawBody'])) {
            $response->setRawBody($expected['rawBody']);
        }

        return $response;
    }

    public function streamClosed()
    {
        $GLOBALS['server_close_count'] += 1;
    }

    public function __destruct()
    {
        $GLOBALS['server_destruct_count'] += 1;
    }
}

$server = new Server(function () {
    return new MyProtocol();
});

$server->listen('0.0.0.0', 10000);

$requests = array_keys($responses);
$expectResponses = array_values($responses);
$count = 0;


$server->reactor->addPeriodicTimer(2, function ($timer) use ($server, $requests, $expectResponses, &$count) {
    $request = $requests[$idx = rand(0, 3)];
    $exceptResponse = $expectResponses[$idx];
    $count ++;
    if ($count > 1000) {
        $timer->cancel();
        //we should stop the reactor later, make sure all streams are closed.
        $server->reactor->addTimer(50, function () use ($server) {
            $server->reactor->stop();
        });
        return;
    }
    //var_dump(memory_get_usage());
    $GLOBALS['request_count'] += 1;

    $client = new SimpleHttpClient($server->reactor, function ($response) use ($exceptResponse) {
        $rawBody = isset($exceptResponse['rawBody']) ? $exceptResponse['rawBody'] : json_encode($exceptResponse['body']);
        if ($rawBody != $response->body) {
            print 'error' . PHP_EOL;
        }
    });
    $client->request('GET', 'http://127.0.0.1:10000' . $request);
});

$server->start();
?>
--EXPECTF--
client requests: 1000
client closed: 1000
client destruct count: 1000
server closed: 1000
server destruct count: 1000
