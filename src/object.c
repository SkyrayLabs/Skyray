/*
 * object.c
 *
 *  Created on: Apr 20, 2015
 *      Author: hujin
 */

#include "skyray.h"

zend_bool zend_class_method_exists(zend_class_entry *ce, zend_string *method)
{
    zend_bool exists;
    zend_string *lcmethod = zend_string_tolower(method);

    exists = zend_hash_exists(&ce->function_table, lcmethod);
    zend_string_release(lcmethod);

    return exists;
}

void skyray_object___set(zval *self, zend_string *name, zval *value)
{
    int result = 0;
    zend_object *object = Z_OBJ_P(self);

    smart_str setter = {0};
    smart_str_appendl(&setter, "set", 3);
    smart_str_append_ex(&setter, name, 0);
    smart_str_0(&setter);

    smart_str getter = {0};
    smart_str_appendl(&getter, "get", 3);
    smart_str_append_ex(&getter, name, 0);
    smart_str_0(&getter);

    zend_str_tolower(setter.s->val, setter.s->len);
    zend_str_tolower(getter.s->val, getter.s->len);

    if (zend_class_method_exists(object->ce, setter.s)) {
        zend_call_method(self, object->ce, NULL, setter.s->val, setter.s->len, NULL, 1, value, NULL);
        result = 1;
    }else if (zend_class_method_exists(object->ce, getter.s)) {
        zend_throw_exception_ex(skyray_ce_InvalidCallException, 0, "Setting read-only property: %s::%s", object->ce->name->val, name->val);
    }else {
        zend_throw_exception_ex(skyray_ce_UnknownPropertyException, 0, "Setting unknown property: %s::%s", object->ce->name->val, name->val);
    }
    /*
    if (!result) {
        zval_dtor(value);
        efree(value);
    }
    */
    smart_str_free(&setter);
    smart_str_free(&getter);
}

void skyray_object___get(zval *self, const zend_string *name, zval *retval) {
    zend_object *object = Z_OBJ_P(self);
    smart_str getter = {0};
    smart_str_appendl(&getter, "get", 3);
    smart_str_append_ex(&getter, name, 0);
    smart_str_0(&getter);
    zend_str_tolower(getter.s->val, getter.s->len);

    if (zend_class_method_exists(object->ce, getter.s)) {
        zend_call_method(self, object->ce, NULL, getter.s->val, getter.s->len, retval, 0, NULL, NULL);
    }else {
        zend_throw_exception_ex(skyray_ce_UnknownPropertyException, 0, "Getting unknown property: %s::%s", object->ce->name->val, name->val);
    }
    smart_str_free(&getter);

}

zend_bool skyray_object___isset(zval *self, const zend_string *name) {
    zend_object *object = Z_OBJ_P(self);
    smart_str getter = {0};
    smart_str_appendl(&getter, "get", 3);
    smart_str_append_ex(&getter, name, 0);
    smart_str_0(&getter);
    zend_str_tolower(getter.s->val, getter.s->len);
    zval *value = NULL;
    zend_bool ret = 0;

    if (zend_class_method_exists(object->ce, getter.s)) {
        zend_call_method(self, object->ce, NULL, getter.s->val, getter.s->len, &value, 0, NULL, NULL);
        ret = Z_TYPE_P(value) != IS_NULL;
    }
    smart_str_free(&getter);
    return ret;
}

void skyray_object___unset(zval *self, const zend_string *name) {
    zend_object *object = Z_OBJ_P(self);
    int result = 0;

    smart_str setter = {0};
    smart_str_appendl(&setter, "set", 3);
    smart_str_append_ex(&setter, name, 0);
    smart_str_0(&setter);

    smart_str getter = {0};
    smart_str_appendl(&getter, "get", 3);
    smart_str_append_ex(&getter, name, 0);
    smart_str_0(&getter);

    zend_str_tolower(setter.s->val, setter.s->len);
    zend_str_tolower(getter.s->val, getter.s->len);

    if (zend_class_method_exists(object->ce, setter.s)) {
        zval value;
        ZVAL_NULL(&value);
        zend_call_method(self, object->ce, NULL, setter.s->val, setter.s->len, NULL, 1, value, NULL);

    }else if (zend_class_method_exists(object->ce, getter.s)) {
        zend_throw_exception_ex(skyray_ce_InvalidCallException, 0, "Unsetting read-only property: %s::%s", object->ce->name->val, name->val);
    }
    /*
    if (!result) {
        zval_dtor(value);
        efree(value);
    }
    */
    smart_str_free(&setter);
    smart_str_free(&getter);
}

void skyray_object_configure(zval *self, zend_array *properties) {
    HashTable * ht = properties;
    zval *value, key;

    zend_hash_internal_pointer_reset(ht);
    while (zend_hash_has_more_elements(ht) == SUCCESS) {
        zend_hash_get_current_key_zval(ht, &key);
        value = zend_hash_get_current_data(ht);
        Z_OBJ_P(self)->handlers->write_property(self, &key, value, NULL);
        zval_dtor(&key);
        zend_hash_move_forward(ht);
    }
}


SKYRAY_METHOD(Object, __construct) {
    zval *config = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &config) == FAILURE) {
        return;
    }

    if (config) {
        skyray_object_configure(getThis(), Z_ARR_P(config));
    }
}

SKYRAY_METHOD(Object, __get) {
    zend_string *name;
    zval retval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

    skyray_object___get(getThis(), name, &retval);

    RETURN_ZVAL(&retval, 0, 0);
}

SKYRAY_METHOD(Object, __set) {
    zend_string *name;
    zval *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
        return;
    }

    skyray_object___set(getThis(), name, value);
}

SKYRAY_METHOD(Object, __isset) {
    zend_string *name;
    zend_bool isset;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

    isset = skyray_object___isset(getThis(), name);
    RETURN_BOOL(isset);
}

SKYRAY_METHOD(Object, __unset) {
    zend_string *name;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
        return;
    }

    skyray_object___unset(getThis(), name);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_class___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(Object, __construct,    arginfo_class___construct,    ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(Object, __set,          arginfo_magic_set,            ZEND_ACC_PUBLIC)
    SKYRAY_ME(Object, __get,          arginfo_magic_get,            ZEND_ACC_PUBLIC)
    SKYRAY_ME(Object, __isset,        arginfo_magic_isset,          ZEND_ACC_PUBLIC)
    SKYRAY_ME(Object, __unset,        arginfo_magic_unset,          ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_object)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\Object", class_methods);
    skyray_ce_Object = zend_register_internal_class(&ce);
    return 0;
}
