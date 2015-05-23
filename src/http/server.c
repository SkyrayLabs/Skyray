/*
 * server.c
 *
 *  Created on: Apr 19, 2015
 *      Author: hujin
 */

#include "http.h"

zend_class_entry *skyray_ce_HttpServer;
zend_object_handlers skyray_handler_HttpServer;


zend_object * skyray_http_server_object_new(zend_class_entry *ce)
{
    skyray_http_server_t *intern;
    intern = ecalloc(1, sizeof(skyray_http_server_t) + zend_object_properties_size(ce));

    skyray_stream_server_object_init(&intern->base, ce);
    intern->base.std.handlers = &skyray_handler_HttpServer;

    return &intern->base.std;
}

zend_object * skyray_http_server_object_clone(zval *obj)
{
    return NULL;
}

void skyray_http_server_object_free(zend_object *object)
{
    skyray_http_server_t *intern = skyray_http_server_from_obj(object);
    zval_ptr_dtor(&intern->upgrader);
    zval_ptr_dtor(&intern->ws_factory);
    skyray_stream_server_object_free(object);
}

SKYRAY_METHOD(HttpServer, setUpgrader)
{
    zval *upgrader = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &upgrader) == FAILURE) {
        return;
    }

    if (!zend_is_callable(upgrader, 0, NULL)) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The upgrader is not a valid callable");
        return;
    }

    skyray_http_server_t *intern = skyray_http_server_from_obj(Z_OBJ_P(getThis()));
    ZVAL_COPY(&intern->upgrader, upgrader);
}

SKYRAY_METHOD(HttpServer, getUpgrader)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_http_server_t *intern = skyray_http_server_from_obj(Z_OBJ_P(getThis()));

    RETURN_ZVAL(&intern->upgrader, 1, 0);
}

SKYRAY_METHOD(HttpServer, setWsFactory)
{
    zval *factory = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &factory) == FAILURE) {
        return;
    }

    if (!zend_is_callable(factory, 0, NULL)) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The factory is not a valid callable");
        return;
    }

    skyray_http_server_t *intern = skyray_http_server_from_obj(Z_OBJ_P(getThis()));
    ZVAL_COPY(&intern->ws_factory, factory);
}

SKYRAY_METHOD(HttpServer, getWsFactory)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_http_server_t *intern = skyray_http_server_from_obj(Z_OBJ_P(getThis()));

    RETURN_ZVAL(&intern->ws_factory, 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_setUpgrader, 0, 0, 1)
    ZEND_ARG_INFO(0, upgrader)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setWsFactory, 0, 0, 1)
    ZEND_ARG_INFO(0, upgrader)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(HttpServer, setUpgrader, arginfo_setUpgrader, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpServer, getUpgrader, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpServer, setWsFactory, arginfo_setWsFactory, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpServer, getWsFactory, arginfo_empty, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SKYRAY_MINIT_FUNCTION(http_server)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\http\\Server", class_methods);
    skyray_ce_HttpServer = zend_register_internal_class_ex(&ce, skyray_ce_StreamServer);
    skyray_ce_HttpServer->create_object = skyray_http_server_object_new;

    memcpy(&skyray_handler_HttpServer, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_HttpServer.free_obj = skyray_http_server_object_free;
    skyray_handler_HttpServer.clone_obj = skyray_http_server_object_clone;

    return SUCCESS;
}
