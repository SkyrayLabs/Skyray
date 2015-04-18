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
extern zend_class_entry *skyray_ce_HttpResponse;

enum {
    SR_HTTP_VERSION_10 = 10,
    SR_HTTP_VERSION_11 = 11,
    SR_HTTP_VERSION_20 = 20,
};

static char *sr_http_versions[3] = {
    "1.0",
    "1.1",
    "2.0",
    NULL,
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

typedef struct _skyray_http_message {
    zend_object std;
    unsigned short version_major;
    unsigned short version_minor;
    zend_array headers;
    zend_array iheaders; // case insensitive headers
    zval body;
    zval raw_body;
}skyray_http_message_t;

typedef struct _skyray_http_request {
    skyray_http_message_t message;
    int method;
    zval query_params;
    zval cookie_params;
    zval uri;
}skyray_http_request_t;

typedef struct _skyray_http_response {
    skyray_http_message_t message;
    int code;
    zend_string *phrase;
}skyray_http_response_t;

static inline skyray_http_request_t *skyray_http_request_from_obj(zend_object *obj) {
    return (skyray_http_request_t*)(obj);
}

static inline skyray_http_response_t *skyray_http_response_from_obj(zend_object *obj) {
    return (skyray_http_response_t*)(obj);
}

void skyray_http_message_init(skyray_http_message_t *self, zend_class_entry *ce);

PHP_MINIT_FUNCTION(skyray_http_message);
PHP_MINIT_FUNCTION(skyray_http_request);
PHP_MINIT_FUNCTION(skyray_http_response);


#endif /* SRC_HTTP_HTTP_H_ */
