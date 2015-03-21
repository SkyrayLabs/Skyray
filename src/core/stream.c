/*
 * stream.c
 *
 *  Created on: Mar 15, 2015
 *      Author: hujin
 */

#include "stream.h"


zend_object_handlers skyray_handler_Stream;

zend_object * skyray_stream_object_new(zend_class_entry *ce)
{
    skyray_stream_t *intern;
    intern = ecalloc(1, sizeof(skyray_stream_t) + zend_object_properties_size(ce));
    intern->fd = 0;

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



SKYRAY_METHOD(stream, __construct)
{

}

SKYRAY_METHOD(stream, write)
{
    zend_string *data = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &data) == FAILURE) {
        return;
    }

    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    if (!intern->writable) {
        skyray_throw_exception("The stream is not writable.");
        RETURN_FALSE;
    }

    int ret = write(intern->fd, data->val, data->len);
    if (ret < 0) {
        skyray_throw_exception_from_errno(errno);
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

SKYRAY_METHOD(stream, read)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    if (!intern->readable) {
        skyray_throw_exception("The stream is not readable.");
        RETURN_FALSE;
    }

    zend_string *buffer = zend_string_alloc(8192, 0);

    int ret = read(intern->fd, buffer->val, 8192);
    if (ret < 0) {
        skyray_throw_exception_from_errno(errno);
        RETURN_FALSE;
    }
    buffer->len = ret;
    buffer->val[ret] = '\0';
    RETURN_STR(buffer);
}

SKYRAY_METHOD(stream, close)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }
    skyray_stream_t * intern = skyray_stream_from_obj(Z_OBJ_P(getThis()));

    if (close(intern->fd) < 0) {
        skyray_throw_exception_from_errno(errno);
        RETURN_FALSE;
    }
    RETURN_TRUE;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, callable)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_write, 0, 0, 1)
    ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()


static const zend_function_entry class_methods[] = {
    SKYRAY_ME(stream, __construct, arginfo___construct, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, write, arginfo_write, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, read, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    SKYRAY_ME(stream, close, arginfo_empty, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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

