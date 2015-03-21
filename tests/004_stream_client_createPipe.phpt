--TEST--
Test for StreamClient::createPipe()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\StreamClient;
use skyray\core\SkyrayException;

$client = new StreamClient(null);
$streams = $client->createPipe(false);

try {
    $streams[0]->write("foo");
    echo "should never reached\n";
} catch (SkyrayException $e) {
    echo $e->getMessage() . PHP_EOL;
}

var_dump($streams[1]->write("data"));
var_dump($streams[0]->read());
$streams[1]->close();
var_dump($streams[0]->read());

try {
    $streams[1]->write("data");
    echo "should never reached\n";
} catch (SkyrayException $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECTF--
The stream is not writable.
bool(true)
string(4) "data"
string(0) ""
[9] Bad file descriptor
