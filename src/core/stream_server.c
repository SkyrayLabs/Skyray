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

    skyray_stream_server_object_init(intern, ce);
    intern->std.handlers = &skyray_handler_StreamServer;

    return &intern->std;
}

void skyray_stream_server_object_init(skyray_stream_server_t *self, zend_class_entry *ce)
{
    self->reactor = NULL;
    array_init(&self->streams);
    zend_object_std_init(&self->std, ce);
    object_properties_init(&self->std, ce);
}

void skyray_stream_server_object_free(zend_object *object)
{
    skyray_stream_server_t *intern = skyray_stream_server_from_obj(object);

    zval_dtor(&intern->streams);

    zval_ptr_dtor(intern->protocol_creator);
    efree(intern->protocol_creator);

    if (intern->reactor) {
        zend_object_release(&intern->reactor->std);
    }

    zend_object_std_dtor(&intern->std);
}

void skyray_stream_server_init(skyray_stream_server_t *self)
{
    zval reactor;
    if (!self->reactor) {
        object_init_ex(&reactor, skyray_ce_Reactor);
        self->reactor = skyray_reactor_from_obj(Z_OBJ(reactor));
    }

    uv_tcp_init(&self->reactor->loop, &self->serv);
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

    skyray_stream_init_nonblocking(stream, SR_TCP, server->reactor, protocol);

    int result = uv_accept(serv, &stream->stream);
    if (result < 0) {
        uv_close((uv_handle_t *)&stream->stream, NULL);
        zend_object_release(protocol);
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
    zval *factory = NULL;
    zval *config = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|a", &factory, &config) == FAILURE) {
        return;
    }

    if (!zend_is_callable(factory, 0, NULL)) {
        zend_throw_exception_ex(skyray_ce_InvalidParamException, 0, "The parameter $factory is not a valid callable");
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));

    intern->protocol_creator = emalloc(sizeof(zval));
    ZVAL_COPY(intern->protocol_creator, factory);

    if (config) {
        skyray_object_configure(getThis(), Z_ARR_P(config));
    }
}

SKYRAY_METHOD(stream_server, setReactor)
{
    zval *reactor;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &reactor, skyray_ce_Reactor) == FAILURE) {
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));
    zval_add_ref(reactor);
    intern->reactor = skyray_reactor_from_obj(Z_OBJ_P(reactor));
}

SKYRAY_METHOD(stream_server, getReactor)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));

    ++GC_REFCOUNT(&intern->reactor->std);

    RETURN_OBJ(&intern->reactor->std);
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

    skyray_stream_server_init(intern);

    skyray_stream_server_listen(intern, host, port, backlog);
}

SKYRAY_METHOD(stream_server, start)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_stream_server_t *intern = skyray_stream_server_from_obj(Z_OBJ_P(getThis()));
    skyray_reactor_run(intern->reactor);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_setReactor, 0, 0, 1)
    ZEND_ARG_INFO(0, reactor)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(stream_server, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream_server, listen, arginfo_listen, ZEND_ACC_PUBLIC)
    SKYRAY_ME(stream_server, getReactor, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(stream_server, setReactor, arginfo_setReactor, ZEND_ACC_PUBLIC)
    SKYRAY_ME(stream_server, start, arginfo_empty, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(stream_server)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\StreamServer", class_methods);
    skyray_ce_StreamServer = zend_register_internal_class_ex(&ce, skyray_ce_Object);

    skyray_ce_StreamServer->create_object = skyray_stream_server_object_new;

    memcpy(&skyray_handler_StreamServer, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_StreamServer.offset = XtOffsetOf(skyray_stream_server_t, std);
    skyray_handler_StreamServer.free_obj = skyray_stream_server_object_free;

    return SUCCESS;
}
