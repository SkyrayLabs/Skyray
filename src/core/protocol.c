/*
 * protocol.c
 *
 *  Created on: Mar 27, 2015
 *      Author: hujin
 */

#include "protocol.h"

void skyray_protocol_on_connect_stream(zval *protocol, zend_object *stream)
{
    zval function_name;
    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("connectStream"), 0));

    zval retval;
    zval params[1];

    ZVAL_OBJ(&params[0], stream);

    call_user_function(EG(function_table), protocol, &function_name, &retval, 1, params);
    zval_dtor(&function_name);
}

void skyray_protocol_on_stream_connected(zval *protocol)
{
    zval function_name;
    zval retval;

    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("streamConnected"), 0));

    call_user_function(EG(function_table), protocol, &function_name, &retval, 0, NULL);
    zval_dtor(&function_name);
}

void skyray_protocol_on_data_received(zval *protocol, zend_string *data)
{
    zval function_name;
    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("dataReceived"), 0));

    zval retval;
    zval params[1];

    ZVAL_STR(&params[0], data);


    call_user_function(EG(function_table), protocol, &function_name, &retval, 1, params);
    zval_dtor(&function_name);
}

void skyray_protocol_on_stream_closed(zval *protocol)
{
    zval function_name;
    zval retval;

    ZVAL_STR(&function_name, zend_string_init(ZEND_STRL("streamClosed"), 0));

    call_user_function(EG(function_table), protocol, &function_name, &retval, 0, NULL);
    zval_dtor(&function_name);
}


zend_object * skyray_protocol_create_from_factory(zval *creator)
{
    zval protocol;
    call_user_function(EG(function_table), NULL, creator, &protocol, 0, NULL);

    if (!instanceof_function(Z_OBJCE(protocol), skyray_ce_ProtocolInterface)) {
        skyray_throw_exception("The protocol created by $protocolCreator must be instance of 'skyray\\core\\ProtocolInterface'");
        return NULL;
    }

    return Z_OBJ_P(&protocol);
}