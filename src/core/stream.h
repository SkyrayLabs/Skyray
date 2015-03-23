/*
 * stream.h
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_STREAM_H_
#define SRC_CORE_STREAM_H_


#include "src/skyray.h"

zend_class_entry *skyray_ce_Stream;

typedef struct _skyray_stream {
    int fd;
    zend_bool readable;
    zend_bool writable;
    zend_object std;
}skyray_stream_t;

static inline skyray_stream_t *skyray_stream_from_obj(zend_object *obj) /* {{{ */ {
    return (skyray_stream_t*)((char*)(obj) - XtOffsetOf(skyray_stream_t, std));
}

int skyray_stream_write(skyray_stream_t * self, zend_string *buffer);
zend_string * skyray_stream_read(skyray_stream_t * self);

PHP_MINIT_FUNCTION(stream);


#endif /* SRC_CORE_STREAM_H_ */
