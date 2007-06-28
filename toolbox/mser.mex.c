/** @file     mser.c
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
  int ner = 0 ;          
  int nmer = 0 ;         
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
    
    /* process image */
    vl_mser_process (filt, data) ;
    
    /* save regions back to array */
    nregions         = vl_mser_get_num_regions (filt) ;
    regions          = vl_mser_get_regions     (filt) ;
    odims [0]        = nregions ;
    out [OUT_PIVOTS] = mxCreateNumericArray (1, odims, mxUINT32_CLASS,mxREAL) ;
    pt               = mxGetPr (out [OUT_PIVOTS]) ;

    for (i = 0 ; i < nregions ; ++i) pt [i] = regions [i] ;

    /* optionally compute and save frames */
    if (nout >= 1) {
      vl_mser_fit_ell (filt) ;
      
      nframes = vl_mser_get_num_ell (filt) ;
      dof     = vl_mser_get_dof_ell (filt) ;
      frames  = vl_mser_get_ell     (filt) ;
      
      odims [0] = dof ;
      odims [1] = nframes ;
      
      out [OUT_FRAMES] = mxCreteNumericArray (2, odims, mxDOUBLE_CLASS) ;
      pt               = mxGetPr (out [OUT_FRAMES]) ;
      
      for (i = 0 ; i < nframes ; ++i) {
        for (j = 0 ; j < dof ; ++j) {
          pt [i * dof + j] = frames [i * dof + j] + ((j < ndims)?1.0:0.0) ;
        }
      }
    }

    /* cleanup */
    vl_mser_delete (filt) ;
  }

}
