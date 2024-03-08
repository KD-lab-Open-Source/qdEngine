/* boost random/ranlux.hpp header file
 *
 * Copyright Jens Maurer 2002
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * Jens Maurer makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * See http://www.boost.org for most recent version including documentation.
 *
 * $Id: ranlux.hpp,v 1.4 2002/10/03 22:06:48 jmaurer Exp $
 *
 * Revision history
 *  2001-02-18  created
 */

#ifndef BOOST_RANDOM_RANLUX_HPP
#define BOOST_RANDOM_RANLUX_HPP

#include <boost/config.hpp>
#include <boost/random/subtract_with_carry.hpp>
#include <boost/random/discard_block.hpp>

namespace boost {

namespace random {
  typedef subtract_with_carry<int, (1<<24), 10, 24, 0> ranlux_base;
  typedef subtract_with_carry_01<float, 24, 10, 24> ranlux_base_01;
  typedef subtract_with_carry_01<double, 48, 10, 24> ranlux64_base_01;
}

typedef random::discard_block<random::ranlux_base, 223, 24> ranlux3;
typedef random::discard_block<random::ranlux_base, 389, 24> ranlux4;

typedef random::discard_block<random::ranlux_base_01, 223, 24> ranlux3_01;
typedef random::discard_block<random::ranlux_base_01, 389, 24> ranlux4_01;

typedef random::discard_block<random::ranlux64_base_01, 223, 24> ranlux64_3_01;
typedef random::discard_block<random::ranlux64_base_01, 389, 24> ranlux64_4_01;

#if !defined(BOOST_NO_INT64_T) && !defined(BOOST_NO_INTEGRAL_INT64_T)
namespace random {
  typedef random::subtract_with_carry<int64_t, (int64_t(1)<<48), 10, 24, 0> ranlux64_base;
}
typedef random::discard_block<random::ranlux64_base, 223, 24> ranlux64_3;
typedef random::discard_block<random::ranlux64_base, 389, 24> ranlux64_4;
#endif /* !BOOST_NO_INT64_T && !BOOST_NO_INTEGRAL_INT64_T */

} // namespace boost

#endif // BOOST_RANDOM_LINEAR_CONGRUENTIAL_HPP
