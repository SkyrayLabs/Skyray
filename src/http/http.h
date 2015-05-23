/*
 * http.h
 *
 *  Created on: Apr 15, 2015
 *      Author: hujin
 */

#ifndef SRC_HTTP_HTTP_H_
#define SRC_HTTP_HTTP_H_


#include "src/skyray.h"
#include "src/stream/server.h"
#include "deps/http-parser/http_parser.h"

extern zend_class_entry *skyray_ce_HttpMessage;
extern zend_class_entry *skyray_ce_HttpRequest;
extern zend_class_entry *skyray_ce_HttpResponse;
extern zend_class_entry *skyray_ce_HttpProtocol;
extern zend_class_entry *skyray_ce_HttpServer;


extern zend_string *intern_str_content_type; // Content-Type
extern zend_string *intern_str_content_length; // Content-Length
extern zend_string *intern_str_application_json; // application/json
extern zend_string *intern_str_text_plain; // text/plain
extern zend_string *intern_str_connection; // Connection
extern zend_string *intern_str_server; // Server
extern zend_string *intern_str_date; // Date
extern zend_string *intern_str_cookie; // "Cookie"

extern zend_string *intern_str_param_delimiter; // "; "
extern zend_string *intern_str_equal_delimiter; // "="


enum {
    SR_HTTP_VERSION_10 = 10,
    SR_HTTP_VERSION_11 = 11,
    SR_HTTP_VERSION_20 = 20,
};

static char *sr_http_versions[4] = {
    "1.0",
    "1.1",
    "2.0",
    NULL
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

typedef struct _skyray_buffer {
    size_t size;
    zend_string *buf;
}skyray_buffer_t;

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


typedef struct _skyray_http_protocol {
    http_parser parser;
    zend_object *req; /* the current request */

    skyray_buffer_t uri;
    skyray_buffer_t header_name;
    skyray_buffer_t header_value;
    skyray_buffer_t body;

    unsigned int chunked:1; // indicate the response is chunked
    unsigned int close:1; // indicate whether or not to close the stream after response is sent.
    unsigned int reserved:30;

    zend_object *stream;
    zend_object std;
}skyray_http_protocol_t;

typedef struct _skyray_http_server {
    skyray_stream_server_t base;
    zval upgrader;
    zval ws_factory;
}skyray_http_server_t;

static inline void skyray_buffer_init(skyray_buffer_t *buf, int size)
{
    buf->size = size;
    buf->buf = zend_string_alloc(size, 0);
    buf->buf->len = 0;
    buf->buf->val[0] = '\0';
}

static inline void skyray_buffer_init_stringl(skyray_buffer_t *buf, const char *at, int len)
{
    buf->size = len;
    buf->buf = zend_string_alloc(len, 0);
    memcpy(buf->buf->val, at, len);
    buf->buf->len = len;
    buf->buf->val[len] = '\0';
}

static inline zend_bool skyray_buffer_initialized(skyray_buffer_t *buf)
{
    return buf->buf != NULL && buf->size != 0;
}

static inline void skyray_buffer_appendl(skyray_buffer_t *buf, const char *at, int len)
{
    int oldlen = buf->buf->len;
    int newlen = oldlen + len;
    if (buf->size < newlen) {
        buf->buf = zend_string_extend(buf->buf, buf->size * 2, 0);
        buf->size *= 2;
    }
    memcpy(buf->buf->val + oldlen, at, len);
    buf->buf->len = newlen;
    buf->buf->val[oldlen + len] = '\0';
}

static inline void skyray_buffer_reset(skyray_buffer_t *buf)
{
    buf->size = 0;
    buf->buf = NULL;
}

static inline void skyray_buffer_release(skyray_buffer_t *buf)
{
    assert(buf->buf != NULL);
    zend_string_release(buf->buf);
    buf->size = 0;
    buf->buf = NULL;
}

static inline skyray_http_request_t *skyray_http_request_from_obj(zend_object *obj) {
    return (skyray_http_request_t*)(obj);
}

static inline skyray_http_response_t *skyray_http_response_from_obj(zend_object *obj) {
    return (skyray_http_response_t*)(obj);
}

static inline skyray_http_protocol_t *skyray_http_protocol_from_obj(zend_object *obj) {
    return (skyray_http_protocol_t*)((char*)(obj) - XtOffsetOf(skyray_http_protocol_t, std));;
}

static inline skyray_http_server_t *skyray_http_server_from_obj(zend_object *obj) {
    return (skyray_http_server_t*)(obj);
}

void skyray_http_message_init(skyray_http_message_t *self, zend_class_entry *ce);
zval* skyray_http_message_get_header(skyray_http_message_t *self, zend_string *name, zend_bool first);

SKYRAY_MINIT_FUNCTION(http_message);
SKYRAY_MINIT_FUNCTION(http_request);
SKYRAY_MINIT_FUNCTION(http_response);
SKYRAY_MINIT_FUNCTION(http_protocol);
SKYRAY_MINIT_FUNCTION(http_server);


#endif /* SRC_HTTP_HTTP_H_ */
