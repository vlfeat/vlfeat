/** @file    mathop.h
 ** @author  Andrea Vedaldi
 ** @brief   Math operations
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_MATHOP
#define VL_MATHOP

#include "generic.h"

/** --------------------------------------------------------------- */
/** @{ */
/** @brief Modulo 2 PI
 **
 ** The function is optimized for small values of @a x.
 **
 ** @return @c mod(x, 2 * VL_PI)
 **/
VL_INLINE
float vl_mod_2pi_f (float x)
{
  while (x < 0.0      ) x += (float) (2 * VL_PI);
  while (x > 2 * VL_PI) x -= (float) (2 * VL_PI);
  return x ;
}

VL_INLINE
double vl_mod_2pi_d (double x)
{
  while (x < 0.0      ) x += 2 * VL_PI ;
  while (x > 2 * VL_PI) x -= 2 * VL_PI ;
  return x ;
}
/*@}*/

/** --------------------------------------------------------------- */
/** @{ */
/** @brief Floor operation (@c (int) floor(x))
 ** @param x argument.
 ** @return @c (int) floor(x)
 **/
VL_INLINE
int
vl_floor_f (float x)
{
  int xi = (int) x ;
  if (x >= 0 || (float) xi == x) return xi ;
  else return xi - 1 ;
}

VL_INLINE
int
vl_floor_d (double x)
{
  int xi = (int) x ;
  if (x >= 0 || (double) xi == x) return xi ;
  else return xi - 1 ;
}
/* @} */


/** --------------------------------------------------------------- */
/** @{ */
/** @brief Fast @c abs(x)
 ** @param x argument.
 ** @return @c abs(x)
 **/
VL_INLINE
float
vl_abs_f (float x)
{
  return (x >= 0) ? x : -x ;
}

VL_INLINE
double
vl_abs_d (double x)
{
  return (x >= 0) ? x : -x ;
}
/* @} */

/** ---------------------------------------------------------------- */
/** @{ */
/** @brief Fast @c atan2 approximation
 **
 ** @section Algorithm
 **
 ** We approximate the function f(r)=atan((1-r)/(1+r)) for r in [-1,1]
 ** by fitting a third order polynomial f(r)=c0 + c1 r + c2 r^2 + + c3 r^3.
 ** We impose the constraints
 ** 
 ** <pre>
 ** f(+1) = c0 + c1 + c2 + c3 = atan(0) = 0
 ** f(-1) = c0 - c1 + c2 - c3 = atan(inf) = pi/2
 ** f(0)  = c0                = atan(1) = pi/4
 ** </pre>
 **
 ** We are still missing a constraint, which we may obtain by
 ** minimizing the Linf approximation error. This yields
 **
 ** <pre>
 ** c0=pi/4, c1=-0.9675, c2=0, c3=0.1821
 ** </pre>
 **
 ** which has maxerr = 0.0061 rad = 0.35 grad.
 **
 ** @param x argument.
 ** @param y argument.
 ** @return Approximation of @c atan2(x).
 **/

VL_INLINE
float
vl_fast_atan2_f (float y, float x)
{
  /*
   
  */

  float angle, r ;
  float const c3 = 0.1821F ;
  float const c1 = 0.9675F ;
  float abs_y    = vl_abs_f (y) + 1e-10F ;
  
  if (x >= 0) {
    r = (x - abs_y) / (x + abs_y) ;
    angle = (float) (VL_PI / 4) ;
  } else {
    r = (x + abs_y) / (abs_y - x) ;
    angle = (float) (3 * VL_PI / 4) ;
  } 
  angle += (c3*r*r - c1) * r ; 
  return (y < 0) ? - angle : angle ;
}

VL_INLINE
double
vl_fast_atan2_d (double y, double x)
{
  double angle, r ;
  double const c3 = 0.1821 ;
  double const c1 = 0.9675 ;
  double abs_y    = vl_abs_d (y) + 1e-10 ;
  
  if (x >= 0) {
    r = (x - abs_y) / (x + abs_y) ;
    angle = VL_PI / 4 ;
  } else {
    r = (x + abs_y) / (abs_y - x) ;
    angle = 3 * VL_PI / 4 ;
  } 
  angle += (c3*r*r - c1) * r ; 
  return (y < 0) ? - angle : angle ;
}
/* @} */

/** --------------------------------------------------------------- */
/** @{ */
/** @brief Fast @c resqrt approximation
 **
 ** @param x argument.
 ** @return Approximation to @c resqrt(x).
 **/

VL_INLINE
float
vl_fast_resqrt_f (float x)
{
  /* 32-bit version */
  union {
    float x ;
    vl_int32  i ;
  } u ;
  
  float xhalf = (float) 0.5 * x ;

  /* convert floating point value in RAW integer */
  u.x = x ;                   
  
  /* gives initial guess y0 */
  u.i = 0x5f3759df - (u.i >> 1);  
  /*u.i = 0xdf59375f - (u.i>>1);*/
  
  /* two Newton steps */
  u.x = u.x * ( (float) 1.5  - xhalf*u.x*u.x) ; 
  u.x = u.x * ( (float) 1.5  - xhalf*u.x*u.x) ;
  return u.x ;
}

VL_INLINE
double
vl_fast_resqrt_d (double x)
{
  /* 64-bit version */
  union {
    double x ;
    vl_int64  i ;
  } u ;
  
  double xhalf = (double) 0.5 * x ;
  
  /* convert floating point value in RAW integer */
  u.x = x ;                   
  
  /* gives initial guess y0 */
#ifdef VL_COMPILER_MSC
  u.i = 0x5fe6ec85e7de30dai64 - (u.i >> 1) ; 
#else
  u.i = 0x5fe6ec85e7de30daLL - (u.i >> 1) ;
#endif

  /* two Newton steps */
  u.x = u.x * ( (double) 1.5  - xhalf*u.x*u.x) ; 
  u.x = u.x * ( (double) 1.5  - xhalf*u.x*u.x) ;
  return u.x ;
}
/* @} */


/** --------------------------------------------------------------- */
/** @{ */
/** @brief Fast @c sqrt approximation
 ** @param x argument.
 ** @return Approximation to @c sqrt(x).
 **/

VL_INLINE
float
vl_fast_sqrt_f (float x)
{
  return (x < 1e-8) ? 0 : x * vl_fast_resqrt_f (x) ;
}

VL_INLINE
double
vl_fast_sqrt_d (float x)
{
  return (x < 1e-8) ? 0 : x * vl_fast_resqrt_d (x) ;
}

VL_INLINE
vl_uint32
vl_fast_sqrt_i(vl_uint32 x) 
{
  vl_uint32 temp, g=0, b = 0x8000, bshft = 15 ;
  do {
    if (x >= (temp = (((g << 1) + b)<<bshft--))) {
      g += b;
      x -= temp;
    }
  } while (b >>= 1);
  return g;
}

/* @} */

/* VL_MATHOP */
#endif 
