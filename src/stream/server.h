/*
 * stream_server.h
 *
 *  Created on: Mar 24, 2015
 *      Author: hujin
 */

#ifndef SRC_STREAM_SERVER_H_
#define SRC_STREAM_SERVER_H_


#include "src/skyray.h"
#include "src/reactor.h"

extern zend_class_entry *skyray_ce_StreamServer;

typedef struct _skyray_stream_server {
    zend_object std;
    // protocol
    zval *protocol_creator;
    skyray_reactor_t *reactor;
    uv_tcp_t serv;
    zval streams; // connected streams
}skyray_stream_server_t;

void skyray_stream_server_object_init(skyray_stream_server_t *self, zend_class_entry *ce);
void skyray_stream_server_object_free(zend_object *object);
PHP_MINIT_FUNCTION(stream_server);


#endif /* SRC_STREAM_SERVER_H_ */
