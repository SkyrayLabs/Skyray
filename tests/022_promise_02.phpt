--TEST--
Test for skyray\defer\Deferred resolve
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
    $deferred->resolve('value');
});

$promise->then(function ($value) {
    echo "--- then 1 ---\n";
    var_dump($value);
    return $value . '|' . $value;
})->then(null)
->then(function ($value) {
    echo "--- then 2 ---\n";
    var_dump($value);
    return $value;
})
->done(function ($value) {
    echo "--- done1 ---\n";
    var_dump($value);
});

$promise->then(function ($value) {
    echo "--- then 3 ---\n";
    throw new Exception($value);
})->then($notCalled, function ($reason) {
    echo "--- then 4 ---\n";
    var_dump('exception: ' . $reason->getMessage());
    throw $reason;
})->done($notCalled, function ($reason) {
    echo "--- done2 ---\n";
    var_dump('exception: ' . $reason->getMessage());
});

$reactor->run();

return;
?>
--EXPECTF--
--- then 1 ---
string(5) "value"
--- then 2 ---
string(11) "value|value"
--- done1 ---
string(11) "value|value"
--- then 3 ---
--- then 4 ---
string(16) "exception: value"
--- done2 ---
string(16) "exception: value"
