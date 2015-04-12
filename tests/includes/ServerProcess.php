<?php

use skyray\processing\Process;

require_once __DIR__ . '/SimpleHttpServer.php';

class ServerProcess
{
    protected $process;

    public function run()
    {
        $server = new SimpleHttpServer('0.0.0.0', 2333);
        $server->start();
    }

    public function start()
    {
        $this->process = new Process([$this, 'run']);
        $this->process->start();
        usleep(200000);
    }

    public function stop()
    {
        posix_kill($this->process->getPid(), 15);
        $this->process->join();
    }
}

$process = new ServerProcess();
$process->start();

return $process;
