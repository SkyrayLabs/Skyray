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

zend_class_entry * skyray_ce_SkyrayException;
zend_class_entry * skyray_ce_ProtocolInterface;

#define SKYRAY_MN(name) zim_skyray_##name
#define SKYRAY_METHOD(classname, name) ZEND_NAMED_FUNCTION(SKYRAY_MN(classname##_##name))

#define SKYRAY_ME(classname, name, arg_info, flags)    ZEND_FENTRY(name, SKYRAY_MN(classname##_##name), arg_info, flags)
#define SKYRAY_ABSTRACT_ME ZEND_ABSTRACT_ME

#define SKYRAY_CAST(variable, type)  (type)(variable)

#define skyray_throw_exception(format, ...)  \
    zend_throw_exception_ex(skyray_ce_SkyrayException, 0, format, ## __VA_ARGS__)

#define skyray_throw_exception_from_errno(errno)  \
    zend_throw_exception_ex(skyray_ce_SkyrayException, 0, "[%d] %s", errno, strerror(errno))


ZEND_BEGIN_ARG_INFO_EX(arginfo_empty, 0, 0, 0)
ZEND_END_ARG_INFO()

#endif /* SKYRAY_H_ */
