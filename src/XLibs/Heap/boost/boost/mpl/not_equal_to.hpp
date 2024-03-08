
#ifndef BOOST_MPL_NOT_EQUAL_TO_HPP_INCLUDED
#define BOOST_MPL_NOT_EQUAL_TO_HPP_INCLUDED

// Copyright (c) 2000-03 Aleksey Gurtovoy
//
// Use, modification and distribution are subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy 
// at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Source: /cvsroot/boost/boost/boost/mpl/not_equal_to.hpp,v $
// $Date: 2004/01/17 10:06:16 $
// $Revision: 1.2 $

#include "boost/mpl/bool.hpp"
#include "boost/mpl/integral_c.hpp"
#include "boost/mpl/aux_/value_wknd.hpp"
#include "boost/mpl/aux_/void_spec.hpp"
#include "boost/mpl/aux_/lambda_support.hpp"
#include "boost/config.hpp"

namespace boost {
namespace mpl {

template<
      typename BOOST_MPL_AUX_VOID_SPEC_PARAM(T1)
    , typename BOOST_MPL_AUX_VOID_SPEC_PARAM(T2)
    >
struct not_equal_to
{
    enum
    { 
        msvc71_wknd_ = ( BOOST_MPL_AUX_VALUE_WKND(T1)::value 
                        != BOOST_MPL_AUX_VALUE_WKND(T2)::value )
    };

    BOOST_STATIC_CONSTANT(bool, value = msvc71_wknd_);

#if !defined(__BORLANDC__)
    typedef bool_<value> type;
#else
    typedef bool_<(
          BOOST_MPL_AUX_VALUE_WKND(T1)::value 
            != BOOST_MPL_AUX_VALUE_WKND(T2)::value
        )> type;
#endif

    BOOST_MPL_AUX_LAMBDA_SUPPORT(2,not_equal_to,(T1,T2))
};

BOOST_MPL_AUX_VOID_SPEC(2, not_equal_to)

template< long N >
struct ne
{
    template< typename T > struct apply
#if !defined(__BORLANDC__)
        : not_equal_to< T,integral_c<long,N> >
    {
#else
    {
        typedef typename not_equal_to< T,integral_c<long,N> >::type type;
#endif
    };
};

} // namespace mpl
} // namespace boost

#endif // BOOST_MPL_NOT_EQUAL_TO_HPP_INCLUDED
