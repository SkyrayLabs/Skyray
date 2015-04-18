--TEST--
Test for skyray\http\Response class
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\http\Response;

$response = new Response();
var_dump($response->getStatusCode()); // default 200
var_dump($response->getReasonPhrase()); // default to Success

$response->setStatus(201, 'Created');
var_dump($response->getStatusCode());
var_dump($response->getReasonPhrase());

?>
--EXPECTF--
int(200)
NULL
int(201)
string(7) "Created"
