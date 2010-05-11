/** @internal
 ** @file    inthist.c
 ** @author  Andrea Vedaldi
 ** @brief   Calculate an integral histogram - MEX definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>

#include <vl/generic.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

enum {opt_mass = 1, opt_numLabels, opt_verbose} ;

/* options */
vlmxOption  options [] = {
  {"Mass",         1,   opt_mass,         },
  {"NumLabels",    1,   opt_numLabels     },
  {"Verbose",      0,   opt_verbose       },
  {0,              0,   0                 }
} ;

#undef T
#undef SFX
#define T vl_int32
#define SFX i
#include "inthist.tc"

#undef T
#undef SFX
#define T vl_uint32
#define SFX ui
#include "inthist.tc"

#undef T
#undef SFX
#define T double
#define SFX d
#include "inthist.tc"

#undef T
#undef SFX
#define T float
#define SFX f
#include "inthist.tc"

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  mwSize dims [3] ;
  vl_size numDims ;
  mwSize const * dimsPt = 0 ;

  vl_uint32* labelsPt  = 0 ;
  void*      massPt    = 0 ;
  void*      histPt    = 0 ;
  vl_uint32  numLabels = 0 ;
  mxClassID  dataClass = mxUINT32_CLASS ;
  vl_size width, height, numMaps ;
  vl_uindex k, q ;

  enum {IN_LABELS = 0, IN_END} ;
  enum {OUT_HIST = 0} ;
  int opt ;
  int nextOpt = IN_END ;
  mxArray const  *optArg ;
  int verb = 0 ;

  /* ------------------------------------------------------------------
  **                                                Check the arguments
  ** --------------------------------------------------------------- */
  if (nin < 1) {
    mexErrMsgTxt("At least one input argument is required.") ;
  } else if (nout > 1) {
    mexErrMsgTxt("Too many output arguments.");
  }

  if (mxGetClassID(IN(LABELS)) != mxUINT32_CLASS) {
    mexErrMsgTxt("LABELS must be of class UINT32.") ;
  }
  labelsPt = mxGetData(IN(LABELS)) ;

  numDims = mxGetNumberOfDimensions(IN(LABELS)) ;
  if (numDims > 3) {
    mexErrMsgTxt("LABELS must be a MxNxK array.") ;
  }

  labelsPt = mxGetData(IN(LABELS)) ;
  dimsPt   = mxGetDimensions(IN(LABELS)) ;
  height   = dimsPt [0] ;
  width    = dimsPt [1] ;
  if (numDims > 2) {
    numMaps = dimsPt [2] ;
  } else {
    numMaps = 1 ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &nextOpt, &optArg)) >= 0) {
    switch (opt) {
    case opt_mass :
      {
        massPt = mxGetData(optArg) ;
        dimsPt = mxGetDimensions(optArg) ;

        if (mxGetNumberOfDimensions(optArg) != numDims ||
            height != dimsPt [0] ||
            width  != dimsPt [1] ||
            ((numDims > 2) && numMaps < dimsPt[2])) {
          mexErrMsgTxt("MASS must have the same dimensions of LABELS.") ;
        }

        /* the data is DOUBLE or UINT32 depending on the class of MASS */
        dataClass = mxGetClassID(optArg) ;
        if (dataClass != mxDOUBLE_CLASS &&
            dataClass != mxUINT32_CLASS) {
          mexErrMsgTxt("MASS must be of either class DOUBLE or UINT32.") ;
        }
        break ;
      }

    case opt_numLabels :
      if (!vlmxIsPlainScalar(optArg)) {
        mexErrMsgTxt("NUMLABELS must be a real scalar.") ;
      }
      numLabels = *mxGetPr(optArg) ;
      break ;

    case opt_verbose :
      ++ verb ;
      break ;

    default:
      abort() ;
    }
  }

  /* If numLabels is not specified, guess it from LABELS */
  if (numLabels == 0) {
    for (k = 0 ; k < width*height ; ++k) {
      numLabels = VL_MAX(numLabels, labelsPt [k]) ;
    }
  } else {
    for (k = 0 ; k < width*height ; ++k) {
      if (labelsPt [k] > numLabels) {
        mexErrMsgTxt("LABELS contains an element greater than NUMLABELS.") ;
      }
    }
  }

  /* Allocate space for the integral histogram */
  dims [0] = height ;
  dims [1] = width ;
  dims [2] = numLabels ;
  OUT(HIST) = mxCreateNumericArray(3, dims, dataClass, mxREAL) ;
  histPt = mxGetData(OUT(HIST)) ;

  if (verb) {
    mexPrintf("inthist: integrating %d x %d label map with %d labels\n", width, height, numLabels) ;
    mexPrintf("         custom mass map: %s\n", VL_YESNO(massPt)) ;
  }

  /* ------------------------------------------------------------------
   *                                                    Distribute data
   * --------------------------------------------------------------- */

#define PROCESS(T, INTEGRAL)                                            \
  size_t const K = width*height ;                                       \
  T* dataPt = histPt ;                                                  \
  if (massPt == 0) {                                                    \
    for (q = 0 ; q < numMaps ; ++ q) {                                  \
      for (k = 0 ; k < K ; ++ k) {                                      \
        size_t label = *labelsPt++ ;                                    \
        if (label) dataPt [k + (label - 1) * K] += (T) 1 ;              \
      }                                                                 \
    }                                                                   \
  } else {                                                              \
    for (q = 0 ; q < numMaps ; ++q) {                                   \
      for (k = 0 ; k < K ; ++ k) {                                      \
        size_t label = *labelsPt++ ;                                    \
        if (label) dataPt [k + (label - 1) * K]                         \
                     += ((T*)massPt) [k] ;                              \
      }                                                                 \
    }                                                                   \
  }                                                                     \
  for (k = 0 ; k < numLabels ; ++k) {                                   \
    INTEGRAL (dataPt + k*K, height,                                     \
              dataPt + k*K, height, width, height) ;                    \
  }

  switch (dataClass) {
  case mxUINT32_CLASS: { PROCESS(vl_uint32, integral_ui) } ; break ;
  case mxDOUBLE_CLASS: { PROCESS(double,    integral_d)  } ; break ;
  default :
    abort() ;
  }
}

