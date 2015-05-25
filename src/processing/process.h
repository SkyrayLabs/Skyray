/*
 * process.h
 *
 *  Created on: Mar 14, 2015
 *      Author: hujin
 */

#ifndef PROCESS_H_
#define PROCESS_H_


#include "src/skyray.h"
#include "src/reactor.h"

extern zend_class_entry *skyray_ce_Process;

extern zend_long skyray_sigchld_count;
extern uv_signal_t skyray_sigchld;

typedef enum _skyray_process_status {
    SKYRAY_PROCESS_INIT = 0,
    SKYRAY_PROCESS_RUNNING,
    SKYRAY_PROCESS_STOPPED,
    SKYRAY_PROCESS_TERMINATED
}skyray_process_status_t;

typedef struct _skyray_process {
    pid_t pid;  /* The pid of the process */
    int status; /* The running status of the process */
    zval *callable;
    zval *args;
    int exit_code;
    void *watcher;
    zend_object std;
}skyray_process_t;

static inline skyray_process_t *skyray_process_from_obj(zend_object *obj) /* {{{ */ {
    return (skyray_process_t*)((char*)(obj) - XtOffsetOf(skyray_process_t, std));
}

skyray_process_t *skyray_process_get_by_pid(pid_t pid);
zend_bool skyray_process_delete_by_pid(pid_t pid);

SKYRAY_MINIT_FUNCTION(process);
SKYRAY_RINIT_FUNCTION(process);
SKYRAY_RSHUTDOWN_FUNCTION(process);

#endif /* PROCESS_H_ */
