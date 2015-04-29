/*
 * timer.c
 *
 *  Created on: Mar 31, 2015
 *      Author: hujin
 */

#include "timer.h"
#include "reactor.h"

zend_class_entry *skyray_ce_Timer;
zend_object_handlers skyray_handler_Timer;

zend_object * skyray_timer_object_new(zend_class_entry *ce)
{
    skyray_timer_t *intern;
    intern = ecalloc(1, sizeof(skyray_timer_t) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_Timer;
    return &intern->std;
}

void skyray_timer_object_free(zend_object *object)
{
    skyray_timer_t *intern = skyray_timer_from_obj(object);

    zval_ptr_dtor(&intern->callback);
    zend_object_std_dtor(&intern->std);
}

void skyray_timer_init(skyray_timer_t *self, skyray_reactor_t *reactor, zval *callback)
{
    uv_timer_init(&reactor->loop, &self->timer);
    ZVAL_COPY(&self->callback, callback);
}

zend_long skyray_timer_get_interval(skyray_timer_t *self)
{
    return self->timer.timeout;
}

zend_bool skyray_timer_is_periodic(skyray_timer_t *self)
{
    return uv_timer_get_repeat(&self->timer);
}

static void close_cb(uv_handle_t *uv_timer)
{
    skyray_timer_t *timer = (skyray_timer_t *)uv_timer;
    zend_object_release(&timer->std);
}

static void timer_callback(uv_timer_t *uv_timer)
{
    skyray_timer_t *timer = (skyray_timer_t *)uv_timer;
    zval retval, params[1];

    ZVAL_OBJ(&params[0], &timer->std);

    call_user_function(NULL, NULL, &timer->callback, &retval, 1, params);


    zval_ptr_dtor(&retval);
    if (EG(exception)) {
        skyray_handle_uncaught_exception(EG(exception));
    }

    if (uv_timer_get_repeat(&timer->timer) == 0) {
        uv_close((uv_handle_t *)uv_timer, close_cb);
    }
}

zend_bool skyray_timer_start(skyray_timer_t *self, zend_long interval, zend_long priodic)
{
    if (uv_timer_start(&self->timer, timer_callback, interval, priodic) < 0) {
        return 0;
    }

    return 1;
}

void skyray_timer_cancel(skyray_timer_t *self)
{
    if (uv_is_active((uv_handle_t *)&self->timer)) {
        uv_close((uv_handle_t *)&self->timer, close_cb);
    }
}

SKYRAY_METHOD(timer, __construct)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
}

SKYRAY_METHOD(timer, getInterval)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_timer_t *intern = skyray_timer_from_obj(Z_OBJ_P(getThis()));

    RETURN_LONG(skyray_timer_get_interval(intern));
}

SKYRAY_METHOD(timer, isPeriodic)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_timer_t *intern = skyray_timer_from_obj(Z_OBJ_P(getThis()));

    RETURN_BOOL(skyray_timer_is_periodic(intern));
}

SKYRAY_METHOD(timer, cancel)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_timer_t *intern = skyray_timer_from_obj(Z_OBJ_P(getThis()));
    skyray_timer_cancel(intern);
}

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(timer, __construct, arginfo_empty, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    SKYRAY_ME(timer, getInterval, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(timer, isPeriodic, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(timer, cancel, arginfo_empty, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_timer)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\Timer", class_methods);
    skyray_ce_Timer = zend_register_internal_class(&ce);
    skyray_ce_Timer->create_object = skyray_timer_object_new;

    memcpy(&skyray_handler_Timer, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Timer.offset = XtOffsetOf(skyray_timer_t, std);
    skyray_handler_Timer.free_obj = skyray_timer_object_free;

    return SUCCESS;
}

