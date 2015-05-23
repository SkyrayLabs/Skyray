/*
 * response.c
 *
 *  Created on: Apr 19, 2015
 *      Author: hujin
 */

#include "http.h"

zend_class_entry *skyray_ce_HttpResponse;
zend_object_handlers skyray_handler_HttpResponse;

zend_object * skyray_http_response_object_new(zend_class_entry *ce)
{
    skyray_http_response_t *intern;
    intern = ecalloc(1, sizeof(skyray_http_response_t) + zend_object_properties_size(ce));

    skyray_http_message_init((skyray_http_message_t *)intern, ce);

    intern->code = 200;
    intern->phrase = NULL;

    intern->message.std.handlers = &skyray_handler_HttpResponse;
    return &intern->message.std;
}

zend_object * skyray_http_response_object_clone(zval *obj)
{
    return NULL;
}

void skyray_http_response_object_free(zend_object *object)
{
    skyray_http_response_t *intern = skyray_http_response_from_obj(object);

    if (intern->phrase) {
        zend_string_release(intern->phrase);
    }

    skyray_http_message_object_free(object);
}

SKYRAY_METHOD(HttpResponse, getStatusCode)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_response_t *intern = skyray_http_response_from_obj(Z_OBJ_P(getThis()));

    RETURN_LONG(intern->code);
}

SKYRAY_METHOD(HttpResponse, getReasonPhrase)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_response_t *intern = skyray_http_response_from_obj(Z_OBJ_P(getThis()));

    if (intern->phrase) {
        zend_string_addref(intern->phrase);
        RETURN_STR(intern->phrase);
    } else {

    }
}

SKYRAY_METHOD(HttpResponse, setStatus)
{
    zend_string *phrase;
    zend_long code;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l|S", &code, &phrase) ==  FAILURE) {
        return;
    }

    skyray_http_response_t *intern = skyray_http_response_from_obj(Z_OBJ_P(getThis()));

    intern->code = code;
    intern->phrase = phrase;
    zend_string_addref(phrase);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_setStatus, 0, 0, 1)
    ZEND_ARG_INFO(0, code)
    ZEND_ARG_INFO(0, reasonPhrase)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(HttpResponse, getStatusCode, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpResponse, getReasonPhrase, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpResponse, setStatus, arginfo_setStatus, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(http_response)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\http\\Response", class_methods);
    skyray_ce_HttpResponse = zend_register_internal_class_ex(&ce, skyray_ce_HttpMessage);
    skyray_ce_HttpResponse->create_object = skyray_http_response_object_new;

    memcpy(&skyray_handler_HttpResponse, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_HttpResponse.free_obj = skyray_http_response_object_free;
    skyray_handler_HttpResponse.clone_obj = skyray_http_response_object_clone;

    return SUCCESS;
}
