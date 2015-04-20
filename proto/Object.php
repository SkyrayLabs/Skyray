<?php
/**
 * Created by PhpStorm.
 * User: hujin
 * Date: 15-4-20
 */

namespace skyray;

/**
 * Object is the base class that implements the *property* feature, this was inspired by Yii framework.
 *
 * A property is defined by a getter method (e.g. `getLabel`), and/or a setter method (e.g. `setLabel`).
 *
 * Besides the property feature, Object also can be initialized via given configuration.
 *
 * @package skyray
 * @since 0.1
 */
class Object
{
    /**
     * Constructor
     *
     * @param array $config The params to configure the object, this should be always the last parameter.
     */
    public function __construct($config = [])
    {

    }

    /**
     * Returns the value of an object property.
     *
     * @param $name
     */
    public function __get($name)
    {

    }

    /**
     * Sets value of an object property.
     *
     * @param $name
     * @param $value
     */
    public function __set($name, $value)
    {

    }

    /**
     * Checks if the named property is set (not null).
     *
     * @param $name
     */
    public function __isset($name)
    {

    }

    /**
     * Sets an object property to null.
     *
     * @param $name
     */
    public function __unset($name)
    {

    }
}
