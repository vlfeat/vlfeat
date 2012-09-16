/** @file vl_svmpegasos.c
 ** @brief vl_svmpegasos MEX definition
 ** @author Andrea Vedaldi
 ** @author Daniele Perrone
 **/

/*
Copyright (C) 2011-12 Andrea Vedaldi.
Copyright (C) 2012 Daniele Perrone.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/svmdataset.h>
#include <vl/pegasos.h>
#include <vl/homkermap.h>
#include <vl/stringop.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* Diagnostic Dispatcher Object */
typedef struct _DiagnosticsDispatcher {
  mxArray* diagnosticsHandle ;
  mxArray * callerRef ;
  int verbose ;
} DiagnosticsDispatcher ;


/* Info */
enum {dimension,
      iterations,
      maxIterations,
      epsilon,
      lambda,
      biasMultiplier,
      biasLearningRate,
      energyFrequency,
      elapsedTime,
      energy,
      regularizerTerm,
      lossPos,
      lossNeg,
      hardLossPos,
      hardLossNeg};


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set value of scalar double mxArray
 ** @param array scalar mxArray.
 ** @param value new value.
 **/

void setDoubleValue(mxArray* array, double value)
{
  double* temp = (double*) mxGetData(array) ;
  *temp = value ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set value of scalar integer  mxArray
 ** @param array scalar mxArray.
 ** @param value new value.
 **/

void setUintValue(mxArray* array, vl_uint32 value)
{
  vl_uint32 * temp = (vl_uint32*) mxGetData(array) ;
  *temp = value ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Create a Matlab struct with diagnostics informations.
 ** @param svm SVM status.
 **
 **  The function create a Matlab struct with the following fields:
 **  - dimension: SVM dimension.
 **  - iterations: number of iterations performed so far.
 **  - maxIterations: maximum number of iterations.
 **  - epsilon: SVM stop criterion.
 **  - lambda: SVM pegasos paramater.
 **  - biasMultiplier: constant multiplying the SVM bias.
 **  - biasLearningRate: learning rate for the SVM bias.
 **  - energyFrequency: rate of SVM energy update.
 **  - elapseTime: elapsed time since the start of the SVM learning.
 **  - energy: SVM energy value.
 **  - regulizerTerm: value of the only SVM regulizer term.
 **  - lossPos: value of loss function only for data points labeled positives.
 **  - lossNeg: value of loss function only for data points labeled negatives.
 **  - hardLossPos: number of points labeled negative instead of positive.
 **  - hardLossNeg: number of points labeled positive instead of negative.
 **/

mxArray * createInfoStruct(VlSvmPegasos* svm)
{
  mwSize dims[] = {1 , 1} ;

  const char* names [15] = {
    "dimension", "iterations", "maxIterations",
    "epsilon", "lambda", "biasMultiplier",
    "biasLearningRate", "energyFrequency", "elapsedTime",
    "energy", "regularizerTerm", "lossPos",
    "lossNeg", "hardLossPos", "hardLossNeg"} ;

  mxArray *output, *dimension, *iterations, *maxIterations, *epsilon, *lambda ;
  mxArray *biasMultiplier, *biasLearningRate, *energyFrequency, *elapsedTime ;

  output = mxCreateStructArray(1, dims, 15, names);

  dimension = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(dimension,svm->dimension) ;

  iterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(iterations,svm->iterations) ;

  maxIterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(maxIterations,svm->maxIterations) ;

  epsilon = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(epsilon,svm->epsilon) ;

  lambda = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(lambda,svm->lambda) ;

  biasMultiplier = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(biasMultiplier,svm->biasMultiplier) ;

  biasLearningRate = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(biasLearningRate,svm->biasLearningRate) ;

  energyFrequency = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(energyFrequency,svm->energyFrequency) ;

  elapsedTime = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(elapsedTime,svm->elapsedTime) ;


  mxSetField(output, 0, "dimension", dimension) ;
  mxSetField(output, 0, "iterations", iterations) ;
  mxSetField(output, 0, "maxIterations", maxIterations) ;
  mxSetField(output, 0, "epsilon", epsilon) ;
  mxSetField(output, 0, "lambda", lambda) ;
  mxSetField(output, 0, "biasMultiplier", biasMultiplier) ;
  mxSetField(output, 0, "biasLearningRate", biasLearningRate) ;
  mxSetField(output, 0, "energyFrequency", energyFrequency) ;
  mxSetField(output, 0, "elapsedTime", elapsedTime) ;

  if (svm->objective) {
    mxArray *energy, *regularizerTerm, *lossPos, *lossNeg, *hardLossPos, *hardLossNeg ;
    energy = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(energy,svm->objective->energy) ;

    regularizerTerm = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(regularizerTerm,svm->objective->regularizer) ;

    lossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(lossPos,svm->objective->lossPos) ;

    lossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(lossNeg,svm->objective->lossNeg) ;

    hardLossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(hardLossPos,svm->objective->hardLossPos) ;

    hardLossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
    setDoubleValue(hardLossNeg,svm->objective->hardLossNeg) ;

    mxSetField(output, 0, "energy", energy) ;
    mxSetField(output, 0, "regularizerTerm", regularizerTerm) ;
    mxSetField(output, 0, "lossPos", lossPos) ;
    mxSetField(output, 0, "lossNeg", lossNeg) ;
    mxSetField(output, 0, "hardLossPos", hardLossPos) ;
    mxSetField(output, 0, "hardLossNeg", hardLossNeg) ;
  }

  return output ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Diagnostics Callback.
 **
 ** @param svm SVM status.
 **
 **  The function is the callbach called by pegasos SVM when the full
 **  energy and the diagnostic informations are computed. Is a Matlab
 **  Callback has been given in input, the function calls it passing
 **  the same informations as a matlab struct.
 **
 **/

void diagnosticDispatcher(VlSvmPegasos* svm)
{
  if (svm->diagnosticCallerRef)
    {
      mxArray *lhs,*rhs[3];
      DiagnosticsDispatcher* dispatcherObject = (DiagnosticsDispatcher*)  svm->diagnosticCallerRef ;

      rhs[0] = dispatcherObject->diagnosticsHandle ;
      if (dispatcherObject->callerRef) {
        rhs[1] = dispatcherObject->callerRef ;
      } else {
        mwSize dims[2] ;
        dims[0] = 1 ;
        dims[1] = 1 ;
        rhs[1] = mxCreateNumericArray(2, dims,
                                      mxDOUBLE_CLASS, mxREAL) ;
      }
      rhs[2] =  createInfoStruct(svm) ;


      if (mxIsClass( rhs[0] , "function_handle")) {
        mexCallMATLAB(0,&lhs,3,rhs,"feval") ;
      }

      mxDestroyArray(rhs[2]) ;

      if (dispatcherObject->verbose) {
        mexPrintf("vl_pegasos: Iteration = %d\n",svm->iterations) ;
        mexPrintf("vl_pegasos: elapsed time = %f\n",svm->elapsedTime) ;
        mexPrintf("vl_pegasos: energy = %f\n",svm->objective->energy) ;
      }
    }
}

/* option codes */
enum {
  opt_verbose, opt_bias_multiplier, opt_max_iterations,
  opt_epsilon, opt_starting_iteration, opt_starting_model,
  opt_permutation, opt_bias_learningrate, opt_diagnostic,
  opt_energy_freq, opt_diagnostic_caller_ref, opt_block_sparse,
  opt_homkermap, opt_KCHI2, opt_KL1, opt_KJS, opt_KINTERS,
  opt_gamma, opt_period, opt_window, opt_starting_bias
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"BiasMultiplier",    1,   opt_bias_multiplier     },
  {"MaxIterations",     1,   opt_max_iterations      },
  {"Epsilon",           1,   opt_epsilon             },
  {"StartingIteration", 1,   opt_starting_iteration  },
  {"StartingModel",     1,   opt_starting_model      },
  {"StartingBias",      1,   opt_starting_bias       },
  {"Permutation",       1,   opt_permutation         },
  {"BiasLearningRate",  1,   opt_bias_learningrate   },
  {"DiagnosticFunction",1,   opt_diagnostic          },
  {"DiagnosticCallRef", 1,   opt_diagnostic_caller_ref},
  {"EnergyFreq",        1,   opt_energy_freq         },
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
  enum {IN_DATA, IN_LABELS, IN_LAMBDA, IN_END} ;
  enum {OUT_MODEL = 0, OUT_BIAS, OUT_INFO} ;

  int verbose = 0 ;
  int opt ;
  int next = IN_END ;
  mxArray const *optarg ;

  mxArray *inputModel = NULL;
  VlSvmPegasos* svm = NULL ;

  vl_bool freeModel = VL_TRUE ;
  vl_size dataDimension ;

  vl_uint32* matlabPermutation ;

  void * data ;
  mxClassID dataClass ;
  vl_type dataType ;
  vl_size numSamples ;

  vl_uint32 * permutation  = NULL ;
  vl_size permutationSize = 0 ;

  DiagnosticsDispatcher* disp ;
  VlSvmDatasetInnerProduct innerProduct = NULL ;
  VlSvmDatasetAccumulator accumulator = NULL ;

  /* maps */
  VlSvmDatasetFeatureMap mapFunc  = NULL ;

  /* Homkermap */
  VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
  VlHomogeneousKernelMapWindowType windowType = VlHomogeneousKernelMapWindowRectangular ;
  double gamma = 1.0 ;
  int n = 0 ;
  double period = -1 ;

  VlSvmDataset* dataset ;

  vl_bool homkermap = VL_FALSE ;
  void * map = NULL ;

  VL_USE_MATLAB_ENV ;

  disp = (DiagnosticsDispatcher*) vl_malloc(sizeof(DiagnosticsDispatcher)) ;
  disp->diagnosticsHandle = NULL ;
  disp->callerRef = NULL ;
  disp->verbose = 0 ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError(vlmxErrInvalidArgument,
              "At least three arguments are required.") ;
  } else if (nout > 3) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  dataClass = mxGetClassID(IN(DATA)) ;

  if (! vlmxIsMatrix (IN(DATA), -1, -1) ||
      ! vlmxIsReal (IN(DATA))) {
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be a real matrix.") ;
  }

  data = mxGetData (IN(DATA)) ;
  dataDimension = mxGetM(IN(DATA)) ;
  numSamples = mxGetN(IN(DATA)) ;

  /* Get order of the HOMKERMAP, if used. */
  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    if  (opt == opt_homkermap) {
      homkermap = VL_TRUE ;
      if (! vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
      }
      n = *mxGetPr(optarg) ;
      if (n < 0) {
        vlmxError(vlmxErrInvalidArgument, "N is negative.") ;
      }
    }
  }

  next = IN_END ;

  if (! vlmxIsVector(IN(LABELS), numSamples)) {
    vlmxError(vlmxErrInvalidArgument, "LABELS is not a vector of dimension compatible with DATA.") ;
  }


  switch (dataClass) {
  case mxSINGLE_CLASS : dataType = VL_TYPE_FLOAT ; break ;
  case mxDOUBLE_CLASS : dataType = VL_TYPE_DOUBLE ; break ;
  default:
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be either SINGLE or DOUBLE.") ;
  }

  if (mxGetClassID(IN(LABELS)) != mxINT8_CLASS) {
    vlmxError(vlmxErrInvalidArgument, "LABELS must be INT8.") ;
  }


  if (! vlmxIsPlainScalar(IN(LAMBDA))) {
    vlmxError(vlmxErrInvalidArgument, "LAMBDA is not a plain scalar.") ;
  }

  svm = vl_svmpegasos_new ((2*n + 1)*dataDimension,*mxGetPr(IN(LAMBDA))) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_bias_multiplier :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "BIASMULTIPLIER is not a plain scalar.") ;
      }
      vl_svmpegasos_set_bias_multiplier(svm, *mxGetPr(optarg)) ;
      break ;

    case opt_max_iterations :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "MAXITERATIONS is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 0) {
        vlmxError(vlmxErrInvalidArgument, "MAXITERATIONS is negative.") ;
      }
      vl_svmpegasos_set_maxiterations(svm, (vl_size) *mxGetPr(optarg)) ;
      break ;

    case opt_epsilon :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "EPSILON is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 0) {
        vlmxError(vlmxErrInvalidArgument, "EPSILON is negative.") ;
      }
      vl_svmpegasos_set_epsilon(svm, (double) *mxGetPr(optarg)) ;
      break ;

    case opt_starting_iteration :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 1) {
        vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is smaller than 1.") ;
      }
      vl_svmpegasos_set_iterations(svm, (vl_size) *mxGetPr(optarg) - 1) ;
      break ;

    case opt_starting_model :
      if (!vlmxIsVector(optarg, -1) ||
          mxIsComplex(optarg) ||
          mxGetClassID(optarg) != mxDOUBLE_CLASS) {
        vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL is not a real vector.") ;
      }
      inputModel = mxDuplicateArray(optarg) ;
      vl_svmpegasos_set_model(svm,(double*) mxGetData(inputModel)) ;
      freeModel = VL_FALSE ;
      break ;

    case opt_starting_bias :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "STARTINGBIAS is not a plain scalar.") ;
      }
      vl_svmpegasos_set_bias(svm, (double) *mxGetPr(optarg)) ;
      break ;

    case opt_permutation :
      if (!vlmxIsVector(optarg, -1) ||
          mxIsComplex(optarg) ||
          mxGetClassID(optarg) != mxUINT32_CLASS) {
        vlmxError(vlmxErrInvalidArgument, "PERMUTATION is not a UINT32 vector.") ;
      }
      permutationSize = mxGetNumberOfElements(optarg) ;
      permutation = mxMalloc(sizeof(vl_uint32) * permutationSize) ;
      matlabPermutation = mxGetData(optarg) ;
      {
        /* adjust indexing */
        vl_uindex k ;
        for (k = 0 ; k < permutationSize ; ++k) {
          permutation[k] = matlabPermutation[k] - 1 ;
          if (permutation[k] >= numSamples) {
            vlmxError(vlmxErrInconsistentData,
                      "Permutation indexes out of bounds: PERMUTATION(%d) = %d > %d = number of data samples.",
                      k + 1, permutation[k] + 1, numSamples) ;
          }
        }
      }
      vl_svmpegasos_set_permutation(svm,permutation,permutationSize) ;
      break ;

    case opt_bias_learningrate :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "BIASLEARNINGRATE is not a plain scalar.") ;
      }
      if (mxGetClassID(optarg) != mxDOUBLE_CLASS) {
        vlmxError(vlmxErrInvalidArgument, "BIASLEARNINGRATE must be double.") ;
      }
      vl_svmpegasos_set_bias_learningrate(svm, (double)*mxGetPr(optarg)) ;
      break ;
    case opt_diagnostic :

      if( !mxIsClass( optarg , "function_handle")) {
        mexErrMsgTxt("DIAGNOSTICSFUNCTION must be  a function handle.");
      }
      disp->diagnosticsHandle = (mxArray*)(optarg) ;
      break ;

    case opt_diagnostic_caller_ref :
      disp->callerRef = (mxArray*)(optarg) ;
      break ;

    case opt_energy_freq :
      if (!vlmxIsPlainScalar(optarg)) {
        vlmxError(vlmxErrInvalidArgument, "ENERGYFREQ is not a plain scalar.") ;
      }
      vl_svmpegasos_set_energy_frequency (svm, (vl_size)*mxGetPr(optarg)) ;
      break ;

    case opt_verbose :
      ++ verbose ;
      disp->verbose = 1 ;
      break ;

    case opt_KINTERS:
    case opt_KL1:
      kernelType = VlHomogeneousKernelIntersection ;
      break ;

    case opt_KCHI2:
      kernelType = VlHomogeneousKernelChi2 ;
      break ;

    case opt_KJS:
      kernelType = VlHomogeneousKernelJS ;
      break ;

    case opt_period:
      if (! vlmxIsPlainScalar(optarg)){
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
          windowType = VlHomogeneousKernelMapWindowUniform ;
        } else if (vl_string_casei_cmp("rectangular", buffer) == 0) {
          windowType = VlHomogeneousKernelMapWindowRectangular ;
        } else {
          vlmxError(vlmxErrInvalidArgument, "WINDOW=%s is not recognized.", buffer) ;
        }
      }
      break ;
    }
  }

  if (verbose) {
    mexPrintf("vl_pegasos: Lambda = %g\n", svm->lambda) ;
    mexPrintf("vl_pegasos: BiasMultiplier = %g\n", svm->biasMultiplier) ;
    mexPrintf("vl_pegasos: MaxIterations = %d\n", svm->maxIterations) ;
    mexPrintf("vl_pegasos: permutation size = %d\n", permutationSize) ;
  }

  switch (dataType) {
  case VL_TYPE_FLOAT :
    innerProduct = (VlSvmDatasetInnerProduct)&vl_svmdataset_innerproduct_f ;
    accumulator = (VlSvmDatasetAccumulator)&vl_svmdataset_accumulator_f ;
    break ;
  case VL_TYPE_DOUBLE:
    innerProduct = (VlSvmDatasetInnerProduct)&vl_svmdataset_innerproduct_d ;
    accumulator = (VlSvmDatasetAccumulator)&vl_svmdataset_accumulator_d ;
    break ;
  }

  dataset = vl_svmdataset_new(data,dataDimension) ;
  if (homkermap) {
    map = vl_homogeneouskernelmap_new (kernelType, gamma, n, period, windowType) ;
    mapFunc = (VlSvmDatasetFeatureMap)&vl_homogeneouskernelmap_evaluate_d ;
    vl_svmdataset_set_map(dataset,map,mapFunc,2*n + 1) ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  if (disp->diagnosticsHandle) {
    vl_svmpegasos_set_diagnostic (svm, (VlSvmDiagnostics)&diagnosticDispatcher, disp) ;
  }

  vl_svmpegasos_train (svm,dataset, numSamples,innerProduct, accumulator,
                       (vl_int8 const *)mxGetData(IN(LABELS))) ;

  /* -----------------------------------------------------------------
   *                                                            Output
   * -------------------------------------------------------------- */

  if (nout >= 1) {
    double * tempBuffer ;
    mwSize dims[2] ;
    dims[0] = svm->dimension ;
    dims[1] = 1 ;
    out[OUT_MODEL] = mxCreateNumericArray(2, dims,
                                          mxDOUBLE_CLASS, mxREAL) ;
    tempBuffer  = (double*) mxGetData(out[OUT_MODEL]) ;
    memcpy(tempBuffer,svm->model,svm->dimension * sizeof(double)) ;
  }

  if (nout >= 2) {
    double * tempBuffer ;
    mwSize dims[2] ;
    dims[0] = 1 ;
    dims[1] = 1 ;

    out[OUT_BIAS] = mxCreateNumericArray(2, dims,
                                         mxDOUBLE_CLASS, mxREAL) ;
    tempBuffer = (double*) mxGetData(out[OUT_BIAS]) ;
    *tempBuffer = svm->bias ;
  }

  if (nout == 3) {
    out[OUT_INFO] = createInfoStruct(svm) ;
  }

  if (homkermap) {
    vl_homogeneouskernelmap_delete(map);
  }
  vl_svmdataset_delete(dataset) ;
  vl_svmpegasos_delete(svm,freeModel) ;
  vl_free(disp) ;
}
