/*
 * reactor.h
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_REACTOR_H_
#define SRC_CORE_REACTOR_H_

#include "src/skyray.h"
#include "uv.h"

extern zend_class_entry *skyray_ce_Reactor;


typedef struct _skyray_reactor {
    uv_loop_t loop;
    zend_object std;
}skyray_reactor_t;

PHP_MINIT_FUNCTION(skyray_reactor);

#endif /* SRC_CORE_REACTOR_H_ */
