--TEST--
Test for skyray\watcher\FdWatcher()
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\Reactor;
use skyray\watcher\FdWatcher;
use skyray\watcher\FdWatcherHandler;

$server = require_once __DIR__ . '/includes/ServerProcess.php';
register_shutdown_function(function () use ($server) {
    $server->stop();
});

$reactor = new Reactor();

class Handler extends FdWatcherHandler
{
    protected $type;

    public function __construct($type = '')
    {
        $this->type = $type;
    }

    public function onReadable($fd)
    {
        printf("readable: %s\n", $this->type);
        print_stat($this->watcher, 'before readable');
        printf("received: %s\n", trim(fread($fd, 8192)));
        $this->watcher->stop(FdWatcher::READABLE);
        print_stat($this->watcher, 'after  readable');
    }

    public function onWritable($fd)
    {
        printf("writable: %s\n", $this->type);
        print_stat($this->watcher, 'before writable');
        fwrite($fd, "GET / HTTP/1.1\r\n\r\n");
        $this->watcher->watch(FdWatcher::READABLE);
        print_stat($this->watcher, 'after  writable');
    }

    public function onError($fd, $error)
    {
        print 'error: ' . $error . PHP_EOL;
    }

    public function __destruct()
    {
        var_dump(get_class($this->watcher));
    }
}

function print_stat(FdWatcher $watcher, $prefix = '')
{
    printf("%s: readable: %d, writable: %d\n", $prefix,
        $watcher->isWatched(FdWatcher::READABLE),
        $watcher->isWatched(FdWatcher::WRITABLE));
}

$stream = stream_socket_client('tcp://0.0.0.0:2333');
$watcher = $reactor->watch($stream, new Handler('stream'), FdWatcher::WRITABLE);
print_stat($watcher, 'init');

$reactor->addTimer(100, function () use ($watcher, $reactor) {

    print_stat($watcher, 'stop');
});

$reactor->run();

return;
?>
--EXPECTF--
init: readable: 0, writable: 1
writable: stream
before writable: readable: 0, writable: 1
after  writable: readable: 1, writable: 0
readable: stream
before readable: readable: 1, writable: 0
received: HTTP/1.1 404 Not Found
after  readable: readable: 0, writable: 0
stop: readable: 0, writable: 0
string(24) "skyray\watcher\FdWatcher"
