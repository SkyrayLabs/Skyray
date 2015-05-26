/*
 * process.c
 *
 *  Created on: Mar 14, 2015
 *      Author: hujin
 */


#include "src/skyray.h"
#include "src/processing/process.h"


zend_class_entry *skyray_ce_Process;
zend_object_handlers skyray_handler_Process;

zend_array skyray_child_processes;
zend_long skyray_sigchld_count = -1;
uv_signal_t skyray_sigchld;


static zend_object * skyray_process_object_new(zend_class_entry *entry)
{
    skyray_process_t *intern;
    intern = ecalloc(1, sizeof(skyray_process_t) + zend_object_properties_size(entry));
    zend_object_std_init(&intern->std, entry);
    object_properties_init(&intern->std, entry);

    intern->pid = 0;
    intern->status = SKYRAY_PROCESS_INIT;
    intern->callable = NULL;
    intern->std.handlers = &skyray_handler_Process;

    return &intern->std;
}

static void skyray_process_object_free(zend_object *object)
{
    skyray_process_t *intern = skyray_process_from_obj(object);
    zend_object_std_dtor(&intern->std);

    zval_ptr_dtor(intern->callable);
    efree(intern->callable);

    if (intern->args) {
        zval_ptr_dtor(intern->args);
        efree(intern->args);
    }
}

int call_user_function_array(HashTable *function_table, zval *object, zval *function_name, zval *retval_ptr, zval *params)
{
    zend_fcall_info fci;
    int retval;

    fci.size = sizeof(fci);
    fci.function_table = EG(function_table);
    fci.object = NULL;
    ZVAL_COPY_VALUE(&fci.function_name, function_name);
    fci.retval = retval_ptr;;
    fci.no_separation = (zend_bool) 1;
    fci.symbol_table = NULL;
    fci.param_count = 0;
    fci.params = NULL;

    zend_fcall_info_args(&fci, params);

    retval = zend_call_function(&fci, NULL);

    zend_fcall_info_args_clear(&fci, 1);

    return retval;
}

skyray_process_t *skyray_process_get_by_pid(pid_t pid)
{
    zval *zprocess = zend_hash_index_find(&skyray_child_processes, pid);
    if (!zprocess) {
        return NULL;
    }

    return skyray_process_from_obj(Z_OBJ_P(zprocess));
}

zend_bool skyray_process_delete_by_pid(pid_t pid)
{
    return zend_hash_index_del(&skyray_child_processes, pid) == SUCCESS;
}

SKYRAY_METHOD(Process, __construct)
{
    zval *callable = NULL;
    zval *args = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &callable, &args) == FAILURE) {
        return;
    }

    if (!zend_is_callable(callable, 0, NULL)) {
        php_error_docref(NULL, E_ERROR, "The parameter $callable is not a valid callable");
        return;
    }

    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));
    object->callable = emalloc(sizeof(zval));
    ZVAL_COPY(object->callable, callable);

    if (args) {
        object->args = emalloc(sizeof(zval));
        ZVAL_COPY(object->args, args);
    }
}

SKYRAY_METHOD(Process, start)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));

    pid_t pid = fork();
    zval func_name, retval;
    object->status = SKYRAY_PROCESS_RUNNING;

    if (pid == 0) { // child process
        skyray_sigchld_count = -1;
        zend_hash_clean(&skyray_child_processes);

        object->pid = getpid();
        ZVAL_STRING(&func_name, "run");
        call_user_function(EG(function_table), getThis(), &func_name, &retval, 0, NULL);
        zval_dtor(&func_name);
    } else {
        object->pid = pid;
        zval_add_ref(getThis());
        zend_hash_index_update(&skyray_child_processes, pid, getThis());
    }
}

SKYRAY_METHOD(Process, getPid)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));
    RETURN_LONG(object->pid);
}

SKYRAY_METHOD(Process, exec)
{

}

SKYRAY_METHOD(Process, run)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));
    zval retval;

    if (object->args) {
        call_user_function_array(EG(function_table), NULL, object->callable, &retval, object->args);
    } else {
        call_user_function(EG(function_table), NULL, object->callable, &retval, 0, NULL);
    }

    if (EG(exception)) {
        skyray_handle_uncaught_exception(EG(exception), 0);
        exit(101);
    } else {
        convert_to_long(&retval);
        exit(Z_LVAL(retval));
    }
}

SKYRAY_METHOD(Process, join)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));
    int status = 0;
    int pid = waitpid(object->pid, &status, 0);
    if (pid > 0) {
        object->status = SKYRAY_PROCESS_TERMINATED;
        object->exit_code = status;
    }
    RETURN_LONG(status);
}

SKYRAY_METHOD(Process, isAlive)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));

    if (object->status != SKYRAY_PROCESS_RUNNING) {
        RETURN_FALSE;
    }
    int status = 0;
    pid_t pid = waitpid(object->pid, &status, WNOHANG);

    if (pid == object->pid) {
        object->status = SKYRAY_PROCESS_TERMINATED;
    }

    RETURN_BOOL(object->status == SKYRAY_PROCESS_RUNNING);
}


SKYRAY_METHOD(Process, kill)
{

}

SKYRAY_METHOD(Process, getExitCode)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_process_t *object = skyray_process_from_obj(Z_OBJ(EX(This)));
    if (object->status != SKYRAY_PROCESS_TERMINATED) {
        RETURN_NULL();
    }

    RETURN_LONG(WEXITSTATUS(object->exit_code));
}

SKYRAY_METHOD(Process, signal)
{

}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, callable)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_kill, 0, 0, 1)
    ZEND_ARG_INFO(0, signal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_signal, 0, 0, 2)
    ZEND_ARG_INFO(0, signal)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(Process, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(Process, start, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, join, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, exec, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, getPid, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, run, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, isAlive, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, getExitCode, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, kill, arginfo_kill, ZEND_ACC_PUBLIC)
    SKYRAY_ME(Process, signal, arginfo_signal, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(process)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\processing\\Process", class_methods);
    skyray_ce_Process = zend_register_internal_class(&ce);
    skyray_ce_Process->create_object = skyray_process_object_new;

    memcpy(&skyray_handler_Process, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Process.offset = XtOffsetOf(skyray_process_t, std);
    skyray_handler_Process.free_obj = skyray_process_object_free;

    return SUCCESS;
}

SKYRAY_RINIT_FUNCTION(process)
{
    zend_hash_init(&skyray_child_processes, 16, NULL, ZVAL_PTR_DTOR, 0);

    return SUCCESS;
}

SKYRAY_RSHUTDOWN_FUNCTION(process)
{
    zend_hash_destroy(&skyray_child_processes);

    return SUCCESS;
}
