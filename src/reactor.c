/*
 * reactor.c
 *
 *  Created on: Mar 24, 2015
 *      Author: hujin
 */

#include "reactor.h"
#include "timer.h"
#include "stream/stream.h"
#include "watcher/watcher.h"
#include "processing/process.h"

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
    zend_object_std_dtor(&intern->std);
}

static void walk_cb(uv_handle_t *handle, void *arg)
{
    uv_close(handle, NULL);
}

void skyray_reactor_run(skyray_reactor_t *self)
{
    do {
        uv_run(&self->loop, UV_RUN_DEFAULT);
        uv_walk(&self->loop, walk_cb, NULL);
    } while(uv_loop_close(&self->loop) == UV_EBUSY);
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

SKYRAY_METHOD(reactor, attach)
{
    zval *zstream;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z", &zstream, skyray_ce_Stream) == FAILURE) {
        return;
    }

    skyray_stream_t *stream = skyray_stream_from_obj(Z_OBJ_P(zstream));
    if (skyray_stream_to_nonblocking(stream, skyray_reactor_from_obj(Z_OBJ_P(getThis())))) {
        skyray_stream_read_start(stream);
    }
}

SKYRAY_METHOD(reactor, detach)
{
    zval *zstream;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|z", &zstream, skyray_ce_Stream) == FAILURE) {
        return;
    }

    skyray_stream_t *stream = skyray_stream_from_obj(Z_OBJ_P(zstream));
    skyray_stream_read_stop(stream);
}

SKYRAY_METHOD(reactor, watch)
{
    zval *fd, *handler;
    zend_long events = UV_READABLE | UV_WRITABLE;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zO|l", &fd, &handler, skyray_ce_FdWatcherHandler, &events) == FAILURE) {
        return;
    }

    if ((events & ~(UV_READABLE | UV_WRITABLE)) != 0) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The $events parameter is invalid");
        return;
    }

    if (skyray_get_watchable_fd(fd) < 0) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The specified file descriptor or resource can not be watched");
        return;
    }

    skyray_reactor_t *reactor = skyray_reactor_from_obj(Z_OBJ_P(getThis()));

    skyray_fdwatcher_t *watcher = skyray_fdwatcher_new(reactor, fd, handler);

    if (watcher) {
        skyray_fdwatcher_watch(watcher, events);
        RETURN_OBJ(&watcher->std);
    }
}

SKYRAY_METHOD(reactor, watchProcess)
{
    zval *process, *handler;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &process, skyray_ce_Process, &handler, skyray_ce_ProcessWatcherHandler) == FAILURE) {
        return;
    }

    skyray_reactor_t *reactor = skyray_reactor_from_obj(Z_OBJ_P(getThis()));

    skyray_process_watcher_t *watcher = skyray_process_watcher_new(reactor, process, handler);
    if (watcher) {
        RETURN_OBJ(&watcher->std);
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

SKYRAY_METHOD(reactor, printActiveObjects)
{
    zend_objects_store *objects = &EG(objects_store);
    zend_object **obj_ptr, **end, *obj;
    zval value;

    end = objects->object_buckets + 1;
    obj_ptr = objects->object_buckets + objects->top;

    do {
        obj_ptr--;
        obj = *obj_ptr;
        if (IS_OBJ_VALID(obj)) {
            ZVAL_OBJ(&value, obj);
            printf("%s\n", obj->ce->name->val);
        }
    } while (obj_ptr != end);

    //shutdown_memory_manager(0, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_attach, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_detach, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_watch, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, handler)
    ZEND_ARG_INFO(0, events)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_watchProcess, 0, 0, 2)
    ZEND_ARG_INFO(0, process)
    ZEND_ARG_INFO(0, handler)
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
    SKYRAY_ME(reactor, attach, arginfo_attach, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, detach, arginfo_detach, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, watch, arginfo_watch, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, watchProcess, arginfo_watchProcess, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, addTimer, arginfo_addTimer, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, addPeriodicTimer, arginfo_addTimer, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, cancelTimer, arginfo_cancelTimer, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, run, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, stop, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(reactor, printActiveObjects, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_reactor)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\Reactor", class_methods);
    skyray_ce_Reactor = zend_register_internal_class(&ce);
    skyray_ce_Reactor->create_object = skyray_reactor_object_new;

    memcpy(&skyray_handler_Reactor, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Reactor.offset = XtOffsetOf(skyray_reactor_t, std);
    skyray_handler_Reactor.free_obj = skyray_reactor_object_free;

    return SUCCESS;
}
