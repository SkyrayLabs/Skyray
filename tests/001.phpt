--TEST--
Test for basic process management
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\processing\Process;

function sub_process() {
    sleep(1);
    echo "it works\n";
}

$process = new Process('sub_process');
$process->start();
echo $process->getPid() . PHP_EOL;
$process->join();
echo "done\n";
?>
--EXPECTF--
%d
it works
done
