/*
 * timer.c
 *
 *  Created on: Mar 17, 2015
 *      Author: hujin
 */

#ifndef SRC_CORE_TIMER_C_
#define SRC_CORE_TIMER_C_


#include "src/skyray.h"
#include "uv.h"

extern zend_class_entry *skyray_ce_Timer;

typedef struct _skyray_timer {
    uv_timer_t timer;
    zval callback;
    zend_object std;
}skyray_timer_t;

static inline skyray_timer_t *skyray_timer_from_obj(zend_object *obj) {
    return (skyray_timer_t*)((char*)(obj) - XtOffsetOf(skyray_timer_t, std));
}

PHP_MINIT_FUNCTION(skyray_timer);


#endif /* SRC_CORE_TIMER_C_ */
