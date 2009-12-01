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

  VL_USE_MATLAB_ENV ;  
  
  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  
  if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
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
          mexErrMsgTxt("Too many dimensions.") ;
        }
        for (k = 0 ; k < nin ; ++k) {
          if (! uIsRealScalar(in[k])) {
            mexErrMsgTxt("Invalid arguments.") ;
          }
          dims [k] = *mxGetPr(in[k]) ;
        }
        ndims = k ;

      } else if (nin == 1) {
        /* TWISTER([N1 N2 ...]) style */
        if (! uIsRealVector(in[0], -1)) {
          mexErrMsgTxt("Invalid arguments.") ;
        }

        x     = mxGetPr(in[0]) ;
        n     = mxGetNumberOfElements(in[0]) ;
        ndims = VL_MAX(2, n) ;
        
        if (ndims > max_ndims) {
          mexErrMsgTxt("Too many dimensions.") ;
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
        x [k] = vl_rand_res53() ;
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
        mexErrMsgTxt("Invalid arguments.") ;
      }
      
      /* TWISTER('state') */
      if (nin == 1) {
        out[0] = mxCreateNumericMatrix (625, 1, mxUINT32_CLASS, mxREAL) ;
        vl_rand_get_state (mxGetData(out[0])) ;        
      } else {
        if (uIsRealScalar(in[1])) {
          /* TWISTER('state', X) */
          vl_uint32 x = (vl_uint32) mxGetScalar(in[1]) ;
          vl_rand_seed (x) ;
        } else if (mxIsNumeric(in[1])                                &&
                   mxGetClassID(in[1]) == mxUINT32_CLASS             &&
                   mxGetNumberOfElements(in[1]) == 624+1             &&
                   ((vl_uint32 const*)mxGetData(in[1]))[624] <= 624  ) {  
          /* TWISTER('state', STATE) */
          vl_rand_set_state (mxGetData(in[1])) ;
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
          vl_rand_seed_by_array (key, n) ;
        }
      }
    }
    break ;
    
  default:
    assert(0) ;
  }
}
