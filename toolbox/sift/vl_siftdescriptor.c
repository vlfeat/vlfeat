/** @internal
 ** @file     siftdescriptor.c
 ** @author   Andrea Vedaldi
 ** @brief    SIFT descriptor - MEX
 **/

#include <mexutils.h>
#include <vl/mathop.h>
#include <vl/sift.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_magnif,
  opt_verbose 
} ;

/* options */
uMexOption options [] = {
  {"Magnif",       1,   opt_magnif        },
  {"Verbose",      0,   opt_verbose       },
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
transpose_descriptor (vl_sift_pix* dst, vl_sift_pix* src) 
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
  enum {IN_GRAD=0,IN_FRAMES,IN_END} ;
  enum {OUT_DESCRIPTORS} ;

  int                verbose = 0 ;
  int                opt ;
  int                next = IN_END ;
  mxArray const     *optarg ;
     
  mxArray           *grad_array ;
  vl_sift_pix       *grad ;
  int                M, N ;

  double             magnif = -1 ;
  double            *ikeys = 0 ;
  int                nikeys = 0 ;

  int i,j ;
  
  VL_USE_MATLAB_ENV ;
  
  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    mexErrMsgTxt("Two arguments required.") ;
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if (mxGetNumberOfDimensions (in[IN_GRAD])    != 3              ||
      mxGetClassID            (in[IN_GRAD])    != mxSINGLE_CLASS ||
      mxGetDimensions         (in[IN_GRAD])[0] != 2              ) {
    mexErrMsgTxt("GRAD must be a 2xMxN matrix of class SINGLE.") ;
  }

  if (!uIsRealMatrix(in[IN_FRAMES], 4, -1)) {
    mexErrMsgTxt("FRAMES must be a 4xN matrix.") ;
  }
  nikeys = mxGetN (in[IN_FRAMES]) ;
  ikeys  = mxGetPr(in[IN_FRAMES]) ;
  
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
        
      case opt_verbose :
        ++ verbose ;
        break ;
        
      case opt_magnif :
        if (!uIsRealScalar(optarg) || (magnif = *mxGetPr(optarg)) < 0) {
          mexErrMsgTxt("MAGNIF must be a non-negative scalar.") ;
        }
        break ;
        
      default :
        assert(0) ;
        break ;
    }
  }
  
  grad_array = mxDuplicateArray(in[IN_GRAD]) ;  
  grad = (vl_sift_pix*) mxGetData (grad_array) ;
  M    = mxGetDimensions(in[IN_GRAD])[1] ;
  N    = mxGetDimensions(in[IN_GRAD])[2] ;

  /* transpose angles */
  for (i = 1 ; i < 2*M*N ; i+=2) {
    grad [i] = VL_PI/2 - grad [i] ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  {
    VlSiftFilt *filt = 0 ;    
    vl_uint8   *descr = 0 ;

    /* create a filter to process the image */
    filt = vl_sift_new (M, N, -1, -1, 0) ;
    
    if (magnif >= 0) vl_sift_set_magnif (filt, magnif) ;
    
    if (verbose) {    
      mexPrintf("siftdescriptor: filter settings:\n") ;
      mexPrintf("siftdescriptor:   magnif                = %g\n",
                vl_sift_get_magnif (filt)) ;
      mexPrintf("siftdescriptor:   num of frames         = %d\n",
                nikeys) ;
    }

    {
      mwSize dims [2] ;
      dims [0] = 128 ;
      dims [1] = nikeys ;
      out[OUT_DESCRIPTORS]= mxCreateNumericArray 
        (2, dims, mxUINT8_CLASS,  mxREAL) ;
      descr = mxGetData(out[OUT_DESCRIPTORS]) ;
    }
    
    /* ...............................................................
     *                                             Process each octave
     * ............................................................ */
    for (i = 0 ; i < nikeys ; ++i) {
      vl_sift_pix  buf [128], rbuf [128] ;

      double y  = *ikeys++ - 1 ;
      double x  = *ikeys++ - 1 ;
      double s  = *ikeys++ ;
      double th = VL_PI / 2 - *ikeys++ ;

      vl_sift_calc_raw_descriptor (filt, 
                                   grad, 
                                   buf,
                                   M, N,
                                   x, y, s, th) ;
      
      transpose_descriptor (rbuf, buf) ;
      
      for (j = 0 ; j < 128 ; ++j) {
        double x = 512.0 * rbuf [j] ;
        x = (x < 255.0) ? x : 255.0 ;
        *descr++ = (vl_uint8) (x) ;
      }
    }    
    /* cleanup */
    mxDestroyArray (grad_array) ;
    vl_sift_delete (filt) ;
  } /* job done */    
}
