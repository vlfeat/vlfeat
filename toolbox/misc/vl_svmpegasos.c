/** @file vl_svmpegasos.c
 ** @brief vl_svmpegasos MEX definition
 ** @author Daniele Perrone
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2011-12 Andrea Vedaldi.
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
 **
 ** @param array scalar mxArray.
 ** @param value new value.
 **
 **/

void setUintValue(mxArray* array, vl_uint32 value)
{
  vl_uint32 * temp = (vl_uint32*) mxGetData(array) ;
  *temp = value ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Get training data from training dataset struct
 **
 ** @param trainingData Matlab training dataset struct.
 ** @param data  pointer variable where to save the data pointer.
 ** @param dataDimension pointer to variable where to save data dimension.
 ** @param dataType pointer to variable where to save data type.
 ** @param numSamples pointer to variable where to save the number of samples.
 ** @param labels pointer to variable where to save the set of labels.
 **
 ** The function extracts the information provided in the Matlab
 ** training set struct.
 **
 **/

void  getTrainingData(const mxArray* trainingData, void** data, vl_size* dataDimension, vl_type* dataType, vl_size* numSamples, vl_int8** labels)
{
  mxClassID dataClass ;
  mxArray* field = NULL ;


  if (! mxIsStruct(trainingData))
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be a valid TRAINING SET Struct.") ;

  /* Get data */
  field = mxGetField(trainingData, 0, "data") ;
  if (field == NULL)
    vlmxError(vlmxErrInvalidArgument,
              "DATA array missing in TRAINING SET Struct.") ;
  *data = mxGetData (field) ;

  *dataDimension = mxGetM(field) ;
  *numSamples = mxGetN(field) ;

  dataClass = mxGetClassID(field) ;

  switch (dataClass) {
  case mxSINGLE_CLASS : *dataType = VL_TYPE_FLOAT ; break ;
  case mxDOUBLE_CLASS : *dataType = VL_TYPE_DOUBLE ; break ;
  default:
    vlmxError(vlmxErrInvalidArgument,
              "DATA must be either SINGLE or DOUBLE.") ;
  }

  /* Get labels */
  field = mxGetField(trainingData, 0, "labels") ;
  if (field == NULL)
    vlmxError(vlmxErrInvalidArgument,
              "DATA array missing in TRAINING SET Struct.") ;
  if (mxGetClassID(field) != mxINT8_CLASS)
    vlmxError(vlmxErrInvalidArgument, "LABELS must be INT8.") ;

  if (! vlmxIsVector(field, *numSamples)) {
    vlmxError(vlmxErrInvalidArgument, "LABELS is not a vector of dimension compatible with DATA.") ;
  }

  *labels = (vl_int8*) mxGetData(field) ;
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set a point map to the training dataset
 ** @param trainingData Matlab training dataset struct.
 ** @param dataset      dataset struct to be passed to pegasos SVM.
 ** @param n            pointer to a variable where the order of the
 ** map is saved.
 **
 ** The function checks if a map is present in @a trainingData and add
 ** it to @a dataset.
 **/

void setMap(const mxArray* trainingData, VlSvmDataset* dataset, int* n)
{
  mxArray* field = NULL ;
  mxArray* subfield = NULL ;

  /* map function */
  VlSvmDatasetFeatureMap mapFunc  = NULL ;

  /* Homkermap */
  VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
  VlHomogeneousKernelMapWindowType windowType = VlHomogeneousKernelMapWindowRectangular ;
  double gamma = 1.0 ;
  double period = -1 ;

  void * map = NULL ;

  field = mxGetField(trainingData, 0, "map") ;
  if (field != NULL)
    {
      subfield = mxGetField(field, 0, "order") ;
      if (subfield != NULL)
        {
          if (! vlmxIsPlainScalar(subfield)) {
            vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
          }
          *n = *mxGetPr(subfield) ;
          if (*n < 0) {
            vlmxError(vlmxErrInvalidArgument, "N is negative.") ;
          }

          subfield = mxGetField(field, 0, "kernelType") ;
          if (subfield != NULL)
            {
              char buffer [1024] ;
              mxGetString(subfield, buffer, sizeof(buffer) / sizeof(char)) ;
              if (vl_string_casei_cmp("kl1", buffer) == 0)
                kernelType = VlHomogeneousKernelIntersection ;
              else if (vl_string_casei_cmp("kjs", buffer) == 0)
                kernelType = VlHomogeneousKernelJS ;
              else if (vl_string_casei_cmp("kinters", buffer) == 0)
                kernelType = VlHomogeneousKernelIntersection ;
            }

          /* get window type */
          subfield = mxGetField(field, 0, "windowType") ;
          if (subfield != NULL)
            {
              char buffer [1024] ;
              mxGetString(subfield, buffer, sizeof(buffer) / sizeof(char)) ;
              if (vl_string_casei_cmp("uniform", buffer) == 0)
                windowType = VlHomogeneousKernelMapWindowUniform ;
            }

          /* get gamma */
          subfield = mxGetField(field, 0, "gamma") ;
          if (subfield != NULL)
            {
              if (! vlmxIsPlainScalar(subfield))
                vlmxError(vlmxErrInvalidArgument, "GAMMA is not a scalar.") ;

              gamma = *mxGetPr(subfield) ;
              if (gamma <= 0)
                vlmxError(vlmxErrInvalidArgument, "GAMMA is not positive.") ;
            }

          /* get period */
          subfield = mxGetField(field, 0, "period") ;
          if (subfield != NULL)
            {
              if (! vlmxIsPlainScalar(subfield))
                vlmxError(vlmxErrInvalidArgument, "PERIOD is not a scalar.") ;

              period = *mxGetPr(subfield) ;
              if (period <= 0)
                vlmxError(vlmxErrInvalidArgument, "PERIOD is not positive.") ;

            }
          else {
            period = -1 ;
          }

          map = vl_homogeneouskernelmap_new (kernelType, gamma, *n, period, windowType) ;
          mapFunc = (VlSvmDatasetFeatureMap)&vl_homogeneouskernelmap_evaluate_d ;

          vl_svmdataset_set_map(dataset,map,mapFunc,2*(*n) + 1) ;
        }
      else {
        vlmxError(vlmxErrInvalidArgument, "Unknown feature map type.") ;
      }
    }
}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Create a Matlab struct with diagnostics informations.
 **
 ** @param svm SVM status.
 **
 **  The function create a Matlab struct with the following fields:
 **  - dimension: SVM dimension.
 **  - iterations: number of iterations performed.
 **  - maxIterations: maximum number of iterations.
 **  - epsilon: SVM stop criterion.
 **  - lambda: SVM pegasos paramater.
 **  - biasMultiplier: constant multiplying the SVM bias.
 **  - biasLearningRate: learning rate for the SVM bias.
 **  - energyFrequency: rate of SVM energy update.
 **  - elapseTime: elapsed time since the start of the SVM learning.
 **  - energy: SVM energy value.
 **  - regulizerTerm: value of the SVM regulizer term.
 **  - lossPos: value of loss function only for data points labeled positives.
 **  - lossNeg: value of loss function only for data points labeled negatives.
 **  - hardLossPos: number of mislabeled positive points.
 **  - hardLossNeg: number of mislabeled negative points.
 **/

mxArray * createInfoStruct(VlSvmPegasos* svm)
{
  mwSize dims[] = {1 , 1} ;

  const char* names [15] = {"dimension", "iterations", "maxIterations",
                            "epsilon", "lambda", "biasMultiplier",
                            "biasLearningRate", "energyFrequency", "elapsedTime",
                            "energy", "regularizerTerm", "lossPos",
                            "lossNeg", "hardLossPos", "hardLossNeg"} ;

  mxArray *output, *dimension, *iterations, *maxIterations, *epsilon ;
  mxArray *lambda, *biasMultiplier ;
  mxArray *biasLearningRate, *energyFrequency, *elapsedTime ;

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
    mxArray * energy, *regularizerTerm, *lossPos, *lossNeg, *hardLossPos, *hardLossNeg ;
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
 ** @param svm SVM status.
 **
 ** The function is the callbach called by pegasos SVM when the full
 ** energy and the diagnostic informations are computed. Is a Matlab
 ** Callback has been given in input, the function calls it passing
 ** the same informations as a matlab struct.
 **/

void diagnosticDispatcher(VlSvmPegasos* svm)
{
  if (svm->diagnosticCallerRef) {
    mxArray *lhs[1],*rhs[3] ;
    DiagnosticsDispatcher* dispatcherObject = (DiagnosticsDispatcher*) svm->diagnosticCallerRef ;

    lhs[0] = NULL ;
    rhs[0] = dispatcherObject->diagnosticsHandle ;
    rhs[1] = createInfoStruct(svm) ;

    if (dispatcherObject->callerRef) {
      rhs[2] = dispatcherObject->callerRef ;
    } else {
      mwSize dims[2] ;
      dims[0] = 1 ;
      dims[1] = 1 ;
      rhs[2] = mxCreateNumericArray(2, dims,
                                    mxDOUBLE_CLASS, mxREAL) ;
    }

    if( mxIsClass( rhs[0] , "function_handle")) {
      mexCallMATLAB(1,lhs,3,rhs,"feval");
    }

    if (lhs[0]) {
      dispatcherObject->callerRef = lhs[0] ;
    }

    mxDestroyArray(rhs[1]) ;

    if (dispatcherObject->verbose) {
      mexPrintf("vl_svmpegasos: Iteration = %d\n",svm->iterations) ;
      mexPrintf("vl_svmpegasos: elapsed time = %f\n",svm->elapsedTime) ;
      mexPrintf("vl_svmpegasos: energy = %f\n",svm->objective->energy) ;
    }
  }
}

/* option codes */
enum {
  opt_verbose, opt_bias_multiplier, opt_max_iterations, opt_epsilon,
  opt_starting_iteration, opt_starting_model, opt_permutation,
  opt_bias_learningrate, opt_diagnostic, opt_energy_freq,
  opt_diagnostic_caller_ref, opt_block_sparse, opt_starting_bias, opt_validation_data
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
  {"ValidationData",    1,   opt_validation_data     },
  {0,                   0,   0                       }
} ;

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_DATA, IN_LAMBDA, IN_END} ;
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

  void * data = NULL ;
  vl_type dataType ;
  vl_size numSamples = 0 ;
  vl_int8 * labels = NULL ;

  vl_uint32 * permutation  = NULL ;
  vl_size permutationSize = 0 ;

  VlSvmDatasetInnerProduct innerProduct = NULL ;
  VlSvmDatasetAccumulator accumulator = NULL ;

  DiagnosticsDispatcher* disp ;
  int n = 0 ;

  /* Validation Data */
  void * validationData = NULL ;
  vl_size validationDataDimension ;
  vl_type validationDataType ;
  vl_size validationNumSamples = 0 ;
  vl_int8 * validationLabels = NULL ;

  int validationN = 0 ;

  VlSvmDataset* validationDataset = NULL ;
  VlSvmDataset* dataset = NULL ;

  VL_USE_MATLAB_ENV ;

  disp = (DiagnosticsDispatcher*) vl_malloc(sizeof(DiagnosticsDispatcher)) ;
  disp->diagnosticsHandle = NULL ;
  disp->callerRef = NULL ;
  disp->verbose = 0 ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 2) {
    vlmxError(vlmxErrInvalidArgument,
              "At least three arguments are required.") ;
  } else if (nout > 3) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }

  getTrainingData(IN(DATA),&data,&dataDimension,&dataType,&numSamples,&labels) ;

  dataset = vl_svmdataset_new(data,dataDimension) ;

  setMap(IN(DATA),dataset,&n) ;

  svm = vl_svmpegasos_new ((2*n + 1)*dataDimension,*mxGetPr(IN(LAMBDA))) ;

  if (! vlmxIsPlainScalar(IN(LAMBDA))) {
    vlmxError(vlmxErrInvalidArgument, "LAMBDA is not a plain scalar.") ;
  }

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
      {
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
      }

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

    case opt_validation_data :
      getTrainingData(optarg,
                      &validationData,&validationDataDimension,&validationDataType,
                      &validationNumSamples,&validationLabels) ;

      if (validationDataType != dataType) {
        vlmxError(vlmxErrInvalidArgument, "VALIDATIONDATA type must be the same of DATA type.") ;
      }

      validationDataset = vl_svmdataset_new(validationData,validationDataDimension) ;
      setMap(optarg,validationDataset,&validationN) ;

      if (validationDataDimension*(1+2*validationN) != dataDimension*(1+2*n)) {
        vlmxError(vlmxErrInvalidArgument, "VALIDATIONDATA dimension must be the same of DATA dimension.") ;
      }
      break ;

    case opt_verbose :
      ++ verbose ;
      disp->verbose = 1 ;
      break ;
    }
  }

  if (verbose) {
    mexPrintf("vl_svmpegasos: Lambda = %g\n", svm->lambda) ;
    mexPrintf("vl_svmpegasos: BiasMultiplier = %g\n", svm->biasMultiplier) ;
    mexPrintf("vl_svmpegasos: MaxIterations = %d\n", svm->maxIterations) ;
    mexPrintf("vl_svmpegasos: permutation size = %d\n", permutationSize) ;
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

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  if (disp->diagnosticsHandle) {
    vl_svmpegasos_set_diagnostic (svm, (VlSvmDiagnostics)&diagnosticDispatcher, disp) ;
  }

  if (validationDataset == NULL) {
    vl_svmpegasos_train (svm,dataset, numSamples,innerProduct, accumulator,labels) ;
  } else {
    vl_svmpegasos_train_validation_data (svm,dataset, numSamples,innerProduct,
                                         accumulator,labels, validationDataset,
                                         validationNumSamples, validationLabels) ;
  }

  /* -----------------------------------------------------------------
   *                                                            Output
   * -------------------------------------------------------------- */

  if (nout >= 1) {
    mwSize dims[2] ;
    double * tempBuffer ;
    dims[0] = svm->dimension ;
    dims[1] = 1 ;

    out[OUT_MODEL] = mxCreateNumericArray(2, dims,
                                          mxDOUBLE_CLASS, mxREAL) ;

    tempBuffer = (double*) mxGetData(out[OUT_MODEL]) ;
    memcpy(tempBuffer,svm->model,svm->dimension * sizeof(double)) ;
  }

  if (nout >= 2) {
    mwSize dims[2] ;
    double * tempBuffer ;
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

  if (dataset->map) {
    vl_homogeneouskernelmap_delete(dataset->map);
  }
  vl_svmdataset_delete(dataset) ;
  vl_svmpegasos_delete(svm,freeModel) ;

  vl_free(disp) ;
}
