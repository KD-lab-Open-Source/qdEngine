/*=============================================================================
    Copyright (c) 2003 Joel de Guzman

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_ITERATOR_DETAIL_TYPE_SEQUENCE_ITERATOR_NEXT_TRAITS_HPP)
#define FUSION_ITERATOR_DETAIL_TYPE_SEQUENCE_ITERATOR_NEXT_TRAITS_HPP

#include <boost/spirit/fusion/detail/config.hpp>
#include <boost/mpl/next.hpp>

namespace boost { namespace fusion
{
    struct type_sequence_iterator_tag;

    template <typename Tag>
    struct next_traits;

    template <typename Iterator>
    struct type_sequence_iterator;

    template <>
    struct next_traits<type_sequence_iterator_tag>
    {
        template <typename Iterator>
        struct algorithm
        {
            typedef type_sequence_iterator<
                typename mpl::next<typename Iterator::iterator_type>::type
            > type;

            static type
            apply(Iterator)
            {
                FUSION_RETURN_DEFAULT_CONSTRUCTED;
            };
        };
    };
}}

#endif


