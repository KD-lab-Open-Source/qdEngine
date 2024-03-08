//  Boost string_algo library iter_find.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_ITER_FIND_HPP
#define BOOST_STRING_ITER_FIND_HPP

#include <boost/algorithm/string/config.hpp>
#include <algorithm>
#include <iterator>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/concept.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/detail/util.hpp>

/*! \file
    Defines generic split algorithms. Split algorithms can be 
    used to divide a sequence into several part according 
    to a given criterium. Result is given as a 'container 
    of containers' where elements are copies or references 
    to extracted parts.

    There are two algorithms provided. One iterates over matching
    substrings, the other one over the gasps between these matches.
*/

namespace boost {
    namespace algorithm {

//  iterate find ---------------------------------------------------//

        //! Iter find algorithm
        /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real 
            searching is performed only when needed.
            In each iteration new match is found and added to the result.

            \param Result A 'container container' to contain the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Finder A Finder object used for searching
            \return A reference the result

            \note Prior content of the result will be overridden.
        */
        template< 
            typename SequenceSequenceT,
            typename CollectionT,
            typename FinderT >
        inline SequenceSequenceT&
        iter_find(
            SequenceSequenceT& Result,
            CollectionT& Input,
            FinderT Finder )
        {
            function_requires< 
                FinderConcept<FinderT,
                BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type> >();

            typedef BOOST_STRING_TYPENAME 
                result_iterator_of<CollectionT>::type input_iterator_type;
            typedef find_iterator<input_iterator_type> find_iterator_type;
            typedef detail::copy_iterator_rangeF<
                BOOST_STRING_TYPENAME 
                    value_type_of<SequenceSequenceT>::type,
                input_iterator_type> copy_range_type;
            
            input_iterator_type InputEnd=end(Input);

            SequenceSequenceT Tmp(
                make_transform_iterator( 
                    find_iterator_type( begin(Input), InputEnd, Finder ),
                    copy_range_type() ),
                make_transform_iterator( 
                    find_iterator_type(),
                    copy_range_type() ) );

            Result.swap(Tmp);
            return Result;
        }

//  iterate split ---------------------------------------------------//

        //! Split find algorithm
        /*!
            This algorithm executes a given finder in iteration on the input,
            until the end of input is reached, or no match is found.
            Iteration is done using built-in find_iterator, so the real 
            searching is performed only when needed.
            Each match is used as a separator of segments. These segments are then
            returned in the result.

            \param Result A 'container container' to container the result of search.
                Both outer and inner container must have constructor taking a pair
                of iterators as an argument.
                Typical type of the result is 
                    \c std::vector<boost::iterator_range<iterator>>
                (each element of such a vector will container a range delimiting 
                a match).
            \param Input A container which will be searched.
            \param Finder A finder object used for searching
            \return A reference the result

            \note Prior content of the result will be overridden.
        */
        template< 
            typename SequenceSequenceT,
            typename CollectionT,
            typename FinderT >
        inline SequenceSequenceT&
        iter_split(
            SequenceSequenceT& Result,
            CollectionT& Input,
            FinderT Finder )
        {
            function_requires< 
                FinderConcept<FinderT,
                BOOST_STRING_TYPENAME result_iterator_of<CollectionT>::type> >();

            typedef BOOST_STRING_TYPENAME 
                result_iterator_of<CollectionT>::type input_iterator_type;
            typedef split_iterator<input_iterator_type> find_iterator_type;
            typedef detail::copy_iterator_rangeF<
                BOOST_STRING_TYPENAME 
                    value_type_of<SequenceSequenceT>::type,
                input_iterator_type> copy_range_type;
            
            input_iterator_type InputEnd=end(Input);

            SequenceSequenceT Tmp(
                make_transform_iterator( 
                    find_iterator_type( begin(Input), InputEnd, Finder ),
                    copy_range_type() ),
                make_transform_iterator( 
                    find_iterator_type(),
                    copy_range_type() ) );
            
            Result.swap(Tmp);
            return Result;
        }

    } // namespace algorithm

    // pull names to the boost namespace
    using algorithm::iter_find;
    using algorithm::iter_split;

} // namespace boost


#endif  // BOOST_STRING_ITER_FIND_HPP
