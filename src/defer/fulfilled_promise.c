/*
 * fulfilled_promise.c
 *
 *  Created on: May 3, 2015
 *      Author: hujin
 */

#include "defer.h"

zend_class_entry *skyray_ce_FulfilledPromise;

zend_object_handlers skyray_handler_FulfilledPromise;


zend_object * skyray_fulfilled_promise_object_new(zend_class_entry *ce)
{
    skyray_fulfilled_promise_t *intern;
    intern = ecalloc(1, sizeof(skyray_fulfilled_promise_t) + zend_object_properties_size(ce));

    skyray_promise_object_init(intern, ce);

    intern->std.handlers = &skyray_handler_FulfilledPromise;
    return &intern->std;
}

void skyray_fulfilled_promise_object_free(zend_object *object)
{
    skyray_fulfilled_promise_t *intern = skyray_fulfilled_promise_from_obj(object);

    zend_llist_destroy(&intern->on_fulfilled);
    zend_llist_destroy(&intern->on_rejcted);
    zend_llist_destroy(&intern->on_notify);

    zval_ptr_dtor(&intern->result);

    zend_object_std_dtor(&intern->std);
}

skyray_fulfilled_promise_t * skyray_fulfilled_promise_new(zval *value, zend_bool is_copy_required)
{
    zval tmp;
    skyray_fulfilled_promise_t *intern;

    object_init_ex(&tmp, skyray_ce_FulfilledPromise);

    intern = skyray_fulfilled_promise_from_obj(Z_OBJ_P(&tmp));
    if (is_copy_required) {
        ZVAL_COPY(&intern->result, value);
    } else {
        ZVAL_COPY_VALUE(&intern->result, value);
    }

    return intern;
}

skyray_promise_t * skyray_fulfilled_promise_then(skyray_fulfilled_promise_t *self, zval *on_fulfilled)
{
    if (on_fulfilled == NULL && !zend_is_callable(on_fulfilled, 0, NULL)) {
        return NULL;
    }

    zval zpromise;
    skyray_promise_t *promise;

    object_init_ex(&zpromise, skyray_ce_Promise);
    promise = skyray_promise_from_obj(Z_OBJ_P(&zpromise));

    zval value;
    skyray_promise_call_handler(on_fulfilled, &self->result, &value);

    if (EG(exception)) {
        ZVAL_OBJ(&value, EG(exception));
        EG(exception) = NULL;
        skyray_promise_do_reject(promise, &value, 0);
    } else {
        skyray_promise_do_resolve(promise, &value, 0);
    }

    return promise;
}

void skyray_fulfilled_promise_done(skyray_fulfilled_promise_t *self, zval *on_fulfilled)
{
    if (on_fulfilled == NULL && !zend_is_callable(on_fulfilled, 0, NULL)) {
        return;
    }

    skyray_promise_call_handler(on_fulfilled, &self->result, NULL);
}

SKYRAY_METHOD(fulfilled_promise, __construct)
{
    zval *ptr = NULL, value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", ptr) == FAILURE) {
        return;
    }

    if (ptr == NULL) {
        ptr = &value;
        ZVAL_NULL(ptr);
    }

    skyray_fulfilled_promise_t *intern = skyray_fulfilled_promise_from_obj(Z_OBJ_P(getThis()));
    ZVAL_COPY(&intern->result, ptr);
}

SKYRAY_METHOD(fulfilled_promise, then)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &on_fulfilled, &on_rejected) == FAILURE) {
        return;
    }

    skyray_fulfilled_promise_t *intern = skyray_fulfilled_promise_from_obj(Z_OBJ_P(getThis()));

    skyray_promise_t *promise = skyray_fulfilled_promise_then(intern, on_fulfilled);

    if (promise != NULL) {
        RETURN_OBJ(&promise->std);
    } else {
        RETURN_ZVAL(getThis(), 1, 0);
    }
}

SKYRAY_METHOD(fulfilled_promise, done)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &on_fulfilled, &on_rejected) == FAILURE) {
        return;
    }

    skyray_fulfilled_promise_t *intern = skyray_fulfilled_promise_from_obj(Z_OBJ_P(getThis()));

    skyray_fulfilled_promise_done(intern, on_fulfilled);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_then_or_done, 0, 0, 1)
    ZEND_ARG_INFO(0, fulfilledHandler)
    ZEND_ARG_INFO(0, errorHandler)
    ZEND_ARG_INFO(0, notifyHandler)
ZEND_END_ARG_INFO()



static const zend_function_entry class_methods[] = {
    SKYRAY_ME(fulfilled_promise, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(fulfilled_promise, then, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(fulfilled_promise, done, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(fulfilled_promise)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\defer\\FulfilledPromise", class_methods);
    skyray_ce_FulfilledPromise = zend_register_internal_class(&ce);
    skyray_ce_FulfilledPromise->create_object = skyray_fulfilled_promise_object_new;

    memcpy(&skyray_handler_FulfilledPromise, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_FulfilledPromise.offset = XtOffsetOf(skyray_fulfilled_promise_t, std);
    skyray_handler_FulfilledPromise.free_obj = skyray_fulfilled_promise_object_free;

    return SUCCESS;
}

