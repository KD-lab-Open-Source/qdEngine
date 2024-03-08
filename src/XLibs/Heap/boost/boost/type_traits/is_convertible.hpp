
// Copyright (C) 2000 John Maddock (john@johnmaddock.co.uk)
// Copyright (C) 2000 Jeremy Siek (jsiek@lsc.nd.edu)
// Copyright (C) 1999, 2000 Jaakko J�rvi (jaakko.jarvi@cs.utu.fi)
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
#define BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED

#include "boost/type_traits/detail/yes_no_type.hpp"
#include "boost/type_traits/config.hpp"
#include "boost/type_traits/is_array.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "boost/type_traits/ice.hpp"

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)
#   include "boost/type_traits/is_void.hpp"
#endif

// should be always the last #include directive
#include "boost/type_traits/detail/bool_trait_def.hpp"

namespace boost {

// is one type convertable to another?
//
// there are multiple versions of the is_convertible
// template, almost every compiler seems to require its
// own version.
//
// Thanks to Andrei Alexandrescu for the original version of the
// conversion detection technique!
//

namespace detail {

// MS specific version:

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)

// This workaround is necessary to handle when From is void
// which is normally taken care of by the partial specialization
// of the is_convertible typename.
using ::boost::type_traits::yes_type;
using ::boost::type_traits::no_type;

template< typename From >
struct does_conversion_exist
{
    template< typename To > struct result_
    {
        static no_type BOOST_TT_DECL _m_check(...);
        static yes_type BOOST_TT_DECL _m_check(To);
        static From _m_from;
        enum { value = sizeof( _m_check(_m_from) ) == sizeof(yes_type) };
    };
};

template<>
struct does_conversion_exist<void>
{
    template< typename To > struct result_
    {
        enum { value = ::boost::is_void<To>::value };
    };
};

template <typename From, typename To>
struct is_convertible_basic_impl
    : does_conversion_exist<From>::template result_<To>
{
};

#elif defined(__BORLANDC__) && (__BORLANDC__ < 0x560)
//
// special version for Borland compilers
// this version breaks when used for some
// UDT conversions:
//
template <typename From, typename To>
struct is_convertible_impl
{
#pragma option push -w-8074
    // This workaround for Borland breaks the EDG C++ frontend,
    // so we only use it for Borland.
    template <typename T> struct checker
    {
        static ::boost::type_traits::no_type BOOST_TT_DECL _m_check(...);
        static ::boost::type_traits::yes_type BOOST_TT_DECL _m_check(T);
    };

    static From _m_from;
    static bool const value = sizeof( checker<To>::_m_check(_m_from) )
        == sizeof(::boost::type_traits::yes_type);
#pragma option pop
};

#elif defined(__GNUC__) || defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
// special version for gcc compiler + recent Borland versions
// note that this does not pass UDT's through (...)

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename T> struct checker
{
    static boost::type_traits::no_type _m_check(any_conversion ...);
    static boost::type_traits::yes_type _m_check(T, int);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    static From _m_from;
    static bool const value = sizeof( detail::checker<To>::_m_check(_m_from, 0) )
        == sizeof(::boost::type_traits::yes_type);
};

#elif (defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 245) && !defined(__ICL)) \
      || defined(__IBMCPP__)
//
// This is *almost* an ideal world implementation as it doesn't rely
// on undefined behaviour by passing UDT's through (...).
// Unfortunately it doesn't quite pass all the tests for most compilers (sigh...)
// Enable this for your compiler if is_convertible_test.cpp will compile it...
//
// Note we do not enable this for VC7.1, because even though it passes all the
// type_traits tests it is known to cause problems when instantiation occurs
// deep within the instantiation tree :-(
//
struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    // we need this constructor to catch references to functions
    // (which can not be cv-qualified):
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::boost::type_traits::no_type BOOST_TT_DECL _m_check(any_conversion ...);
    static ::boost::type_traits::yes_type BOOST_TT_DECL _m_check(To, int);
       static From _m_from;

    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from, 0) ) == sizeof(::boost::type_traits::yes_type)
        );
};

#else

//
// This version seems to work pretty well for a wide spectrum of compilers,
// however it does rely on undefined behaviour by passing UDT's through (...).
//
template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::boost::type_traits::no_type BOOST_TT_DECL _m_check(...);
    static ::boost::type_traits::yes_type BOOST_TT_DECL _m_check(To);
    static From _m_from;

    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from) ) == sizeof(::boost::type_traits::yes_type)
        );
};

#endif // is_convertible_impl

#if !defined(__BORLANDC__) || __BORLANDC__ > 0x551
template <typename From, typename To>
struct is_convertible_impl
{
    typedef typename add_reference<From>::type ref_type;
    BOOST_STATIC_CONSTANT(bool, value =
        (::boost::type_traits::ice_and<
            ::boost::detail::is_convertible_basic_impl<ref_type,To>::value,
            ::boost::type_traits::ice_not<
               ::boost::is_array<To>::value
            >::value
        >::value)
        );
};
#endif

//
// Now add the full and partial specialisations
// for void types, these are common to all the
// implementation above:
//
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 volatile,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const volatile,value) \
    /**/

#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 volatile,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const volatile,spec2,value) \
    /**/

    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(is_convertible,void,void,true)

#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2
#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1

#else
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(is_convertible,void,void,true)
#endif // BOOST_NO_CV_VOID_SPECIALIZATIONS

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void,false)
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void volatile,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const volatile,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void volatile,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const volatile,false)
#endif
#endif // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF2(is_convertible,From,To,(::boost::detail::is_convertible_impl<From,To>::value))


#if defined(__GNUC__)

// Declare specializations of is_convertible for all of the floating
// types to all of the integral types. This suppresses some nasty
// warnings

#   define TT_AUX_IS_CONVERTIBLE_SPEC(T1,T2) \
    BOOST_TT_AUX_BOOL_TRAIT_SPEC2(is_convertible,T1,T2,true) \
    /**/

#   define TT_AUX_IS_CONVERTIBLE_SPEC_2(T1,T2) \
    TT_AUX_IS_CONVERTIBLE_SPEC(T1,signed T2) \
    TT_AUX_IS_CONVERTIBLE_SPEC(T1,unsigned T2) \
    /**/

#   define TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC(F) \
    TT_AUX_IS_CONVERTIBLE_SPEC(F,char) \
    TT_AUX_IS_CONVERTIBLE_SPEC_2(F,char) \
    TT_AUX_IS_CONVERTIBLE_SPEC_2(F,short) \
    TT_AUX_IS_CONVERTIBLE_SPEC_2(F,int) \
    TT_AUX_IS_CONVERTIBLE_SPEC_2(F,long) \
    TT_AUX_IS_CONVERTIBLE_SPEC_2(F,long long) \
    /**/

#   define TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_CV_SPEC(F) \
    TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC(F) \
    TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC(F const) \
    TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC(F volatile) \
    TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC(F const volatile) \
    /**/

TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_CV_SPEC(float)
TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_CV_SPEC(double)
TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_CV_SPEC(long double)

#   undef TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_CV_SPEC
#   undef TT_AUX_IS_CONVERTIBLE_FROM_FLOAT_SPEC
#   undef TT_AUX_IS_CONVERTIBLE_SPEC_2
#   undef TT_AUX_IS_CONVERTIBLE_SPEC

#endif // __GNUC__

} // namespace boost

#include "boost/type_traits/detail/bool_trait_undef.hpp"

#endif // BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
