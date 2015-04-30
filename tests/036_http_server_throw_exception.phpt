--TEST--
Test for skyray\http\Server with exception handling
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Request;
use skyray\http\ServerProtocol;
use skyray\http\Server;

require __DIR__ . '/includes/SimpleHttpClient.php';

set_exception_handler(function ($e) {
    print "[error] {$e->getMessage()}\n";
});

class MyProtocol extends ServerProtocol
{
    public function onRequest(Request $request)
    {
        if ($request->getUri() == '/return_non_response') {
            return 'this is not a response';
        } else if ($request->getUri() == '/throw_exception') {
            throw new Exception("a exception");
        }
    }
}

$server = new Server(function () {
    return new MyProtocol();
});

$server->listen('0.0.0.0', 10000);

$server->reactor->addTimer(10, function ($timer) use ($server) {
    $client = new SimpleHttpClient($server->reactor, function ($response) {
        print "[response] {$response->statusCode} {$response->statusPhase}: {$response->body}\n";
    });
    $client->request('GET', 'http://127.0.0.1:10000/return_non_response');
    $client->request('GET', 'http://127.0.0.1:10000/throw_exception');

    $server->reactor->addTimer(20, function () use ($server) {
        $server->reactor->stop();
    });
});

$server->start();

?>
--EXPECTF--
[error] The onReqeust() method excepts 'skyray\http\Response' instance as return value
[error] a exception
[response] 500 Internal Server Error: There was an internal error occurred on the server.
[response] 500 Internal Server Error: There was an internal error occurred on the server.
