dnl $Id$
dnl config.m4 for extension symfony_dic

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(symfony_dic, for symfony_dic support,
dnl Make sure that the comment is aligned:
dnl [  --with-symfony_dic             Include symfony_dic support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(symfony_dic, whether to enable symfony_dic support,
dnl Make sure that the comment is aligned:
[  --enable-symfony_dic           Enable symfony_dic support])

if test "$PHP_SYMFONY_DIC" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-symfony_dic -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/symfony_dic.h"  # you most likely want to change this
  dnl if test -r $PHP_SYMFONY_DIC/$SEARCH_FOR; then # path given as parameter
  dnl   SYMFONY_DIC_DIR=$PHP_SYMFONY_DIC
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for symfony_dic files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SYMFONY_DIC_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SYMFONY_DIC_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the symfony_dic distribution])
  dnl fi

  dnl # --with-symfony_dic -> add include path
  dnl PHP_ADD_INCLUDE($SYMFONY_DIC_DIR/include)

  dnl # --with-symfony_dic -> check for lib and symbol presence
  dnl LIBNAME=symfony_dic # you may want to change this
  dnl LIBSYMBOL=symfony_dic # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SYMFONY_DIC_DIR/$PHP_LIBDIR, SYMFONY_DIC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SYMFONY_DICLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong symfony_dic lib version or lib not found])
  dnl ],[
  dnl   -L$SYMFONY_DIC_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SYMFONY_DIC_SHARED_LIBADD)

  PHP_NEW_EXTENSION(symfony_dic, symfony_dic.c, $ext_shared)
fi
