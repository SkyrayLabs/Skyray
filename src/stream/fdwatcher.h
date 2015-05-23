/*
 * fdwatcher.h
 *
 *  Created on: May 23, 2015
 *      Author: hujin
 */

#ifndef SRC_STREAM_FDWATCHER_H_
#define SRC_STREAM_FDWATCHER_H_


#include "src/skyray.h"
#include "src/reactor.h"

extern zend_class_entry *skyray_ce_FdWatcher;
extern zend_class_entry *skyray_ce_FdWatcherHandler;


typedef struct _skyray_fdwatcher {
    uv_poll_t poll;
    int events;
    zval fd;
    skyray_reactor_t *reactor;
    zend_object *handler;
    zend_object std;
} skyray_fdwatcher_t;

int skyray_fdwatcher_parse_events(char *events, zend_bool reverse);

skyray_fdwatcher_t * skyray_fdwatcher_new(skyray_reactor_t *reactor, zval *fd, zval *handler);
void skyray_fdwatcher_watch(skyray_fdwatcher_t *self, int events);
void skyray_fdwatcher_stop(skyray_fdwatcher_t *self,  int events);

SKYRAY_MINIT_FUNCTION(fdwatcher);

#endif /* SRC_STREAM_FDWATCHER_H_ */
