/** @internal
 ** @file     dhog.c
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (SIFT) - MEX
 **/

/* AUTORIGHTS */

#include <mexutils.h>
#include <vl/mathop.h>
#include <vl/dhog.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_step = 0,
  opt_size,
  opt_fast,
  opt_verbose,
  opt_norm
} ;

/* options */
uMexOption options [] = {
{"Step",         1,   opt_step          },
{"Size",         1,   opt_size          },
{"Verbose",      0,   opt_verbose       },
{"Fast",         0,   opt_fast          },
{"Norm",         0,   opt_norm          },
{0,              0,   0                 }
} ;

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Transpose descriptor
 **
 ** @param dst destination buffer.
 ** @param src source buffer.
 **
 ** The function writes to @a dst the transpose of the SIFT descriptor
 ** @a src. The transpose is defined as the descriptor that one
 ** obtains from computing the normal descriptor on the transposed
 ** image.
 **/

VL_INLINE void 
transpose_descriptor (float* dst, float const* src) 
{
  int const BO = 8 ;  /* number of orientation bins */
  int const BP = 4 ;  /* number of spatial bins     */
  int i, j, t ;
  
  for (j = 0 ; j < BP ; ++j) {
    int jp = BP - 1 - j ;
    for (i = 0 ; i < BP ; ++i) {
      int o  = BO * i + BP*BO * j  ;
      int op = BO * i + BP*BO * jp ;      
      dst [op] = src[o] ;      
      for (t = 1 ; t < BO ; ++t) 
        dst [BO - t + op] = src [t + o] ;
    }
  }
}

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0,IN_END} ;
  enum {OUT_FRAMES=0, OUT_DESCRIPTORS} ;
  
  int                verbose = 0 ;
  int                opt ;
  int                next = IN_END ;
  mxArray const     *optarg ;
  
  float const       *data ;
  int                M, N ;
  
  int                step = 1 ;
  int                size = 3 ;
  vl_bool            fast = 0 ;
  vl_bool            norm = 0 ;
  
  VL_USE_MATLAB_ENV ;
  
  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  
  if (nin < 1) {
    mexErrMsgTxt("One argument required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if (mxGetNumberOfDimensions (in[IN_I]) != 2              ||
      mxGetClassID            (in[IN_I]) != mxSINGLE_CLASS  ) {
    mexErrMsgTxt("I must be a matrix of class SINGLE") ;
  }
  
  data = (float*) mxGetData (in[IN_I]) ;
  M    = mxGetM (in[IN_I]) ;
  N    = mxGetN (in[IN_I]) ;
  
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
        
      case opt_verbose :
        ++ verbose ;
        break ;
        
      case opt_fast :
        fast = 1 ;
        break ;

      case opt_norm :
        norm = 1 ;
        break ;
        
      case opt_size :
        if (!uIsRealScalar(optarg) || (size = (int) *mxGetPr(optarg)) < 0) {
          mexErrMsgTxt("'Size' must be a positive integer.") ;
        }
        break ;
        
      case opt_step :
        if (!uIsRealScalar(optarg) || (step = (int) *mxGetPr(optarg)) < 0) {
          mexErrMsgTxt("'Step' must be a positive integer.") ;
        }
        break ;
        
      default :
        assert(0) ;
        break ;
    }
  }
  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  {
    int nkeys, k, i ;
    VlDhogKeypoint* keys ;
    float* descs ;

    VlDhogFilter *dhog ;    
    dhog = vl_dhog_new (M, N, step, size) ; 
 
    nkeys = vl_dhog_get_keypoint_num (dhog) ;
    keys  = vl_dhog_get_keypoints (dhog) ;
    descs = vl_dhog_get_descriptors (dhog) ;
    
    if (verbose) {
      mexPrintf("dhog: image size:        %d x %d\n", M, N) ;
      mexPrintf("      subsampling step:  %d\n", step) ;
      mexPrintf("      bin size:          %d\n", size) ;
      mexPrintf("      fast descriptors:  %s\n", VL_YESNO(fast)) ;
      mexPrintf("      num. of keypoints: %d\n", nkeys) ;  
    }
    
    vl_dhog_process (dhog, data, fast) ;
    
    /* ---------------------------------------------------------------
     *                                            Create output arrays
     * ------------------------------------------------------------ */
    {
      int dims [2] ;
      
      dims [0] = 128 ;
      dims [1] = nkeys ;
      
      out[OUT_DESCRIPTORS] = mxCreateNumericArray
      (2, dims, mxUINT8_CLASS, mxREAL) ;
      
      if (norm)
        dims [0] = 3 ;
      else
        dims [0] = 2 ;
      
      out[OUT_FRAMES] = mxCreateNumericArray
      (2, dims, mxDOUBLE_CLASS, mxREAL) ;
    }
    
    /* ---------------------------------------------------------------
     *                                                       Copy back
     * ------------------------------------------------------------ */
    {
      double *kpt = mxGetPr(out[OUT_FRAMES]) ;
      vl_uint8  *dpt = mxGetData(out[OUT_DESCRIPTORS]) ;
      for (k = 0 ; k < nkeys ; ++k) {
        float tmpdesc [128] ;
        *kpt++ = keys [k].y + 1 ;
        *kpt++ = keys [k].x + 1 ;
        if (norm)
          /* We have an implied / 2 in the norm, because of the clipping below */
          *kpt++ = keys [k].norm / 2 ;

        vl_dhog_transpose_descriptor (tmpdesc, descs + 128 * k) ;
        for (i = 0 ; i < 128 ; ++i) {
          *dpt++ = (vl_uint8) (VL_MIN(512.0f * tmpdesc[i], 256.0f)) ;
        }
      }
    }
    
    vl_dhog_delete (dhog) ;
  }
}
