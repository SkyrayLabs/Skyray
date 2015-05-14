/*
 * deferred.c
 *
 *  Created on: May 3, 2015
 *      Author: hujin
 */

#include "defer.h"

zend_class_entry *skyray_ce_Deferred;
zend_object_handlers skyray_handler_Deferred;

static inline skyray_deferred_t * skyray_deferred_from_obj(zend_object *obj) {
    return (skyray_deferred_t*)((char*)(obj) - XtOffsetOf(skyray_deferred_t, std));
}

zend_object * skyray_deferred_object_new(zend_class_entry *ce)
{
    skyray_deferred_t *intern;
    intern = ecalloc(1, 100 + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->promise = NULL;

    intern->std.handlers = &skyray_handler_Deferred;
    return &intern->std;
}

void skyray_deferred_object_free(zend_object *object)
{
    skyray_deferred_t *intern = skyray_deferred_from_obj(object);

    if (intern->promise) {
        zend_object_release(intern->promise);
    }

    zend_object_std_dtor(&intern->std);
}

zend_object * skyray_deferred_create_promise()
{
    zval promise;
    object_init_ex(&promise, skyray_ce_Promise);
    return Z_OBJ_P(&promise);
}


SKYRAY_METHOD(deferred, __construct)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(deferred, promise)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_deferred_t *intern = skyray_deferred_from_obj(Z_OBJ_P(getThis()));
    if (!intern->promise) {
        intern->promise = skyray_deferred_create_promise();
    }

    GC_REFCOUNT(intern->promise) ++;
    RETURN_OBJ(intern->promise);
}

SKYRAY_METHOD(deferred, resolve)
{
    zval *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
        return;
    }

    skyray_deferred_t *intern = skyray_deferred_from_obj(Z_OBJ_P(getThis()));
    skyray_promise_t *promise = skyray_promise_from_obj(intern->promise);
    skyray_promise_do_resolve(promise, value, 1);
}

SKYRAY_METHOD(deferred, reject)
{
    zval *reason;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &reason) == FAILURE) {
        return;
    }

}

SKYRAY_METHOD(deferred, notify)
{
    zval *update;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &update) == FAILURE) {
        return;
    }
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolve, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reject, 0, 0, 1)
    ZEND_ARG_INFO(0, reason)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_notify, 0, 0, 1)
    ZEND_ARG_INFO(0, update)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(deferred, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(deferred, promise, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(deferred, resolve, arginfo_resolve, ZEND_ACC_PUBLIC)
    SKYRAY_ME(deferred, reject, arginfo_reject, ZEND_ACC_PUBLIC)
    SKYRAY_ME(deferred, notify, arginfo_notify, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(deferred)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\defer\\Deferred", class_methods);
    skyray_ce_Deferred = zend_register_internal_class(&ce);
    skyray_ce_Deferred->create_object = skyray_deferred_object_new;

    memcpy(&skyray_handler_Deferred, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Deferred.offset = XtOffsetOf(skyray_deferred_t, std);
    skyray_handler_Deferred.free_obj = skyray_deferred_object_free;

    return SUCCESS;
}
