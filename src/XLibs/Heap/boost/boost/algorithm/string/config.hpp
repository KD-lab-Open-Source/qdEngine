//  Boost string_algo library config.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#ifndef BOOST_STRING_CONFIG_HPP
#define BOOST_STRING_CONFIG_HPP

#include <boost/config.hpp>

#ifdef BOOST_STRING_DEDUCED_TYPENAME
#   error "macro already defined!"
#endif

#ifdef __BORLANDC__
#define BOOST_STRING_TYPENAME
#else
#define BOOST_STRING_TYPENAME BOOST_DEDUCED_TYPENAME
#endif

#endif  // BOOST_STRING_CONFIG_HPP
