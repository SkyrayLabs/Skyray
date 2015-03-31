--TEST--
Test for Reactor() timer support
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\Reactor;

$reactor = new Reactor();
$reactor->addTimer(10, function ($timer) {
    echo 'timer1' . PHP_EOL;
    echo get_class($timer) . PHP_EOL;
});

$reactor->addPeriodicTimer(10, function ($timer2) {
    echo 'timer2' . PHP_EOL;
    static $count = 2;
    if ($count-- == 0) {
        $timer2->cancel();
    }
});

$reactor->run();

?>
--EXPECTF--
timer1
skyray\core\Timer
timer2
timer2
timer2
