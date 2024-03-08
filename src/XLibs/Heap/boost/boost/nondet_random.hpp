/* boost nondet_random.hpp header file
 *
 * Copyright Jens Maurer 2000
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * Jens Maurer makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * $Id: nondet_random.hpp,v 1.8 2004/02/28 21:14:04 eric_niebler Exp $
 *
 * Revision history
 *  2000-02-18  Portability fixes (thanks to Beman Dawes)
 */

//  See http://www.boost.org/libs/random for documentation.


#ifndef BOOST_NONDET_RANDOM_HPP
#define BOOST_NONDET_RANDOM_HPP

#include <string>                       // std::abs
#include <algorithm>                    // std::min
#include <cmath>
#include <boost/config.hpp>
#include <boost/utility.hpp>            // noncopyable
#include <boost/integer_traits.hpp>     // compile-time integral limits

namespace boost {

// use some OS service to generate non-deterministic random numbers
class random_device : private noncopyable
{
public:
  typedef unsigned int result_type;
  BOOST_STATIC_CONSTANT(bool, has_fixed_range = true);
  BOOST_STATIC_CONSTANT(result_type, min_value = integer_traits<result_type>::const_min);
  BOOST_STATIC_CONSTANT(result_type, max_value = integer_traits<result_type>::const_max);

  result_type min BOOST_PREVENT_MACRO_SUBSTITUTION () const { return min_value; }
  result_type max BOOST_PREVENT_MACRO_SUBSTITUTION () const { return max_value; }
  explicit random_device(const std::string& token = default_token);
  ~random_device();
  double entropy() const;
  unsigned int operator()();

private:
  static const char * const default_token;

  /*
   * std:5.3.5/5 [expr.delete]: "If the object being deleted has incomplete
   * class type at the point of deletion and the complete class has a
   * non-trivial destructor [...], the behavior is undefined".
   * This disallows the use of scoped_ptr<> with pimpl-like classes
   * having a non-trivial destructor.
   */
  class impl;
  impl * pimpl;
};


// TODO: put Schneier's Yarrow-160 algorithm here.

} // namespace boost

#endif /* BOOST_NONDET_RANDOM_HPP */
