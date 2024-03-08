#ifndef DATE_TIME_COMPILER_CONFIG_HPP___
#define DATE_TIME_COMPILER_CONFIG_HPP___

/* Copyright (c) 2002,2003 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE-1.0 or http://www.boost.org/LICENSE-1.0)
 * Author: Jeff Garland, Bart Garst
 * $Date: 2004/03/06 23:40:20 $
 */

// This file performs some local compiler configurations

#include "boost/date_time/locale_config.hpp" //set up locale configurations

//Set up a configuration parameter for platforms that have 
//GetTimeOfDay
#ifdef BOOST_HAS_GETTIMEOFDAY
#define BOOST_DATE_TIME_HAS_GETTIMEOFDAY_HIGH_PRECISION_CLOCK
#endif


#if (defined(BOOST_NO_INCLASS_MEMBER_INITIALIZATION) || (defined(__BORLANDC__)))
#define BOOST_DATE_TIME_NO_MEMBER_INIT
#endif


// include these types before we try to re-define them
#include "boost/cstdint.hpp"

//Define INT64_C for compilers that don't have it
#if (!defined(INT64_C))
#define INT64_C(value)  int64_t(value)
#endif


/* Workaround for Borland iterator error. Error was "Cannot convert 'istream *' to 'wistream *' in function istream_iterator<>::istream_iterator() */
#if defined(__BORLANDC__) && (__BORLANDC__ <= 0x0551)
#define BOOST_DATE_TIME_NO_WISTREAM_ITERATOR
#endif


// Borland v5.64 does not have the following in std namespace; v5.5.1 does
#if defined(__BORLANDC__) && (__BORLANDC__ >= 0x0564) 
#include <locale>
namespace std {
  using stlport::tolower;
  using stlport::ctype;
  using stlport::use_facet;
}
#endif

// workaround for errors associated with wide string output 
// modifications. compilers affected are:
// Borland 551, gcc295 (not stlport), msvc6, mingw, cygwin
// Any of these compilers *should* work if used with StlPort's streams
#if ((defined(__GNUC__) && (__GNUC__ < 3)) || \
     (defined(_MSC_VER) && (_MSC_VER <= 1200)) || \
     (defined(__BORLANDC__) && (__BORLANDC__ <= 0x0564)) || \
     (defined(BOOST_NO_STD_WSTRING))) && \
     !defined(_STLP_OWN_IOSTREAMS)
#define BOOST_DATE_TIME_NO_WSTRING_CONVERSIONS
#endif

/* The following handles the definition of the necessary macros
 * for dll building on Win32 platforms.
 * 
 * For code that will be placed in the date_time .dll, 
 * it must be properly prefixed with BOOST_DATE_TIME_DECL.
 * The corresponding .cpp file must have BOOST_DATE_TIME_SOURCES
 * defined before including its header. For examples see:
 * greg_month.hpp & greg_month.cpp
 * 
 */

#ifdef BOOST_HAS_DECLSPEC // defined in config system
   // we need to import/export our code only if the user has specifically
   // asked for it by defining either BOOST_ALL_DYN_LINK if they want all boost
   // libraries to be dynamically linked, or BOOST_DATE_TIME_DYN_LINK
   // if they want just this one to be dynamically liked:
#  if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_DATE_TIME_DYN_LINK)
      // export if this is our own source, otherwise import:
#     ifdef BOOST_DATE_TIME_SOURCE
#       define BOOST_DATE_TIME_DECL __declspec(dllexport)
#     else
#       define BOOST_DATE_TIME_DECL __declspec(dllimport)
#     endif  // BOOST_DATE_TIME_SOURCE
#  endif  // DYN_LINK
#endif  // BOOST_HAS_DECLSPEC
//
// if BOOST_WHATEVER_DECL isn't defined yet define it now:
#ifndef BOOST_DATE_TIME_DECL
#  define BOOST_DATE_TIME_DECL
#endif

//
// Automatically link to the correct build variant where possible. 
// 
#if !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_DATE_TIME_NO_LIB) && !defined(BOOST_DATE_TIME_SOURCE)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define BOOST_LIB_NAME boost_date_time
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_DATE_TIME_DYN_LINK)
#  define BOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <boost/config/auto_link.hpp>
#endif  // auto-linking disabled

#endif
