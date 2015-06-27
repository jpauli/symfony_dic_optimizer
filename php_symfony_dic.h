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

#ifndef PHP_SYMFONY_DIC_H
#define PHP_SYMFONY_DIC_H

extern zend_module_entry symfony_dic_module_entry;
#define phpext_symfony_dic_ptr &symfony_dic_module_entry
zend_module_entry *get_module(void);

#define PHP_SYMFONY_DIC_VERSION "0.1.0"

#ifdef PHP_WIN32
#	define PHP_SYMFONY_DIC_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SYMFONY_DIC_API __attribute__ ((visibility("default")))
#else
#	define PHP_SYMFONY_DIC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef RETVAL_ZVAL_FAST
#define RETVAL_ZVAL_FAST(z) do {      \
	zval *_z = (z);                   \
	if (Z_ISREF_P(_z)) {              \
		RETVAL_ZVAL(_z, 1, 0);        \
	} else {                          \
		zval_ptr_dtor(&return_value); \
		Z_ADDREF_P(_z);               \
		*return_value_ptr = _z;       \
	}                                 \
} while (0)
#endif

ZEND_BEGIN_MODULE_GLOBALS(symfony_dic)
	char cache_done;
	long invalid_behavior;
	zend_class_entry *ServiceCircularReferenceException;
	zend_class_entry *ServiceNotFoundException;
	zend_class_entry *InactiveScopeException;
ZEND_END_MODULE_GLOBALS(symfony_dic)

ZEND_DECLARE_MODULE_GLOBALS(symfony_dic);

PHP_GINIT_FUNCTION(symfony_dic);
PHP_GSHUTDOWN_FUNCTION(symfony_dic);
PHP_MINIT_FUNCTION(symfony_dic);
PHP_MSHUTDOWN_FUNCTION(symfony_dic);
PHP_RINIT_FUNCTION(symfony_dic);
PHP_RSHUTDOWN_FUNCTION(symfony_dic);
PHP_MINFO_FUNCTION(symfony_dic);

#define THROW_CIRCULAR_REF_EXCEPTION do { \
	ALLOC_INIT_ZVAL(ServiceCircularReferenceException); /* ctor_args */ \
	zend_lookup_class(SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP, sizeof(SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP) - 1, &exception_ce); \
	object_init_ex(ServiceCircularReferenceException, *exception_ce); \
	zend_throw_exception_object(ServiceCircularReferenceException TSRMLS_CC); \
	return; \
} while (0);


#define RETURN_THIS RETURN_ZVAL(getThis(), 1, 0)
#define SYMFONY_DIC_TOKEN_ALIASES_KEY   "aliases"
#define SYMFONY_DIC_TOKEN_SERVICES_KEY  "services"
#define SYMFONY_DIC_TOKEN_LOADING_KEY   "loading"
#define SYMFONY_DIC_TOKEN_METHODMAP_KEY "methodMap"

/* lowercase class names please */
#define SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY  "symfony\\component\\dependencyinjection\\exception\\servicecircularreferenceexception"
#define SYMFONY_DIC_TOKEN_SERVICE_CIRCULAR_REFERENCE_EXCEPTION_KEY_UP  "Symfony\\Component\\DependencyInjection\\Exception\\ServiceCircularReferenceException"
#define SYMFONY_DIC_TOKEN_SERVICE_NOT_FOUND_EXCEPTION_KEY           "symfony\\component\\dependencyinjection\\exception\\servicenotfoundexception"
#define SYMFONY_DIC_TOKEN_SERVICE_NOT_FOUND_EXCEPTION_KEY_UP        "Symfony\\Component\\DependencyInjection\\Exception\\ServiceNotFoundException"
#define SYMFONY_DIC_TOKEN_INACTIVE_SCOPE_EXCEPTION_KEY              "symfony\\component\\dependencyinjection\\exception\\inactivescopeexception"
#define SYMFONY_DIC_TOKEN_INACTIVE_SCOPE_EXCEPTION_KEY_UP           "Symfony\\Component\\DependencyInjection\\Exception\\InactiveScopeException"

#ifdef ZTS
#define SYMFONY_DIC_G(v) TSRMG(symfony_dic_globals_id, zend_symfony_dic_globals *, v)
#else
#define SYMFONY_DIC_G(v) (symfony_dic_globals.v)
#endif

#endif	/* PHP_SYMFONY_DIC_H */
