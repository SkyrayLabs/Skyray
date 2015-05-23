/*
 * process.h
 *
 *  Created on: Mar 14, 2015
 *      Author: hujin
 */

#ifndef PROCESS_H_
#define PROCESS_H_


#include "src/skyray.h"

extern zend_class_entry *skyray_ce_Process;

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
    zend_object std;
}skyray_process_t;

SKYRAY_MINIT_FUNCTION(process);

#endif /* PROCESS_H_ */
