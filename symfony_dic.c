/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Julien Pauli <jpauli@php.net>                                |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_symfony_dic.h"
#include "Zend/zend_exceptions.h"

static zend_class_entry *dic_optimizer_ce;

ZEND_BEGIN_ARG_INFO(dic_optimizer_get_arg_infos, 0)
ZEND_ARG_INFO(0, id)
ZEND_ARG_INFO(0, invalidBehavior)
ZEND_END_ARG_INFO()

zend_module_entry symfony_dic_module_entry = {
	STANDARD_MODULE_HEADER,
	"symfony_dic",
	NULL,
	PHP_MINIT(symfony_dic),
	NULL,
	PHP_RINIT(symfony_dic),
	NULL,
	PHP_MINFO(symfony_dic),
	PHP_SYMFONY_DIC_VERSION,
	PHP_MODULE_GLOBALS(symfony_dic),
	PHP_GINIT(symfony_dic),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_SYMFONY_DIC
ZEND_GET_MODULE(symfony_dic)
#endif

PHP_GINIT_FUNCTION(symfony_dic)
{
	memset(&symfony_dic_globals, 0, sizeof(symfony_dic_globals));
}

static void dic_optimizer_get_handler(INTERNAL_FUNCTION_PARAMETERS)
{
	char *id, should_free = 1;
	zend_ulong id_hash;
	int id_len, i;
	long oninvalid, exception_on_invalid_reference_const;
	zval **found_service, **alias, **method_in_map = NULL, *this_services, *this_aliases, *this_methodMap, *this_loading;
	zend_function *method = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &id, &id_len, &oninvalid) == FAILURE) {
		return;
	}

	if (strcasecmp(id, "service_container") == 0) {
		RETURN_THIS;
	}

	this_services  = zend_read_property(Z_OBJCE_P(getThis()), getThis(), SYMFONY_DIC_TOKEN_SERVICES_KEY, sizeof(SYMFONY_DIC_TOKEN_SERVICES_KEY) - 1, 0 TSRMLS_CC);
	this_aliases   = zend_read_property(Z_OBJCE_P(getThis()), getThis(), SYMFONY_DIC_TOKEN_ALIASES_KEY, sizeof(SYMFONY_DIC_TOKEN_ALIASES_KEY) - 1, 0 TSRMLS_CC);
	this_loading   = zend_read_property(Z_OBJCE_P(getThis()), getThis(), SYMFONY_DIC_TOKEN_LOADING_KEY, sizeof(SYMFONY_DIC_TOKEN_LOADING_KEY) - 1, 0 TSRMLS_CC);
	this_methodMap = zend_read_property(Z_OBJCE_P(getThis()), getThis(), SYMFONY_DIC_TOKEN_METHODMAP_KEY, sizeof(SYMFONY_DIC_TOKEN_METHODMAP_KEY) - 1, 0 TSRMLS_CC);

	if (!SYMFONY_DIC_G(cache_done)) {
		zval invalidBehavior;
		zend_class_entry **exception_ce;

		zend_get_constant_ex("self::EXCEPTION_ON_INVALID_REFERENCE", sizeof("self::EXCEPTION_ON_INVALID_REFERENCE") - 1, &invalidBehavior, Z_OBJCE_P(getThis()), 0  TSRMLS_CC);
		SYMFONY_DIC_G(invalid_behavior) = Z_LVAL(invalidBehavior);

		if (FAILURE == zend_lookup_class(SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP, sizeof(SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP) - 1, &exception_ce TSRMLS_CC)) {
			zend_error_noreturn(E_ERROR, "Class %s not found", SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP);
		}
		SYMFONY_DIC_G(ServiceCircularReferenceException) = *exception_ce;
		if (FAILURE == zend_lookup_class(SYMFONY_DIC_TOKEN_SERVICE_NOT_FOUND_EXCEPTION_KEY_UP, sizeof(SYMFONY_DIC_TOKEN_SERVICE_NOT_FOUND_EXCEPTION_KEY_UP) - 1, &exception_ce TSRMLS_CC)) {
			zend_error_noreturn(E_ERROR, "Class %s not found", SYMFONY_DIC_TOKEN_SERVICE_NOT_FOUND_EXCEPTION_KEY_UP);
		}
		SYMFONY_DIC_G(ServiceNotFoundException) = *exception_ce;
		if (FAILURE == zend_lookup_class(SYMFONY_DIC_TOKEN_INACTIVE_SCOPE_EXCEPTION_KEY_UP, sizeof(SYMFONY_DIC_TOKEN_INACTIVE_SCOPE_EXCEPTION_KEY_UP) - 1, &exception_ce TSRMLS_CC)) {
			zend_error_noreturn(E_ERROR, "Class %s not found", SYMFONY_DIC_TOKEN_INACTIVE_SCOPE_EXCEPTION_KEY_UP);
		}
		SYMFONY_DIC_G(InactiveScopeException) = *exception_ce;

		SYMFONY_DIC_G(cache_done) = 1;
	}

	exception_on_invalid_reference_const = SYMFONY_DIC_G(invalid_behavior);

	if (ZEND_NUM_ARGS() <= 1) {
		oninvalid = exception_on_invalid_reference_const;
	}

	if (IS_INTERNED(id)) {
		id_hash = INTERNED_HASH(id);
	} else {
		id_hash = zend_inline_hash_func(id, id_len + 1);
	}

	id = estrndup(id, id_len); /* zend_str_tolower will change it otherwise */

	for (i = 0; i <= 1; i++, zend_str_tolower(id, id_len), id_hash = zend_inline_hash_func(id, id_len + 1)) {
		if (zend_hash_quick_find(Z_ARRVAL_P(this_aliases), id, id_len + 1, id_hash, (void **)&alias) == SUCCESS) {
			should_free = 0;
			efree(id);
			id      = Z_STRVAL_PP(alias);
			id_len  = Z_STRLEN_PP(alias);
			id_hash = zend_inline_hash_func(id, id_len + 1);
		}
		if (zend_hash_quick_find(Z_ARRVAL_P(this_services), id, id_len + 1, id_hash, (void **)&found_service) == SUCCESS) {
			RETVAL_ZVAL_FAST(*found_service);
			goto free_and_return;
		}
	}

	if (zend_hash_quick_exists(Z_ARRVAL_P(this_loading), id, id_len + 1, id_hash)) {
		zval *ServiceCircularReferenceException;
		ALLOC_INIT_ZVAL(ServiceCircularReferenceException); /* ctor_args */
		object_init_ex(ServiceCircularReferenceException, SYMFONY_DIC_G(ServiceCircularReferenceException));
		zend_throw_exception_object(ServiceCircularReferenceException TSRMLS_CC);
		goto free_and_return;
	}

	zend_hash_quick_find(Z_ARRVAL_P(this_methodMap), id, id_len + 1, id_hash, (void **)&method_in_map);

	if (!method_in_map) {
		char *new_id;

		for (i=0; i < id_len; i++) {
			if (id[i] == '_') {
				memmove(&id[i], &id[i + 1], --id_len);
			}
		}
		php_strtr(id, id_len, ".\\", "__", 2);
		id_len = spprintf(&new_id, 0, "get%sservice", id);
		efree(id);
		id      = new_id;
		id_hash = zend_inline_hash_func(id, id_len + 1);

		zend_hash_quick_find(&Z_OBJCE_P(getThis())->function_table, id, id_len + 1, id_hash, (void **)&method);
		if (!method) {
			if (oninvalid == exception_on_invalid_reference_const) {
				zval *ServiceNotFoundException;
				ALLOC_INIT_ZVAL(ServiceNotFoundException);
				object_init_ex(ServiceNotFoundException, SYMFONY_DIC_G(ServiceNotFoundException));
				zend_throw_exception_object(ServiceNotFoundException TSRMLS_CC); /* ctor_args */
			}
			goto free_and_return;
		}
	} else {
		char *method_name_lc;
		method_name_lc = zend_str_tolower_dup(Z_STRVAL_PP(method_in_map), Z_STRLEN_PP(method_in_map));
		zend_hash_find(&Z_OBJCE_P(getThis())->function_table, method_name_lc, Z_STRLEN_PP(method_in_map) + 1, (void **)&method);
		efree(method_name_lc);
	}

	zend_fcall_info fci = {0};
	zend_fcall_info_cache fcic = {0};
	zval *loading, *result;

	ALLOC_INIT_ZVAL(loading); ZVAL_BOOL(loading, 1);
	zend_hash_quick_add(Z_ARRVAL_P(this_loading), id, id_len + 1, id_hash, &loading, sizeof(zval *), NULL);

	fcic.called_scope = Z_OBJCE_P(getThis());
	fcic.calling_scope = Z_OBJCE_P(getThis());
	fcic.function_handler = method;
	fcic.initialized = 1;
	fcic.object_ptr = getThis();

	fci.retval_ptr_ptr = &result;
	fci.size = sizeof(zend_fcall_info);

	zend_call_function(&fci, &fcic TSRMLS_CC);

	zend_hash_quick_del(Z_ARRVAL_P(this_loading), id, id_len + 1, id_hash);

	if (!EG(exception)) {
		RETVAL_ZVAL_FAST(result);
	} else {
		zend_hash_quick_del(Z_ARRVAL_P(this_services), id, id_len, id_hash);

		if (instanceof_function(Z_OBJCE_P(EG(exception)), SYMFONY_DIC_G(InactiveScopeException) TSRMLS_CC) && oninvalid == exception_on_invalid_reference_const) {
			EG(exception) = NULL;
		}
	}
	zval_ptr_dtor(&result);
free_and_return:
	if (should_free) { efree(id); }
	return;
}

static int dic_optimizer_interface_gets_implemented(zend_class_entry *iface, zend_class_entry *ce TSRMLS_DC)
{
	zend_function *get;

	if (zend_hash_find(&ce->function_table, "get", sizeof("get"), (void **)&get) == FAILURE) {
		return FAILURE;
	}

	zend_function_dtor(get);

	get->type = ZEND_INTERNAL_FUNCTION;
	get->internal_function.handler = dic_optimizer_get_handler;
	get->internal_function.module  = &symfony_dic_module_entry;
	get->internal_function.arg_info = (zend_arg_info *)dic_optimizer_get_arg_infos;

	return SUCCESS;
}

PHP_MINIT_FUNCTION(symfony_dic)
{
	zend_class_entry dic_optimizer;
	INIT_NS_CLASS_ENTRY(dic_optimizer, "Symfony\\Component\\DependencyInjection", "ContainerOptimizerInterface", NULL);

	dic_optimizer_ce = zend_register_internal_interface(&dic_optimizer TSRMLS_CC);
	dic_optimizer_ce->interface_gets_implemented = dic_optimizer_interface_gets_implemented;

	return SUCCESS;
}

PHP_RINIT_FUNCTION(symfony_dic)
{
	SYMFONY_DIC_G(cache_done) = 0;
	SYMFONY_DIC_G(ServiceCircularReferenceException) =
	SYMFONY_DIC_G(ServiceNotFoundException) =
	SYMFONY_DIC_G(InactiveScopeException) = NULL;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(symfony_dic)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Symfony DIC optimizer support", "enabled");
	php_info_print_table_end();
	php_info_print_box_start(0);
	php_write("Symfony DIC optimizer by Julien Pauli", strlen("Symfony DIC optimizer by Julien Pauli") TSRMLS_CC);
	php_info_print_box_end();
}
