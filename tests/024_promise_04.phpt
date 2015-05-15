--TEST--
Test for skyray\defer\Promise uncaught exception
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\defer\Promise;
use skyray\core\Reactor;

$reactor = new Reactor();

$func = function () {
    throw new Exception('error1');
};
$notCalled = function () {
    var_dump('should not be called');
};

try {
    $promise = Promise::resolve(1);
    $promise->then(function ($value) {
        return $value;
    })->done($func, $notCalled);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $promise = Promise::resolve(1);
    $promise->then(function ($value) {
        throw new Exception('error2');
    })->done($notCalled); // unhandled error should be thrown too
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
string(6) "error1"
string(6) "error2"
