/** @file    random.c
 ** @author  Andrea Vedaldi
 ** @brief   Random number generator - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file random.h
 ** @author Andrea Vedaldi
 ** @brief  Random number generator
 
 This module implements the popular Mersenne Twister algorithm (MATLAB
 random generator from version 7.4).
**/

#include "random.h"

/* 
A C-program for MT19937, with initialization improved 2002/1/26.
Coded by Takuji Nishimura and Makoto Matsumoto.

Before using, initialize the state by using init_genrand(seed)  
or init_by_array(init_key, key_length).

Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
All rights reserved.                          

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. The names of its contributors may not be used to endorse or promote 
products derived from this software without specific prior written 
permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Any feedback is very welcome.
http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <stdio.h>

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */

/** @brief Read the state of the random number generator
 ** @param state pointer to an array of 625 numbers.
 **/

VL_EXPORT
void vl_rand_get_state (vl_uint32 state [625])
{
  int k ;
  for (k = 0 ; k < 624 ; ++k) state [k] = mt [k] ;
  state [k] = mti ; 
}

/** @brief Write the state of the random number generator
 ** @param state pointer to an array of 625 numbers.
 **/

VL_EXPORT
void vl_rand_set_state (vl_uint32 const state [625])
{
  int k ;
  for (k = 0 ; k < 624 ; ++k) mt [k] = state [k] ;
  mti = VL_MIN (state [624], 624) ;
}

/** @brief Seed the state of the random number generator
 **
 ** @param s seed.
 **/

VL_EXPORT
void vl_rand_seed (vl_uint32 s)
{
  mt[0]= s & 0xffffffffUL;
  for (mti=1; mti<N; mti++) {
    mt[mti] = 
      (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt[mti] &= 0xffffffffUL;
    /* for >32 bit machines */
  }
}

/** @brief Seed the state of the random number generator by an array
 **
 ** @param init_key    array of numbers.
 ** @param key_length  length of the array.
 **/

VL_EXPORT
void vl_rand_seed_by_array (vl_uint32 const init_key [], int key_length)
{
  int i, j, k;
  vl_rand_seed (19650218UL);
  i=1; j=0;
  k = (N>key_length ? N : key_length);
  for (; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
      + init_key[j] + j; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++; j++;
    if (i>=N) { mt[0] = mt[N-1]; i=1; }
    if (j>=key_length) j=0;
  }
  for (k=N-1; k; k--) {
    mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
      - i; /* non linear */
    mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
    i++;
    if (i>=N) { mt[0] = mt[N-1]; i=1; }
  }

  mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/** @brief Generate a random UINT32 
 ** @return a random number in [0, 0xffffffff]. 
 **/

VL_EXPORT
vl_uint32 vl_rand_uint32 ()
{
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (mti >= N) { /* generate N words at one time */
    int kk;

    if (mti == N+1)   /* if init_genrand() has not been called, */
      vl_rand_seed (5489UL); /* a default initial seed is used */

    for (kk=0;kk<N-M;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<N-1;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

    mti = 0;
  }
  
  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/** @brief Generate a random INT31
 ** @return a random number in [0, 0x7fffffff].
 **/

VL_EXPORT
vl_int32 vl_rand_int31 ()
{
  return (vl_int32)(vl_rand_uint32()>>1);
}

/** @brief Generate a random number in [0,1]
 ** @return a random number.
 **/

VL_EXPORT
double vl_rand_real1 ()
{
  return vl_rand_uint32()*(1.0/4294967295.0); 
  /* divided by 2^32-1 */ 
}

/** @brief Generate a random number in [0,1)
 ** @return a random number.
 **/

VL_EXPORT
double vl_rand_real2 ()
{
  return vl_rand_uint32()*(1.0/4294967296.0); 
  /* divided by 2^32 */
}

/** @brief Generate a random number in (0,1)
 ** @return a random number.
 **/

VL_EXPORT
double vl_rand_real3 ()
{
  return (((double)vl_rand_uint32()) + 0.5)*(1.0/4294967296.0); 
  /* divided by 2^32 */
}

/** @brief Generate a random number in [0,1) with 53-bit resolution
 ** @return a random number. 
 **/

VL_EXPORT
double vl_rand_res53 () 
{ 
  vl_uint32 
    a = vl_rand_uint32() >> 5, 
    b = vl_rand_uint32() >> 6 ; 
  return (a * 67108864.0 + b) * (1.0 / 9007199254740992.0) ; 
} 
