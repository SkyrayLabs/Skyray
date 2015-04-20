--TEST--
Test for skyray\Object
--SKIPIF--
<?php if (!extension_loaded("skyray")) print "skip"; ?>
--FILE--
<?php
use skyray\Object;

class MyObject extends Object
{
    public $prop1;

    private $_prop2;

    public function setProp2($value)
    {
        $this->_prop2 = $value;
    }

    public function getProp2()
    {
        return $this->_prop2;
    }

    public function getProp3()
    {
        return 3;
    }
}

$o = new MyObject([
    'prop1' => 1,
    'prop2' => 2,
]);
var_dump($o);
var_dump($o->prop2);

try {
    $o->prop3 = 3;
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    $o->prop4 = 4;
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

var_dump($o->prop3);

try {
    $a = $o->prop4;
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECTF--
object(MyObject)#1 (2) {
  ["prop1"]=>
  int(1)
  ["_prop2":"MyObject":private]=>
  int(2)
}
int(2)
Setting read-only property: MyObject::prop3
Setting unknown property: MyObject::prop4
int(3)
Getting unknown property: MyObject::prop4