/*
 * rejected_promise.c
 *
 *  Created on: May 9, 2015
 *      Author: hujin
 */

#include "defer.h"

zend_class_entry *skyray_ce_RejectedPromise;

zend_object_handlers skyray_handler_RejectedPromise;


zend_object * skyray_rejected_promise_object_new(zend_class_entry *ce)
{
    skyray_rejected_promise_t *intern;
    intern = ecalloc(1, sizeof(skyray_rejected_promise_t) + zend_object_properties_size(ce));

    skyray_promise_object_init(intern, ce);

    intern->std.handlers = &skyray_handler_RejectedPromise;
    return &intern->std;
}

skyray_rejected_promise_t * skyray_rejected_promise_new(zval *value, zend_bool is_copy_required)
{
    zval tmp;
    skyray_rejected_promise_t *intern;

    object_init_ex(&tmp, skyray_ce_RejectedPromise);

    intern = skyray_rejected_promise_from_obj(Z_OBJ_P(&tmp));
    if (is_copy_required) {
        ZVAL_COPY(&intern->result, value);
    } else {
        ZVAL_COPY_VALUE(&intern->result, value);
    }

    return intern;
}
skyray_promise_t * skyray_rejected_promise_then(skyray_rejected_promise_t *self, zval *on_rejected)
{
    if (on_rejected == NULL && !zend_is_callable(on_rejected, 0, NULL)) {
        return NULL;
    }

    zval zpromise;
    skyray_promise_t *promise;

    object_init_ex(&zpromise, skyray_ce_Promise);
    promise = skyray_promise_from_obj(Z_OBJ_P(&zpromise));

    zval value;
    skyray_promise_call_handler(on_rejected, &self->result, &value);

    if (EG(exception)) {
        ZVAL_OBJ(&value, EG(exception));

        if (Z_TYPE_P(&self->result) == IS_OBJECT
                && instanceof_function(Z_OBJCE_P(&self->result), zend_exception_get_base())
                && EG(exception) != Z_OBJ_P(&self->result)) {

            zval_add_ref(&self->result);
            zend_exception_set_previous(EG(exception), Z_OBJ_P(&self->result));
        }
        EG(exception) = NULL;
        skyray_promise_do_reject(promise, &value, 0);
    } else {
        skyray_promise_do_reject(promise, &value, 0);
    }

    return promise;
}

void skyray_rejected_promise_done(skyray_rejected_promise_t *self, zval *on_rejected)
{
    if (on_rejected == NULL && !zend_is_callable(on_rejected, 0, NULL)) {
        return;
    }

    skyray_promise_call_handler(on_rejected, &self->result, NULL);
}

SKYRAY_METHOD(rejected_promise, __construct)
{
    zval *ptr = NULL, value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", ptr) == FAILURE) {
        return;
    }

    if (ptr == NULL) {
        ptr = &value;
        ZVAL_NULL(ptr);
    }

    skyray_rejected_promise_t *intern = skyray_rejected_promise_from_obj(Z_OBJ_P(getThis()));
    ZVAL_COPY(&intern->result, ptr);
}

SKYRAY_METHOD(rejected_promise, then)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &on_fulfilled, &on_rejected) == FAILURE) {
        return;
    }

    skyray_rejected_promise_t *intern = skyray_rejected_promise_from_obj(Z_OBJ_P(getThis()));

    skyray_promise_t *promise = skyray_rejected_promise_then(intern, on_rejected);

    if (promise != NULL) {
        RETURN_OBJ(&promise->std);
    } else {
        RETURN_ZVAL(getThis(), 1, 0);
    }
}

SKYRAY_METHOD(rejected_promise, done)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &on_fulfilled, &on_rejected) == FAILURE) {
        return;
    }

    skyray_rejected_promise_t *intern = skyray_rejected_promise_from_obj(Z_OBJ_P(getThis()));

    skyray_rejected_promise_done(intern, on_rejected);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_then_or_done, 0, 0, 1)
    ZEND_ARG_INFO(0, rejectedHandler)
    ZEND_ARG_INFO(0, errorHandler)
    ZEND_ARG_INFO(0, notifyHandler)
ZEND_END_ARG_INFO()



static const zend_function_entry class_methods[] = {
    SKYRAY_ME(rejected_promise, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(rejected_promise, then, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(rejected_promise, done, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(rejected_promise)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\defer\\RejectedPromise", class_methods);
    skyray_ce_RejectedPromise = zend_register_internal_class_ex(&ce, skyray_ce_Promise);
    skyray_ce_RejectedPromise->create_object = skyray_rejected_promise_object_new;

    memcpy(&skyray_handler_RejectedPromise, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_RejectedPromise.offset = XtOffsetOf(skyray_rejected_promise_t, std);
    skyray_handler_RejectedPromise.free_obj = skyray_promise_object_free;

    return SUCCESS;
}
