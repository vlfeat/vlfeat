/** @file vl_maketrainingset.c
 ** @brief vl_maketrainingset MEX definition
 ** @author Daniele Perrone
 **/

/*
Copyright (C) 2012 Daniele Perrone.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/stringop.h>

/* Hided Matlab function for shared memory copy */
extern mxArray *mxCreateSharedDataCopy(const mxArray* pr);


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Create a plain training set struct
 ** @param data   array containing the training data.
 ** @param labels array containing the labels.
 **
 ** The function create a plain training set struct with only data and
 ** labels fields.
 **
 **/

mxArray* createOutputStruct(const mxArray* data, const mxArray* labels)
{
  mwSize dims[] = {1 , 1} ;

  const char* names [2] = {"data","labels"} ;

  mxArray* output = mxCreateStructArray(1, dims, 2, names) ;
  mxArray* dataOut = mxCreateSharedDataCopy(data) ;
  mxArray* labelsOut = mxCreateSharedDataCopy(labels) ;

  mxSetField(output, 0, "data", dataOut) ;
  mxSetField(output, 0, "labels", labelsOut) ;

  return output ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Create a training set struct with an Homogeneous kernel map
 ** @param data   training data.
 ** @param labels labels.
 ** @param order  order of the homogeneous kernel map.
 ** @param kernelType type of kernel.
 ** @param windowType type of window.
 ** @param gamma  gamma value.
 ** @param period period of the map.
 ** @return the training set struct.
 **
 ** The function create a training set struct with data, labels and an
 ** homogeneous kernel map.
 **/

mxArray* createHomekermapStruct(const mxArray* data, const mxArray* labels,
                                const double order,
                                mxArray* kernelType, mxArray* windowType,
                                const double gamma, const double period)
{
  mwSize dims[] = {1 , 1} ;
  const char* homkermapNames [5] = {"order","kernelType","windowType","gamma","period"} ;
  const char* names [3] = {"data","labels","map"} ;
  mxArray *output, *map, *dataOut, *labelsOut, *orderOut, *kernelTypeOut, *windowTypeOut, *gammaOut;

  if (kernelType == NULL) {
    kernelType = mxCreateString("kchi2") ;
  }

  if (windowType == NULL) {
    windowType = mxCreateString("rectangular") ;
  }

  output = mxCreateStructArray(1, dims, 3, names) ;
  map = mxCreateStructArray(1, dims, 5, homkermapNames) ;

  dataOut = mxCreateSharedDataCopy(data) ;
  labelsOut = mxCreateSharedDataCopy(labels) ;

  orderOut = mxCreateDoubleScalar(order) ;

  kernelTypeOut = mxCreateSharedDataCopy(kernelType) ;
  windowTypeOut = mxCreateSharedDataCopy(windowType) ;

  gammaOut = mxCreateDoubleScalar(gamma) ;

  if (period > 0) {
    mxArray * periodOut = mxCreateDoubleScalar(period) ;
    mxSetField(map, 0, "period", periodOut) ;
  }

  mxSetField(map, 0, "order", orderOut) ;
  mxSetField(map, 0, "kernelType", kernelTypeOut) ;
  mxSetField(map, 0, "windowType", windowTypeOut) ;
  mxSetField(map, 0, "gamma", gammaOut) ;

  mxSetField(output, 0, "data", dataOut) ;
  mxSetField(output, 0, "labels", labelsOut) ;
  mxSetField(output, 0, "map", map) ;
  return output ;
}

/* option codes */
enum {
  opt_homkermap, opt_KCHI2, opt_KL1, opt_KJS, opt_KINTERS,
  opt_gamma, opt_period, opt_window,
} ;

/* options */
vlmxOption  options [] = {
  {"homkermap",         1,   opt_homkermap           },
  {"kl1",               0,   opt_KL1                 },
  {"kchi2",             0,   opt_KCHI2               },
  {"kjs",               0,   opt_KJS                 },
  {"kinters",           0,   opt_KINTERS             },
  {"gamma",             1,   opt_gamma               },
  {"period",            1,   opt_period              },
  {"window",            1,   opt_window              },
  {0,                   0,   0                       }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATA, IN_LABELS, IN_END} ;
  enum {OUT_TRAINING_DATA = 0} ;

  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  mxClassID dataClass ;
  vl_size numSamples ;

  /* Homkermap */
  mxArray* kernelType = NULL ;
  mxArray* windowType = NULL ;
  double gamma = 1.0 ;
  double period = -1 ;
  vl_bool homkermap = VL_FALSE ;
  vl_int order = 0 ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    vlmxError(vlmxErrInvalidArgument,
              "At least two arguments are required.") ;
  } else if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  dataClass = mxGetClassID(IN(DATA)) ;

  if (! vlmxIsMatrix (IN(DATA), -1, -1) ||
      ! vlmxIsReal (IN(DATA))) {
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be a real matrix.") ;
  }

  numSamples = mxGetN(IN(DATA)) ;

  if (! vlmxIsVector(IN(LABELS), numSamples)) {
    vlmxError(vlmxErrInvalidArgument, "LABELS is not a vector of dimension compatible with DATA.") ;
  }

  if (dataClass != mxSINGLE_CLASS && dataClass != mxDOUBLE_CLASS) {
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be either SINGLE or DOUBLE.") ;
  }

  if (mxGetClassID(IN(LABELS)) != mxINT8_CLASS) {
    vlmxError(vlmxErrInvalidArgument, "LABELS must be INT8.") ;
  }

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_homkermap:
      homkermap = VL_TRUE ;
      if (! vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
      }
      order = *mxGetPr(optarg) ;
      if (order < 0) {
        vlmxError(vlmxErrInvalidArgument, "N is negative.") ;
      }
      break ;

    case opt_KINTERS:
    case opt_KL1:
    case opt_KCHI2:
    case opt_KJS:
      kernelType = (mxArray*) in[next-1] ;
      break ;

    case opt_period:
      if (! vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "PERIOD is not a scalar.") ;
      }
      period = *mxGetPr(optarg) ;
      if (period <= 0) {
        vlmxError(vlmxErrInvalidArgument, "PERIOD is not positive.") ;
      }
      break ;

    case opt_gamma:
      if (! vlmxIsPlainScalar(optarg)){
        vlmxError(vlmxErrInvalidArgument, "GAMMA is not a scalar.") ;
      }
      gamma = *mxGetPr(optarg) ;
      if (gamma <= 0) {
        vlmxError(vlmxErrInvalidArgument, "GAMMA is not positive.") ;
      }
      break ;

    case opt_window:
      if (! vlmxIsString(optarg,-1)){
        vlmxError(vlmxErrInvalidArgument, "WINDOW is not a string.") ;
      } else {
        char buffer [1024] ;
        mxGetString(optarg, buffer, sizeof(buffer) / sizeof(char)) ;
        if (vl_string_casei_cmp("uniform", buffer) == 0) {
          windowType = (mxArray*) optarg ;
        } else if (vl_string_casei_cmp("rectangular", buffer) == 0) {
          windowType = (mxArray*) optarg ;
        } else {
          vlmxError(vlmxErrInvalidArgument, "WINDOW=%s is not recognized.", buffer) ;
        }
      }
      break ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                            Output
   * -------------------------------------------------------------- */
  if (homkermap) {
    OUT(TRAINING_DATA) = createHomekermapStruct(IN(DATA), IN(LABELS),
                                                (double)order, kernelType,
                                                windowType,
                                                gamma, period) ;
  } else {
    OUT(TRAINING_DATA) = createOutputStruct(IN(DATA), IN(LABELS)) ;
  }
}
