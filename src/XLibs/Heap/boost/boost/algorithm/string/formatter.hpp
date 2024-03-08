//  Boost string_algo library formatter.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_FORMATTER_HPP
#define BOOST_STRING_FORMATTER_HPP

#include <boost/detail/iterator.hpp>
#include <boost/algorithm/string/collection_traits.hpp>
#include <boost/algorithm/string/iterator_range.hpp>
#include <boost/algorithm/string/detail/formatter.hpp>

/*! \file
    Defines formatter generators. Formatter is a functor which formats
    a string according to give parameters. A Formatter works
    in conjunction with a Finder. A Finder can provide additional information
    for a specific Formatter. An example of such a cooperation is reger_finder
    and regex_formatter.

    Formatters are used by as a pluggable components for replace facilities. 
    This header contains generator functions for the Formatters provided in this library.
*/

namespace boost {
    namespace algorithm {

// generic formaters  ---------------------------------------------------------------//

        //! Constant formatter
        /*!
            Construct the \c const_formatter. Const formatter always returns
            the same value, regardless of the parameter.

            \param Format A predefined value used as a result for formating
            \return An instance of the \c const_formatter object.
        */
        template<typename CollectionT>
        inline detail::const_formatF<CollectionT>
        const_formatter(const CollectionT& Format)
        {
            return detail::const_formatF<CollectionT>(Format);
        }

        //! Identity formatter
        /*!
            Construct the \c identity_formatter. Identity formatter always returns
            the parameter.

            \return An instance of the \c identity_formatter object.
        */
        template<typename CollectionT>
        inline detail::identity_formatF<CollectionT>
        identity_formatter()
        {
            return detail::identity_formatF<CollectionT>();
        }

        //! Empty formatter
        /*!
            Construct the \c empty_formatter. Empty formater always returns an empty
            sequence. 

            \param Input container used to select a correct value_type for the
                         resulting empty_container<>.
            \return An instance of the \c empty_formatter object.
        */
        template<typename CollectionT>
        inline detail::empty_formatF< 
            BOOST_STRING_TYPENAME value_type_of<CollectionT>::type>
        empty_formatter(const CollectionT& Input)
        {
            return detail::empty_formatF<
                BOOST_STRING_TYPENAME value_type_of<CollectionT>::type>();
        }


    } // namespace algorithm

    // pull the names to the boost namespace
    using algorithm::const_formatter;
    using algorithm::identity_formatter;
    using algorithm::empty_formatter;

} // namespace boost


#endif  // BOOST_FORMATTER_HPP
