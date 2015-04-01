/*
 * stream.h
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_STREAM_H_
#define SRC_CORE_STREAM_H_


#include "src/skyray.h"

extern zend_class_entry *skyray_ce_Stream;

typedef enum _skyray_stream_status {
    SKYRAY_STREAM_STATUS_OPENING = 1,
    SKYRAY_STREAM_STATUS_OPENED,
    SKYRAY_STREAM_STATUS_CLOSED,
}skyray_stream_status_t;

#define SKYRAY_STREAM_READABLE 1
#define SKYRAY_STREAM_WRITABLE 2

typedef struct _skyray_stream {
    int fd;
    int rw_mask;
    skyray_stream_status_t status;
    zval protocol;
    zend_object std;
}skyray_stream_t;

static inline skyray_stream_t *skyray_stream_from_obj(zend_object *obj) /* {{{ */ {
    return (skyray_stream_t*)((char*)(obj) - XtOffsetOf(skyray_stream_t, std));
}

void skyray_stream_on_data(skyray_stream_t *self, zend_string *buffer);
int skyray_stream_write(skyray_stream_t * self, zend_string *buffer);
zend_string * skyray_stream_read(skyray_stream_t * self, zend_bool slient);
zend_bool skyray_stream_close(skyray_stream_t *self);

PHP_MINIT_FUNCTION(stream);


#endif /* SRC_CORE_STREAM_H_ */
