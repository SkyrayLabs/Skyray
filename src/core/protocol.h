/*
 * protocol.h
 *
 *  Created on: Mar 27, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_PROTOCOL_H_
#define SRC_CORE_PROTOCOL_H_

#include "src/skyray.h"

void skyray_protocol_on_connect_stream(zval *protocol, zend_object *stream);
void skyray_protocol_on_stream_connected(zval *protocol);
void skyray_protocol_on_data_received(zval *protocol, zend_string *data);
void protocol_on_stream_closed(zval *protocol);
zend_object * skyray_protocol_create_from_factory(zval *creator);

#endif /* SRC_CORE_PROTOCOL_H_ */
