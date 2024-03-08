//-----------------------------------------------------------------------------
// boost mpl/pop_front_impl.hpp header file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2000-02
// Aleksey Gurtovoy
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee, 
// provided that the above copyright notice appears in all copies and 
// that both the copyright notice and this permission notice appear in 
// supporting documentation. No representations are made about the 
// suitability of this software for any purpose. It is provided "as is" 
// without express or implied warranty.

#ifndef BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
#define BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED

#include "boost/config.hpp"
#include "boost/mpl/pop_front_fwd.hpp"
#include "boost/mpl/aux_/traits_lambda_spec.hpp"
#include "boost/detail/workaround.hpp"

namespace boost {
namespace mpl {

// no default implementation; the definition is needed to make MSVC happy

template< typename Tag >
struct pop_front_traits
{
    template< typename Sequence > struct algorithm
    // conservatively placed, but maybe should go outside surrounding
    // braces.
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300) 
    {
        typedef int type;
    }
#endif
    ;
};

BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1,pop_front_traits)

} // namespace mpl
} // namespace boost

#endif // BOOST_MPL_AUX_POP_FRONT_IMPL_HPP_INCLUDED
