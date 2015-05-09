--TEST--
Test for skyray\defer\Promise with resolve & reject
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\defer\Deferred;
use skyray\defer\Promise;

echo "==== 1. tests for Promise::resolve() ====\n";
$promise = Promise::resolve('hello');

$promise->done(function ($value) {
    echo "-- done1 --\n";
    var_dump($value);
    return $value;
});

$promise->then(function ($value) {
    return $value . $value;
})->then(function ($value) {
    return $value . $value;
})->done(function ($value) {
    echo "-- done2 --\n";
    var_dump($value);
    return $value;
});
?>
--EXPECTF--
==== 1. tests for Promise::resolve() ====
-- done1 --
string(5) "hello"
-- done2 --
string(20) "hellohellohellohello"
