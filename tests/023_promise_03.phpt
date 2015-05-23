--TEST--
Test for skyray\defer\Promise chained resolving
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\defer\Deferred;
use skyray\Reactor;

$reactor = new Reactor();

$deferred = new Deferred();
$promise = $deferred->promise();

$notCalled = function ($valueOrReason) {
    var_dump('should not be called');
    return $valueOrReason;
};

$reactor->addTimer(100, function () use ($deferred) {
    $deferred->resolve(1);
});


$promise->then(function ($value) {
    echo "--- then 1 ---\n";
    var_dump($value);
    return $value * 2;
})->then(function ($value) use ($reactor) {
    return \skyray\defer\Promise::resolve($value * 2);
})->done(function ($value) {
    echo "--- done 1 ---\n";
    var_dump($value);
});


$promise->then(function ($value) {
    echo "--- then 2 ---\n";
    var_dump($value);
    return $value * 2;
})->then(function ($value) use ($reactor) {
    $deferred = new Deferred();
    $reactor->addTimer(100, function () use ($deferred, $value) {
        $deferred->resolve($value * 2);
    });
    return $deferred->promise();
})->then(function ($value) {
    echo "--- then 3 ---\n";
    var_dump($value);
    return $value;
})->done(function ($value) {
    echo "--- done 2 ---\n";
    var_dump($value);
});

$reactor->run();
?>
--EXPECTF--
--- then 1 ---
int(1)
--- done 1 ---
int(4)
--- then 2 ---
int(1)
--- then 3 ---
int(4)
--- done 2 ---
int(4)
