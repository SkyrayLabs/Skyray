/*
 * stream_server.c
 *
 *  Created on: Mar 24, 2015
 *      Author: hujin
 */


#include "stream_server.h"
#include "stream.h"
#include "protocol.h"

zend_class_entry *skyray_ce_StreamServer;
zend_object_handlers skyray_handler_StreamServer;

static inline skyray_stream_server_t *skyray_stream_server_from_obj(zend_object *obj) {
    return (skyray_stream_server_t*)((char*)(obj) - XtOffsetOf(skyray_stream_server_t, std));
}

zend_object * skyray_stream_server_object_new(zend_class_entry *ce)
{
    skyray_stream_server_t *intern;
    intern = ecalloc(1, sizeof(skyray_stream_server_t) + zend_object_properties_size(ce));

    array_init(&intern->streams);

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_StreamServer;
    return &intern->std;
}

void skyray_stream_server_object_free(zend_object *object)
{
    skyray_stream_server_t *intern = skyray_stream_server_from_obj(object);
    zend_object_std_dtor(&intern->std);

    zval_dtor(&intern->streams);

    efree(intern->protocol_creator);
}

void skyray_stream_server_init(skyray_stream_server_t *self, skyray_reactor_t *reactor)
{
    self->reactor = reactor;
    uv_tcp_init(&reactor->loop, &self->serv);
    self->serv.data = self;
}

static void on_connection(uv_stream_t *serv, int status)
{
    if (status < 0) {
        //TODO error handling
        printf("error: %s\n", uv_strerror(status));
        return;
    }
    skyray_stream_server_t *server = serv->data;

    zval zstream;
    zend_object *protocol = skyray_protocol_create_from_factory(server->protocol_creator);

    object_init_ex(&zstream, skyray_ce_Stream);
    skyray_stream_t *stream = skyray_stream_from_obj(Z_OBJ_P(&zstream));

    skyray_stream_init_nonblocking(stream, server->reactor, protocol);

    int result = uv_accept(serv, (uv_stream_t *)&stream->tcp);
    if (result < 0) {
        uv_close((uv_handle_t *)&stream->tcp, NULL);
        zend_object_release(Z_OBJ(zstream));
        return;
    }

    skyray_stream_on_opened(stream, SR_READABLE | SR_WRITABLE);

    skyray_stream_read_start(stream);
}

zend_bool skyray_stream_server_listen(skyray_stream_server_t *self, zend_string *host, zend_long port, zend_long backlog)
{
    struct sockaddr_in addr;
    uv_ip4_addr(host->val, port, &addr);

    uv_tcp_bind(&self->serv, (const struct sockaddr *)&addr, 0);

    int result = uv_listen((uv_stream_t *)&self->serv, backlog, on_connection);

    if (result < 0) {
        skyray_throw_exception_from_errno(result);
    }

    return result >= 0;
}


SKYRAY_METHOD(stream_server, __construct)
{
    zval *protocol_creator = NULL;
    zval *reactor = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &protocol_creator, &reactor) == FAILURE) {
        return;
    }

    if (!zend_is_callable(protocol_creator, 0, NULL)) {
        skyray_throw_exception("The parameter $protocolCreator is not a valid callable");
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));

    intern->protocol_creator = emalloc(sizeof(zval));
    ZVAL_COPY(intern->protocol_creator, protocol_creator);

    skyray_stream_server_init(intern, skyray_reactor_from_obj(Z_OBJ_P(reactor)));
}

SKYRAY_METHOD(stream_server, listen)
{
    zend_string *host;
    zend_long port;
    zend_long backlog = 10;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl|l", &host, &port, &backlog) == FAILURE) {
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));

    skyray_stream_server_listen(intern, host, port, backlog);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, factory)
    ZEND_ARG_INFO(0, reactor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_listen, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()



static const zend_function_entry class_methods[] = {
    SKYRAY_ME(stream_server, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream_server, listen, arginfo_listen, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(stream_server)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\StreamServer", class_methods);
    skyray_ce_StreamServer = zend_register_internal_class(&ce);
    skyray_ce_StreamServer->create_object = skyray_stream_server_object_new;

    memcpy(&skyray_handler_StreamServer, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_StreamServer.offset = XtOffsetOf(skyray_stream_server_t, std);
    skyray_handler_StreamServer.free_obj = skyray_stream_server_object_free;

    return SUCCESS;
}
