/*
 * promise.c
 *
 *  Created on: May 3, 2015
 *      Author: hujin
 */

#include "defer.h"

zend_class_entry *skyray_ce_Promise;
zend_class_entry *skyray_ce_PromiseInterface;

zend_object_handlers skyray_handler_Promise;

zend_string *intern_str_then;
zend_string *intern_str_done;

typedef struct _resolve_context {
    zend_refcounted gc;
    skyray_promise_t *self;
    skyray_promise_t *late;
    zval callback;
} promise_resolve_context_t;

static promise_resolve_context_t * promise_resolve_context_create(skyray_promise_t *self, zval *callback, skyray_promise_t *late)
{
    promise_resolve_context_t *context = emalloc(sizeof(promise_resolve_context_t));

    GC_REFCOUNT(context) = 1;

    // if context->self is NULL, this context is created for done()
    context->self = self;
    if (self) {
        GC_REFCOUNT(&self->std) ++;
    }

    ZVAL_NULL(&context->callback);

    context->late = late;

    if (callback) {
        ZVAL_COPY(&context->callback, callback);
    }

    return context;
}

static void context_copy_func(void *pElement)
{
    promise_resolve_context_t *context = Z_PTR_P((zval *)pElement);
    GC_REFCOUNT(context) ++;
}

static void promise_resolve_context_call(promise_resolve_context_t *context, zval *value)
{
    zval retval;

    if (!context->self) {
        if (!ZVAL_IS_NULL(&context->callback)) {
            skyray_promise_call_handler(&context->callback, value, &retval);
            zval_ptr_dtor(&retval);
        }
        return;
    }

    if (!ZVAL_IS_NULL(&context->callback)) {
        skyray_promise_call_handler(&context->callback, value, &retval);

        if (!EG(exception)) {
            skyray_promise_do_resolve(context->self, &retval, 0);
        } else {
            ZVAL_OBJ(&retval, EG(exception));
            EG(exception) = NULL;
            skyray_promise_do_reject(context->self, &retval, 0);
        }
    } else {
        skyray_promise_do_resolve(context->self, &context->late->result, 0);
    }
}

static void promise_resolve_context_free(zval *value)
{
    promise_resolve_context_t *context = Z_PTR_P(value);

    if (--GC_REFCOUNT(context) != 0) {
        return;
    }

    if (context->self) {
        zend_object_release(&context->self->std);
    }

    zval_ptr_dtor(&context->callback);
    efree(context);
}

static void call_resolved_handlers(skyray_promise_t *self, zend_array *on_fulfilled, zend_array *on_rejected)
{
    skyray_promise_t *promise = skyray_promise_from_obj(Z_OBJ_P(&self->result));
    zend_array *handlers;

    if (instanceof_function(Z_OBJCE_P(&self->result), skyray_ce_FulfilledPromise)) {
        handlers = on_fulfilled;
    } else {
        handlers = on_rejected;
    }

    zend_hash_internal_pointer_reset(handlers);
    while(zend_hash_has_more_elements(handlers) == SUCCESS) {
        promise_resolve_context_t *context = zend_hash_get_current_data_ptr(handlers);
        promise_resolve_context_call(context, &promise->result);
        zend_hash_move_forward(handlers);
    }

    zend_hash_destroy(&self->on_fulfilled);
    zend_hash_destroy(&self->on_rejcted);
}

void skyray_promise_object_init(skyray_promise_t *self, zend_class_entry *ce)
{
    zend_object_std_init(&self->std, ce);
    object_properties_init(&self->std, ce);

    ZVAL_UNDEF(&self->result);

    zend_hash_init(&self->on_fulfilled, 8, NULL, promise_resolve_context_free, 0);
    zend_hash_init(&self->on_rejcted, 8, NULL, promise_resolve_context_free, 0);
    zend_hash_init(&self->on_notify, 8, NULL, promise_resolve_context_free, 0);
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

    // The promise is still pending
    if (Z_TYPE(intern->result) == IS_UNDEF) {
        zend_hash_destroy(&intern->on_fulfilled);
        zend_hash_destroy(&intern->on_rejcted);
    }

    zend_hash_destroy(&intern->on_notify);

    zval_ptr_dtor(&intern->result);

    zend_object_std_dtor(&intern->std);
}

zval* skyray_promise_unwrap_zval(zval *promise)
{
    zval *retval = promise;
    skyray_promise_t *tmp;

    while (!skyray_is_resolved_promise(promise)) {
        tmp = skyray_promise_from_obj(Z_OBJ_P(retval));
        if (Z_TYPE(tmp->result) == IS_UNDEF) {
            break;
        }
        retval = &tmp->result;
    }

    return retval;
}

zval* skyray_promise_unwrap(skyray_promise_t *promise)
{
    zval *retval = &promise->result;

    while (!skyray_is_resolved_promise(retval)) {
        promise = skyray_promise_from_obj(Z_OBJ_P(retval));
        retval = &promise->result;
    }

    return retval;
}


void skyray_promise_then(skyray_promise_t *self, zval *on_fulfilled, zval *on_rejected, zval *retval)
{
    zval *result, params[2], function_name;

    if (Z_TYPE(self->result) != IS_UNDEF) {
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
    } else {
        object_init_ex(retval, skyray_ce_Promise);
        skyray_promise_t *promise = skyray_promise_from_obj(Z_OBJ_P(retval));

        promise_resolve_context_t *context;

        if (on_fulfilled != NULL && !ZVAL_IS_NULL(on_fulfilled)) {
            context = promise_resolve_context_create(promise, on_fulfilled, NULL);
        } else {
            context = promise_resolve_context_create(promise, NULL, self);
        }
        zend_hash_next_index_insert_ptr(&self->on_fulfilled, context);

        if (on_rejected != NULL && !ZVAL_IS_NULL(on_rejected)) {
            context = promise_resolve_context_create(promise, on_rejected, NULL);
        } else {
            context = promise_resolve_context_create(promise, NULL, self);
        }
        zend_hash_next_index_insert_ptr(&self->on_rejcted, context);
    }
}

void skyray_promise_done(skyray_promise_t *self, zval *on_fulfilled, zval *on_rejected)
{
    zval *result, params[2], function_name, retval;

    if (Z_TYPE(self->result) != IS_UNDEF) {
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
    } else {
        promise_resolve_context_t *context;

        if (on_fulfilled != NULL && !ZVAL_IS_NULL(on_fulfilled)) {
            context = promise_resolve_context_create(NULL, on_fulfilled, NULL);
            zend_hash_next_index_insert_ptr(&self->on_fulfilled, context);
        }

        if (on_rejected != NULL && !ZVAL_IS_NULL(on_rejected)) {
            context = promise_resolve_context_create(NULL, on_rejected, NULL);
        } else {
            context = promise_resolve_context_create(NULL, NULL, self);
        }
        zend_hash_next_index_insert_ptr(&self->on_rejcted, context);
    }
}

void skyray_promise_do_resolve(skyray_promise_t *self, zval *value, zend_bool is_copy_required)
{
    zval *result, tmp;

    if (Z_TYPE(self->result) != IS_UNDEF) {
        return;
    }

    if (skyray_is_promise_instance(value)) {

        result = skyray_promise_unwrap_zval(value);
    } else {
        skyray_fulfilled_promise_t *promise = skyray_fulfilled_promise_new(value, is_copy_required);
        ZVAL_OBJ(&tmp, &promise->std);
        result = &tmp;
    }

    if (skyray_is_promise_instance(result)) {
        ZVAL_COPY_VALUE(&self->result, result);
    } else {
        ZVAL_COPY(&self->result, result);
    }

    if (skyray_is_resolved_promise(result)) {
        call_resolved_handlers(self, &self->on_fulfilled, &self->on_rejcted);
    } else {
        skyray_promise_t *result_promsie = skyray_promise_from_obj(Z_OBJ_P(result));
        zend_hash_copy(&result_promsie->on_fulfilled, &self->on_fulfilled, (copy_ctor_func_t)context_copy_func);
        zend_hash_copy(&result_promsie->on_rejcted, &self->on_rejcted, (copy_ctor_func_t)context_copy_func);

        zend_hash_destroy(&self->on_fulfilled);
        zend_hash_destroy(&self->on_rejcted);
    }
}

void skyray_promise_do_reject(skyray_promise_t *self, zval *reason, zend_bool is_copy_required)
{
    if (Z_TYPE(self->result) != IS_UNDEF) {
        return;
    }

    skyray_fulfilled_promise_t *promise = skyray_rejected_promise_new(reason, is_copy_required);
    ZVAL_OBJ(&self->result, &promise->std);

    skyray_promise_t *resolved = skyray_promise_from_obj(Z_OBJ_P(&self->result));

    call_resolved_handlers(self, &self->on_fulfilled, &self->on_rejcted);
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

SKYRAY_METHOD(promise, catch)
{
    zval *on_rejected = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &on_rejected) == FAILURE) {
        return;
    }

    skyray_promise_t *intern = skyray_promise_from_obj(Z_OBJ_P(getThis()));
    skyray_promise_then(intern, NULL, on_rejected, return_value);
}

SKYRAY_METHOD(promise, finally)
{
    zval *handler = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &handler) == FAILURE) {
        return;
    }

    skyray_promise_t *intern = skyray_promise_from_obj(Z_OBJ_P(getThis()));
    skyray_promise_done(intern, handler, handler);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_then_or_done, 0, 0, 0)
    ZEND_ARG_INFO(0, fulfilledHandler)
    ZEND_ARG_INFO(0, errorHandler)
    ZEND_ARG_INFO(0, notifyHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_catch, 0, 0, 0)
    ZEND_ARG_INFO(0, errorHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_finally, 0, 0, 0)
    ZEND_ARG_INFO(0, handler)
    ZEND_ARG_INFO(0, notifyHandler)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_resolve, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reject, 0, 0, 1)
    ZEND_ARG_INFO(0, reason)
ZEND_END_ARG_INFO()

static const zend_function_entry iface_methods[] = {
    SKYRAY_ME(promise, then, arginfo_then_or_done, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    SKYRAY_ME(promise, done, arginfo_then_or_done, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    SKYRAY_ME(promise, catch, arginfo_catch, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    SKYRAY_ME(promise, finally, arginfo_finally, ZEND_ACC_PUBLIC | ZEND_ACC_ABSTRACT)
    PHP_FE_END
};

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(promise, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(promise, then, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, done, arginfo_then_or_done, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, catch, arginfo_catch, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, finally, arginfo_finally, ZEND_ACC_PUBLIC)
    SKYRAY_ME(promise, resolve, arginfo_resolve, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    SKYRAY_ME(promise, reject, arginfo_reject, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(promise)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "skyray\\defer\\PromiseInterface", iface_methods);
    skyray_ce_PromiseInterface = zend_register_internal_interface(&ce);

    INIT_CLASS_ENTRY(ce, "skyray\\defer\\Promise", class_methods);
    skyray_ce_Promise = zend_register_internal_class(&ce);
    skyray_ce_Promise->create_object = skyray_promise_object_new;

    zend_class_implements(skyray_ce_Promise, 1, skyray_ce_PromiseInterface);

    memcpy(&skyray_handler_Promise, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Promise.offset = XtOffsetOf(skyray_promise_t, std);
    skyray_handler_Promise.free_obj = skyray_promise_object_free;

    intern_str_then           = zend_new_interned_string(zend_string_init(ZEND_STRL("then"), 1));
    intern_str_done           = zend_new_interned_string(zend_string_init(ZEND_STRL("done"), 1));

    return SUCCESS;
}
