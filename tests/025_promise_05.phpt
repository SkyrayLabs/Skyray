--TEST--
Test for skyray\defer\Promise shorthand method catch() and finally()
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

echo "=== Fulfilled Promise ===\n";

$promise = Promise::resolve(1);
$promise->finally(function ($value) {
    var_dump($value);
});
$promise->catch($notCalled);

echo "=== Rejected Promise ===\n";

$promise = Promise::reject('error');
$promise->finally(function ($value) {
    var_dump($value);
});
$promise->catch(function ($reason) {
    var_dump($reason);
});

echo "=== Normal Promise Finally 1 ===\n";

$promise = Promise::resolve(1);
$promise->then(function ($value) {
    return $value;
})->finally(function ($value) {
    var_dump($value);
});

echo "=== Normal Promise Finally 2 ===\n";

$promise = Promise::resolve(1);
$promise->then(function ($value) {
    throw new Exception("reason");
})->finally(function ($valueOrReason) {
    var_dump($valueOrReason->getMessage());
});

echo "=== Normal Promise Catch ===\n";

$promise = Promise::resolve(1);
$promise->then(function ($value) {
    throw new Exception('error2');
})->catch(function ($reason) {
    return 'try again';
})->finally(function ($value) {
    var_dump($value);
});
return;
?>
--EXPECTF--
=== Fulfilled Promise ===
int(1)
=== Rejected Promise ===
string(5) "error"
string(5) "error"
=== Normal Promise Finally 1 ===
int(1)
=== Normal Promise Finally 2 ===
string(6) "reason"
=== Normal Promise Catch ===
string(9) "try again"
