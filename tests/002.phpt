--TEST--
Test for connections
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\core\Stream;

?>
--EXPECTF--
