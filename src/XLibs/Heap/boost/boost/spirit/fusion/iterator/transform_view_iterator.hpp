/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_TRANSFORM_VIEW_ITERATOR_HPP)
#define FUSION_ITERATOR_TRANSFORM_VIEW_ITERATOR_HPP

#include <boost/spirit/fusion/iterator/detail/iterator_base.hpp>
#include <boost/spirit/fusion/iterator/detail/transform_view_iterator/deref_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/transform_view_iterator/next_traits.hpp>
#include <boost/spirit/fusion/iterator/detail/transform_view_iterator/value_traits.hpp>

namespace boost { namespace fusion
{
    struct transform_view_iterator_tag;

    template <typename First, typename F>
    struct transform_view_iterator
        : iterator_base<transform_view_iterator<First, F> >
    {
        typedef transform_view_iterator_tag tag;
        typedef First first_type;
        typedef F transform_type;

        transform_view_iterator(First const& first, F f)
            : first(first), f(f) {}

        first_type first;
        transform_type f;
    };
}}

#endif

