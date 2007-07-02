/** @file     sift.mser.c
 ** @author   Andrea Vedaldi
 ** @brief    SIFT MEX driver
 ** @internal
 **/

#include "mexutils.h"
#include <vl/mathop.h>
#include <vl/sift.h>
#include <math.h>
#include <assert.h>

enum {
  opt_octaves = 0,
  opt_levels,
  opt_first_octave,
  opt_verbose 
} ;

uMexOption options [] = {
  {"Octaves",      1,   opt_octaves       },
  {"Levels",       1,   opt_levels        },
  {"FirstOctave",  1,   opt_first_octave  },
  {"Verbose",      0,   opt_verbose       }
} ;

/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0,IN_END} ;
  enum {OUT_FRAMES=0, OUT_DESCRIPTORS} ;

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;
     
  vl_sift_pix const *data ;
  int M, N ;

  int O    = - 1 ;
  int S    =   3 ;
  int omin =   0 ;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  if (nin < 1) {
    mexErrMsgTxt("One argument required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if (mxGetNumberOfDimensions (in[IN_I]) != 2             ||
      mxGetClassID            (in[IN_I]) != mxSINGLE_CLASS ) {
    mexErrMsgTxt("I must be a matrix of class SINGLE") ;
  }
  
  data = (vl_sift_pix*) mxGetData (in[IN_I]) ;
  M    = mxGetM (in[IN_I]) ;
  N    = mxGetN (in[IN_I]) ;
  
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_verbose :
      ++ verbose ;
      break ;

    case opt_octaves :
      if (!uIsRealScalar(optarg)) {
        mexErrMsgTxt("'Octaves' must be a positive integer.") ;
      }
      O = *mxGetPr(optarg) ;
      break ;
      
    case opt_levels :
      if (!uIsRealScalar(optarg)) {
        mexErrMsgTxt("'Levels' must be a positive integer.") ;
      }
      S = *mxGetPr(optarg) ;
      break ;
      
    case opt_first_octave :
      if (!uIsRealScalar(optarg)) {
        mexErrMsgTxt("'FirstOctave' must be a positive integer.") ;
      }
      omin = *mxGetPr(optarg) ;
      break ;
      
    default :
        assert(0) ;
        break ;
    }
  }

  if (verbose) {    
    mexPrintf("sift: settings:\n") ;
    mexPrintf("sift:   octaves (O)         = %d\n", O) ;
    mexPrintf("sift:   levels (S)          = %d\n", S) ;
    mexPrintf("sift:   first octave (omin) = %d\n", omin) ;
  }
  
  /* -----------------------------------------------------------------
   *                                                     Run algorithm
   * -------------------------------------------------------------- */
  {
    VlSiftFilt        *filt ;    
    int                first  = 1 ;
    double            *frames = 0 ;
    vl_uint8          *descr  = 0 ;
    int                nframes = 0, reserved = 0, i,j,q ;
    
    /* create a filter to process the image */
    filt = vl_sift_new (M, N, O, S, omin) ;
    
    /* ...............................................................
     *                                             process each octave
     * ............................................................ */

    while (true) {
      int                   err ;
      VlSiftKeypoint const *keys ;
      int                   nkeys ;
      
      if (verbose) {
        mexPrintf ("sift: processing next octave\n") ;
      }

      /* calculate the GSS for the next octave .................... */
      if (first) {
        err = vl_sift_process_first_octave (filt, data) ;
        first = 0 ;
      } else {
        err = vl_sift_process_next_octave (filt) ;
      }        

      if (err) break ;

      /* run detector ............................................. */
      vl_sift_detect (filt) ;

      keys  = vl_sift_get_keypoints (filt) ;
      nkeys = vl_sift_get_keypoints_num (filt) ;

      /* for each keypoint ........................................ */
      for (i = 0 ; i < nkeys ; ++i) {
        double angles [4] ;
        int    nangles ;

        /* obtain keypoint orientations ........................... */
        nangles = vl_sift_calc_keypoint_orientations 
          (filt, angles, keys + i) ;

        /* for each orientation ................................... */
        for (q = 0 ; q < nangles ; ++q) {
          vl_sift_pix buf [128] ;
          vl_sift_calc_keypoint_descriptor (filt, buf, keys + i, angles [q]) ;

          /* make enough room for all these keypoints */
          if (reserved < nframes + 1) {
            reserved += 2 * nkeys ;
            frames = mxRealloc (frames,   4 * sizeof(double) * reserved) ;
            descr  = mxRealloc (descr,  128 * sizeof(double) * reserved) ;
          }

          /* Save back with MATLAB conventions. Notice tha the input image was
           * the transpose of the actual image. */
          frames [4 * nframes + 0] = keys [i] .y + 1 ;
          frames [4 * nframes + 1] = keys [i] .x + 1 ;
          frames [4 * nframes + 2] = keys [i] .sigma ;
          frames [4 * nframes + 3] = VL_PI / 2  - angles [q] ;
          
          for (j = 0 ; j < 128 ; ++j) {
            descr [128 * nframes + j] = (vl_uint8) 512.0 * buf [j] ;
          }

          ++ nframes ;
        } /* next orientation */
      } /* next keypoint */
    } /* next octave */
    
    if (verbose) {
      mexPrintf ("sift: found %d keypoints\n", nframes) ;
    }
    
    /* save back */
    {
      int dims [2] ;
      
      /* empty array */
      dims [0] = 0 ;
      dims [1] = 0 ;      
      out[OUT_FRAMES]      = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL) ;
      out[OUT_DESCRIPTORS] = mxCreateNumericArray(2, dims, mxUINT8_CLASS,  mxREAL) ;

      /* set to our stuff */
      dims [0] = 4 ;
      dims [1] = nframes ;
      mxSetDimensions (out[OUT_FRAMES],      dims, 2) ;
      mxSetPr         (out[OUT_FRAMES],      frames) ;

      dims [0] = 128 ;
      dims [1] = nframes ;
      mxSetDimensions (out[OUT_DESCRIPTORS], dims, 2) ;
      mxSetData       (out[OUT_DESCRIPTORS], descr) ;
    }
    
    /* cleanup */
    vl_sift_delete (filt) ;
  }
}
