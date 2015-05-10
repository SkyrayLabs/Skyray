/*
 * promise.c
 *
 *  Created on: May 3, 2015
 *      Author: hujin
 */

#include "defer.h"

zend_class_entry *skyray_ce_Promise;

zend_object_handlers skyray_handler_Promise;

zend_string *intern_str_then;
zend_string *intern_str_done;

void skyray_promise_object_init(skyray_promise_t *self, zend_class_entry *ce)
{
    zend_object_std_init(&self->std, ce);
    object_properties_init(&self->std, ce);

    ZVAL_NULL(&self->result);

    zend_llist_init(&self->on_fulfilled, 10, zend_object_release, 0);
    zend_llist_init(&self->on_rejcted, 10, zend_object_release, 0);
    zend_llist_init(&self->on_notify, 10, zend_object_release, 0);
}

zend_object * skyray_promise_object_new(zend_class_entry *ce)
{
    skyray_promise_t *intern;
    intern = ecalloc(1, sizeof(skyray_promise_t) + zend_object_properties_size(ce));

    skyray_promise_object_init(intern, ce);

    intern->std.handlers = &skyray_handler_Promise;
    return &intern->std;
}

void skyray_promise_object_free(zend_object *object)
{
    skyray_promise_t *intern = skyray_promise_from_obj(object);

    zend_llist_destroy(&intern->on_fulfilled);
    zend_llist_destroy(&intern->on_rejcted);
    zend_llist_destroy(&intern->on_notify);

    zval_ptr_dtor(&intern->result);

    zend_object_std_dtor(&intern->std);
}

zval* skyray_promise_unwrap(skyray_promise_t *promise)
{
    zval *retval = &promise->result;

    while (skyray_is_promise_instance(retval)) {
        promise = skyray_promise_from_obj(Z_OBJ_P(retval));
        retval = &promise->result;
    }

    return retval;
}

void skyray_promise_then(skyray_promise_t *self, zval *on_fulfilled, zval *on_rejected, zval *retval)
{
    zval *result, params[2], function_name;

    if (!ZVAL_IS_NULL(&self->result)) {
        result = skyray_promise_unwrap(self);

        ZVAL_NULL(&params[0]);
        ZVAL_NULL(&params[1]);

        ZVAL_STR(&function_name, intern_str_then);

        if (on_fulfilled) {
            ZVAL_COPY_VALUE(&params[0], on_fulfilled);
        }
        if (on_rejected) {
            ZVAL_COPY_VALUE(&params[1], on_rejected);
        }

        call_user_function(NULL, result, &function_name, retval, 2, params);

        assert(EG(exception) == NULL);
    }
}

void skyray_promise_done(skyray_promise_t *self, zval *on_fulfilled, zval *on_rejected)
{
    zval *result, params[2], function_name, retval;

    if (!ZVAL_IS_NULL(&self->result)) {
        result = skyray_promise_unwrap(self);

        ZVAL_NULL(&params[0]);
        ZVAL_NULL(&params[1]);

        ZVAL_STR(&function_name, intern_str_done);

        if (on_fulfilled) {
            ZVAL_COPY_VALUE(&params[0], on_fulfilled);
        }
        if (on_rejected) {
            ZVAL_COPY_VALUE(&params[1], on_rejected);
        }

        call_user_function(NULL, result, &function_name, &retval, 2, params);

        zval_ptr_dtor(&retval);
    }
}

void skyray_promise_do_resolve(skyray_promise_t *self, zval *value, zend_bool is_copy_required)
{
    zval *result, tmp;

    if (!ZVAL_IS_NULL(&self->result)) {
        return;
    }

    if (skyray_is_promise_instance(value)) {
        result = skyray_promise_unwrap(skyray_promise_from_obj(Z_OBJ_P(value)));
    } else {
        skyray_fulfilled_promise_t *promise = skyray_fulfilled_promise_new(value, is_copy_required);
        ZVAL_OBJ(&tmp, &promise->std);
        result = &tmp;
    }

    if (result == &tmp) {
        ZVAL_COPY_VALUE(&self->result, result);
    } else {
        ZVAL_COPY(&self->result, result);
    }

    // call callbacks
}

void skyray_promise_do_reject(skyray_promise_t *self, zval *reason, zend_bool is_copy_required)
{
    if (!ZVAL_IS_NULL(&self->result)) {
        return;
    }

    skyray_fulfilled_promise_t *promise = skyray_rejected_promise_new(reason, is_copy_required);
    ZVAL_OBJ(&self->result, &promise->std);

    // call callbacks
}

SKYRAY_METHOD(promise, __construct)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(promise, then)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL, *on_notify;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zzz", &on_fulfilled, &on_rejected, &on_notify) == FAILURE) {
        return;
    }

    skyray_promise_t *intern = skyray_promise_from_obj(Z_OBJ_P(getThis()));

    skyray_promise_then(intern, on_fulfilled, on_rejected, return_value);
}

SKYRAY_METHOD(promise, done)
{
    zval *on_fulfilled = NULL, *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &on_fulfilled, &on_rejected) == FAILURE) {
        return;
    }

    skyray_promise_t *intern = skyray_promise_from_obj(Z_OBJ_P(getThis()));
    skyray_promise_done(intern, on_fulfilled, on_rejected);
}

SKYRAY_METHOD(promise, resolve)
{
    zval *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
        return;
    }

    skyray_fulfilled_promise_t *promise = skyray_fulfilled_promise_new(value, 1);
    RETURN_OBJ(&promise->std);
}

SKYRAY_METHOD(promise, reject)
{
    zval *reason;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &reason) == FAILURE) {
        return;
    }

    skyray_rejected_promise_t *promise = skyray_rejected_promise_new(reason, 1);
    RETURN_OBJ(&promise->std);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_then_or_done, 0, 0, 1)
    ZEND_ARG_INFO(0, fulfilledHandler)
    ZEND_ARG_INFO(0, errorHandler)
    ZEND_ARG_INFO(0, notifyHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolve, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reject, 0, 0, 1)
    ZEND_ARG_INFO(0, reason)
ZEND_END_ARG_INFO()



static const zend_function_entry class_methods[] = {
    SKYRAY_ME(promise, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(promise, then, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, done, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, resolve, arginfo_resolve, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    SKYRAY_ME(promise, reject, arginfo_reject, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(promise)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\defer\\Promise", class_methods);
    skyray_ce_Promise = zend_register_internal_class(&ce);
    skyray_ce_Promise->create_object = skyray_promise_object_new;

    memcpy(&skyray_handler_Promise, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Promise.offset = XtOffsetOf(skyray_promise_t, std);
    skyray_handler_Promise.free_obj = skyray_promise_object_free;

    intern_str_then           = zend_new_interned_string(zend_string_init(ZEND_STRL("then"), 1));
    intern_str_done           = zend_new_interned_string(zend_string_init(ZEND_STRL("done"), 1));

    return SUCCESS;
}

