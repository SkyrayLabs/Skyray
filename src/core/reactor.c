/*
 * reactor.c
 *
 *  Created on: Mar 24, 2015
 *      Author: hujin
 */

#include "reactor.h"
#include "timer.h"

zend_class_entry *skyray_ce_Reactor;
zend_object_handlers skyray_handler_Reactor;


zend_object * skyray_reactor_object_new(zend_class_entry *ce)
{
    skyray_reactor_t *intern;
    intern = ecalloc(1, sizeof(skyray_reactor_t) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    uv_loop_init(&intern->loop);

    intern->std.handlers = &skyray_handler_Reactor;
    return &intern->std;
}

void skyray_reactor_object_free(zend_object *object)
{
    skyray_reactor_t *intern = skyray_reactor_from_obj(object);
    uv__loop_close(&intern->loop);
    zend_object_std_dtor(&intern->std);
}

int skyray_reactor_run(skyray_reactor_t *self)
{
    return uv_run(&self->loop, UV_RUN_DEFAULT);
}

void skyray_reactor_stop(skyray_reactor_t *self)
{
    uv_stop(&self->loop);
}

SKYRAY_METHOD(reactor, __construct)
{

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, addReader)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, addWriter)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, addBoth)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, removeReader)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, removeWriter)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, removeBoth)
{
    zval *zstream, *callback = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|z", &zstream, &callback) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, addTimer)
{
    zend_long interval;
    zval *callback;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &interval, &callback) == FAILURE) {
        return;
    }

    if (!zend_is_callable(callback, 0, NULL)) {
        skyray_throw_exception("The parameter $callback is not a valid callable");
        return;
    }

    skyray_reactor_t *reactor = skyray_reactor_from_obj(Z_OBJ_P(getThis()));

    object_init_ex(return_value, skyray_ce_Timer);

    skyray_timer_t *timer = skyray_timer_from_obj(Z_OBJ_P(return_value));
    skyray_timer_init(timer, reactor, callback);
    skyray_timer_start(timer, interval, 0);
    zval_add_ref(return_value);
}

SKYRAY_METHOD(reactor, addPeriodicTimer)
{
    zend_long interval;
    zval *callback;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &interval, &callback) == FAILURE) {
        return;
    }

    if (!zend_is_callable(callback, 0, NULL)) {
        skyray_throw_exception("The parameter $callback is not a valid callable");
        return;
    }

    skyray_reactor_t *reactor = skyray_reactor_from_obj(Z_OBJ_P(getThis()));

    object_init_ex(return_value, skyray_ce_Timer);

    skyray_timer_t *timer = skyray_timer_from_obj(Z_OBJ_P(return_value));
    skyray_timer_init(timer, reactor, callback);
    skyray_timer_start(timer, interval, interval);
    zval_add_ref(return_value);
}

SKYRAY_METHOD(reactor, cancelTimer)
{
    zval *ztimer;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &ztimer) == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(reactor, run)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_reactor_t *intern = skyray_reactor_from_obj(Z_OBJ_P(getThis()));
    skyray_reactor_run(intern);
}

SKYRAY_METHOD(reactor, stop)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_reactor_t *intern = skyray_reactor_from_obj(Z_OBJ_P(getThis()));
    skyray_reactor_stop(intern);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addWriter, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addReader, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addBoth, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_removeWriter, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_removeReader, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_removeBoth, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addTimer, 0, 0, 2)
    ZEND_ARG_INFO(0, interval)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cancelTimer, 0, 0, 1)
    ZEND_ARG_INFO(0, timer)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(reactor, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, addReader, arginfo_addReader, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, addWriter, arginfo_addWriter, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, addBoth, arginfo_addBoth, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, removeReader, arginfo_removeReader, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, removeWriter, arginfo_removeWriter, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, removeBoth, arginfo_removeBoth, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, addTimer, arginfo_addTimer, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, addPeriodicTimer, arginfo_addTimer, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, cancelTimer, arginfo_cancelTimer, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, run, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(reactor, stop, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_reactor)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\Reactor", class_methods);
    skyray_ce_Reactor = zend_register_internal_class(&ce);
    skyray_ce_Reactor->create_object = skyray_reactor_object_new;

    memcpy(&skyray_handler_Reactor, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Reactor.offset = XtOffsetOf(skyray_reactor_t, std);
    skyray_handler_Reactor.free_obj = skyray_reactor_object_free;

    return SUCCESS;
}
