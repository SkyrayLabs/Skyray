dnl $Id$
dnl config.m4 for extension skyray

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(skyray, for skyray support,
dnl Make sure that the comment is aligned:
dnl [  --with-skyray             Include skyray support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(skyray, whether to enable skyray support,
[  --enable-skyray           Enable skyray support])

if test "$PHP_SKYRAY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-skyray -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/skyray.h"  # you most likely want to change this
  dnl if test -r $PHP_SKYRAY/$SEARCH_FOR; then # path given as parameter
  dnl   SKYRAY_DIR=$PHP_SKYRAY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for skyray files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SKYRAY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SKYRAY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the skyray distribution])
  dnl fi

  dnl # --with-skyray -> add include path
  dnl PHP_ADD_INCLUDE($SKYRAY_DIR/src)

  dnl # --with-skyray -> check for lib and symbol presence
  dnl LIBNAME=skyray # you may want to change this
  dnl LIBSYMBOL=skyray # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SKYRAY_DIR/$PHP_LIBDIR, SKYRAY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SKYRAYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong skyray lib version or lib not found])
  dnl ],[
  dnl   -L$SKYRAY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SKYRAY_SHARED_LIBADD)
  
  sources="
    src/skyray.c                    \
    src/core/stream_client.c        \
    src/core/stream.c               \
    src/processing/process.c        \
  "
  
  PHP_NEW_EXTENSION(skyray, $sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
