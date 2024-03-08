//-----------------------------------------------------------------------------
// boost mpl/aux_/traits_lambda_spec.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2001-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#ifndef BOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED
#define BOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED

#include "boost/mpl/void.hpp"
#include "boost/mpl/aux_/preprocessor/params.hpp"
#include "boost/mpl/aux_/config/lambda.hpp"

#if !defined(BOOST_MPL_NO_FULL_LAMBDA_SUPPORT)

#   define BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(i, trait) /**/

#elif !defined(BOOST_MPL_MSVC_ETI_BUG)

#   define BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(i, trait) \
template<> struct trait<void_> \
{ \
    template< BOOST_MPL_PP_PARAMS(i, typename T) > struct algorithm \
    { \
    }; \
}; \
/**/

#else

#   define BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(i, trait) \
template<> struct trait<void_> \
{ \
    template< BOOST_MPL_PP_PARAMS(i, typename T) > struct algorithm \
    { \
    }; \
}; \
template<> struct trait<int> \
{ \
    template< BOOST_MPL_PP_PARAMS(i, typename T) > struct algorithm \
    { \
        typedef int type; \
    }; \
}; \
/**/

#endif // BOOST_MPL_NO_FULL_LAMBDA_SUPPORT

#endif // BOOST_MPL_AUX_TRAITS_LAMBDA_SPEC_HPP_INCLUDED
