/* Boost interval/detail/sparc_rounding_control.hpp file
 *
 * Copyright Jens Maurer 2000
 * Copyright Herv� Br�nnimann, Guillaume Melquiond, Sylvain Pion 2002
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * None of the above authors nor Polytechnic University make any
 * representation about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * $Id: sparc_rounding_control.hpp,v 1.2 2003/02/05 17:34:32 gmelquio Exp $
 */

/* The basic code in this file was kindly provided by Jeremy Siek. */

#ifndef BOOST_NUMERIC_INTERVAL_DETAIL_SPARC_ROUNDING_CONTROL_HPP
#define BOOST_NUMERIC_INTERVAL_DETAIL_SPARC_ROUNDING_CONTROL_HPP

#if !defined(sparc) && !defined(__sparc__)
#  error This header is only intended for SPARC CPUs.
#endif

#ifdef __SUNPRO_CC
#  include <ieeefp.h>
#endif


namespace boost {
namespace numeric {
namespace interval_lib {
namespace detail {

struct sparc_rounding_control
{
  typedef unsigned int rounding_mode;

  static void set_rounding_mode(const rounding_mode& mode)
  {
#   if defined(__GNUC__)
    __asm__ __volatile__("ld %0, %%fsr" : : "m"(mode));
#   elif defined (__SUNPRO_CC)
    fpsetround(fp_rnd(mode));
#   elif defined(__KCC)
    asm("sethi %hi(mode), %o1");
    asm("ld [%o1+%lo(mode)], %fsr");
#   else
#     error Unsupported compiler for Sparc rounding control.
#   endif
  }

  static void get_rounding_mode(rounding_mode& mode)
  {
#   if defined(__GNUC__)
    __asm__ __volatile__("st %%fsr, %0" : "=m"(mode));
#   elif defined (__SUNPRO_CC)
    mode = fpgetround();
#   elif defined(__KCC)
#     error KCC on Sun SPARC get_round_mode: please fix me
    asm("st %fsr, [mode]");
#   else
#     error Unsupported compiler for Sparc rounding control.
#   endif
  }

#if defined(__SUNPRO_CC)
  static void downward()    { set_rounding_mode(FP_RM); }
  static void upward()      { set_rounding_mode(FP_RP); }
  static void to_nearest()  { set_rounding_mode(FP_RN); }
  static void toward_zero() { set_rounding_mode(FP_RZ); }
#else
  static void downward()    { set_rounding_mode(0xc0000000); }
  static void upward()      { set_rounding_mode(0x80000000); }
  static void to_nearest()  { set_rounding_mode(0x00000000); }
  static void toward_zero() { set_rounding_mode(0x40000000); }
#endif
};

} // namespace detail

extern "C" {
  float rintf(float);
  double rint(double);
}

template<>
struct rounding_control<float>:
  detail::sparc_rounding_control
{
  static const float& force_rounding(const float& x) { return x; }
  static float to_int(const float& x) { return rintf(x); }
};

template<>
struct rounding_control<double>:
  detail::sparc_rounding_control
{
  static const double& force_rounding(const double& x) { return x; }
  static double to_int(const double& x) { return rint(x); }
};

template<>
struct rounding_control<long double>:
  detail::sparc_rounding_control
{
  static const long double& force_rounding(const long double& x) { return x; }
  static long double to_int(const long double& x) { return rint(x); }
};

} // namespace interval_lib
} // namespace numeric
} // namespace boost

#endif /* BOOST_NUMERIC_INTERVAL_DETAIL_SPARC_ROUNDING_CONTROL_HPP */
