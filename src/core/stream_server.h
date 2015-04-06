/*
 * stream_server.h
 *
 *  Created on: Mar 24, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_STREAM_SERVER_H_
#define SRC_CORE_STREAM_SERVER_H_


#include "src/skyray.h"
#include "reactor.h"

extern zend_class_entry *skyray_ce_StreamServer;

typedef struct _skyray_stream_server {
    // protocol
    zval *protocol_creator;
    skyray_reactor_t *reactor;
    uv_tcp_t serv;
    zval streams; // connected streams
    zend_object std;
}skyray_stream_server_t;

PHP_MINIT_FUNCTION(stream_server);


#endif /* SRC_CORE_STREAM_SERVER_H_ */
