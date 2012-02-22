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

#include <assert.h>
#include <string.h>

#include "kdtree.h"

/* Diagnostics */
enum {model=0, 
      dimension,
      iterationsSoFar,
      maxIterations,
      regularizer,
      biasMultiplier,
      preConditioner,
      elapsedTime,
      energy,
      regularizerTerm,
      lossPoss,
      lossNeg,
      hardLossPos,
      hardLossNeg};

void setValue(mxArray* array, double value)
{
  double* temp = (double*) mxGetData(array) ;
  *temp = value ; 
}

mxArray * createOutputStruct(VlSvm* svm)
{
  mwSize dims[] = {1 , 1} ;

  const char* names [8] = {"model","dimension","iterationsSoFar","maxIterations","regularizer","biasMultiplier","preConditioner","elapsedTime"} ; 
  mxArray* output = mxCreateStructArray(1, dims,
				       8, names);

  vl_size extDimension = svm->dimension + (svm->biasMultiplier != 0) ;

  mxArray * model = mxCreateNumericMatrix(extDimension, 1,mxDOUBLE_CLASS, mxREAL) ; 
   
  memcpy(mxGetData(model),svm->model,extDimension*sizeof(double)) ; 

  mxArray * dimension = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ; 
  setValue(dimension,svm->dimension) ; 

   
  mxArray * iterationsSoFar = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ; 

  setValue(iterationsSoFar,svm->iterationsSoFar) ; 


  mxArray * maxIterations = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ; 

  setValue(maxIterations,svm->maxIterations) ; 

  mxArray * regularizer = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ; 

  setValue(regularizer,svm->regularizer) ;
   
  mxArray * biasMultiplier = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ; 

  setValue(maxIterations,svm->maxIterations) ;

  mxArray * preConditioner = mxCreateNumericMatrix(extDimension, 1,mxDOUBLE_CLASS, mxREAL) ; 
  if (svm->preConditioner)
    memcpy(mxGetData(preConditioner),svm->preConditioner,extDimension*sizeof(double)) ; 

  mxArray * elapsedTime = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(elapsedTime,svm->elapsedTime) ;

  mxSetField(output, 0, "model", model) ;
  mxSetField(output, 0, "dimension", dimension) ;
  mxSetField(output, 0, "iterationsSoFar", iterationsSoFar) ;
  mxSetField(output, 0, "maxIterations", maxIterations) ;
  mxSetField(output, 0, "regularizer", regularizer) ;
  mxSetField(output, 0, "biasMultiplier", biasMultiplier) ;
  mxSetField(output, 0, "preConditioner", preConditioner) ;
  mxSetField(output, 0, "elapsedTime", elapsedTime) ;

  return output ; 
}

mxArray * createDiagnosticsStruct(VlSvm* svm, VlSvmStatus* status)
{
  mxArray * output = createOutputStruct(svm) ; 

  mxAddField (output,"energy") ; 
  mxAddField (output,"regularizerTerm") ; 
  mxAddField (output,"lossPos") ; 
  mxAddField (output,"lossNeg") ;
  mxAddField (output,"hardLossPos") ; 
  mxAddField (output,"hardLossNeg") ;
  

  mxArray * energy = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(energy,status->energy) ;

  mxArray * regularizerTerm = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(regularizerTerm,status->regularizer) ;

  mxArray * lossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(lossPos,status->lossPos) ;

  mxArray * lossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(lossNeg,status->lossNeg) ;

  mxArray * hardLossPos = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(hardLossPos,status->hardLossPos) ;

  mxArray * hardLossNeg = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

  setValue(hardLossNeg,status->hardLossNeg) ;
  

  mxSetField(output, 0, "energy", energy) ;
  mxSetField(output, 0, "regularizerTerm", regularizerTerm) ;
  mxSetField(output, 0, "lossPos", lossPos) ;
  mxSetField(output, 0, "lossNeg", lossNeg) ;
  mxSetField(output, 0, "hardLossPos", hardLossPos) ;
  mxSetField(output, 0, "hardLossNeg", hardLossNeg) ;

  return output ; 
}


mxArray* diagnosticsHandle = NULL ; 
int            verbose = 0 ;

void diagnosticsDispatcher(VlSvm* svm, VlSvmStatus *status)
{
  if (diagnosticsHandle)
    {
      mxArray *lhs,*rhs[2];
      
      rhs[0] = diagnosticsHandle ;
      rhs[1] =  createDiagnosticsStruct(svm,status) ; 

      if( mxIsClass( rhs[0] , "function_handle"))
	mexCallMATLAB(0,&lhs,2,rhs,"feval");

      if (verbose)
	{
	  mexPrintf("vl_pegasos: Iteration = %d\n",svm->iterationsSoFar) ;
	  mexPrintf("vl_pegasos: elapsed time = %f\n",svm->elapsedTime) ;

	  mexPrintf("vl_pegasos: energy = %f\n",status->energy) ;
	}
    }
  
} 



/* option codes */
enum {
  opt_verbose, opt_bias_multiplier, opt_num_iterations,
  opt_starting_iteration, opt_starting_model, opt_permutation,
  opt_preconditioner, opt_diagnostics, opt_diagnostics_freq
} ;

/* options */
vlmxOption  options [] = {
  {"Verbose",           0,   opt_verbose             },
  {"BiasMultiplier",    1,   opt_bias_multiplier     },
  {"NumIterations",     1,   opt_num_iterations      },
  {"StartingIteration", 1,   opt_starting_iteration  },
  {"StartingModel",     1,   opt_starting_model      },
  {"Permutation",       1,   opt_permutation         },
  {"Preconditioner",    1,   opt_preconditioner      },
  {"DiagnosticsFunction",       1,   opt_diagnostics         },
  {"DiagnosticsFreq",   1, opt_diagnostics_freq      },
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

  
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;

  mxArray *inputModel = NULL; 

  VlSvm* svm ;
  
  vl_size preconditionerDimension = 0 ;

  svm = (VlSvm*) vl_malloc(sizeof(VlSvm)) ; 

  svm->biasMultiplier = 0 ;
  svm->preConditioner = NULL ;
  
  svm->maxIterations = 0 ;
  svm->iterationsSoFar = 0 ; 

  svm->elapsedTime = 0 ;

  void * data ;
  mxClassID dataClass ;
  vl_type dataType ;
  vl_size numSamples ;

  vl_uint32 * permutation  = NULL ;
  vl_size permutationSize = 0 ;

  vlSvmInnerProductFunction innerProduct ;
  vlSvmAccumulatorFunction accumulator ; 
 

  vl_size diagnosticsFrequency = 10 ;

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
  svm->dimension = mxGetM(IN(DATA)) ;
  numSamples = mxGetN(IN(DATA)) ;

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
  if (! vlmxIsVector(IN(LABELS), numSamples)) {
    vlmxError(vlmxErrInvalidArgument, "LABELS is not a vector of dimension compatible with DATA.") ;
  }

  if (! vlmxIsPlainScalar(IN(LAMBDA))) {
    vlmxError(vlmxErrInvalidArgument, "LAMBDA is not a plain scalar.") ;
  }
  svm->regularizer = *mxGetPr(IN(LAMBDA)) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {
    case opt_bias_multiplier :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "BIASMULTIPLIER is not a plain scalar.") ;
      }
      svm->biasMultiplier = *mxGetPr(optarg) ;
      break ;

    case opt_num_iterations :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 0) {
	vlmxError(vlmxErrInvalidArgument, "NUMITERATIONS is negative.") ;
      }
      svm->maxIterations = (vl_size) *mxGetPr(optarg) ;
      break ;

    case opt_starting_iteration :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is not a plain scalar.") ;
      }
      if (*mxGetPr(optarg) < 1) {
	vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is smaller than 1.") ;
      }
      svm->iterationsSoFar = (vl_size) *mxGetPr(optarg) ;
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
      {
	/* adjust indexing */
	vl_uint32 const * matlabPermutation = mxGetData(optarg) ;
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
      break ;

    case opt_preconditioner :
      if (!vlmxIsVector(optarg, -1) ||
	  mxIsComplex(optarg) ||
	  !mxIsNumeric(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "PRECONDITIONER is not a real vector.") ;
      }
      if (mxGetClassID(optarg) != mxDOUBLE_CLASS) {
	vlmxError(vlmxErrInvalidArgument, "PRECODNITIONER storage class must be double.") ;
      }
      svm->preConditioner = (double*) mxGetData(optarg) ;
      preconditionerDimension = mxGetNumberOfElements(optarg) ;
      break ;
    case opt_diagnostics :

      if( !mxIsClass( optarg , "function_handle")) {
	mexErrMsgTxt("DIAGNOSTICSFUNCTION must be  a function handle.");
      }
      diagnosticsHandle = (mxArray*)(optarg) ;
      break ;
    case opt_diagnostics_freq :
      if (!vlmxIsPlainScalar(optarg)) {
	vlmxError(vlmxErrInvalidArgument, "DIAGNOSTICSFREQ is not a plain scalar.") ;
      }
      diagnosticsFrequency = *mxGetPr(optarg) ;
      
      break ;
    case opt_verbose :
      ++ verbose ;
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
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  if (diagnosticsHandle)
    vl_pegasos_train_binary_svm_diagnostics(svm,data,
					    numSamples,
					    (vl_int8 const *)mxGetData(IN(LABELS)),
					    innerProduct,
					    accumulator,
					    NULL,
					    permutation,
					    permutationSize,
					    diagnosticsDispatcher,
					    diagnosticsFrequency) ;
  else
    vl_pegasos_train_binary_svm (svm,data,
				  numSamples,
				  (vl_int8 const *)mxGetData(IN(LABELS)),
				  innerProduct,
				  accumulator,
				  NULL,
				  permutation,
				  permutationSize) ;


  /* -----------------------------------------------------------------
   *                                                            Output
   * -------------------------------------------------------------- */
  
  out[OUT_MODEL] = createOutputStruct(svm) ; 

  if (permutation) vl_free(permutation) ;
  
  deleteSvm(svm) ; 
  
  verbose = 0 ;
  diagnosticsHandle = NULL ; 

}
