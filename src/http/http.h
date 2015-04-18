/*
 * http.h
 *
 *  Created on: Apr 15, 2015
 *      Author: hujin
 */

#ifndef SRC_HTTP_HTTP_H_
#define SRC_HTTP_HTTP_H_


#include "src/skyray.h"
#include "deps/http-parser/http_parser.h"

extern zend_class_entry *skyray_ce_HttpMessage;
extern zend_class_entry *skyray_ce_HttpRequest;

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

enum _http_method {
#define XX(num, name, string) SR_HTTP_##name = num,
    HTTP_METHOD_MAP(XX)
#undef XX
    SR_HTTP_METHOD_LAST
};

static char *sr_http_methods[] = {
#define XX(num, name, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};

#define SR_HTTP_METHOD_GET_NAME(num) sr_http_methods[num]

typedef struct _skyray_message {
    zend_object std;
    int version; /* the protocol version*/
    zend_array headers;
    zend_array iheaders; // case insensitive headers
    zval body;
    zval raw_body;
}skyray_http_message_t;

typedef struct _skyray_request {
    skyray_http_message_t message;
    int method;
    zval query_params;
    zval cookie_params;
    zval parsed_body;
}skyray_http_request_t;


void skyray_http_message_init(skyray_http_message_t *self, zend_class_entry *ce);

PHP_MINIT_FUNCTION(skyray_http_message);
PHP_MINIT_FUNCTION(skyray_http_request);


#endif /* SRC_HTTP_HTTP_H_ */
