#include "skyray.h"

#include "core/reactor.h"
#include "core/stream.h"
#include "processing/process.h"

zend_class_entry * skyray_ce_BaseException;
zend_class_entry * skyray_ce_InvalidParamException;
zend_class_entry * skyray_ce_InvalidConfigException;
zend_class_entry * skyray_ce_ProtocolInterface;


void skyray_handle_uncaught_exception(zend_object *old_exception)
{
    EG(exception) = NULL;
    if (Z_TYPE(EG(user_exception_handler)) != IS_UNDEF) {
        zval orig_user_exception_handler;
        zval params[1], retval2;
        ZVAL_OBJ(&params[0], old_exception);
        ZVAL_COPY_VALUE(&orig_user_exception_handler, &EG(user_exception_handler));
        ZVAL_UNDEF(&retval2);
        if (call_user_function_ex(CG(function_table), NULL, &orig_user_exception_handler, &retval2, 1, params, 1, NULL) == SUCCESS) {
            zval_ptr_dtor(&retval2);
            OBJ_RELEASE(old_exception);
            if (EG(exception)) {
                zend_exception_error(EG(exception), E_ERROR);
            }
        } else {
            zend_exception_error(old_exception, E_ERROR);
        }
    } else {
        zend_exception_error(old_exception, E_ERROR);
    }
}


PHP_MINIT_FUNCTION(skyray_interfaces)
{
    zend_class_entry ce;

    ZEND_BEGIN_ARG_INFO_EX(arginfo_dataReceived, 0, 0, 1)
        ZEND_ARG_INFO(0, data)
    ZEND_END_ARG_INFO()

    ZEND_BEGIN_ARG_INFO_EX(arginfo_connectStream, 0, 0, 1)
        ZEND_ARG_INFO(0, stream)
    ZEND_END_ARG_INFO()

    static const zend_function_entry class_methods[] = {
        SKYRAY_ABSTRACT_ME(ProtocolInterface, connectStream, arginfo_connectStream)
        SKYRAY_ABSTRACT_ME(ProtocolInterface, streamConnected, arginfo_empty)
        SKYRAY_ABSTRACT_ME(ProtocolInterface, dataReceived, arginfo_dataReceived)
        SKYRAY_ABSTRACT_ME(ProtocolInterface, streamClosed, arginfo_empty)
        PHP_FE_END
    };


    INIT_CLASS_ENTRY(ce, "skyray\\core\\StreamProtocolInterface", class_methods);
    skyray_ce_ProtocolInterface = zend_register_internal_interface(&ce);

    return SUCCESS;
}

PHP_MINIT_FUNCTION(skyray_exceptions)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\BaseException", NULL);
    skyray_ce_BaseException = zend_register_internal_class_ex(&ce, zend_exception_get_default());

    INIT_CLASS_ENTRY(ce, "skyray\\InvalidParamException", NULL);
    skyray_ce_InvalidParamException = zend_register_internal_class_ex(&ce, skyray_ce_BaseException);

    INIT_CLASS_ENTRY(ce, "skyray\\InvalidConfigException", NULL);
    skyray_ce_InvalidConfigException = zend_register_internal_class_ex(&ce, skyray_ce_BaseException);

    return SUCCESS;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(skyray)
{
    ZEND_TSRMLS_CACHE_UPDATE();

    PHP_MINIT(skyray_exceptions)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_interfaces)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_reactor)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_timer)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(stream_client)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(stream_server)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(stream)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(process)(INIT_FUNC_ARGS_PASSTHRU);

    PHP_MINIT(skyray_http_message)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_http_request)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_http_response)(INIT_FUNC_ARGS_PASSTHRU);
    PHP_MINIT(skyray_http_protocol)(INIT_FUNC_ARGS_PASSTHRU);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(skyray)
{

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(skyray)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "skyray support", "enabled");
    php_info_print_table_end();
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_hash_1, 0, 0, 2)
    ZEND_ARG_INFO(0, algo)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()


/* {{{ hash_module_entry
 */
zend_module_entry skyray_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "skyray",
    NULL,
    PHP_MINIT(skyray),
    PHP_MSHUTDOWN(skyray),
    NULL, /* RINIT */
    NULL, /* RSHUTDOWN */
    PHP_MINFO(skyray),
#if ZEND_MODULE_API_NO >= 20010901
    "0.0.1", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_SKYRAY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(skyray)
#endif
