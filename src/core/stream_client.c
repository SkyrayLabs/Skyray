/*
 * stream_client.c
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#include "stream_client.h"
#include "protocol.h"
#include "stream.h"
#include "reactor.h"

zend_class_entry *skyray_ce_StreamClient;
zend_object_handlers skyray_handler_StreamClient;

static inline skyray_stream_client_t *skyray_stream_client_from_obj(zend_object *obj) {
    return (skyray_stream_client_t*)((char*)(obj) - XtOffsetOf(skyray_stream_client_t, std));
}

zend_object * skyray_stream_client_object_new(zend_class_entry *ce)
{
    skyray_stream_client_t *intern;
    intern = ecalloc(1, sizeof(skyray_stream_client_t) + zend_object_properties_size(ce));

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_StreamClient;
    return &intern->std;
}

void skyray_stream_client_object_free(zend_object *object)
{
    skyray_stream_client_t *intern = skyray_stream_client_from_obj(object);
    zend_object_std_dtor(&intern->std);

    if (intern->protocol_creator) {
        zval_ptr_dtor(intern->protocol_creator);
        efree(intern->protocol_creator);
    }
    if (intern->reactor) {
        zval_ptr_dtor(intern->reactor);
        efree(intern->reactor);
    }
}

SKYRAY_METHOD(stream_client, __construct)
{
    zval *protocol_creator = NULL;
    zval *reactor = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &protocol_creator, &reactor) == FAILURE) {
        return;
    }

    if (protocol_creator && Z_TYPE_P(protocol_creator) != IS_NULL && !zend_is_callable(protocol_creator, 0, NULL)) {
        skyray_throw_exception("The parameter $protocolCreator is not a valid callable");
        return;
    }

    skyray_stream_client_t *intern = skyray_stream_client_from_obj(Z_OBJ_P(getThis()));

    if (protocol_creator && Z_TYPE_P(protocol_creator) != IS_NULL) {
        intern->protocol_creator = emalloc(sizeof(zval));
        ZVAL_COPY(intern->protocol_creator, protocol_creator);
    }
    if (reactor && Z_TYPE_P(reactor) != IS_NULL) {
        intern->reactor = emalloc(sizeof(zval));
        ZVAL_COPY(intern->reactor, reactor);
    }
}

int stream_client_do_connect(char *host, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0) {
        skyray_throw_exception_from_errno(errno);
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;

    inet_aton(host, &addr.sin_addr);
    addr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr))  < 0) {
        skyray_throw_exception_from_errno(errno);
        close(fd);
        return -1;
    }

    return fd;
}

void stream_client_do_connect_blocking(
        skyray_stream_client_t *self, zend_object *protocol_obj,
        zend_string *host, zend_long port, zval *return_value)
{
    int fd = stream_client_do_connect(host->val, port);

    if (fd < 0) {
        return;
    }

    zval zstream;

    object_init_ex(&zstream, skyray_ce_Stream);
    zend_object *stream = Z_OBJ_P(&zstream);

    skyray_stream_t * stream_intern = skyray_stream_from_obj(stream);
    skyray_stream_init_blocking(stream_intern, SR_TCP, fd, protocol_obj);

    skyray_stream_on_opened(stream_intern, SR_READABLE | SR_WRITABLE);

    if (!self->protocol_creator) {
        ZVAL_COPY(return_value, &zstream);
        return;
    }

    zend_string *buffer;

    ++GC_REFCOUNT(protocol_obj); //preventing protocol instance be free'd after connection closed.
    while((buffer = skyray_stream_read(stream_intern, 0))) {
        if (buffer->len == 0) {
            zend_string_free(buffer);
            break;
        }
        skyray_stream_on_data(stream_intern, buffer);
        zend_string_free(buffer);
    }

    RETURN_OBJ(protocol_obj);
}

static void on_connected(uv_connect_t *req, int status)
{
    skyray_stream_t *stream = req->data;

    if (status < 0) {
        zval_ptr_dtor(&stream->protocol);
        zend_object_release(&stream->std);
        goto clean;
    }

    skyray_protocol_on_stream_connected(&stream->protocol);
    req->handle->data = req->data;
    skyray_stream_read_start(stream);

clean:
    efree(req);
}

void stream_client_do_connect_nonblocking(
        skyray_stream_client_t *self, zend_object *protocol_obj,
        zend_string *host, zend_long port, zval *return_value)
{
    skyray_reactor_t *reactor = skyray_reactor_from_obj(Z_OBJ_P(self->reactor));
    zval zstream;

    object_init_ex(&zstream, skyray_ce_Stream);

    skyray_stream_t * stream = skyray_stream_from_obj(Z_OBJ(zstream));

    skyray_stream_init_nonblocking(stream, SR_TCP, reactor, protocol_obj);

    struct sockaddr_in addr;

    uv_ip4_addr(host->val, port, &addr);

    uv_connect_t *req = (uv_connect_t*)emalloc(sizeof(uv_connect_t));
    req->type = UV_TCP;
    req->data = stream;

    uv_tcp_connect(req, &stream->tcp, (struct sockaddr*)&addr, on_connected);

    ZVAL_COPY(return_value, &zstream);
}

SKYRAY_METHOD(stream_client, connectTCP)
{
    zend_string *host;
    zend_long port;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl", &host, &port) == FAILURE) {
        return;
    }

    zval protocol;
    zend_object *protocol_obj = NULL;
    skyray_stream_client_t *intern = skyray_stream_client_from_obj(Z_OBJ_P(getThis()));

    if (intern->protocol_creator) {
        protocol_obj = skyray_protocol_create_from_factory(intern->protocol_creator);

        if (!protocol_obj) {
            return;
        }
        ZVAL_OBJ(&protocol, protocol_obj);
    }

    if (!intern->reactor) {
        stream_client_do_connect_blocking(intern, protocol_obj, host, port, return_value);
    } else {
        stream_client_do_connect_nonblocking(intern, protocol_obj, host, port, return_value);
    }
}

SKYRAY_METHOD(stream_client, createPipe)
{
    zend_bool duplex = 0;
    zval stream1, stream2;
    int fds[2];
    int sockret;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &duplex) == FAILURE) {
        return;
    }

    if (duplex) {
        sockret = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    } else {
        sockret = pipe(fds);
    }

    if (sockret == -1) {
        RETURN_FALSE;
    }

    array_init(return_value);

    object_init_ex(&stream1, skyray_ce_Stream);
    object_init_ex(&stream2, skyray_ce_Stream);

    skyray_stream_t * intern1 = skyray_stream_from_obj(Z_OBJ_P(&stream1));
    skyray_stream_t * intern2 = skyray_stream_from_obj(Z_OBJ_P(&stream2));

    skyray_stream_init_blocking(intern1, SR_PIPE, fds[0], NULL);
    skyray_stream_init_blocking(intern2, SR_PIPE, fds[1], NULL);

    if (duplex) {
        skyray_stream_on_opened(intern1, SR_READABLE | SR_WRITABLE);
        skyray_stream_on_opened(intern2, SR_READABLE | SR_WRITABLE);
    } else {
        skyray_stream_on_opened(intern1, SR_READABLE);
        skyray_stream_on_opened(intern2, SR_WRITABLE);
    }

    add_index_zval(return_value, 0, &stream1);
    add_index_zval(return_value, 1, &stream2);

    zval_add_ref(&stream1);
    zval_add_ref(&stream2);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, factory)
    ZEND_ARG_INFO(0, reactor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_connectTCP, 0, 0, 2)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_createPipe, 0, 0, 1)
    ZEND_ARG_INFO(0, duplex)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(stream_client, __construct, arginfo___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream_client, connectTCP, arginfo_connectTCP, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream_client, createPipe, arginfo_createPipe, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(stream_client)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\StreamClient", class_methods);
    skyray_ce_StreamClient = zend_register_internal_class(&ce);
    skyray_ce_StreamClient->create_object = skyray_stream_client_object_new;

    memcpy(&skyray_handler_StreamClient, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_StreamClient.offset = XtOffsetOf(skyray_stream_client_t, std);
    skyray_handler_StreamClient.free_obj = skyray_stream_client_object_free;

    return SUCCESS;
}
