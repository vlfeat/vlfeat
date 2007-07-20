/** @file     mser.mser.c
 ** @author   Andrea Vedaldi
 ** @brief    MSER MEX driver
 **/

#include "mexutils.h"
#include <vl/mser.h>

/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_DELTA} ;
  enum {OUT_PIVOTS=0, OUT_FRAMES} ;

  int verbose = 1 ;
  double delta ;   

  int nel ;              
  int ndims ;            
  int const* dims ; 
     
  vl_mser_pix const *data ; 
  
  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  if (nin != 2) {
    mexErrMsgTxt("Two arguments required.") ;
  } else if (nout > 4) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if(mxGetClassID(in[IN_I]) != mxUINT8_CLASS) {
    mexErrMsgTxt("I must be of class UINT8") ;
  }

  if(!uIsRealScalar(in[IN_DELTA])) {
    mexErrMsgTxt("DELTA must be a real scalar") ;
  }
  delta = *mxGetPr(in[IN_DELTA])  ;
  if(delta < 0.0) {
    mexErrMsgTxt("DELTA must be non-negative") ;
  }

  /* get dimensions */
  nel   = mxGetNumberOfElements(in[IN_I]) ;
  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims  = mxGetDimensions(in[IN_I]) ;
  data  = mxGetData(in[IN_I]) ;

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

    vl_mser_set_delta (filt, delta) ;

    if (verbose) {
      mexPrintf("mser: filter settings:\n") ;
      mexPrintf("mser:  delta    = %d\n", vl_mser_get_delta    (filt) ) ;
      mexPrintf("mser:  epsilon  = %g\n", vl_mser_get_epsilon  (filt) ) ;
      mexPrintf("mser:  max_area = %g\n", vl_mser_get_max_area (filt) ) ;
      mexPrintf("mser:  min_area = %g\n", vl_mser_get_min_area (filt) ) ;
      mexPrintf("mser:  max_var  = %g\n", vl_mser_get_max_var  (filt) ) ;
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
