/*
 * fdwatcher.c
 *
 *  Created on: May 23, 2015
 *      Author: hujin
 */

#include "fdwatcher.h"

zend_class_entry *skyray_ce_FdWatcher;
zend_class_entry *skyray_ce_FdWatcherHandler;

zend_object_handlers skyray_handler_FdWatcher;

static inline skyray_fdwatcher_t *skyray_fdwatcher_from_obj(zend_object *obj) {
    return (skyray_fdwatcher_t*)((char*)(obj) - XtOffsetOf(skyray_fdwatcher_t, std));
}

static zend_object * skyray_fdwatcher_object_new(zend_class_entry *ce)
{
    skyray_fdwatcher_t *intern;
    intern = ecalloc(1, sizeof(skyray_fdwatcher_t) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_FdWatcher;
    return &intern->std;
}

static  void skyray_fdwatcher_object_free(zend_object *object)
{
    skyray_fdwatcher_t *intern = skyray_fdwatcher_from_obj(object);

    if (intern->events) {
        skyray_fdwatcher_stop(intern, UV_READABLE | UV_WRITABLE);
    }

    zval_ptr_dtor(&intern->fd);
    zend_object_release(intern->handler);

    zend_object_std_dtor(&intern->std);
}

static void poll_cb(uv_poll_t* handle, int status, int events)
{
    skyray_fdwatcher_t *watcher = handle->data;
    zval handler;

    ZVAL_OBJ(&handler, watcher->handler);

    if (status < 0) {
        zval error;
        ZVAL_STRING(&error, uv_strerror(status));
        zend_call_method(&handler, NULL, NULL, ZEND_STRL("onError"), NULL, 2, &watcher->fd, &error);
        return;
    }

    if (events & UV_READABLE) {
        zend_call_method(&handler, NULL, NULL, ZEND_STRL("onReadable"), NULL, 1, &watcher->fd, NULL);
    }
    if (events & UV_WRITABLE) {
        zend_call_method(&handler, NULL, NULL, ZEND_STRL("onWritable"), NULL, 1, &watcher->fd, NULL);
    }
}

zend_bool skyray_get_watchable_fd(zval *fd)
{
    if (Z_TYPE_P(fd) == IS_LONG) {
        if (Z_LVAL_P(fd) < 0) {
            return -1;
        }
        return Z_LVAL_P(fd);
    }
    if (Z_TYPE_P(fd) == IS_RESOURCE) {
        php_stream *stream = zend_fetch_resource(Z_RES_P(fd), NULL, php_file_le_stream());
        int ret;
        if (!stream) {
            return -1;
        }

        if (php_stream_cast(stream, PHP_STREAM_AS_FD_FOR_SELECT | PHP_STREAM_CAST_INTERNAL, (void *)&ret, 0) == FAILURE) {
            return -1;
        }
        return ret;
    }
    return -1;
}

skyray_fdwatcher_t * skyray_fdwatcher_new(skyray_reactor_t *reactor, zval *fd, zval *handler)
{
    zval object;
    skyray_fdwatcher_t *intern;
    int ifd = skyray_get_watchable_fd(fd);

    if (reactor->loop.nwatchers > ifd && reactor->loop.watchers[ifd]) {
        zend_throw_exception_ex(skyray_ce_BaseException, 0, "The specified fd is already watched, can not watch again");
        return NULL;
    }

    object_init_ex(&object, skyray_ce_FdWatcher);
    zend_update_property(skyray_ce_FdWatcherHandler, handler, ZEND_STRL("watcher"), &object);

    intern = skyray_fdwatcher_from_obj(Z_OBJ(object));
    intern->reactor = reactor;
    intern->handler = Z_OBJ_P(handler);
    zval_addref_p(handler);

    ZVAL_COPY(&intern->fd, fd);

    uv_poll_init(&intern->reactor->loop, &intern->poll, ifd);;

    return intern;
}


void skyray_fdwatcher_watch(skyray_fdwatcher_t *self, int events)
{
    assert((events & ~(UV_READABLE | UV_WRITABLE)) == 0);

    self->poll.data = self;
    self->events = events;
    uv_poll_start(&self->poll, events, poll_cb);
}

void skyray_fdwatcher_stop(skyray_fdwatcher_t *self, int events)
{
    assert((events & ~(UV_READABLE | UV_WRITABLE)) == 0);

    self->events &= ~events;
    uv_poll_stop(&self->poll);

    if (self->events) {
        skyray_fdwatcher_watch(self, self->events);
    }
}

SKYRAY_METHOD(FdWatcher, watch)
{
    zend_long events = UV_READABLE | UV_WRITABLE;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &events) == FAILURE) {
        return;
    }

    if ((events & ~(UV_READABLE | UV_WRITABLE)) != 0) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The $events parameter is invalid");
        return;
    }
    skyray_fdwatcher_t *intern = skyray_fdwatcher_from_obj(Z_OBJ_P(getThis()));

    skyray_fdwatcher_watch(intern, events);
}

SKYRAY_METHOD(FdWatcher, isWatched)
{
    zend_long events;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &events) == FAILURE) {
        return;
    }

    skyray_fdwatcher_t *intern = skyray_fdwatcher_from_obj(Z_OBJ_P(getThis()));

    if ((events & ~(UV_READABLE | UV_WRITABLE)) != 0) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The $events parameter is invalid");
        return;
    }

    if ((events == (UV_READABLE | UV_WRITABLE))) {
        RETURN_BOOL(intern->events == (UV_READABLE | UV_WRITABLE));
    }

    if ((events & UV_WRITABLE)) {
        RETURN_BOOL(intern->events & UV_WRITABLE);
    }

    if ((events & UV_READABLE)) {
        RETURN_BOOL(intern->events & UV_READABLE);
    }

    RETURN_FALSE;
}

SKYRAY_METHOD(FdWatcher, stop)
{
    zend_long events = UV_READABLE | UV_WRITABLE;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &events) == FAILURE) {
        return;
    }

    if ((events & ~(UV_READABLE | UV_WRITABLE)) != 0) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The $events parameter is invalid");
        return;
    }

    skyray_fdwatcher_t *intern = skyray_fdwatcher_from_obj(Z_OBJ_P(getThis()));

    skyray_fdwatcher_stop(intern, events);
}

SKYRAY_METHOD(FdWatcher, getFd)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_fdwatcher_t *intern = skyray_fdwatcher_from_obj(Z_OBJ_P(getThis()));

    RETURN_ZVAL(&intern->fd, 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_onReadableOrWritable, 0, 0, 1)
    ZEND_ARG_INFO(0, fd)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_onError, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_watch, 0, 0, 0)
    ZEND_ARG_INFO(0, events)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_isWatched, 0, 0, 1)
    ZEND_ARG_INFO(0, events)
ZEND_END_ARG_INFO()

static const zend_function_entry fdwatcher_handler_method[] = {
    SKYRAY_ABSTRACT_ME(FdHandlerWatcher, onReadable, arginfo_onReadableOrWritable)
    SKYRAY_ABSTRACT_ME(FdHandlerWatcher, onWritable, arginfo_onReadableOrWritable)
    SKYRAY_ABSTRACT_ME(FdHandlerWatcher, onError, arginfo_onError)
    PHP_FE_END
};

static const zend_function_entry fdwacher_methods[] = {
    SKYRAY_ME(FdWatcher, watch, arginfo_watch, ZEND_ACC_PUBLIC)
    SKYRAY_ME(FdWatcher, isWatched, arginfo_isWatched, ZEND_ACC_PUBLIC)
    SKYRAY_ME(FdWatcher, stop, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(FdWatcher, getFd, arginfo_empty, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(fdwatcher)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\watcher\\FdWatcherHandler", fdwatcher_handler_method);
    skyray_ce_FdWatcherHandler = zend_register_internal_class(&ce);
    zend_declare_property_null(skyray_ce_FdWatcherHandler, ZEND_STRL("watcher"), ZEND_ACC_PUBLIC);


    INIT_CLASS_ENTRY(ce, "skyray\\watcher\\FdWatcher", fdwacher_methods);
    skyray_ce_FdWatcher = zend_register_internal_class(&ce);
    skyray_ce_FdWatcher->create_object = skyray_fdwatcher_object_new;

    zend_declare_class_constant_long(skyray_ce_FdWatcher, ZEND_STRL("READABLE"), UV_READABLE);
    zend_declare_class_constant_long(skyray_ce_FdWatcher, ZEND_STRL("WRITABLE"), UV_WRITABLE);

    memcpy(&skyray_handler_FdWatcher, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_FdWatcher.offset = XtOffsetOf(skyray_fdwatcher_t, std);
    skyray_handler_FdWatcher.free_obj = skyray_fdwatcher_object_free;

    return SUCCESS;
}
