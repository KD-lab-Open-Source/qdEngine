
#ifndef BOOST_MPL_MULTISET_AUX_MULTISET0_HPP_INCLUDED
#define BOOST_MPL_MULTISET_AUX_MULTISET0_HPP_INCLUDED

// + file: boost/mpl/multiset/aux_/multiset0.hpp
// + last modified: 05/nov/03

// Copyright Aleksey Gurtovoy 2003
//
// Use, modification and distribution are subject to the Boost Software 
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy 
// at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

#include "boost/mpl/multiset/aux_/tag.hpp"
#include "boost/mpl/int.hpp"

namespace boost { namespace mpl {

template< int dummy_ = 0 >
struct multiset0
{
    typedef aux::multiset_tag tag;

    typedef int_<1> count_;
    static char (& key_count(...) )[count_::value];
    static char (& ref_key_count(...) )[count_::value];
};

}} // namespace boost::mpl

#endif // BOOST_MPL_MULTISET_AUX_MULTISET0_HPP_INCLUDED
