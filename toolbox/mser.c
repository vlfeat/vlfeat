/** @file     mser.mser.c
 ** @author   Andrea Vedaldi
 ** @brief    MSER MEX driver
 **/

#include "mexutils.h"

#include <vl/mser.h>

#include <assert.h>

enum {
  opt_delta = 0,
  opt_epsilon,
  opt_max_area,
  opt_min_area,
  opt_max_var,
  opt_dups,
  opt_no_dups,
  opt_verbose
} ;
]
uMexOption options [] = {
  {"Delta",               1,   opt_delta    },
  {"Epsilon",             1,   opt_epsilon  },
  {"MaxArea",             1,   opt_max_area },
  {"MinArea",             1,   opt_min_area },
  {"MaxVariation",        1,   opt_max_var  },
  {"Dups",                0,   opt_dups     },
  {"NoDups",              0,   opt_no_dups  },
  {"Verbose",             0,   opt_verbose  },
  {0,                     0,   0            }
} ;


/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_END} ;
  enum {OUT_PIVOTS=0, OUT_FRAMES} ;

  int                verbose = 0 ;
  int                opt ;
  int                next = IN_END ;
  mxArray const     *optarg ;

  /* algorithm parameters */ 
  double   delta    = -1 ;
  double   epsilon  = -1 ;
  int      no_dups  = -1 ;
  double   max_area = -1 ;
  double   min_area = -1 ;
  double   max_var  = -1 ;

  int nel ;              
  int ndims ;            
  int const* dims ; 
     
  vl_mser_pix const *data ; 
  
  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
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

    case opt_epsilon :
      if (!uIsRealScalar(optarg) || (epsilon = *mxGetPr(optarg)) < 0) {
        mexErrMsgTxt("'Epsilon' must be non-negative.") ;
      }
      break ;

    case opt_max_area : 
      if (!uIsRealScalar(optarg)           || 
          (max_area = *mxGetPr(optarg)) < 0 ||
          max_area > 1) {
        mexErrMsgTxt("'MaxArea' must be in the range [0,1].") ;
      }
      break ;

    case opt_min_area : 
      if (!uIsRealScalar(optarg)           || 
          (min_area = *mxGetPr(optarg)) < 0 ||
          min_area > 1) {
        mexErrMsgTxt("'MinArea' must be in the range [0,1].") ;
      }
      break ;

    case opt_max_var : 
      if (!uIsRealScalar(optarg)           || 
          (max_var = *mxGetPr(optarg)) < 0 ||
          max_var > 1) {
        mexErrMsgTxt("'MaxVariation' must be in the range [0,1].") ;
      }
      break ;

    case opt_dups : 
      no_dups = 0 ;
      break ;

    case opt_no_dups : 
      no_dups = 1 ;
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
    vl_single   const *frames ;    
    int                i, j, nregions, nframes, dof ;    
    int                odims [2] ;
    double            *pt ;
    
    /* new filter */
    filt = vl_mser_new (ndims, dims) ;

    if (delta    >= 0) vl_mser_set_delta         (filt, (vl_mser_pix) delta   ) ;
    if (epsilon  >= 0) vl_mser_set_epsilon       (filt, epsilon ) ;
    if (max_area >= 0) vl_mser_set_max_area      (filt, max_area) ;
    if (min_area >= 0) vl_mser_set_min_area      (filt, min_area) ;
    if (max_var  >= 0) vl_mser_set_max_var       (filt, max_var ) ;
    if (no_dups  >= 0) vl_mser_set_no_dups       (filt, no_dups ) ;

    if (verbose) {
      mexPrintf("mser: filter settings:\n") ;
      mexPrintf("mser:  delta    = %d\n", vl_mser_get_delta    (filt) ) ;
      mexPrintf("mser:  epsilon  = %g\n", vl_mser_get_epsilon  (filt) ) ;
      mexPrintf("mser:  max_area = %g\n", vl_mser_get_max_area (filt) ) ;
      mexPrintf("mser:  min_area = %g\n", vl_mser_get_min_area (filt) ) ;
      mexPrintf("mser:  max_var  = %g\n", vl_mser_get_max_var  (filt) ) ;
      mexPrintf("mser:  no_dups  = %d\n", vl_mser_get_no_dups  (filt) ) ;
    }
    
    /* process image */
    vl_mser_process (filt, data) ;
    
    /* save regions back to array */
    nregions         = vl_mser_get_regions_num (filt) ;
    regions          = vl_mser_get_regions     (filt) ;
    odims [0]        = nregions ;
    out [OUT_PIVOTS] = mxCreateNumericArray (1, odims, mxDOUBLE_CLASS,mxREAL) ;
    pt               = mxGetPr (out [OUT_PIVOTS]) ;

    for (i = 0 ; i < nregions ; ++i) 
      pt [i] = regions [i] + 1 ;

    /* optionally compute and save frames */
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

#define REMAIN(test,num)                                                \
      mexPrintf("mser:   " test " %5d (%5.2g %%)\n",tot-(num),(double)(tot-(num))/tot) ; \
      tot -= (num) ;
      
      mexPrintf("mser: filter statistics:\n") ;
      mexPrintf("mser:       extremal : %d\n", tot) ;
      REMAIN("  max stable :", s->num_unstable) ;
      REMAIN("small enough :", s->num_too_big) ;
      REMAIN("  big enough :", s->num_too_small) ;
      REMAIN("   abs stable:", s->num_abs_unstable) ;
    }

    /* cleanup */
    vl_mser_delete (filt) ;
  }

}
