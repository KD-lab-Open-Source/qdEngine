//  (C) Copyright David Abrahams 2000. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  The author gratefully acknowleges the support of Dragon Systems, Inc., in
//  producing this work.

//  This file serves as a wrapper around <Python.h> which allows it to be
//  compiled with GCC 2.95.2 under Win32 and which disables the default MSVC
//  behavior so that a program may be compiled in debug mode without requiring a
//  special debugging build of the Python library.


//  To use the Python debugging library, #define BOOST_DEBUG_PYTHON on the
//  compiler command-line.

// Revision History:
// 05 Mar 01  Suppress warnings under Cygwin with Python 2.0 (Dave Abrahams)
// 04 Mar 01  Rolled in some changes from the Dragon fork (Dave Abrahams)
// 01 Mar 01  define PyObject_INIT() for Python 1.x (Dave Abrahams)

//
// Python's LongObject.h helpfully #defines ULONGLONG_MAX for us,
// which confuses Boost's config
//
#include <limits.h>
#ifndef ULONG_MAX
# define BOOST_PYTHON_ULONG_MAX_UNDEFINED
#endif
#ifndef LONGLONG_MAX
# define BOOST_PYTHON_LONGLONG_MAX_UNDEFINED
#endif
#ifndef ULONGLONG_MAX
# define BOOST_PYTHON_ULONGLONG_MAX_UNDEFINED
#endif

//
// Get ahold of Python's version number
//
#include <patchlevel.h>

#ifdef _DEBUG
# ifndef BOOST_DEBUG_PYTHON
#  undef _DEBUG // Don't let Python force the debug library just because we're debugging.
#  define DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# endif
#endif

//
// Some things we need in order to get Python.h to work with compilers other
// than MSVC on Win32
//
#if defined(_WIN32) || defined(__CYGWIN__)
# if defined(__GNUC__) && defined(__CYGWIN__)

#  define SIZEOF_LONG 4

#  if PY_MAJOR_VERSION < 2 || PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION <= 2

typedef int pid_t;

#   define WORD_BIT 32
#   define hypot _hypot
#   include <stdio.h>

#   if PY_MAJOR_VERSION < 2
#    define HAVE_CLOCK
#    define HAVE_STRFTIME
#    define HAVE_STRERROR
#   endif

#   define NT_THREADS

#   ifndef NETSCAPE_PI
#    define USE_SOCKET
#   endif

#   ifdef USE_DL_IMPORT
#    define DL_IMPORT(RTYPE) __declspec(dllimport) RTYPE
#   endif

#   ifdef USE_DL_EXPORT
#    define DL_IMPORT(RTYPE) __declspec(dllexport) RTYPE
#    define DL_EXPORT(RTYPE) __declspec(dllexport) RTYPE
#   endif

#   define HAVE_LONG_LONG 1
#   define LONG_LONG long long
#  endif 

# elif defined(__MWERKS__)

#  ifndef _MSC_VER
#   define PY_MSC_VER_DEFINED_FROM_WRAP_PYTHON_H 1
#   define _MSC_VER 900
#  endif

#  if PY_MAJOR_VERSION < 2 || PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 2
#   include <config.h>
#  else
#   include <pyconfig.h>
#  endif
#  undef hypot // undo the evil #define left by Python.

# elif defined(__BORLANDC__)
#  if PY_MAJOR_VERSION < 2 || PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION < 2
#   include <config.h>
#  else
#   include <pyconfig.h>
#  endif
#  undef HAVE_HYPOT
#  define HAVE_HYPOT 1
# endif

#endif // _WIN32

#if PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION == 2 && PY_MICRO_VERSION < 2
# include <boost/python/detail/python22_fixed.h>
#else
# include <Python.h>
#endif

#ifdef BOOST_PYTHON_ULONG_MAX_UNDEFINED
# undef ULONG_MAX
# undef BOOST_PYTHON_ULONG_MAX_UNDEFINED
#endif

#ifdef BOOST_PYTHON_LONGLONG_MAX_UNDEFINED
# undef LONGLONG_MAX
# undef BOOST_PYTHON_LONGLONG_MAX_UNDEFINED
#endif

#ifdef BOOST_PYTHON_ULONGLONG_MAX_UNDEFINED
# undef ULONGLONG_MAX
# undef BOOST_PYTHON_ULONGLONG_MAX_UNDEFINED
#endif

#ifdef PY_MSC_VER_DEFINED_FROM_WRAP_PYTHON_H
# undef _MSC_VER
#endif

#ifdef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# undef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# define _DEBUG
#endif

#if !defined(PY_MAJOR_VERSION) || PY_MAJOR_VERSION < 2
# define PyObject_INIT(op, typeobj) \
        ( (op)->ob_type = (typeobj), _Py_NewReference((PyObject *)(op)), (op) )
#endif

#ifdef __MWERKS__
# pragma warn_possunwant off
#elif _MSC_VER
# pragma warning(disable:4786)
#endif

#if defined(HAVE_LONG_LONG)
# if defined(PY_LONG_LONG)
#  define BOOST_PYTHON_LONG_LONG PY_LONG_LONG
# elif defined(LONG_LONG)
#  define BOOST_PYTHON_LONG_LONG LONG_LONG
# else
#  error "HAVE_LONG_LONG defined but not PY_LONG_LONG or LONG_LONG"
# endif
#endif
