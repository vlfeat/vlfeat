/** @internal
 ** @file    vl_twister.c
 ** @brief   vl_twister - MEX definition
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/random.h>
#include <vl/stringop.h>

#include <assert.h>

/* driver */
void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {
    MANIP_STATE,
    RUN_GENERATOR
  } mode ;

  VlRand * rand ;

  VL_USE_MATLAB_ENV ;

  rand = vl_get_rand() ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */

  if (nout > 1) {
    vlmxError(vlmxErrTooManyInputArguments, NULL) ;
  }

  if (nin > 0 && ! mxIsNumeric(in[0])) {
    mode = MANIP_STATE ;
  } else {
    mode = RUN_GENERATOR ;
  }

  switch (mode) {
  case RUN_GENERATOR:
    {
      enum { maxNumDimensions = 30 } ;
      vl_size numDimensions = 2, n ;
      vl_uindex k ;
      mwSize dimensions [maxNumDimensions] = {1, 1} ;
      double * x ;

      if (nin > 1) {
        /* TWISTER(N1 N2 ...) style */
        if (nin >= maxNumDimensions) {
          vlmxError(vlmxErrTooManyInputArguments,
                    "Too many dimensions specified.") ;
        }
        for (k = 0 ; k < (unsigned)nin ; ++k) {
          if (! vlmxIsPlainScalar(in[k])) {
            vlmxError(vlmxErrInvalidArgument,
                     "The %d-th argument is not a plain scalar.", k + 1) ;
          }
          if (mxGetScalar(in[k]) < 0) {
            vlmxError(vlmxErrInvalidArgument,
                      "The %d-th argument is negative.", k + 1) ;
          }
          dimensions[k] = mxGetScalar(in[k]) ;
        }
        numDimensions = k ;

      } else if (nin == 1) {
        /* TWISTER([N1 N2 ...]) style */
        if (! vlmxIsPlainVector(in[0], -1)) {
          vlmxError(vlmxErrInvalidArgument,
                   "The argument is not a plain vector.") ;
        }

        x = mxGetPr(in[0]) ;
        n = mxGetNumberOfElements(in[0]) ;
        numDimensions = VL_MAX(2, n) ;

        if (numDimensions > maxNumDimensions) {
          vlmxError(vlmxErrInvalidArgument,
                   "Too many dimensions specified.") ;
        }

        if (n == 1) {
          if (*x < 0) {
            vlmxError(vlmxErrInvalidArgument,
                      "The specified dimension is negative.") ;
          }
          dimensions[0] = dimensions[1] = *x ;
        } else {
          for (k = 0 ; k < n ; ++k) {
            if (x[k] < 0) {
              vlmxError(vlmxErrInvalidArgument,
                        "One of the specified dimensions is negative.") ;
            }
            dimensions[k] = x[k] ;
          }
        }
      }

      out[0] = mxCreateNumericArray (numDimensions, dimensions, mxDOUBLE_CLASS, mxREAL) ;
      n = mxGetNumberOfElements (out[0]) ;
      x = mxGetPr (out[0]) ;
      for (k = 0 ; k < n ; ++k) {
        x[k] = vl_rand_res53(rand) ;
      }
    }
    break ;

  case MANIP_STATE:
    {
      enum { buff_size = 32 } ;
      char buff [buff_size] ;

      /* check for 'state' string */
      if (! vlmxIsString(in[0], -1)                 ||
          mxGetString(in[0], buff, buff_size)       ||
          vl_string_casei_cmp ("state", buff) != 0   ) {
        vlmxError(vlmxErrInvalidArgument, NULL) ;
      }

      /* TWISTER('state') */
      if (nin == 1) {
        vl_uindex i ;
        vl_uint32 * data ;
        out[0] = mxCreateNumericMatrix (625, 1, mxUINT32_CLASS, mxREAL) ;
        data = mxGetData(out[0]) ;
        for (i = 0 ; i < 624 ; ++i) data[i] = rand->mt[i] ;
        data[624] = (vl_uint32) rand->mti ;
      } else {
        if (vlmxIsPlainScalar(in[1])) {
          /* TWISTER('state', X) */
          vl_uint32 x = (vl_uint32) mxGetScalar(in[1]) ;
          vl_rand_seed (rand, x) ;
        } else if (mxIsNumeric(in[1])                                &&
                   mxGetClassID(in[1]) == mxUINT32_CLASS             &&
                   mxGetNumberOfElements(in[1]) == 624+1             &&
                   ((vl_uint32 const*)mxGetData(in[1]))[624] <= 624  ) {
          /* TWISTER('state', STATE) */
          vl_uindex i ;
          vl_uint32 * data = mxGetData(in[1]) ;
          for (i = 0 ; i < 624 ; ++i) rand->mt[i] = data[i] ;
          rand->mti = data [624] ;
        } else if (mxIsNumeric(in[1])                    &&
                   mxGetClassID(in[1]) == mxDOUBLE_CLASS &&
                   mxGetNumberOfElements(in[1]) <= 624) {
          /* TWISTER('state', KEY) */
          vl_uint32 key [624] ;
          double const * x = mxGetPr(in[1]) ;
          vl_size n = mxGetNumberOfElements(in[1]) ;
          vl_uindex k ;
          for (k = 0 ; k < n ; ++k) {
            key [k] = x [k] ;
          }
          vl_rand_seed_by_array (rand, key, n) ;
        }
      }
    }
    break ;

  default:
    abort() ;
  }
}
