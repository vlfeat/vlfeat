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

/** @brief Pi (math constant) */
#define VL_PI 3.141592653589793

/** @brief Single precision epsilon (math constant) 
 **
 **  Difference of the smallest representable number greater
 ** than 1.0 and 1.0.
 **
 **/
#define VL_SINGLE_EPSILON 1.19209290E-07F

/** @brief Double precision epsilon (math constant) 
 **
 ** Difference of the smallest representable number greater
 ** than 1.0 and 1.0.
 **/
#define VL_DOUBLE_EPSILON 2.220446049250313e-16

/** --------------------------------------------------------------- */
/** @{ */
/** @brief Modulo 2 PI
 **
 ** The function is optimized for small values of @a x.
 **
 ** @return @c mod(x, 2 * VL_PI)
 **/
VL_INLINE
vl_single vl_mod_2pi_f (vl_single x)
{
  while (x < 0.0      ) x += (vl_single) (2 * VL_PI);
  while (x > 2 * VL_PI) x -= (vl_single) (2 * VL_PI);
  return x ;
}

VL_INLINE
vl_double vl_mod_2pi_d (vl_double x)
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
vl_floor_f (vl_single x)
{
  int xi = (int) x ;
  if (x >= 0 || (vl_single) xi == x) return xi ;
  else return xi - 1 ;
}

VL_INLINE
int
vl_floor_d (vl_double x)
{
  int xi = (int) x ;
  if (x >= 0 || (vl_double) xi == x) return xi ;
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
vl_single
vl_abs_f (vl_single x)
{
  return (x >= 0) ? x : -x ;
}

VL_INLINE
vl_double
vl_abs_d (vl_double x)
{
  return (x >= 0) ? x : -x ;
}
/* @} */

/** ---------------------------------------------------------------- */
/** @{ */
/** @brief Fast @c atan2 approximation
 ** @param x argument.
 ** @param y argument.
 ** @return Approximation of @c atan2(x).
 **/

VL_INLINE
vl_single
vl_fast_atan2_f (vl_single y, vl_single x)
{
  /*
    The function f(r)=atan((1-r)/(1+r)) for r in [-1,1] is easier to
    approximate than atan(z) for z in [0,inf]. To approximate f(r) to
    the third degree we may solve the system
    
    f(+1) = c0 + c1 + c2 + c3 = atan(0) = 0
    f(-1) = c0 - c1 + c2 - c3 = atan(inf) = pi/2
    f(0)  = c0                = atan(1) = pi/4
    
    which constrains the polynomial to go through the end points and
    the middle point.
    
    We still miss a constrain, which might be simply a constarint on
    the derivative in 0. Instead we minimize the Linf error in the
    range [0,1] by searching for an optimal value of the free
    parameter. This turns out to correspond to the solution
    
    c0=pi/4, c1=-0.9675, c2=0, c3=0.1821
    
    which has maxerr = 0.0061 rad = 0.35 grad.
  */

  vl_single angle, r ;
  vl_single const c3 = 0.1821F ;
  vl_single const c1 = 0.9675F ;
  vl_single abs_y    = vl_abs_f (y) + 1e-10F ;
  
  if (x >= 0) {
    r = (x - abs_y) / (x + abs_y) ;
    angle = (vl_single) (VL_PI / 4) ;
  } else {
    r = (x + abs_y) / (abs_y - x) ;
    angle = (vl_single) (3 * VL_PI / 4) ;
  } 
  angle += (c3*r*r - c1) * r ; 
  return (y < 0) ? - angle : angle ;
}

VL_INLINE
vl_double
vl_fast_atan2_d (vl_double y, vl_double x)
{
  vl_double angle, r ;
  vl_double const c3 = 0.1821 ;
  vl_double const c1 = 0.9675 ;
  vl_double abs_y    = vl_abs_d (y) + 1e-10 ;
  
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
vl_single
vl_fast_resqrt_f (vl_single x)
{
  /* 32-bit version */
  union {
    vl_single x ;
    vl_int32  i ;
  } u ;
  
  vl_single xhalf = (vl_single) 0.5 * x ;

  /* convert floating point value in RAW integer */
  u.x = x ;                   
  
  /* gives initial guess y0 */
  u.i = 0x5f3759df - (u.i >> 1);  
  /*u.i = 0xdf59375f - (u.i>>1);*/
  
  /* two Newton steps */
  u.x = u.x * ( (vl_single) 1.5  - xhalf*u.x*u.x) ; 
  u.x = u.x * ( (vl_single) 1.5  - xhalf*u.x*u.x) ;
  return u.x ;
}

VL_INLINE
vl_double
vl_fast_resqrt_d (vl_double x)
{
  /* 64-bit version */
  union {
    vl_double x ;
    vl_int64  i ;
  } u ;
  
  vl_double xhalf = (vl_double) 0.5 * x ;
  
  /* convert floating point value in RAW integer */
  u.x = x ;                   
  
  /* gives initial guess y0 */
  u.i = 0x5fe6ec85e7de30daLL - (u.i >> 1) ; 
  
  /* two Newton steps */
  u.x = u.x * ( (vl_double) 1.5  - xhalf*u.x*u.x) ; 
  u.x = u.x * ( (vl_double) 1.5  - xhalf*u.x*u.x) ;
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
vl_single
vl_fast_sqrt_f (vl_single x)
{
  return (x < 1e-8) ? 0 : x * vl_fast_resqrt_f (x) ;
}

VL_INLINE
vl_double
vl_fast_sqrt_d (vl_double x)
{
  return (x < 1e-8) ? 0 : x * vl_fast_resqrt_d (x) ;
}
/* @} */

/* VL_MATHOP */
#endif 
