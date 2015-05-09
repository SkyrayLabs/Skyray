/*
 * defer.h
 *
 *  Created on: May 2, 2015
 *      Author: hujin
 */

#ifndef SRC_DEFER_DEFER_H_
#define SRC_DEFER_DEFER_H_


#include "src/skyray.h"

extern zend_class_entry *skyray_ce_PromiseInterface;
extern zend_class_entry *skyray_ce_Promise;
extern zend_class_entry *skyray_ce_FulfilledPromise;
extern zend_class_entry *skyray_ce_RejectedPromise;
extern zend_class_entry *skyray_ce_LazyPromise;
extern zend_class_entry *skyray_ce_Deferred;

typedef struct _skyray_promise {
    zval result;
    zend_llist on_fulfilled;
    zend_llist on_rejcted;
    zend_llist on_notify;
    zend_object std;
}skyray_promise_t;

typedef skyray_promise_t skyray_fulfilled_promise_t;
typedef skyray_promise_t skyray_rejected_promise_t;

typedef struct _skyray_deferred {
    zend_object *promise;
    zend_object std;
}skyray_deferred_t;

static inline skyray_promise_t * skyray_promise_from_obj(zend_object *obj) {
    return (skyray_promise_t*)((char*)(obj) - XtOffsetOf(skyray_promise_t, std));
}

static inline skyray_fulfilled_promise_t * skyray_fulfilled_promise_from_obj(zend_object *obj) {
    return (skyray_fulfilled_promise_t*)((char*)(obj) - XtOffsetOf(skyray_fulfilled_promise_t, std));
}

static inline skyray_rejected_promise_t * skyray_rejected_promise_from_obj(zend_object *obj) {
    return (skyray_rejected_promise_t*)((char*)(obj) - XtOffsetOf(skyray_rejected_promise_t, std));
}

void skyray_promise_object_init(skyray_promise_t *self, zend_class_entry *ce);

static inline zend_bool skyray_is_promise_instance(zval *value)
{
    return Z_TYPE_P(value) == IS_OBJECT && instanceof_function(Z_OBJCE_P(value), skyray_ce_Promise);
}

static inline zend_bool skyray_is_resolved_promise(zval *value)
{
    return Z_TYPE_P(value) == IS_OBJECT && (
            instanceof_function(Z_OBJCE_P(value), skyray_ce_RejectedPromise)
            || instanceof_function(Z_OBJCE_P(value), skyray_ce_FulfilledPromise)
    );
}
skyray_fulfilled_promise_t * skyray_fulfilled_promise_new(zval *value, zend_bool is_copy_required);

SKYRAY_MINIT_FUNCTION(deferred);
SKYRAY_MINIT_FUNCTION(promise);
SKYRAY_MINIT_FUNCTION(fulfilled_promise);

#endif /* SRC_DEFER_DEFER_H_ */
