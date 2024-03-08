//  Boost string_algo library slist_traits.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_STD_SLIST_TRAITS_HPP
#define BOOST_STRING_STD_SLIST_TRAITS_HPP

#include <boost/algorithm/string/yes_no_type.hpp>
#include <slist>
#include <boost/algorithm/string/sequence_traits.hpp>

namespace boost {
    namespace algorithm {

//  SGI's std::slist<> traits  -----------------------------------------------//

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

        // stable iterators tester
        template<typename T, typename AllocT>
        yes_type has_stable_iterators_tester( const std::slist<T,AllocT>* );

        // const time insert tester
        template<typename T, typename AllocT>
        yes_type has_const_time_insert_tester( const std::slist<T,AllocT>* );

        // const time erase tester
        template<typename T, typename AllocT>
        yes_type has_const_time_erase_tester( const std::slist<T,AllocT>* );

#else // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    
    // stable iterators trait
        template<typename T, typename AllocT>
        class has_stable_iterators< std::slist<T,AllocT> >
        {
           public:
            BOOST_STATIC_CONSTANT(bool, value=true);
            typedef mpl::bool_<value> type;     
        };

    // const time insert trait
        template<typename T, typename AllocT>
        class has_const_time_insert< std::slist<T,AllocT> >
        {
           public:
            BOOST_STATIC_CONSTANT(bool, value=true);
            typedef mpl::bool_<value> type;     
        };

    // const time erase trait
        template<typename T, typename AllocT>
        class has_const_time_erase< std::slist<T,AllocT> >
        {
           public:
            BOOST_STATIC_CONSTANT(bool, value=true);
            typedef mpl::bool_<value> type;     
        };
#endif


    } // namespace algorithm
} // namespace boost


#endif  // BOOST_STRING_STD_LIST_TRAITS_HPP
