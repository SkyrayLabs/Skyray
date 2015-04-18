/*
 * request.c
 *
 *  Created on: Apr 18, 2015
 *      Author: hujin
 */

#include "http.h"

zend_class_entry *skyray_ce_HttpRequest;
zend_object_handlers skyray_handler_HttpRequest;


static inline skyray_http_request_t *skyray_http_request_from_obj(zend_object *obj) {
    return (skyray_http_request_t*)(obj);
}

zend_object * skyray_http_request_object_new(zend_class_entry *ce)
{
    skyray_http_request_t *intern;
    intern = ecalloc(1, sizeof(skyray_http_request_t) + zend_object_properties_size(ce));

    skyray_http_message_init((skyray_http_message_t *)intern, ce);

    intern->method = SR_HTTP_GET;
    ZVAL_NULL(&intern->cookie_params);
    ZVAL_NULL(&intern->query_params);

    intern->message.std.handlers = &skyray_handler_HttpRequest;
    return &intern->message.std;
}

zend_object * skyray_http_request_object_clone(zval *obj)
{
    return NULL;
}

void skyray_http_request_object_free(zend_object *object)
{
    skyray_http_request_t *intern = skyray_http_request_from_obj(object);

    zval_dtor(&intern->cookie_params);
    zval_dtor(&intern->query_params);
    zval_dtor(&intern->parsed_body);

    skyray_http_message_object_free(object);
}

SKYRAY_METHOD(HttpRequest, getMethod)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));

    RETURN_STRING(SR_HTTP_METHOD_GET_NAME(intern->method));
}

SKYRAY_METHOD(HttpRequest, setMethod)
{
    zend_string *name;
    int i;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));

    for (i = 0; i < SR_HTTP_METHOD_LAST; i ++) {
        if (strcasecmp(name->val, SR_HTTP_METHOD_GET_NAME(i)) == 0) {
            intern->method = i;
            break;
        }
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpRequest, getCookieParams)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));

    if (ZVAL_IS_NULL(&intern->cookie_params)) {
        RETURN_EMPTY_ARR();
    } else {
        RETURN_ZVAL(&intern->cookie_params, 1, 0);
    }
}

SKYRAY_METHOD(HttpRequest, setCookieParams)
{
    zval *params;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &params) ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));
    zval_dtor(&intern->cookie_params);
    ZVAL_COPY(&intern->cookie_params, params);

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpRequest, getQueryParams)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));

    if (ZVAL_IS_NULL(&intern->query_params)) {
        RETURN_EMPTY_ARR();
    } else {
        RETURN_ZVAL(&intern->query_params, 1, 0);
    }
}

SKYRAY_METHOD(HttpRequest, setQueryParams)
{
    zval *params;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &params) ==  FAILURE) {
        return;
    }

    skyray_http_request_t *intern = skyray_http_request_from_obj(Z_OBJ_P(getThis()));
    zval_dtor(&intern->query_params);
    ZVAL_COPY(&intern->query_params, params);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_setMethod, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setCookieParams, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setQueryParams, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(HttpRequest, getMethod, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpRequest, setMethod, arginfo_setMethod, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpRequest, getCookieParams, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpRequest, setCookieParams, arginfo_setCookieParams, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpRequest, getQueryParams, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpRequest, setQueryParams, arginfo_setQueryParams, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_http_request)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\http\\Request", class_methods);
    skyray_ce_HttpRequest = zend_register_internal_class_ex(&ce, skyray_ce_HttpMessage);
    skyray_ce_HttpRequest->create_object = skyray_http_request_object_new;

    memcpy(&skyray_handler_HttpRequest, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_HttpRequest.free_obj = skyray_http_request_object_free;
    skyray_handler_HttpRequest.clone_obj = skyray_http_request_object_clone;

    return SUCCESS;
}
