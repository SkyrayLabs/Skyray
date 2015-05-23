/*
 * stream.h
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_STREAM_STREAM_H_
#define SRC_STREAM_STREAM_H_


#include "src/skyray.h"
#include "uv.h"

extern zend_class_entry *skyray_ce_Stream;

typedef struct _skyray_stream {
    union {
        uv_stream_t stream;
        uv_tcp_t tcp;
        uv_pipe_t pipe;
    };
    int blocking;
    int type;
    zval protocol;
    zend_object std;
}skyray_stream_t;

static inline skyray_stream_t *skyray_stream_from_obj(zend_object *obj) /* {{{ */ {
    return (skyray_stream_t*)((char*)(obj) - XtOffsetOf(skyray_stream_t, std));
}

#define skyray_stream_fd(_self) (_self)->stream.io_watcher.fd

void skyray_stream_on_data(skyray_stream_t *self, zend_string *buffer);
void skyray_stream_on_closed(skyray_stream_t *self);
int skyray_stream_write(skyray_stream_t * self, zend_string *buffer);
int skyray_stream_writel(skyray_stream_t *self, const char *at, size_t len);
zend_string * skyray_stream_read(skyray_stream_t * self, zend_bool slient);
zend_bool skyray_stream_close(skyray_stream_t *self);

void skyray_stream_read_start(skyray_stream_t *self);
void skyray_stream_read_stop(skyray_stream_t *self);

PHP_MINIT_FUNCTION(stream);


#endif /* SRC_STREAM_STREAM_H_ */
