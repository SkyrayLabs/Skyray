/*
 * http.h
 *
 *  Created on: Apr 15, 2015
 *      Author: hujin
 */

#ifndef SRC_HTTP_HTTP_H_
#define SRC_HTTP_HTTP_H_


#include "src/skyray.h"

extern zend_class_entry *skyray_ce_HttpMessage;

enum {
    SR_HTTP_VERSION_10,
    SR_HTTP_VERSION_11,
    SR_HTTP_VERSION_20,
};

static char *sr_http_versions[3] = {
    "1.0",
    "1.1",
    "2.0"
};

typedef struct _skyray_message {
    zend_object std;
    int version; /* the protocol version*/
    zend_array headers;
    zend_array iheaders; // case insensitive headers
    zval body;
}skyray_http_message_t;


PHP_MINIT_FUNCTION(skyray_http_message);


#endif /* SRC_HTTP_HTTP_H_ */
