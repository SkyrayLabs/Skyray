/*
 * stream_client.h
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_STREAM_CLIENT_H_
#define SRC_CORE_STREAM_CLIENT_H_

#include "src/skyray.h"

zend_class_entry *skyray_ce_StreamClient;

typedef struct _skyray_stream_client {
    // protocol
    zval *protocol_creator;
    zval *reactor;
    zend_object std;
}skyray_stream_client_t;

PHP_MINIT_FUNCTION(stream_client);


#endif /* SRC_CORE_STREAM_CLIENT_H_ */
