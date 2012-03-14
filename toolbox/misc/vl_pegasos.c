/** @file vl_pegasos.c
 ** @brief vl_pegasos MEX definition
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/pegasos.h>
#include <vl/svm_solver.h>
#include <vl/bsmatrix.h>

#include <vl/homkermap.h>


#include <vl/stringop.h>

#include <assert.h>
#include <string.h>

#include "kdtree.h"

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

static
void setDoubleValue(mxArray* array, double value)
{
  double* temp = (double*) mxGetData(array) ;
  *temp = value ;
}

static
void setUintValue(mxArray* array, double value)
{
  vl_uint32 * temp = (vl_uint32*) mxGetData(array) ;
  *temp = value ;
}


static
mxArray * createInfoStruct(VlSvmPegasos* svm)
{
  mwSize dims[] = {1 , 1} ;

  const char* names [15] = {"dimension", "iterations", "maxIterations",
                           "epsilon", "lambda", "biasMultiplier",
                           "biasLearningRate", "energyFrequency", "elapsedTime",
                           "energy", "regularizerTerm", "lossPos",
                           "lossNeg", "hardLossPos", "hardLossNeg"} ;

  mxArray* output = mxCreateStructArray(1, dims, 15, names);

  mxArray * dimension = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(dimension,svm->dimension) ;

  mxArray * iterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(iterations,svm->iterations) ;

  mxArray * maxIterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(maxIterations,svm->maxIterations) ;

  mxArray * epsilon = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(epsilon,svm->epsilon) ;

  mxArray * lambda = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(lambda,svm->lambda) ;

  mxArray * biasMultiplier = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(biasMultiplier,svm->biasMultiplier) ;

  mxArray * biasLearningRate = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(biasLearningRate,svm->biasLearningRate) ;

  mxArray * energyFrequency = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(energyFrequency,svm->energyFrequency) ;

  mxArray * elapsedTime = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(elapsedTime,svm->elapsedTime) ;


  mxSetField(output, 0, "dimension", dimension) ;
  mxSetField(output, 0, "iterationsSoFar", iteration) ;
  mxSetField(output, 0, "maxIterations", maxIterations) ;
  mxSetField(output, 0, "epsilon", maxIterations) ;
  mxSetField(output, 0, "lambda", lambda) ;
  mxSetField(output, 0, "biasMultiplier", biasMultiplier) ;
  mxSetField(output, 0, "biasLearningRate", biasLearningRate) ;
  mxSetField(output, 0, "energyFrequency", energyFrequency) ;
  mxSetField(output, 0, "elapsedTime", elapsedTime) ;


  if (svm->objective)
    {
      mxArray * energy = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(energy,svm->objective->energy) ;

      mxArray * regularizerTerm = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(regularizerTerm,svm->objective->regularizer) ;

      mxArray * lossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(lossPos,svm->objective->lossPos) ;

      mxArray * lossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(lossNeg,svm->objective->lossNeg) ;

      mxArray * hardLossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(hardLossPos,svm->objective->hardLossPos) ;

      mxArray * hardLossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
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




typedef struct _DiagnosticsDispatcher {
  mxArray* diagnosticsHandle ;
  mxArray * callerRef ;
  int verbose ;
} DiagnosticsDispatcher ;


void diagnosticsDispatcher(DiagnosticsDispatcher * self, VlSvmPegasos* svm)
{
  if (diagnosticsHandle)
    {
      mxArray *lhs,*rhs[3];

      rhs[0] = self->diagnosticsHandle ;
      rhs[1] = self->callerRef ;
      rhs[2] =  createDiagnosticsStruct(svm) ;

      if( mxIsClass( rhs[0] , "function_handle"))
	mexCallMATLAB(0,&lhs,3,rhs,"feval");

      if (self->verbose)
	{
	  mexPrintf("vl_pegasos: Iteration = %d\n",svm->iterationsSoFar) ;
	  mexPrintf("vl_pegasos: elapsed time = %f\n",svm->elapsedTime) ;

	  mexPrintf("vl_pegasos: energy = %f\n",svm->objective->energy) ;
	}
    }

}



/* option codes */
enum {
  opt_verbose, opt_bias_multiplier, opt_num_iterations,
  opt_starting_iteration, opt_starting_model, opt_permutation,
  opt_bias_learningrate, opt_diagnostics, opt_diagnostics_freq, opt_block_sparse, opt_homkermap, opt_KCHI2, opt_KL1, opt_KJS, opt_KINTERS, opt_gamma, opt_period, opt_window
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"BiasMultiplier",    1,   opt_bias_multiplier     },
  {"NumIterations",     1,   opt_num_iterations      },
  {"StartingIteration", 1,   opt_starting_iteration  },
  {"StartingModel",     1,   opt_starting_model      },
  {"Permutation",       1,   opt_permutation         },
  {"BiasLearningRate",    1,   opt_bias_learningrate     },
  {"DiagnosticsFunction",       1,   opt_diagnostics         },
  {"DiagnosticsFreq",   1, opt_diagnostics_freq      },
  {"homkermap",         1,   opt_homkermap           },
  {"kl1",               0,   opt_KL1                 },
  {"kchi2",             0,   opt_KCHI2               },
  {"kjs",               0,   opt_KJS                 },
  {"kinters",           0,   opt_KINTERS             },
  {"order",             1,   opt_order               },
  {"gamma",             1,   opt_gamma               },
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
  enum {OUT_MODEL = 0} ;


  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  mxArray *inputModel = NULL;

  VlSvmPegasos* svm ;

  vl_size dataDimension ;

  vl_uint32* matlabPermutation ;

  void * data ;
  mxClassID dataClass ;
  vl_type dataType ;
  vl_size numSamples ;

  vl_uint32 * permutation  = NULL ;
  vl_size permutationSize = 0 ;

  vlSvmInnerProductFunction innerProduct ;
  vlSvmAccumulatorFunction accumulator ;

  /* maps */
  vlSvmFeatureMap mapFunc  = NULL ;

  /* Homkermap */
  VlHomogeneousKernelType kernelType = VlHomogeneousKernelChi2 ;
  VlHomogeneousKernelMapWindowType windowType = VlHomogeneousKernelMapWindowRectangular ;
  mwSize numDimensions ;
  mwSize const * dimensions ;
  mxClassID dataClassId ;
  double gamma = 1.0 ;
  int n = 0 ;
  double period = -1 ;

  vl_bool homkermap = VL_FALSE ;
  void * map = NULL;

  /* diagnostics */
  vl_size diagnosticsFrequency = 10 ;

  /* Block Sparse Matrix */
  vl_bool blockSparse = VL_FALSE ;

  VL_USE_MATLAB_ENV ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin < 3) {
    vlmxError(vlmxErrInvalidArgument,
              "At least three arguments are required.") ;
  } else if (nout > 2) {
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

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    if  (opt == opt_homkermap)
      {
        homkermap = VL_TRUE ;
        if (! vlmxIsPlainScalar(optarg))
          {
            vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
          }
        n = *mxGetPr(optarg) ;
        if (n < 0)
          {
            vlmxError(vlmxErrInvalidArgument, "N is negative.") ;
          }

      }
  }



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

  svm = vl_svmpegasos_new ((2*n + 1)*dataDimension,*mxGetPr(IN(LAMBDA)) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_bias_multiplier :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "BIASMULTIPLIER is not a plain scalar.") ;
      }
      vl_svmpegasos_set_bias_multiplier(svm, *mxGetPr(optarg)) ;
      break ;

    case opt_num_iterations :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 0) {
	vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is negative.") ;
      }
      vl_svmpegasos_set_maxiterations(svm, (vl_size) *mxGetPr(optarg)) ;
      break ;

    case opt_starting_iteration :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 1) {
	vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is smaller than 1.") ;
      }
      vl_svmpegasos_set_iterations(svm, (vl_size) *mxGetPr(optarg)) ;
      break ;

    case opt_starting_model :
      if (!vlmxIsVector(optarg, -1) ||
	  mxIsComplex(optarg) ||
	  mxGetClassID(optarg) != mxDOUBLE_CLASS) {
	vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL is not a real vector.") ;
      }
      inputModel = mxDuplicateArray(optarg) ;
      svm->model = (double*) mxGetData(inputModel) ;
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

      if (mxGetClassID(optarg) != mxDOUBLE_CLASS) {
	vlmxError(vlmxErrInvalidArgument, "BIASLEARNINGRATE must be double.") ;
      }
      vl_svmpegasos_set_bias_learningrate(svm, (double)*mxGetPr(optarg)) ;
      break ;
    case opt_diagnostics :

      if( !mxIsClass( optarg , "function_handle")) {
	mexErrMsgTxt("DIAGNOSTICSFUNCTION must be  a function handle.");
      }
      diagnosticsHandle = (mxArray*)(optarg) ;
      break ;
    case opt_energy_freq :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "ENERGYFREQ is not a plain scalar.") ;
      }
      vl_svmpegasos_set_energy_frequency (svm, (vl_size)*mxGetPr(optarg)) ;

      break ;
    case opt_verbose :
      ++ verbose ;
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






  if (svm->preConditioner && preconditionerDimension != (svm->dimension + (svm->biasMultiplier > 0))) {
    vlmxError(vlmxErrInvalidArgument, "PRECONDITIONER has incompatible dimension.") ;
  }

  if (svm->maxIterations == 0) {
    svm->maxIterations = (vl_size) 10 / (svm->regularizer + 1) ;
  }

  if (! inputModel) {
    svm->model = (double*) vl_calloc( svm->dimension + (svm->biasMultiplier != 0),sizeof(double)) ;
  } else {

    if (mxGetNumberOfElements(inputModel) != svm->dimension + (svm->biasMultiplier > 0)) {
      vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL has incompatible dimension.") ;
    }
  }

  if (verbose) {
    mexPrintf("vl_pegasos: Lambda = %g\n", svm->regularizer) ;
    mexPrintf("vl_pegasos: BiasMultiplier = %g\n", svm->biasMultiplier) ;
    mexPrintf("vl_pegasos: NumIterations = %d\n", svm->maxIterations) ;
    mexPrintf("vl_pegasos: permutation size = %d\n", permutationSize) ;
    mexPrintf("vl_pegasos: using preconditioner = %s\n", VL_YESNO(svm->preConditioner)) ;
  }

  switch (dataType) {
  case VL_TYPE_FLOAT :
    innerProduct = (vlSvmInnerProductFunction)&vlSvmInnerProductFunction_f ;
    accumulator = (vlSvmAccumulatorFunction)&vlSvmAccumulatorFunction_f ;
    break ;
  case VL_TYPE_DOUBLE:
    innerProduct = (vlSvmInnerProductFunction)&vlSvmInnerProductFunction_d ;
    accumulator = (vlSvmAccumulatorFunction)&vlSvmAccumulatorFunction_d ;
    break ;
  case VL_TYPE_UINT32:
    if (!blockSparse)
      vlmxError(vlmxErrInvalidArgument, "Invalid Data Type.") ;
    innerProduct = (vlSvmInnerProductFunction)&vlSvmInnerProductFunctionBlockSparseMatrixList ;
    accumulator = (vlSvmAccumulatorFunction)&vlSvmAccumulatorFunctionBlockSparseMatrixList ;
    break ;
  }
  if (homkermap)
    {
      map = vl_homogeneouskernelmap_new (kernelType, gamma, n, period, windowType) ;

      mapFunc = (vlSvmFeatureMap)&vl_homogeneouskernelmap_evaluate_d ;

    }
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  if (diagnosticsHandle)
    vl_pegasos_train s(svm,data,dataDimension,
					    numSamples,
					    (vl_int8 const *)mxGetData(IN(LABELS)),
					    innerProduct,
					    accumulator,
					    NULL,
					    permutation,
					    permutationSize,
					    mapFunc,
					    map,
					    diagnosticsDispatcher,
					    diagnosticsFrequency) ;
  else
    vl_pegasos_train_binary_svm (svm,data,dataDimension,
				 numSamples,
				 (vl_int8 const *)mxGetData(IN(LABELS)),
				 innerProduct,
				 accumulator,
				 NULL,
				 permutation,
				 permutationSize,
				 mapFunc,
				 map) ;


  /* -----------------------------------------------------------------
   *                                                            Output
   * -------------------------------------------------------------- */

  out[OUT_MODEL] = createOutputStruct(svm) ;

  if (permutation) vl_free(permutation) ;

  deleteSvm(svm) ;

  verbose = 0 ;
  diagnosticsHandle = NULL ;

}
