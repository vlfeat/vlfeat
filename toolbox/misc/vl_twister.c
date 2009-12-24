/** @internal
 ** @file    vl_twister.c
 ** @brief   vl_twister - MEX definition
 ** @author  Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

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
    mxuError(vlmxErrInvalidArgument,
             "Too many output arguments");
  }

  mode = RUN_GENERATOR ;
  if (nin > 0 && ! mxIsNumeric(in[0])) {
    mode = MANIP_STATE ;
  }

  switch (mode) {
  case RUN_GENERATOR:
    {
      enum { max_ndims = 30 } ;
      int ndims = 2 ;
      mwSize dims [max_ndims] = {1, 1} ;
      int n, k ;
      double *x ;

      if (nin > 1) {
        /* TWISTER(N1 N2 ...) style */
        if (nin >= max_ndims) {
          mxuError(vlmxErrInvalidArgument,
                   "Too many dimensions") ;
        }
        for (k = 0 ; k < nin ; ++k) {
          if (! vlmxIsPlainScalar(in[k])) {
            mxuError(vlmxErrInvalidArgument,
                     "Invalid arguments") ;
          }
          dims [k] = *mxGetPr(in[k]) ;
        }
        ndims = k ;

      } else if (nin == 1) {
        /* TWISTER([N1 N2 ...]) style */
        if (! vlmxIsPlainVector(in[0], -1)) {
          mxuError(vlmxErrInvalidArgument,
                   "Invalid argument") ;
        }

        x = mxGetPr(in[0]) ;
        n = mxGetNumberOfElements(in[0]) ;
        ndims = VL_MAX(2, n) ;

        if (ndims > max_ndims) {
          mxuError(vlmxErrInvalidArgument,
                   "Invalid argument") ;
        }

        if (n == 1) {
          dims [0] = dims [1] = *x ;
        } else {
          for (k = 0 ; k < n ; ++k) {
            dims [k] = x [k] ;
          }
        }
      }

      /* ensure dims >= 0 */
      for (k = 0 ; k < ndims ; ++k) {
        dims [k] = VL_MAX(0, dims[k]) ;
      }

      out [0] = mxCreateNumericArray (ndims, dims, mxDOUBLE_CLASS, mxREAL) ;
      n = mxGetNumberOfElements (out [0]) ;
      x = mxGetPr (out [0]) ;
      for (k = 0 ; k < n ; ++k) {
        x [k] = vl_rand_res53(rand) ;
      }
    }
    break ;

  case MANIP_STATE:
    {
      enum { buff_size = 32 } ;
      char buff [buff_size] ;

      /* check for 'state' string */
      if (! uIsString(in[0], -1)                    ||
          mxGetString(in[0], buff, buff_size)       ||
          vl_string_casei_cmp ("state", buff) != 0   ) {
        mxuError(vlmxErrInvalidArgument,
                 "Invalid argument") ;
      }

      /* TWISTER('state') */
      if (nin == 1) {
        int i ;
        vl_uint32 * data ;
        out[0] = mxCreateNumericMatrix (625, 1, mxUINT32_CLASS, mxREAL) ;
        data = mxGetData(out[0]) ;
        for (i = 0 ; i < 624 ; ++i) data[i] = rand->mt[i] ;
        data[624] = (vl_uint32) rand->mti ;
      } else {
        if (uIsRealScalar(in[1])) {
          /* TWISTER('state', X) */
          vl_uint32 x = (vl_uint32) mxGetScalar(in[1]) ;
          vl_rand_seed (rand, x) ;
        } else if (mxIsNumeric(in[1])                                &&
                   mxGetClassID(in[1]) == mxUINT32_CLASS             &&
                   mxGetNumberOfElements(in[1]) == 624+1             &&
                   ((vl_uint32 const*)mxGetData(in[1]))[624] <= 624  ) {
          /* TWISTER('state', STATE) */
          int i ;
          vl_uint32 * data = mxGetData(in[1]) ;
          for (i = 0 ; i < 624 ; ++i) rand->mt[i] = data[i] ;
          rand->mti = data [624] ;
        } else if (mxIsNumeric(in[1])                    &&
                   mxGetClassID(in[1]) == mxDOUBLE_CLASS &&
                   mxGetNumberOfElements(in[1]) <= 624) {
          /* TWISTER('state', KEY) */
          vl_uint32 key [624] ;
          double const * x = mxGetPr(in[1]) ;
          int n = mxGetNumberOfElements(in[1]) ;
          int k ;
          for (k = 0 ; k < n ; ++k) {
            key [k] = x [k] ;
          }
          vl_rand_seed_by_array (rand, key, n) ;
        }
      }
    }
    break ;

  default:
    assert(0) ;
  }
}
