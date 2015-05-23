--TEST--
Test for skyray\processing\Process basic process management
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\processing\Process;
echo "=== test for function ===\n";
function sub_process() {
    sleep(1);
    echo "it works\n";
    return 100;
}

$process = new Process('sub_process');
$process->start();
echo 'Process id: ' . $process->getPid() . PHP_EOL;
$process->join();
echo 'Process exited with: ' . $process->getExitCode() . PHP_EOL;

echo "=== test for method ===\n";

class TestWorker {
    public function run($foo, $bar)
    {
        echo "it works $foo $bar\n";
        return 100;
    }
}

$worker = new TestWorker();
$process = new Process([$worker, 'run'], ['foo', 'bar']);
$process->start();
echo 'Process id: ' . $process->getPid() . PHP_EOL;
$process->join();
echo 'Process exited with: ' . $process->getExitCode() . PHP_EOL;

echo "==== test throw exception ====\n";

class TestWorker2 {
    public function run()
    {
        throw new \RuntimeException('i am an exception');
    }
}
set_exception_handler(function ($e) {
    print '[process]: ' . $e->getMessage() . PHP_EOL;
});
$worker = new TestWorker2();
$process = new Process([$worker, 'run']);
$process->start();
$process->join();
echo 'Process exited with: ' . $process->getExitCode() . PHP_EOL;
echo "done\n";
?>
--EXPECTF--
=== test for function ===
Process id: %d
it works
Process exited with: 100
=== test for method ===
Process id: %d
it works foo bar
Process exited with: 100
==== test throw exception ====
[process]: i am an exception
Process exited with: 101
done
