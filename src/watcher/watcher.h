/*
 * fdwatcher.h
 *
 *  Created on: May 23, 2015
 *      Author: hujin
 */

#ifndef SRC_WATCHER_WATCHER_H_
#define SRC_WATCHER_WATCHER_H_


#include "src/skyray.h"
#include "src/reactor.h"
#include "src/processing/process.h"

extern zend_class_entry *skyray_ce_FdWatcher;
extern zend_class_entry *skyray_ce_FdWatcherHandler;

extern zend_class_entry *skyray_ce_ProcessWatcher;
extern zend_class_entry *skyray_ce_ProcessWatcherHandler;

typedef struct _skyray_fdwatcher {
    uv_poll_t poll;
    int events;
    zval fd;
    skyray_reactor_t *reactor;
    zend_object *handler;
    zend_object std;
} skyray_fdwatcher_t;

typedef struct _skyray_process_watcher {
    skyray_process_t *process;
    zend_object *handler;
    zend_object std;
}skyray_process_watcher_t;

int skyray_fdwatcher_parse_events(char *events, zend_bool reverse);

skyray_fdwatcher_t * skyray_fdwatcher_new(skyray_reactor_t *reactor, zval *fd, zval *handler);
void skyray_fdwatcher_watch(skyray_fdwatcher_t *self, int events);
void skyray_fdwatcher_stop(skyray_fdwatcher_t *self,  int events);

skyray_process_watcher_t * skyray_process_watcher_new(skyray_reactor_t *reactor, zval *process, zval *handler);
void skyray_process_watcher_watch(skyray_process_watcher_t *self);
void skyray_process_watcher_stop(skyray_process_watcher_t *self);

SKYRAY_MINIT_FUNCTION(fdwatcher);
SKYRAY_MINIT_FUNCTION(process_watcher);

#endif /* SRC_WATCHER_WATCHER_H_ */
