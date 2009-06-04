/** @file     mser.c
 ** @author   Andrea Vedaldi
 ** @brief    MSER MEX driver
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include <mexutils.h>
#include <vl/mser.h>
#include <vl/mathop.h>
#include <assert.h>

enum {
  opt_delta = 0,
  opt_max_area,
  opt_min_area,
  opt_max_variation,
  opt_min_diversity,
  opt_verbose
} ;

uMexOption options [] = {
  {"Delta",               1,   opt_delta          },
  {"MaxArea",             1,   opt_max_area       },
  {"MinArea",             1,   opt_min_area       },
  {"MaxVariation",        1,   opt_max_variation  },
  {"MinDiversity",        1,   opt_min_diversity  },
  {"Verbose",             0,   opt_verbose        },
  {0,                     0,   0                  }
} ;

/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I = 0, 
        IN_END } ;
  enum {OUT_SEEDS = 0, 
        OUT_FRAMES } ;
  
  int             verbose = 0 ;
  int             opt ;
  int             next = IN_END ;
  mxArray const  *optarg ;

  /* algorithm parameters */ 
  double   delta         = -1 ;
  double   max_area      = -1 ;
  double   min_area      = -1 ;
  double   max_variation = -1 ; 
  double   min_diversity = -1 ;

  int nel ;              
  int ndims ;            
  mwSize const* dims ; 
     
  vl_mser_pix const *data ; 

  VL_USE_MATLAB_ENV ;
  
  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  
  if (nin < 1) {
    mexErrMsgTxt("At least one input argument is required.") ;
  }
  
  if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if(mxGetClassID(in[IN_I]) != mxUINT8_CLASS) {
    mexErrMsgTxt("I must be of class UINT8") ;
  }

  /* get dimensions */
  nel   = mxGetNumberOfElements(in[IN_I]) ;
  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims  = mxGetDimensions(in[IN_I]) ;
  data  = mxGetData(in[IN_I]) ;

  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

    case opt_verbose :
      ++ verbose ;
      break ;

    case opt_delta :
      if (!uIsRealScalar(optarg) || (delta = *mxGetPr(optarg)) < 0) {
        mexErrMsgTxt("'Delta' must be non-negative.") ;
      }
      break ;

    case opt_max_area : 
      if (!uIsRealScalar(optarg)            || 
          (max_area = *mxGetPr(optarg)) < 0 ||
          max_area > 1) {
        mexErrMsgTxt("'MaxArea' must be in the range [0,1].") ;
      }
      break ;

    case opt_min_area : 
      if (!uIsRealScalar(optarg)            || 
          (min_area = *mxGetPr(optarg)) < 0 ||
          min_area > 1) {
        mexErrMsgTxt("'MinArea' must be in the range [0,1].") ;
      }
      break ;

    case opt_max_variation : 
      if (!uIsRealScalar(optarg)           || 
          (max_variation = *mxGetPr(optarg)) < 0) {
        mexErrMsgTxt("'MaxVariation' must be non negative.") ;
      }
      break ;

    case opt_min_diversity : 
      if (!uIsRealScalar(optarg)                 || 
          (min_diversity = *mxGetPr(optarg)) < 0 ||
           min_diversity > 1.0) {
        mexErrMsgTxt("'MinDiversity' must be in the [0,1] range.") ;
      }
      break ;
      
      
    default :
        assert(0) ;
        break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                     Run algorithm
   * -------------------------------------------------------------- */
  {
    VlMserFilt        *filt ;    
    vl_uint     const *regions ;
    float       const *frames ;    
    int                i, j, nregions, nframes, dof ;    
    mwSize             odims [2] ;
    double            *pt ;
    
    /* new filter */
    {
      int * vlDims = mxMalloc(sizeof(int) * ndims) ;
      for (i = 0 ; i < ndims ; ++i) vlDims [i] = dims [i] ;
      filt = vl_mser_new (ndims, vlDims) ;
      mxFree(vlDims) ;
    }

    if (!filt) {
      mexErrMsgTxt("Could not create an MSER filter.") ;
    }
    
    if (delta         >= 0) vl_mser_set_delta          (filt, (vl_mser_pix) delta) ;
    if (max_area      >= 0) vl_mser_set_max_area       (filt, max_area) ;
    if (min_area      >= 0) vl_mser_set_min_area       (filt, min_area) ;
    if (max_variation >= 0) vl_mser_set_max_variation  (filt, max_variation) ;
    if (min_diversity >= 0) vl_mser_set_min_diversity  (filt, min_diversity) ;

    if (verbose) {
      mexPrintf("mser: parameters:\n") ;
      mexPrintf("mser:   delta         = %d\n", vl_mser_get_delta         (filt)) ;
      mexPrintf("mser:   max_area      = %g\n", vl_mser_get_max_area      (filt)) ;
      mexPrintf("mser:   min_area      = %g\n", vl_mser_get_min_area      (filt)) ;
      mexPrintf("mser:   max_variation = %g\n", vl_mser_get_max_variation (filt)) ;
      mexPrintf("mser:   min_diversity = %g\n", vl_mser_get_min_diversity (filt)) ;
    }
    
    /* process the image */
    vl_mser_process (filt, data) ;
    
    /* save regions back to array */
    nregions         = vl_mser_get_regions_num (filt) ;
    regions          = vl_mser_get_regions     (filt) ;
    odims [0]        = nregions ;
    out [OUT_SEEDS] = mxCreateNumericArray (1, odims, mxDOUBLE_CLASS,mxREAL) ;
    pt               = mxGetPr (out [OUT_SEEDS]) ;

    for (i = 0 ; i < nregions ; ++i) 
      pt [i] = regions [i] + 1 ;

    /* optionally compute and save ellipsoids */
    if (nout > 1) {
      vl_mser_ell_fit (filt) ;
      
      nframes = vl_mser_get_ell_num (filt) ;
      dof     = vl_mser_get_ell_dof (filt) ;
      frames  = vl_mser_get_ell     (filt) ;
      
      odims [0] = dof ;
      odims [1] = nframes ;
      
      out [OUT_FRAMES] = mxCreateNumericArray (2, odims, mxDOUBLE_CLASS, mxREAL) ;
      pt               = mxGetPr (out [OUT_FRAMES]) ;
      
      for (i = 0 ; i < nframes ; ++i) {
        for (j = 0 ; j < dof ; ++j) {
          pt [i * dof + j] = frames [i * dof + j] + ((j < ndims)?1.0:0.0) ;
        }
      }
    }

    if (verbose) {
      VlMserStats const* s = vl_mser_get_stats (filt) ;
      int tot = s-> num_extremal ;

      mexPrintf("mser: statistics:\n") ;
      mexPrintf("mser: %d extremal regions of which\n", tot) ;

#define REMAIN(test,num)                                                \
      mexPrintf("mser:  %5d (%7.3g %% of previous) " test "\n",         \
                tot-(num),100.0*(double)(tot-(num))/(tot+VL_EPSILON_D)) ; \
      tot -= (num) ;
      
      REMAIN("maximally stable,", s-> num_unstable    ) ;
      REMAIN("stable enough,",    s-> num_abs_unstable) ;
      REMAIN("small enough,",     s-> num_too_big     ) ;
      REMAIN("big enough,",       s-> num_too_small   ) ;
      REMAIN("diverse enough.",    s-> num_duplicates  ) ;
      
    }

    /* cleanup */
    vl_mser_delete (filt) ;
  }
}
