--TEST--
Test for skyray\defer\Promise with resolve & reject
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
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

echo "==== 2. tests for Promise::resolve() throws exception ====\n";
$promise->then(function ($value) {
    throw new Exception($value);
})->then(null, function ($reason) {
    return $reason;
})->done(null, function ($reason) {
    echo "-- reject --\n";
    var_dump($reason->getMessage());
});

echo "==== 3. tests for Promise::reject() ====\n";
$promise = Promise::reject(new Exception('reason'));
$promise->then(null, function ($reason) {
    throw new Exception('reason 2');
})->done(null, function ($reason) {
    echo "-- reject --\n";
    var_dump($reason->getMessage());
    var_dump($reason->getPrevious()->getMessage());
});
?>
--EXPECTF--
==== 1. tests for Promise::resolve() ====
-- done1 --
string(5) "hello"
-- done2 --
string(20) "hellohellohellohello"
==== 2. tests for Promise::resolve() throws exception ====
-- reject --
string(5) "hello"
==== 3. tests for Promise::reject() ====
-- reject --
string(8) "reason 2"
string(6) "reason"
