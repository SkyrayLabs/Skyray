--TEST--
Test for skyray\watcher\ProcessWatcher
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\Reactor;
use skyray\processing\Process;
use skyray\watcher\ProcessWatcherHandler;

class Handler extends ProcessWatcherHandler
{
    public function onExited($process)
    {
        print 'exited: ' . $process->getExitCode() . PHP_EOL;
    }
}

$reactor = new Reactor();

$process = new Process(function () {
    var_dump('in process');
    return 100;
});
$process->start();

$watcher = $reactor->watchProcess($process, new Handler());
var_dump(get_class($watcher));
$reactor->run();

return;
?>
--EXPECTF--
string(29) "skyray\watcher\ProcessWatcher"
string(10) "in process"
exited: 100
