

AC_DEFUN([AC_PROG_WXWINDOWS_AH],
[
	AC_MSG_CHECKING([wxWidgets version])
	if wxversion=`$WXCONFIG --version`; then
		AC_MSG_RESULT([$wxversion])
	else
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
	fi
	
	_wx_flags=`$WXCONFIG --cflags`
	_wx_libs=`$WXCONFIG --libs`
#	_wx_ldflags=`$WXCONFIG --ldflags`
	CPPFLAGS="$CPPFLAGS $_wx_flags"
	LIBS="$LIBS $_wx_libs"
#	LDFLAGS="$LDFLAGS $_wx_ldflags"

])



AC_DEFUN([AC_PROG_PYTHON_AH],
[

	dnl ------------- Path to Python
	
	xc_with_python=""
	PYTHON=
	
	AC_ARG_WITH(python,
	[  --with-python=DIR       path to python interpreter, "no" for a no-python build],
	xc_with_python=$withval)
	
	if test "x$xc_with_python" = "x" ; then
	AC_PATH_PROG(PYTHON,python)
	elif test "$xc_with_python" = "no" ; then
	PYTHON=""
	elif test "x$xc_with_python" != x; then
	test -f "$xc_with_python" && PYTHON=$xc_with_python
	fi
	
	if test "x$PYTHON" != x; then
	_py_version=`$PYTHON -c 'import sys; print sys.version[[0:3]]'`
	_py_prefix=`$PYTHON -c 'import sys; print sys.prefix'`
	_py_exec_prefix=`$PYTHON -c 'import sys; print sys.exec_prefix'`
	
	_py_include="$_py_prefix/include/python$_py_version"
	_py_lib="$_py_exec_prefix/lib/python$_py_version/config"
	_py_makefile="$_py_lib/Makefile"
	
	if test $_py_version = 1.5; then
	echo
	echo "*** WARNING:"
	echo "*** Python version 1.5 detected:  This version of Python has a known fatal"
	echo "*** bug.  Disabling Python interface.  If you want the embedded Python"
	echo "*** interface, you will need to get an updated version of Python."
	echo
	PYTHON=""
	fi
	fi
	
	if test "x$PYTHON" != x; then
	if test -f $_py_makefile; then
	_py_libs=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $_py_makefile`
	_py_libm=`sed -n -e 's/^LIBM=\(.*\)/\1/p' $_py_makefile`
	else
	_py_libs='-lpthread -ldl -lutil -ldb'
	_py_libm='-lm'
	fi
	
	# Now that we think we know where the python include and
	# library files are, make sure we can build a small
	# program against them.
	_save_CPPFLAGS=${CPPFLAGS}
	_save_LDFLAGS=${LDFLAGS}
	_save_LIBS=${LIBS}
	
	CPPFLAGS="$CPPFLAGS -I$_py_include"
	LDFLAGS="$LDFLAGS -L$_py_lib"
	
	case "$target_os" in
		*cygwin*)
			PYLIB="-lpython$_py_version"
			LIBS="$PYLIB.dll $LIBS $X_PRE_LIBS $_py_libs"
			;;
		*)
	#		LIBS="$LIBS -lpython$_py_version $_py_libs $_py_libm"
			LIBS="$LIBS -lpython$_py_version"
			;;
	esac
	
	
	_have_python=0
	AC_CHECK_HEADER(Python.h,[
	AC_MSG_CHECKING([for Py_Initialize in -lpython$_py_version])
	AC_TRY_LINK([#include "Python.h"],[Py_Initialize();],[
		_have_python=1
		M4_DEFS='-DHAVE_PYTHON=1'
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_PYTHON)],
		AC_MSG_RESULT(no))])
	
	if test "x$_have_python" = x0; then
	CPPFLAGS=$_save_CPPFLAGS
	LDFLAGS=$_save_LDFLAGS
	LIBS=$_save_LIBS
	PYTHON=""
	fi
	fi

])



