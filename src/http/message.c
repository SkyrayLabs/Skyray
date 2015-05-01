/*
 * message.c
 *
 *  Created on: Apr 15, 2015
 *      Author: hujin
 */

#include "http.h"

zend_class_entry *skyray_ce_HttpMessage;
zend_object_handlers skyray_handler_HttpMessage;

zend_string *intern_str_content_type; // Content-Type
zend_string *intern_str_content_length; // Content-Length
zend_string *intern_str_application_json; // application/json
zend_string *intern_str_text_plain; // text/plain
zend_string *intern_str_connection; // Connection
zend_string *intern_str_server; // Server
zend_string *intern_str_date; // Date
zend_string *intern_str_cookie; // "Cookie"


static inline skyray_http_message_t *skyray_http_message_from_obj(zend_object *obj) {
    return (skyray_http_message_t*)(obj);
}

void skyray_http_message_init(skyray_http_message_t *self, zend_class_entry *ce)
{
    self->version_major = self->version_minor = 0;

    zend_hash_init(&self->headers, 32, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_init(&self->iheaders, 32, NULL, ZVAL_PTR_DTOR, 0);

    ZVAL_UNDEF(&self->body);
    ZVAL_UNDEF(&self->raw_body);

    zend_object_std_init(&self->std, ce);
    object_properties_init(&self->std, ce);
}

zend_object * skyray_http_message_object_new(zend_class_entry *ce)
{
    skyray_http_message_t *intern;
    intern = ecalloc(1, sizeof(skyray_http_message_t) + zend_object_properties_size(ce));

    skyray_http_message_init(intern, ce);

    intern->std.handlers = &skyray_handler_HttpMessage;
    return &intern->std;
}

void skyray_http_message_object_free(zend_object *object)
{
    skyray_http_message_t *intern = skyray_http_message_from_obj(object);

    zend_hash_destroy(&intern->headers);
    zend_hash_destroy(&intern->iheaders);

    zval_ptr_dtor(&intern->body);
    zval_ptr_dtor(&intern->raw_body);

    zend_object_std_dtor(&intern->std);
}

static inline void remove_old_header(zend_array *ht, zend_string *name)
{
    zend_string *key;
    zend_hash_internal_pointer_reset(ht);
    while (zend_hash_has_more_elements(ht) == SUCCESS) {
        zend_hash_get_current_key(ht, &key, NULL);

        if (zend_string_equals_ci(key, name)) {
            zend_hash_del(ht, key);
        }
        zend_hash_move_forward(ht);
    }
}

zend_bool skyray_http_message_has_header(skyray_http_message_t *self, zend_string *name)
{
    zend_bool exists;

    zend_string *lname = zend_string_tolower(name);
    exists = zend_hash_exists(&self->iheaders, lname);
    zend_string_release(lname);

    return exists;
}

zval* skyray_http_message_get_header(skyray_http_message_t *self, zend_string *name, zend_bool first)
{
    zval *found;
    zend_string *lname = zend_string_tolower(name);

    found = zend_hash_find(&self->iheaders, lname);
    zend_string_release(lname);

    if (found && first) {
        return zend_hash_index_find(Z_ARR_P(found), 0);
    }

    return found;
}


void skyray_http_message_set_header(skyray_http_message_t *self, zend_string *name, zend_string *value, zend_bool dup)
{
    zval arr;
    zend_string *iname = zend_string_tolower(name);

    array_init(&arr);
    if (dup) {
        zend_string_addref(value);
    }
    add_next_index_str(&arr, value);

    zend_hash_update(&self->iheaders, iname, &arr);
    zend_string_release(iname);

    zval_add_ref(&arr);

    remove_old_header(&self->headers, name);
    zend_hash_update(&self->headers, name, &arr);
}

void skyray_http_message_add_header(skyray_http_message_t *self, zend_string *name, zend_string *value, zend_bool dup)
{
    zval *found;
    zend_string *lname = zend_string_tolower(name);

    found = zend_hash_find(&self->iheaders, lname);
    if (found) {
        if (dup) {
            zend_string_addref(value);
        }
        remove_old_header(&self->headers, name);
        add_next_index_str(found, value);
        zval_add_ref(found);
        zend_hash_update(&self->headers, name, found);

    } else {
        skyray_http_message_set_header(self, name, value, dup);
    }

    zend_string_release(lname);
}

void skyray_http_message_remove_header(skyray_http_message_t *self, zend_string *name)
{
    zend_string *lname = zend_string_tolower(name);
    zend_hash_del(&self->iheaders, lname);

    remove_old_header(&self->headers, lname);

    zend_string_release(lname);
}

SKYRAY_METHOD(HttpMessage, getProtocolVersion)
{
    if (zend_parse_parameters_none() ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    int version = intern->version_major * 10 + intern->version_minor;

    if (intern->version_major == 1 && intern->version_minor == 0) {
        RETURN_STRING("1.0");
    } else if (intern->version_major == 1 && intern->version_minor == 1) {
        RETURN_STRING("1.1");
    } else if (intern->version_major == 1 && intern->version_minor == 0) {
        RETURN_STRING("2.0");
    }
}

SKYRAY_METHOD(HttpMessage, setProtocolVersion)
{
    zend_string *version;
    int i;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &version) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    for (i = 0 ; ; i ++) {
        if (sr_http_versions[i] == NULL) {
            break;
        }
        if (strncmp(sr_http_versions[i], version->val, 3) == 0) {
            if (i == 0 || i == 1) {
                intern->version_major = 1;
            }else {
                intern->version_major = 1;
            }

            if (i == 0 || i == 2) {
                intern->version_minor = 0;
            } else {
                intern->version_minor = 1;
            }
            break;
        }
    }

    RETURN_ZVAL(getThis(), 1, 0);
}


SKYRAY_METHOD(HttpMessage, getHeaders)
{
    skyray_http_message_t *intern;
    zval zheaders;;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    ZVAL_ARR(&zheaders, &intern->headers);
    RETURN_ZVAL(&zheaders, 1, 0);
}

SKYRAY_METHOD(HttpMessage, hasHeader)
{
    zend_string *name;
    skyray_http_message_t *intern;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) ==  FAILURE) {
        return;
    }

    intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    RETURN_BOOL(skyray_http_message_has_header(intern, name));
}

SKYRAY_METHOD(HttpMessage, getHeader)
{
    zend_string *name;
    zend_bool first = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|b", &name, &first) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    zval *header = skyray_http_message_get_header(intern, name, first);
    if (header) {
        RETURN_ZVAL(header, 1, 0);
    }

    if (!first) {
        RETURN_EMPTY_ARR();
    }
}

SKYRAY_METHOD(HttpMessage, setHeader)
{
    zend_string *name, *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &name, &value) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    skyray_http_message_set_header(intern, name, value, 1);

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpMessage, addHeader)
{
    zend_string *name, *value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &name, &value) ==  FAILURE) {
        return;
    }
    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    skyray_http_message_add_header(intern, name, value, 1);

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpMessage, removeHeader)
{
    zend_string *name;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    skyray_http_message_remove_header(intern, name);

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpMessage, getBody)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    if (Z_TYPE_P(&intern->body) != IS_UNDEF) {
        RETURN_ZVAL(&intern->body, 1, 0);
    }
}

SKYRAY_METHOD(HttpMessage, setBody)
{
    zval *body;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &body) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));
    zval_ptr_dtor(&intern->body);
    ZVAL_COPY(&intern->body, body);

    RETURN_ZVAL(getThis(), 1, 0);
}

SKYRAY_METHOD(HttpMessage, getRawBody)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    if (Z_TYPE_P(&intern->raw_body) != IS_UNDEF) {
        RETURN_ZVAL(&intern->raw_body, 1, 0);
    }
}

SKYRAY_METHOD(HttpMessage, setRawBody)
{
    zend_string *body;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &body) ==  FAILURE) {
        return;
    }

    skyray_http_message_t *intern = skyray_http_message_from_obj(Z_OBJ_P(getThis()));

    zval_dtor(&intern->raw_body);
    ZVAL_STR(&intern->raw_body, body);
    zval_add_ref(&intern->raw_body);

    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_setProtocolVersion, 0, 0, 1)
    ZEND_ARG_INFO(0, version)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getHeader, 0, 0, 1)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, first)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_name_value, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_body, 0, 0, 1)
    ZEND_ARG_INFO(0, body)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_rawBody, 0, 0, 1)
    ZEND_ARG_INFO(0, rawBody)
ZEND_END_ARG_INFO()

static const zend_function_entry class_methods[] = {
    SKYRAY_ME(HttpMessage, getProtocolVersion, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, setProtocolVersion, arginfo_setProtocolVersion, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, getHeaders, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, hasHeader, arginfo_name, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, getHeader, arginfo_getHeader, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, setHeader, arginfo_name_value, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, addHeader, arginfo_name_value, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, removeHeader, arginfo_name, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, getBody, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, setBody, arginfo_set_body, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, getRawBody, arginfo_empty, ZEND_ACC_PUBLIC)
    SKYRAY_ME(HttpMessage, setRawBody, arginfo_set_rawBody, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(skyray_http_message)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "skyray\\http\\Message", class_methods);
    skyray_ce_HttpMessage = zend_register_internal_class_ex(&ce, skyray_ce_Object);
    skyray_ce_HttpMessage->create_object = skyray_http_message_object_new;

    memcpy(&skyray_handler_HttpMessage, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    skyray_handler_HttpMessage.free_obj = skyray_http_message_object_free;

    intern_str_application_json = zend_new_interned_string(zend_string_init(ZEND_STRL("application/json"), 1));
    intern_str_text_plain       = zend_new_interned_string(zend_string_init(ZEND_STRL("text/plain"), 1));
    intern_str_content_type     = zend_new_interned_string(zend_string_init(ZEND_STRL("Content-Type"), 1));
    intern_str_content_length   = zend_new_interned_string(zend_string_init(ZEND_STRL("Content-Length"), 1));
    intern_str_connection       = zend_new_interned_string(zend_string_init(ZEND_STRL("Connection"), 1));
    intern_str_server           = zend_new_interned_string(zend_string_init(ZEND_STRL("Server"), 1));
    intern_str_date             = zend_new_interned_string(zend_string_init(ZEND_STRL("Date"), 1));
    intern_str_cookie           = zend_new_interned_string(zend_string_init(ZEND_STRL("Cookie"), 0));

    return SUCCESS;
}
