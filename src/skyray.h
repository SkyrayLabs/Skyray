/*
 * skyray.h
 *
 *  Created on: Mar 14, 2015
 *      Author: hujin
 */

#ifndef SKYRAY_H_
#define SKYRAY_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <netinet/in.h>
#include "uv.h"

#ifdef PHP_WIN32
#   define PHP_SKYRAY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#   define PHP_SKYRAY_API __attribute__ ((visibility("default")))
#else
#   define PHP_SKYRAY_API
#endif


#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_class_entry * skyray_ce_SkyrayException;
extern zend_class_entry * skyray_ce_ProtocolInterface;

#define SKYRAY_MN(name) zim_skyray_##name
#define SKYRAY_METHOD(classname, name) ZEND_NAMED_FUNCTION(SKYRAY_MN(classname##_##name))

#define SKYRAY_ME(classname, name, arg_info, flags)    ZEND_FENTRY(name, SKYRAY_MN(classname##_##name), arg_info, flags)
#define SKYRAY_ABSTRACT_ME ZEND_ABSTRACT_ME

#define SKYRAY_CAST(variable, type)  (type)(variable)

#define skyray_throw_exception(format, ...)  \
    zend_throw_exception_ex(skyray_ce_SkyrayException, 0, format, ## __VA_ARGS__)

static inline int skyray_throw_exception_from_errno(int errcode)
{
    if (errcode > 0) {
        zend_throw_exception_ex(skyray_ce_SkyrayException, 0, "[%d] %s", errcode, strerror(errcode));
    } else {
        zend_throw_exception_ex(skyray_ce_SkyrayException, 0, "[%s] %s", uv_err_name(errcode), uv_strerror(errcode));
    }

    return 1;
}


void skyray_handle_uncaught_exception(zend_object *old_exception);


ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, 0, 0)
ZEND_END_ARG_INFO()

/* constants copied from libuv source files */
enum {
  UV_CLOSING              = 0x01,   /* uv_close() called but not finished. */
  UV_CLOSED               = 0x02,   /* close(2) finished. */
  UV_STREAM_READING       = 0x04,   /* uv_read_start() called. */
  UV_STREAM_SHUTTING      = 0x08,   /* uv_shutdown() called but not complete. */
  UV_STREAM_SHUT          = 0x10,   /* Write side closed. */
  UV_STREAM_READABLE      = 0x20,   /* The stream is readable */
  UV_STREAM_WRITABLE      = 0x40,   /* The stream is writable */
  UV_STREAM_BLOCKING      = 0x80,   /* Synchronous writes. */
  UV_STREAM_READ_PARTIAL  = 0x100,  /* read(2) read less than requested. */
  UV_STREAM_READ_EOF      = 0x200,  /* read(2) read EOF. */
  UV_TCP_NODELAY          = 0x400,  /* Disable Nagle. */
  UV_TCP_KEEPALIVE        = 0x800,  /* Turn on keep-alive. */
  UV_TCP_SINGLE_ACCEPT    = 0x1000, /* Only accept() when idle. */
  UV_HANDLE_IPV6          = 0x10000 /* Handle is bound to a IPv6 socket. */
};

#endif /* SKYRAY_H_ */
