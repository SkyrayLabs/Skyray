--TEST--
Test for skyray\defer\Promise::all()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\defer\Promise;
use skyray\Reactor;

$reactor = new Reactor();


return;
$promise = Promise::all([
    Promise::resolve(1),
    Promise::resolve(2),
]);

$promise->then(function ($values) {
    var_dump($values);
    return $values;
});
?>
--EXPECTF--
