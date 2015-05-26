/*
 * watcher.c
 *
 *  Created on: May 24, 2015
 *      Author: hujin
 */


#include "watcher.h"

zend_class_entry *skyray_ce_ProcessWatcher;
zend_class_entry *skyray_ce_ProcessWatcherHandler;

zend_object_handlers skyray_handler_ProcessWatcher;


static inline skyray_process_watcher_t *skyray_process_watcher_from_obj(zend_object *obj) {
    return (skyray_process_watcher_t*)((char*)(obj) - XtOffsetOf(skyray_process_watcher_t, std));
}

static zend_object * skyray_process_watcher_object_new(zend_class_entry *ce)
{
    skyray_process_watcher_t *intern;
    intern = ecalloc(1, sizeof(skyray_process_watcher_t) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_ProcessWatcher;
    return &intern->std;
}

static  void skyray_process_watcher_object_free(zend_object *object)
{
    skyray_process_watcher_t *intern = skyray_process_watcher_from_obj(object);

    zend_object_release(intern->handler);
    zend_object_release(&intern->process->std);

    zend_object_std_dtor(&intern->std);
}

static void signal_cb(uv_signal_t* handle, int signum)
{
    int status;
    zval zprocess, zhandler;
    pid_t pid = wait(&status);
    skyray_process_t *process = skyray_process_get_by_pid(pid);

    if (!process) {
        return;
    }

    process->status = SKYRAY_PROCESS_TERMINATED;
    process->exit_code = status;

    skyray_process_watcher_t *watcher = process->watcher;

    ZVAL_OBJ(&zprocess, &process->std);

    ZVAL_OBJ(&zhandler, watcher->handler);

    zend_call_method(&zhandler, NULL, NULL, ZEND_STRL("onExited"), NULL, 1, &zprocess, NULL);
    skyray_process_delete_by_pid(pid);

    skyray_process_watcher_stop(watcher);
}

skyray_process_watcher_t * skyray_process_watcher_new(skyray_reactor_t *reactor, zval *zprocess, zval *handler)
{
    zval object;
    skyray_process_watcher_t *intern;
    skyray_process_t *process = skyray_process_from_obj(Z_OBJ_P(zprocess));

    if (process->watcher) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The process is already under watching, unable to watch again");
        return NULL;
    }

    object_init_ex(&object, skyray_ce_ProcessWatcher);
    zend_update_property(skyray_ce_ProcessWatcherHandler, handler, ZEND_STRL("watcher"), &object);

    intern = skyray_process_watcher_from_obj(Z_OBJ(object));
    intern->process = process;
    intern->handler = Z_OBJ_P(handler);
    zval_add_ref(handler);
    zval_add_ref(zprocess);

    process->watcher = intern;

    if (skyray_sigchld_count < 0) {
        uv_signal_init(&reactor->loop, &skyray_sigchld);
        skyray_sigchld_count = 0;
    }

    if (skyray_sigchld_count == 0) {
        uv_signal_start(&skyray_sigchld, signal_cb, SIGCHLD);
    }
    skyray_sigchld_count ++;

    return intern;
}


void skyray_process_watcher_watch(skyray_process_watcher_t *self)
{
    if (self->process->watcher) {
        return;
    }

    if (skyray_sigchld_count == 0) {
        uv_signal_start(&skyray_sigchld, signal_cb, SIGCHLD);
    }
    skyray_sigchld_count ++;
}

void skyray_process_watcher_stop(skyray_process_watcher_t *self)
{
    if (self->process->watcher) {
        self->process->watcher = NULL;
        skyray_sigchld_count --;
    }

    if (skyray_sigchld_count == 0) {
        uv_signal_stop(&skyray_sigchld);
    }
}

SKYRAY_METHOD(ProcessWatcher, watch)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_watcher_t *intern = skyray_process_watcher_from_obj(Z_OBJ_P(getThis()));
    skyray_process_watcher_watch(intern);
}

SKYRAY_METHOD(ProcessWatcher, isWatched)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_watcher_t *intern = skyray_process_watcher_from_obj(Z_OBJ_P(getThis()));
    RETURN_BOOL(intern->process->watcher != NULL);
}

SKYRAY_METHOD(ProcessWatcher, stop)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_watcher_t *intern = skyray_process_watcher_from_obj(Z_OBJ_P(getThis()));

    skyray_process_watcher_stop(intern);
}

SKYRAY_METHOD(ProcessWatcher, getProcess)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_watcher_t *intern = skyray_process_watcher_from_obj(Z_OBJ_P(getThis()));

    GC_REFCOUNT(&intern->process->std) ++;

    RETURN_OBJ(&intern->process->std)
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_onExited, 0, 0, 1)
    ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_onError, 0, 0, 2)
    ZEND_ARG_INFO(0, fd)
    ZEND_ARG_INFO(0, error)
ZEND_END_ARG_INFO()

static const zend_function_entry handler_method[] = {
    SKYRAY_ABSTRACT_ME(ProcessHandlerWatcher, onExited, arginfo_onExited)
    //SKYRAY_ABSTRACT_ME(ProcessHandlerWatcher, onWritable, arginfo_onReadableOrWritable)
    //SKYRAY_ABSTRACT_ME(ProcessHandlerWatcher, onError, arginfo_onError)
    PHP_FE_END
};

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(ProcessWatcher, watch, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(ProcessWatcher, isWatched, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(ProcessWatcher, stop, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(ProcessWatcher, getProcess, arginfo_empty, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(process_watcher)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\watcher\\ProcessWatcherHandler", handler_method);
    skyray_ce_ProcessWatcherHandler = zend_register_internal_class(&ce);
    zend_declare_property_null(skyray_ce_ProcessWatcherHandler, ZEND_STRL("watcher"), ZEND_ACC_PUBLIC);


    INIT_CLASS_ENTRY(ce, "skyray\\watcher\\ProcessWatcher", class_methods);
    skyray_ce_ProcessWatcher = zend_register_internal_class(&ce);
    skyray_ce_ProcessWatcher->create_object = skyray_process_watcher_object_new;

    memcpy(&skyray_handler_ProcessWatcher, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_ProcessWatcher.offset = XtOffsetOf(skyray_process_watcher_t, std);
    skyray_handler_ProcessWatcher.free_obj = skyray_process_watcher_object_free;

    return SUCCESS;
}
