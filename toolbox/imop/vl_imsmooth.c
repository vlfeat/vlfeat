/** @internal
 ** @file   imsmooth.c
 ** @author Andrea Vedaldi
 ** @brief  Smooth an image - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifdef VL_IMSMOOTH_INSTANTIATING

#include <vl/float.th>
#include <vl/host.h>

#if (FLT == VL_TYPE_FLOAT)
#define IMCONVCOL    vl_imconvcol_vf
#define IMCONVCOLTRI vl_imconvcoltri_f
#else
#define IMCONVCOL    vl_imconvcol_vd
#define IMCONVCOLTRI vl_imconvcoltri_d
#endif

VL_INLINE void
VL_XCAT(_vl_imsmooth_smooth_, SFX)
(T * outputImage,
 vl_size numOutputRows,
 vl_size numOutputColumns,
 T const * inputImage,
 vl_size numRows,
 vl_size numColumns,
 vl_size numChannels,
 int kernel,
 double sigma,
 int step,
 int flags)
{
  T * tempImage = (T*) mxMalloc (sizeof(T) * numRows * numOutputColumns) ;
  vl_uindex k ;
  vl_index j ;

  /* Note that MATLAB uses a column major ordering, while VLFeat a row
     major (standard) ordering for the image data. Effectively, VLFeat
     is operating on a transposed image, but this is fine since filters
     are symmetric.

     Therefore:

     input image width  = numRows
     input image height = numColumns
     output image width = numOutputRows (downsamped rows)
     outout image height = numOutputColumns (downsampled columns)

     In addition a temporary buffer is used. This is an image that
     is obtained from the input image by convolving and downsampling
     along the height and saving the result transposed:

     temp image width  = numOutputColumns
     temp image height = numRows
  */

  switch (kernel) {
    case GAUSSIAN :
    {
      vl_size W = ceil (4.0 * sigma) ;
      T * filter = (T*) mxMalloc (sizeof(T) * (2 * W + 1)) ;
      T acc = 0 ;
      for (j = 0 ; j < (signed)(2 * W + 1) ; ++j) {
        T z = ( (T) j - W) / (sigma + VL_EPSILON_F) ;
        filter[j] = exp(- 0.5 * (z*z)) ;
        acc += filter[j] ;
      }
      for (j = 0 ; j < (signed)(2 * W + 1) ; ++j) {
        filter[j] /= acc ;
      }

      for (k = 0 ; k < numChannels ; ++k) {

        IMCONVCOL (tempImage, numOutputColumns,
                   inputImage, numRows, numColumns, numRows,
                   filter, -W, W, step, flags) ;

        IMCONVCOL (outputImage, numOutputRows,
                   tempImage, numOutputColumns, numRows, numOutputColumns,
                   filter, -W, W, step, flags) ;

        inputImage += numRows * numColumns ;
        outputImage += numOutputRows * numOutputColumns ;
      }
      mxFree (filter) ;
      break ;
    }

    case TRIANGULAR:
    {
      unsigned int W = VL_MAX((unsigned int) sigma, 1) ;
      for (k = 0 ; k < numChannels ; ++k) {

       IMCONVCOLTRI (tempImage, numOutputColumns,
                     inputImage, numRows, numColumns, numRows,
                     W, step, flags) ;

       IMCONVCOLTRI (outputImage, numOutputRows,
                     tempImage, numOutputColumns, numRows, numOutputColumns,
                     W, step, flags) ;

        inputImage += numRows * numColumns ;
        outputImage += numOutputRows * numOutputColumns ;
      }
      break ;
    }

    default:
      abort() ;
  }
  mxFree (tempImage) ;
}

#undef FLT
#undef IMCONVCOLTRI
#undef IMCONVCOL
#undef VL_IMSMOOTH_INSTANTIATING

/* ---------------------------------------------------------------- */
/* VL_IMSMOOTH_INSTANTIATING */
#else

#include <mexutils.h>

#include <vl/generic.h>
#include <vl/mathop.h>
#include <vl/imopv.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* option codes */
enum {
  opt_padding = 0,
  opt_subsample,
  opt_kernel,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
{"Padding",      1,   opt_padding       },
{"Verbose",      0,   opt_verbose       },
{"Subsample",    1,   opt_subsample     },
{"Kernel",       1,   opt_kernel        },
{0,              0,   0                 }
} ;

enum {GAUSSIAN, TRIANGULAR} ;


#define VL_IMSMOOTH_INSTANTIATING
#define FLT VL_TYPE_FLOAT
#include "vl_imsmooth.c"

#define VL_IMSMOOTH_INSTANTIATING
#define FLT VL_TYPE_DOUBLE
#include "vl_imsmooth.c"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_I = 0, IN_S, IN_END} ;
  enum {OUT_J = 0} ;
  int opt ;
  int next = IN_END ;
  mxArray const  *optarg ;

  int padding = VL_PAD_BY_CONTINUITY ;
  int kernel = GAUSSIAN ;
  int flags ;
  vl_size step = 1 ;
  int verb = 0 ;
  double sigma ;
  mxClassID classid ;

  mwSize M, N, K, M_, N_, ndims ;
  mwSize dims_ [3] ;
  mwSize const * dims ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    mexErrMsgTxt("At least two input arguments required.");
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
      case opt_padding :
      {
        enum {buflen = 32} ;
        char buf [buflen] ;
        if (!vlmxIsString(optarg, -1)) {
          vlmxError(vlmxErrInvalidArgument,
                   "PADDING argument must be a string.") ;
        }
        mxGetString(optarg, buf, buflen) ;
        buf [buflen - 1] = 0 ;
        if (uStrICmp("zero", buf) == 0) {
          padding = VL_PAD_BY_ZERO ;
        } else if (uStrICmp("continuity", buf) == 0) {
          padding = VL_PAD_BY_CONTINUITY ;
        } else {
          vlmxError(vlmxErrInvalidArgument,
                   "PADDING must be either ZERO or CONTINUITY, was '%s'.",
                   buf) ;
        }
        break ;
      }

      case opt_subsample :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument,
                   "SUBSAMPLE must be a scalar.") ;
        }
        step = *mxGetPr(optarg) ;
        if (step < 1) {
          vlmxError(vlmxErrInvalidArgument,
                   "SUBSAMPLE must be not less than one.") ;
        }
        break ;

      case opt_kernel :
      {
        enum {buflen = 32} ;
        char buf [buflen] ;
        if (!vlmxIsString(optarg, -1)) {
          vlmxError(vlmxErrInvalidArgument,
                   "KERNEL argument must be a string.") ;
        }
        mxGetString(optarg, buf, buflen) ;
        buf [buflen - 1] = 0 ;
        if (uStrICmp("gaussian", buf) == 0) {
          kernel = GAUSSIAN ;
        } else if (uStrICmp("triangular", buf) == 0) {
          kernel = TRIANGULAR ;
        } else {
          vlmxError(vlmxErrInvalidArgument,
                   "Unknown kernel type '%s'.",
                   buf) ;
        }
        break ;
      }

      case opt_verbose :
        ++ verb ;
        break ;

      default:
        abort() ;
    }
  }

  if (! vlmxIsPlainScalar(IN(S))) {
    vlmxError(vlmxErrInvalidArgument,
             "S must be a real scalar.") ;
  }

  classid = mxGetClassID(IN(I)) ;

  if (classid != mxDOUBLE_CLASS &&
      classid != mxSINGLE_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
             "I must be either DOUBLE or SINGLE.") ;
  }
  if (mxGetNumberOfDimensions(IN(I)) > 3) {
    vlmxError(vlmxErrInvalidArgument,
             "I must be either a two or three dimensional array.") ;
  }

  ndims = mxGetNumberOfDimensions(IN(I)) ;
  dims = mxGetDimensions(IN(I)) ;
  M = dims[0] ;
  N = dims[1] ;
  K = (ndims > 2) ? dims[2] : 1 ;

  sigma = * mxGetPr(IN(S)) ;
  if ((sigma < 0.01) && (step == 1)) {
    OUT(J) = mxDuplicateArray(IN(I)) ;
    return ;
  }

  M_ = (M - 1) / step + 1 ;
  N_ = (N - 1) / step + 1 ;
  dims_ [0] = M_ ;
  dims_ [1] = N_ ;
  if (ndims > 2) dims_ [2] = K ;

  OUT(J) = mxCreateNumericArray(ndims, dims_, classid, mxREAL) ;

  if (verb) {
    char const *classid_str = 0, *kernel_str = 0, *padding_str = 0 ;
    switch (padding) {
      case VL_PAD_BY_ZERO       : padding_str = "with zeroes" ; break ;
      case VL_PAD_BY_CONTINUITY : padding_str = "by continuity" ; break ;
      default: abort() ;
    }
    switch (classid) {
      case mxDOUBLE_CLASS: classid_str = "DOUBLE" ; break ;
      case mxSINGLE_CLASS: classid_str = "SINGLE" ; break ;
      default: abort() ;
    }
    switch (kernel) {
      case GAUSSIAN:   kernel_str = "Gaussian" ; break ;
      case TRIANGULAR: kernel_str = "triangular" ; break ;
      default: abort() ;
    }

    mexPrintf("vl_imsmooth: [%dx%dx%d] -> [%dx%dx%d] (%s, subsampling step %d)\n",
              N, M, K, N_, M_, K, classid_str, step) ;
    mexPrintf("vl_imsmooth: padding: %s\n", padding_str) ;
    mexPrintf("vl_imsmooth: kernel: %s\n", kernel_str) ;
    mexPrintf("vl_imsmooth: sigma: %g\n", sigma) ;
    mexPrintf("vl_imsmooth: SIMD enabled: %s\n",
              vl_get_simd_enabled() ? "yes" : "no") ;
  }

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */
  flags  = padding ;
  flags |= VL_TRANSPOSE ;

  switch (classid) {
    case mxSINGLE_CLASS:
      _vl_imsmooth_smooth_f ((float*) mxGetPr(OUT(J)),
                             M_, N_,
                             (float const*) mxGetPr(IN(I)),
                             M, N, K,
                             kernel, sigma, step, flags) ;
      break ;

    case mxDOUBLE_CLASS:
      _vl_imsmooth_smooth_d ((double*) mxGetPr(OUT(J)),
                             M_, N_,
                             (double const*) mxGetPr(IN(I)),
                             M, N, K,
                             kernel, sigma, step, flags) ;
      break ;

    default:
      abort() ;
  }
}

/* VL_IMSMOOTH_INSTANTIATING */
#endif
