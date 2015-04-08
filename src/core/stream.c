/*
 * stream.c
 *
 *  Created on: Mar 15, 2015
 *      Author: hujin
 */

#include "stream.h"
#include "reactor.h"

zend_class_entry *skyray_ce_Stream;
zend_object_handlers skyray_handler_Stream;

zend_object * skyray_stream_object_new(zend_class_entry *ce)
{
    skyray_stream_t *intern;
    intern = ecalloc(1, sizeof(skyray_stream_t) + zend_object_properties_size(ce));
    ZVAL_NULL(&intern->protocol);

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    intern->std.handlers = &skyray_handler_Stream;
    return &intern->std;
}

void skyray_stream_object_free(zend_object *object)
{
    skyray_stream_t *intern = skyray_stream_from_obj(object);
    zend_object_std_dtor(&intern->std);
}

void skyray_stream_init_blocking(skyray_stream_t *self, int fd, zend_object *protocol)
{
    self->blocking = 1;
    self->impl.blk.fd = fd;
    self->impl.blk.status = SKYRAY_STREAM_STATUS_OPENING;
    self->impl.blk.rw_mask = 0;
    if (protocol) {
        ZVAL_OBJ(&self->protocol, protocol);
        zval_addref_p(&self->protocol);
        skyray_protocol_on_connect_stream(&self->protocol, &self->std);
    }
}

void skyray_stream_init_nonblocking(skyray_stream_t *self, skyray_reactor_t *reactor, zend_object *protocol)
{
    self->blocking = 0;
    uv_tcp_init(&reactor->loop, &self->impl.tcp);
    self->impl.tcp.data = self;

    if (protocol) {
        ZVAL_OBJ(&self->protocol, protocol);
        zval_addref_p(&self->protocol);
        skyray_protocol_on_connect_stream(&self->protocol, &self->std);
    }
}

zend_bool skyray_stream_is_readable(skyray_stream_t *self)
{
    zend_bool result;
    if (self->blocking) {
        result = self->impl.blk.status == SKYRAY_STREAM_STATUS_OPENED && (self->impl.blk.rw_mask & SKYRAY_STREAM_READABLE);
    } else {
        result = uv_is_active((uv_handle_t *)&self->impl.tcp) && uv_is_readable((uv_stream_t *)&self->impl.tcp);
    }

    return result;
}

zend_bool skyray_stream_is_writable(skyray_stream_t *self)
{
    zend_bool result;
    if (self->blocking) {
        result = self->impl.blk.status == SKYRAY_STREAM_STATUS_OPENED && (self->impl.blk.rw_mask & SKYRAY_STREAM_WRITABLE);
    } else {
        result = uv_is_active((uv_handle_t *)&self->impl.tcp) && uv_is_writable((uv_stream_t *)&self->impl.tcp);
    }

    return result;
}

static int _skyray_stream_write_blocking(skyray_stream_t *self, zend_string *buffer)
{
    if (self->impl.blk.status == SKYRAY_STREAM_STATUS_CLOSED) {
        skyray_throw_exception("Unable to write data to closed stream.");
        return -1;
    }
    if (!(self->impl.blk.rw_mask & SKYRAY_STREAM_WRITABLE)) {
        skyray_throw_exception("The stream is not writable.");
        return -1;
    }

    int ret = write(self->impl.blk.fd, buffer->val, buffer->len);
    if (ret < 0) {
        skyray_throw_exception_from_errno(errno);
        return -1;
    } else {
        return ret;
    }
}

static void write_cb(uv_write_t *req, int status)
{
    skyray_stream_t *stream = req->handle->data;
    if (status < 0) {
        // TODO error handing
    }
    efree(req);
}

static int _skyray_stream_write_nonblocking(skyray_stream_t *self, zend_string *buffer)
{
    uv_tcp_t *tcp = &self->impl.tcp;

    if ((tcp->flags & UV_CLOSING) || (tcp->flags & UV_CLOSED)) {
        skyray_throw_exception("Unable to write to stream, the stream may already closed\n");
        return 0;
    }

    uv_write_t *req = emalloc(sizeof(uv_write_t));
    req->type = UV_WRITE;

    uv_buf_t bufs[1];
    bufs[0].base = buffer->val;
    bufs[0].len  = buffer->len;

    int result = uv_write(req, (uv_stream_t *)tcp, bufs, 1, write_cb);

    if (result < 0) {
        skyray_throw_exception_from_errno(errno);
    }

    return result >= 0;
}

int skyray_stream_write(skyray_stream_t *self, zend_string *buffer)
{
    int result = -1;
    if (self->blocking) {
        result = _skyray_stream_write_blocking(self, buffer);
    } else {
        result = _skyray_stream_write_nonblocking(self, buffer);
    }

    return result;
}

static zend_string * _skyray_stream_read_blocking(skyray_stream_t * self, zend_bool slient)
{
    if (self->impl.blk.status == SKYRAY_STREAM_STATUS_CLOSED) {
        !slient && skyray_throw_exception("Unable to read data from closed stream.");
        return NULL;
    }

    if (!(self->impl.blk.rw_mask & SKYRAY_STREAM_READABLE)) {
        !slient && skyray_throw_exception("The stream is not readable.");
        return NULL;
    }

    zend_string *buffer = zend_string_alloc(8192, 0);

    int ret = read(self->impl.blk.fd, buffer->val, 8192);
    if (ret < 0) {
        zend_string_free(buffer);
        !slient && skyray_throw_exception_from_errno(errno);
        return NULL;
    }
    buffer->len = ret;
    buffer->val[ret] = '\0';

    if (ret == 0) {
        skyray_stream_on_closed(self);
    }

    return buffer;
}

zend_string * skyray_stream_read(skyray_stream_t * self, zend_bool slient)
{
    if (self->blocking) {
        return _skyray_stream_read_blocking(self, slient);
    } else {
        !slient && skyray_throw_exception("Unable to read from non-blocking stream directly.");
        return NULL;
    }
}

void skyray_stream_on_data(skyray_stream_t *self, zend_string *buffer)
{
    if (!ZVAL_IS_NULL(&self->protocol)) {
        skyray_protocol_on_data_received(&self->protocol, buffer);
    }
}

void skyray_stream_on_opened(skyray_stream_t *self, int rw_mask)
{
    if (self->blocking) {
        self->impl.blk.status = SKYRAY_STREAM_STATUS_OPENED;
        self->impl.blk.rw_mask = rw_mask;
    }

    if (!ZVAL_IS_NULL(&self->protocol)) {
        skyray_protocol_on_stream_connected(&self->protocol);
    }
}

void skyray_stream_on_closed(skyray_stream_t *self)
{
    if (self->blocking) {
        self->impl.blk.status = SKYRAY_STREAM_STATUS_CLOSED;
        self->impl.blk.rw_mask = 0;
    }

    if (!ZVAL_IS_NULL(&self->protocol)) {
        skyray_protocol_on_stream_closed(&self->protocol);
    }

    if (!ZVAL_IS_NULL(&self->protocol)) {
        zend_object_release(Z_OBJ(self->protocol));
    }
    zend_object_release(&self->std);
}

zend_bool _skyray_stream_close_blocking(skyray_stream_t *self)
{
    if (self->impl.blk.status == SKYRAY_STREAM_STATUS_CLOSED) {
        return 1;
    }
    if (close(self->impl.blk.fd) < 0) {
        skyray_throw_exception_from_errno(errno);
        return 0;
    }
    skyray_stream_on_closed(self);

    return 1;
}

static void close_cb(uv_handle_t *uv_stream)
{
    skyray_stream_t *stream = (skyray_stream_t *)uv_stream;
    skyray_stream_on_closed(stream);
}

zend_bool _skyray_stream_close_nonblocking(skyray_stream_t *self)
{
    uv_close((uv_handle_t *)&self->impl.tcp, close_cb);
    return 1;
}

zend_bool skyray_stream_close(skyray_stream_t *self)
{
    zend_bool result;
    if (self->blocking) {
        result = _skyray_stream_close_blocking(self);
    } else {
        result = _skyray_stream_close_nonblocking(self);
    }

    return result;
}

static inline int skyray_stream_fd(skyray_stream_t *self)
{
    return self->blocking ? self->impl.blk.fd : self->impl.tcp.io_watcher.fd;
}

static inline zend_array * sockaddr_to_array(struct sockaddr_in *addr)
{
    zval result;
    char host[64] = {0};
    uv_ip4_name(addr, host, sizeof(struct sockaddr_in));

    array_init(&result);
    add_assoc_string(&result, "type", "tcp");
    add_assoc_string(&result, "host", host);
    add_assoc_long(&result, "port", ntohs(addr->sin_port));

    return Z_ARR(result);
}

zend_array *skyray_stream_get_peername(skyray_stream_t *self)
{
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(addr);
    if (getpeername(skyray_stream_fd(self), &addr, &socklen) < 0) {
        return NULL;
    }

    return sockaddr_to_array(&addr);
}

zend_array *skyray_stream_get_sockname(skyray_stream_t *self)
{
    struct sockaddr_in addr;
    socklen_t socklen = sizeof(addr);
    if (getsockname(skyray_stream_fd(self), &addr, &socklen) < 0) {
        return NULL;
    }

    return sockaddr_to_array(&addr);
}

SKYRAY_METHOD(stream, __construct)
{

}

SKYRAY_METHOD(stream, write)
{
    zend_string *data = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &data) == FAILURE) {
        return;
    }

    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));
    int written = skyray_stream_write(intern, data);
    RETURN_BOOL(written != -1);
}

SKYRAY_METHOD(stream, read)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    zend_string *buffer = skyray_stream_read(intern, 0);

    if (!buffer) {
        RETURN_NULL();
    }

    RETURN_STR(buffer);
}

SKYRAY_METHOD(stream, close)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    skyray_stream_close(intern);

    RETURN_TRUE;
}

SKYRAY_METHOD(stream, isReadable)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    RETURN_BOOL(skyray_stream_is_readable(intern));
}

SKYRAY_METHOD(stream, isWritable)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    RETURN_BOOL(skyray_stream_is_writable(intern));
}

SKYRAY_METHOD(stream, isBlocking)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    RETURN_BOOL(intern->blocking);
}

SKYRAY_METHOD(stream, getPeerName)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    zend_array *name = skyray_stream_get_peername(intern);
    if (!name) {
        RETURN_FALSE;
    } else {
        ZVAL_ARR(return_value, name);
    }
}

SKYRAY_METHOD(stream, getSockName)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    zend_array *name = skyray_stream_get_sockname(intern);
    if (!name) {
        RETURN_FALSE;
    } else {
        ZVAL_ARR(return_value, name);
    }
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_write, 0, 0, 1)
    ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(stream, __construct, arginfo_empty, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, write, arginfo_write, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, read, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, close, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, isReadable, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, isWritable, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, isBlocking, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, getPeerName, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, getSockName, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(stream)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\core\\Stream", class_methods);
    skyray_ce_Stream = zend_register_internal_class(&ce);
    skyray_ce_Stream->create_object = skyray_stream_object_new;

    memcpy(&skyray_handler_Stream, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_Stream.offset = XtOffsetOf(skyray_stream_t, std);
    skyray_handler_Stream.free_obj = skyray_stream_object_free;

    return SUCCESS;
}

