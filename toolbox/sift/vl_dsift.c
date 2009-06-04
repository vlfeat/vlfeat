/** @internal
 ** @file     dsift.c
 ** @author   Andrea Vedaldi
 ** @brief    Dense Feature Transform (SIFT) - MEX
 **/

/* AUTORIGHTS */

#include <mexutils.h>
#include <vl/mathop.h>
#include <vl/dsift.h>

#include <math.h>
#include <assert.h>

/* option codes */
enum {
  opt_step = 0,
  opt_bounds,
  opt_size,
  opt_fast,
  opt_verbose,
  opt_norm
} ;

/* options */
uMexOption options [] = {
  {"Bounds",       1,   opt_bounds        },
  {"Step",         1,   opt_step          },
  {"Size",         1,   opt_size          },
  {"Verbose",      0,   opt_verbose       },
  {"Fast",         0,   opt_fast          },
  {"Norm",         0,   opt_norm          },
  {0,              0,   0                 }
} ;

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
  vl_bool            norm = 0 ;

  vl_bool useFlatWindow = VL_FALSE ;
  
  double *bounds = NULL ;
  double boundBuffer [4] ;
  
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
        useFlatWindow = 1 ;
        break ;

      case opt_norm :
        norm = 1 ;
        break ;

      case opt_bounds :
        if (!uIsRealVector(optarg, 4)) {
          mexErrMsgTxt("BOUNDS must be a 4-dimensional vector.") ;
        }
        bounds = boundBuffer ;
        bounds [0] = mxGetPr(optarg)[0] - 1 ;
        bounds [1] = mxGetPr(optarg)[1] - 1 ;
        bounds [2] = mxGetPr(optarg)[2] - 1 ;
        bounds [3] = mxGetPr(optarg)[3] - 1 ;
        break ;

      case opt_size :
        if (!uIsRealScalar(optarg) || (size = (int) *mxGetPr(optarg)) < 0) {
          mexErrMsgTxt("'SIZE must be a positive integer.") ;
        }
        break ;
        
      case opt_step :
        if (!uIsRealScalar(optarg) || (step = (int) *mxGetPr(optarg)) < 0) {
          mexErrMsgTxt("STEP must be a positive integer.") ;
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
    int numFrames ;
    int descrSize ;
    VlDsiftKeypoint const *frames ;
    VlDsiftDescriptorGeometry const *geom ;
    float const *descrs ;
    int k, i ;

    VlDsiftFilter *dsift ;    
    dsift = vl_dsift_new_basic (M, N, step, size) ; 
    if (bounds) {
      vl_dsift_set_bounds(dsift, 
                         VL_MAX(bounds[0], 0),
                         VL_MAX(bounds[1], 0),
                         VL_MIN(bounds[2], M - 1),
                         VL_MIN(bounds[3], N - 1));
    }
    vl_dsift_set_flat_window(dsift, useFlatWindow) ;
    
    numFrames = vl_dsift_get_keypoint_num (dsift) ;
    descrSize = vl_dsift_get_descriptor_size (dsift) ;
    geom = vl_dsift_get_geometry (dsift) ;
    
    if (verbose) {
      int stepX ;
      int stepY ;
      int minX ;
      int minY ;
      int maxX ;
      int maxY ;
      vl_bool useFlatWindow ;
      
      vl_dsift_get_steps (dsift, &stepX, &stepY) ;
      vl_dsift_get_bounds (dsift, &minX, &minY, &maxX, &maxY) ;
      useFlatWindow = vl_dsift_get_flat_window(dsift) ;
      
      mexPrintf("dsift: image size:        %d x %d\n", N, M) ;
      mexPrintf("      bounds:            [%d, %d, %d, %d]\n", minY, minX, maxY, maxX) ;
      mexPrintf("      subsampling steps: %d, %d\n", stepY, stepX) ;
      mexPrintf("      num bins:          [%d, %d, %d]\n", 
                geom->numBinT,
                geom->numBinX, 
                geom->numBinY) ;
      mexPrintf("      descriptor size:   %d\n", descrSize) ;
      mexPrintf("      bin sizes:         [%d, %d]\n", 
                geom->binSizeX, 
                geom->binSizeY) ;
      mexPrintf("      flat window:       %s\n", VL_YESNO(useFlatWindow)) ;
      mexPrintf("      number of frames:  %d\n", numFrames) ;
    }
    
    vl_dsift_process (dsift, data) ;
    
    frames = vl_dsift_get_keypoints (dsift) ;
    descrs = vl_dsift_get_descriptors (dsift) ;
    
    /* ---------------------------------------------------------------
     *                                            Create output arrays
     * ------------------------------------------------------------ */
    {
      mwSize dims [2] ;
      
      dims [0] = descrSize ;
      dims [1] = numFrames ;
      
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
      float *tmpDescr = mxMalloc(sizeof(float) * descrSize) ;
      double *outFrameIter = mxGetPr(out[OUT_FRAMES]) ;
      vl_uint8  *outDescrIter = mxGetData(out[OUT_DESCRIPTORS]) ;
      for (k = 0 ; k < numFrames ; ++k) {
        *outFrameIter++ = frames[k].y + 1 ;
        *outFrameIter++ = frames[k].x + 1 ;
        
        /* We have an implied / 2 in the norm, because of the clipping
           below */
        if (norm)
          *outFrameIter++ = frames [k].norm ;

        vl_dsift_transpose_descriptor (tmpDescr, 
                                      descrs + descrSize * k,
                                      geom->numBinT,
                                      geom->numBinX,
                                      geom->numBinY) ;
        
        for (i = 0 ; i < descrSize ; ++i) {
          *outDescrIter++ = (vl_uint8) (VL_MIN(512.0F * tmpDescr[i], 255.0F)) ;
        }
      }
      mxFree(tmpDescr) ;
    }
    vl_dsift_delete (dsift) ;
  }
}
