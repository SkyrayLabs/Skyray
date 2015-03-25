/*
 * stream_client.c
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#include "stream_client.h"
#include "stream.h"

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
        efree(intern->protocol_creator);
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

void protocol_on_connect_stream(zval *protocol, zend_object *stream)
{
    zval function_name;
    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("connectStream"), 0));

    zval retval;
    zval params[1];

    ZVAL_OBJ(&params[0], stream);

    call_user_function(EG(function_table), protocol, &function_name, &retval, 1, params);
    zval_dtor(&function_name);
}

void protocol_on_stream_connected(zval *protocol)
{
    zval function_name;
    zval retval;

    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("streamConnected"), 0));

    call_user_function(EG(function_table), protocol, &function_name, &retval, 0, NULL);
    zval_dtor(&function_name);
}

void protocol_on_data_received(zval *protocol, zend_string *data)
{
    zval function_name;
    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("dataReceived"), 0));

    zval retval;
    zval params[1];

    ZVAL_STR(&params[0], data);


    call_user_function(EG(function_table), protocol, &function_name, &retval, 1, params);
    zval_dtor(&function_name);
}

void protocol_on_stream_closed(zval *protocol)
{
    zval function_name;
    zval retval;

    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("streamClosed"), 0));

    call_user_function(EG(function_table), protocol, &function_name, &retval, 0, NULL);
    zval_dtor(&function_name);
}

zend_object * stream_client_create_protocol(skyray_stream_client_t *self)
{
    zval protocol;
    call_user_function(EG(function_table), NULL, self->protocol_creator, &protocol, 0, NULL);

    if (!instanceof_function(Z_OBJCE(protocol), skyray_ce_ProtocolInterface)) {
        skyray_throw_exception("The protocol created by $protocolCreator must be instance of 'skyray\\core\\ProtocolInterface'");
        return NULL;
    }

    return Z_OBJ_P(&protocol);
}

SKYRAY_METHOD(stream_client, connectTCP)
{
    zend_string *host;
    zend_long port;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sl", &host, &port) == FAILURE) {
        return;
    }

    zval protocol;
    zend_object *protocol_obj;
    skyray_stream_client_t *intern = skyray_stream_client_from_obj(Z_OBJ_P(getThis()));

    if (intern->protocol_creator) {
        protocol_obj = stream_client_create_protocol(intern);

        if (!protocol_obj) {
            return;
        }
        ZVAL_OBJ(&protocol, protocol_obj);
    }

    int fd = stream_client_do_connect(host->val, port);

    if (fd < 0) {
        return;
    }

    object_init_ex(return_value, skyray_ce_Stream);
    zend_object *stream = Z_OBJ_P(return_value);

    skyray_stream_t * stream_intern = skyray_stream_from_obj(stream);
    stream_intern->fd = fd;
    stream_intern->writable = stream_intern->readable = 1;

    if (!intern->protocol_creator) {
        RETURN_OBJ(stream);
        return;
    }

    protocol_on_connect_stream(&protocol, stream);

    protocol_on_stream_connected(&protocol);

    zend_string *buffer;

    while((buffer = skyray_stream_read(stream_intern))) {
        if (buffer->len == 0) {
            zend_string_free(buffer);
            break;
        }
        protocol_on_data_received(&protocol, buffer);
        zend_string_free(buffer);
    }

    protocol_on_stream_closed(&protocol);

    RETURN_OBJ(Z_OBJ(protocol));
}

SKYRAY_METHOD(stream_client, createPipe)
{
    zend_bool duplex = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &duplex) == FAILURE) {
        return;
    }

    int fds[2];
    if (pipe(fds) == -1) {
        RETURN_FALSE;
    }

    array_init(return_value);
    zval *stream1 = emalloc(sizeof (zval));
    zval *stream2 = emalloc(sizeof (zval));

    object_init_ex(stream1, skyray_ce_Stream);
    object_init_ex(stream2, skyray_ce_Stream);

    skyray_stream_t * intern1 = skyray_stream_from_obj(Z_OBJ_P(stream1));
    skyray_stream_t * intern2 = skyray_stream_from_obj(Z_OBJ_P(stream2));

    intern1->fd = fds[0];
    intern2->fd = fds[1];

    intern1->readable = intern2->writable = 1;
    intern1->writable = intern2->readable = 0;

    add_index_zval(return_value, 0, stream1);
    add_index_zval(return_value, 1, stream2);

    efree(stream1);
    efree(stream2);
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
