/** @file vl_svmtrain.c
 ** @brief vl_svmtrain MEX definition
 ** @author Milan Sulc
 ** @author Daniele Perrone
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Milan Sulc
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2011-13 Andrea Vedaldi.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/svm.h>

#include <vl/svm_sgd.h>
#include <vl/svm_dca.h>
#include <vl/homkermap.h>
#include <vl/stringop.h>

#include <assert.h>
#include <string.h>


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set value of scalar double to mxArray
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
 ** @brief Set value of scalar integer to mxArray
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
 ** @brief Create a Matlab struct with diagnostics informations.
 **
 ** @param svm SVM status.
 **/

mxArray * createInfoStruct(VlSvm* svm)
{
  mwSize dims[] = {1 , 1} ;

  mxArray *model, *bias;
  mxArray *output, *dimension, *iterations, *maxIterations, *epsilon ;
  mxArray *lambda, *biasMultiplier ;
  mxArray *biasPreconditioner, *energyFrequency, *elapsedTime ;
  mwSize mdims[2] ;
  double * tempBuffer ;


  if (svm->type == VL_SVM_SGD) {

    const char* names [17] = {"model","bias","dimension", "iterations","maxIterations",
                              "epsilon", "lambda", "biasMultiplier",
                              "elapsedTime","energy","regularizerTerm", "lossPos",
                              "lossNeg", "hardLossPos", "hardLossNeg",
                              "biasPreconditioner", "energyFrequency"
                              };
    output = mxCreateStructArray(1, dims, 17, names);

    biasPreconditioner = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;

    setDoubleValue(biasPreconditioner,svm->biasPreconditioner) ;
    mxSetField(output, 0, "biasPreconditioner", biasPreconditioner) ;

    energyFrequency = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
    mxSetField(output, 0, "energyFrequency", energyFrequency) ;
    setUintValue(energyFrequency,svm->energyFrequency) ;


  } else {

    const char* names [17] = {"model","bias","dimension", "iterations","maxIterations",
                              "epsilon", "lambda", "biasMultiplier",
                              "elapsedTime","energy","regularizerTerm", "lossPos",
                              "lossNeg", "hardLossPos", "hardLossNeg",
                              "energyDual","dualityGap"};
    output = mxCreateStructArray(1, dims, 17, names);

    if (svm->objective) {
      mxArray * energyDual, *dualityGap ;

      energyDual = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(energyDual,svm->objective->energyDual) ;

      dualityGap = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
      setDoubleValue(dualityGap,svm->objective->dualityGap) ;

      mxSetField(output, 0, "energyDual", energyDual) ;
      mxSetField(output, 0, "dualityGap", dualityGap) ;


    }

  }


  /*--MODEL--*/
  mdims[0] = svm->dimension ;
  mdims[1] = 1 ;
  model = mxCreateNumericArray(2, mdims, mxDOUBLE_CLASS, mxREAL) ;
  tempBuffer = (double*) mxGetData(model) ;
  memcpy(tempBuffer,svm->model,svm->dimension * sizeof(double)) ;
  mxSetField(output, 0, "model", model) ;
  /*--     --*/

  bias = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(bias,svm->bias) ;
  mxSetField(output, 0, "bias", bias) ;

  dimension = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(dimension,svm->dimension) ;
  mxSetField(output, 0, "dimension", dimension) ;

  iterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(iterations,svm->iterations) ;
  mxSetField(output, 0, "iterations", iterations) ;

  maxIterations = mxCreateNumericMatrix(1, 1,mxUINT32_CLASS, mxREAL) ;
  setUintValue(maxIterations,svm->maxIterations) ;
  mxSetField(output, 0, "maxIterations", maxIterations) ;

  epsilon = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(epsilon,svm->epsilon) ;
  mxSetField(output, 0, "epsilon", epsilon) ;

  lambda = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(lambda,svm->lambda) ;
  mxSetField(output, 0, "lambda", lambda) ;

  biasMultiplier = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(biasMultiplier,svm->biasMultiplier) ;
  mxSetField(output, 0, "biasMultiplier", biasMultiplier) ;

  elapsedTime = mxCreateNumericMatrix(1, 1,mxDOUBLE_CLASS, mxREAL) ;
  setDoubleValue(elapsedTime,svm->elapsedTime) ;
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


VL_INLINE
void diagnosticDispatcher(VlSvm* svm)
{
  if (svm->diagnosticFunction) {
    mxArray *rhs[2] ;

    rhs[0] = (mxArray*) svm->diagnosticFunction ;
    rhs[1] = createInfoStruct(svm) ;

    if( mxIsClass( rhs[0] , "function_handle")) {
      mexCallMATLAB(0,NULL,2,rhs,"feval");
    }

    mxDestroyArray(rhs[1]) ;
  }
}


/** ------------------------------------------------------------------
 ** @internal
 ** @brief Get training data from training dataset struct
 **
 ** @param trainingData Matlab training dataset struct.
 ** @param data pointer to variable where to save the data pointer.
 ** @param dataDimension pointer to variable where to save data dimension.
 ** @param dataType pointer to variable where to save the data type.
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
    vlmxError(vlmxErrInvalidArgument, "DATA must be a valid TRAINING SET Struct.") ;

  /* Get data */
  field = mxGetField(trainingData, 0, "data") ;
  if (field == NULL)
    vlmxError(vlmxErrInvalidArgument, "DATA array missing in TRAINING SET Struct.") ;
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

//TMP! </common>

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set a point map to the training dataset
 ** @param trainingData Matlab training dataset struct.
 ** @param dataset      dataset struct to be passed to SVM.
 ** @param mapdin       pointer to a variable where the map dimension is saved.
 **
 **
 ** The function checks if a map is present in @a trainingData and add
 ** it to @a dataset.
 **/

void setMap(const mxArray* trainingData, VlSvmDataset* dataset, int* mapDim)
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

  int n;

  field = mxGetField(trainingData, 0, "map") ;
  if (field != NULL)
  {
      subfield = mxGetField(field, 0, "order") ;
      if (subfield != NULL)
        {
          if (! vlmxIsPlainScalar(subfield)) {
            vlmxError(vlmxErrInvalidArgument, "N is not a scalar.") ;
          }
          n = *mxGetPr(subfield) ;
          if (n < 0) {
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

          map = vl_homogeneouskernelmap_new (kernelType, gamma, n, period, windowType) ;
          mapFunc = (VlSvmDatasetFeatureMap)&vl_homogeneouskernelmap_evaluate_d ;


          *mapDim = 2*n+1;
          vl_svmdataset_set_map(dataset,map,mapFunc, *mapDim) ;
        }
      else {
        vlmxError(vlmxErrInvalidArgument, "Unknown feature map type.") ;
      }
    }
}

/* option codes */

enum {
  // common:
  opt_epsilon,
  opt_max_iterations,
  opt_bias_multiplier,
  opt_diagnostic,
  opt_energy_freq,
  opt_validation_data,

  // switching to DCA
  opt_dca,

  // SGD specific
  opt_starting_model,
  opt_starting_bias,
  opt_starting_iteration,
  opt_permutation,
  opt_bias_preconditioner,


  // DCA specific:
  opt_loss_function,
  opt_random_permutation,
  opt_online_setting
};


/* options */
vlmxOption  options [] = {
  {"Epsilon",           1,   opt_epsilon             },
  {"MaxIterations",     1,   opt_max_iterations      },
  {"BiasMultiplier",    1,   opt_bias_multiplier     },
  {"DiagnosticFunction",1,   opt_diagnostic          },
  {"EnergyFreq",        1,   opt_energy_freq         },
  {"ValidationData",    1,   opt_validation_data     },


  // switching to DCA
  {"DCA",               0,   opt_dca                 },

  // SGD specific
  {"StartingModel",     1,   opt_starting_model      },
  {"StartingBias",      1,   opt_starting_bias       },
  {"StartingIteration", 1,   opt_starting_iteration  },
  {"Permutation",       1,   opt_permutation         },
  {"BiasPreconditioner",1,   opt_bias_preconditioner },

  // DCA specific:
  {"Loss",              1,   opt_loss_function       },
  {"RandomPermutation", 1,   opt_random_permutation  },
  {"OnlineSetting",     1,   opt_online_setting      },

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


	void * data ;
	vl_size dataDimension,numSamples ;
	vl_type dataType ;
	vl_int8 * labels ;
	VlSvmDataset * dataset;
	vl_int mapDim;
  double lambda;
  VlSvm* svm;
  vl_bool freeModel;

  void * validationData;
  vl_size validationDataDimension ;
  vl_type validationDataType ;
  vl_size validationNumSamples ;
  vl_int8 * validationLabels  ;
  int validationMapDim ;
  VlSvmDataset* validationDataset ;


  VlSvmDatasetInnerProduct innerProduct ;
  VlSvmDatasetAccumulator accumulator ;

  VlSvmLossFunction lossFunction;
  VlSvmLossConjugateFunction lossConjugateFunction;
  VlSvmDeltaAlpha deltaAlpha;
  VlSvmDatasetLengthSquare lengthSquare;

  vl_int opt, next;

  mxArray const *optarg ;

  vl_uint32* matlabPermutation ;
  vl_uint32 * permutation ;
  vl_size permutationSize ;

  char * last_sgd_param ;
  char * last_dca_param ;

  vl_uindex k ;

  VL_USE_MATLAB_ENV ;

last_sgd_param = NULL;
last_dca_param = NULL;

	/* Check number of input and output parameters */

  if (nin < 2) {
    vlmxError(vlmxErrInvalidArgument,
              "At least two arguments are required.") ;
  } else if (nout > 3) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }


	/* Read minimal input */

	getTrainingData(IN(DATA),&data,&dataDimension,&dataType,&numSamples,&labels) ;
  dataset = vl_svmdataset_new(data,dataDimension) ;

  mapDim = 1 ;
  setMap(IN(DATA),dataset,&mapDim) ;

  if (! vlmxIsPlainScalar(IN(LAMBDA))) {
        vlmxError(vlmxErrInvalidArgument, "LAMBDA is not a plain scalar.") ;
  }
  lambda =  *mxGetPr(IN(LAMBDA));
  if (lambda<= 0) {
        vlmxError(vlmxErrInvalidArgument, "LAMBDA must be a positive value.") ;
  }

  /* prepare SVM object */
	svm = vl_svm_new(mapDim*dataDimension,lambda,VL_SVM_SGD);
  freeModel = VL_TRUE;


  innerProduct = NULL ;
  accumulator = NULL ;
  lossFunction = (VlSvmLossFunction)&vl_L1_loss ;
  lossConjugateFunction = (VlSvmLossConjugateFunction)&vl_L1_lossConjugate ;
  deltaAlpha = (VlSvmDeltaAlpha)&vl_L1_deltaAlpha ;

  lengthSquare = NULL ;

  validationDataset = NULL ;

  next = IN_END ;



  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {
    switch (opt) {

      case opt_epsilon :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "EPSILON is not a plain scalar.") ;
        }
        svm->epsilon = (double) *mxGetPr(optarg);
        break;

      case opt_max_iterations :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "MAXITERATIONS is not a plain scalar.") ;
        }
        if (*mxGetPr(optarg) < 0) {
          vlmxError(vlmxErrInvalidArgument, "MAXITERATIONS is negative.") ;
        }
        svm->maxIterations = (vl_size) *mxGetPr(optarg);
        break ;

      case opt_bias_multiplier :
        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "BIASMULTIPLIER is not a plain scalar.") ;
        }
        svm->biasMultiplier = (double) *mxGetPr(optarg);
        break ;


      case opt_diagnostic :
          if( !mxIsClass( optarg , "function_handle")) {
            mexErrMsgTxt("DIAGNOSTICSFUNCTION must be  a function handle.");
          }
          svm->diagnostic = (VlSvmDiagnostics) &diagnosticDispatcher;
          svm->diagnosticFunction = (mxArray*) optarg;

        break ;


      case opt_energy_freq :

        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "ENERGYFREQ is not a plain scalar.") ;
        }

        svm->energyFrequency = (vl_size)*mxGetPr(optarg) ;

        break ;


      case opt_validation_data :
        getTrainingData(optarg,
                        &validationData,&validationDataDimension,&validationDataType,
                        &validationNumSamples,&validationLabels) ;

        if (validationDataType != dataType) {
          vlmxError(vlmxErrInvalidArgument, "VALIDATIONDATA type must be the same of DATA type.") ;
        }

        validationDataset = vl_svmdataset_new(validationData,validationDataDimension) ;
        setMap(optarg,validationDataset,&validationMapDim) ;

        if (validationDataDimension*validationMapDim != dataDimension*mapDim) {
          vlmxError(vlmxErrInvalidArgument, "VALIDATIONDATA dimension must be the same of DATA dimension.") ;
        }

        break ;


      case opt_dca :
           svm->type = VL_SVM_DCA;
        break ;


// SGD specific
      case opt_starting_model :
          if (!vlmxIsVector(optarg, -1) ||
              mxIsComplex(optarg) ||
              mxGetClassID(optarg) != mxDOUBLE_CLASS) {
            vlmxError(vlmxErrInvalidArgument, "STARTINGMODEL is not a real vector (double).") ;
          }
          last_sgd_param = "STARTINGMODEL";

          vl_free(svm->model);
          svm->model = (double*) mxGetData(mxDuplicateArray(optarg));
          freeModel = VL_FALSE ;
        break ;
      case opt_starting_bias :
          if (!vlmxIsPlainScalar(optarg)) {
            vlmxError(vlmxErrInvalidArgument, "STARTINGBIAS is not a plain scalar.") ;
          }
          last_sgd_param = "STARTINGBIAS";

          svm->bias = (double) *mxGetPr(optarg) ;
        break ;

      case opt_starting_iteration :
          if (!vlmxIsPlainScalar(optarg)) {
            vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is not a plain scalar.") ;
          }
          if (*mxGetPr(optarg) < 1) {
            vlmxError(vlmxErrInvalidArgument, "STARTINGITERATION is smaller than 1.") ;
          }
          last_sgd_param = "STARTINGITERATION";
          svm->iterations = (vl_size) (*mxGetPr(optarg) - 1) ;
        break ;

      case opt_permutation :

        if (!vlmxIsVector(optarg, -1) ||
            mxIsComplex(optarg) ||
            mxGetClassID(optarg) != mxUINT32_CLASS) {
          vlmxError(vlmxErrInvalidArgument, "PERMUTATION is not a UINT32 vector.") ;
        }
        last_sgd_param = "PERMUTATION";


        permutationSize = mxGetNumberOfElements(optarg) ;
        permutation = mxMalloc(sizeof(vl_uint32) * permutationSize) ;
        matlabPermutation = mxGetData(optarg) ;

        /* adjust (and check) indexing */
        for (k = 0 ; k < permutationSize ; ++k) {
          permutation[k] = matlabPermutation[k] - 1 ;
          if (permutation[k] >= numSamples) {
            vlmxError(vlmxErrInconsistentData,
                      "Permutation indexes out of bounds: PERMUTATION(%d) = %d > %d = number of data samples.",
                      k + 1, permutation[k] + 1, numSamples) ;
          }
        }

        svm->permutation = permutation;
        svm->permutationSize = permutationSize;

        break ;


      case opt_bias_preconditioner :

        if (!vlmxIsPlainScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "BIASPRECONDITIONER is not a plain scalar.") ;
        }
        if (mxGetClassID(optarg) != mxDOUBLE_CLASS) {
          vlmxError(vlmxErrInvalidArgument, "BIASPRECONDITIONER  must be double.") ;
        }
        last_sgd_param = "BIASPRECONDITIONER";

        svm->biasPreconditioner = (double) *mxGetPr(optarg);

        break ;


// DCA specific
      case opt_loss_function:
        if (!vlmxIsPlainScalar(optarg) || (*mxGetPr(optarg) !=1 && *mxGetPr(optarg) !=2) ) {
          vlmxError(vlmxErrInvalidArgument, "LOSS must be either 1 for L1 loss function or 2 for L2 loss function.") ;
        }
        last_dca_param = "LOSS";
        if (*mxGetPr(optarg) == 2) {
          lossFunction = (VlSvmLossFunction)&vl_L2_loss ;
          lossConjugateFunction = (VlSvmLossConjugateFunction)&vl_L2_lossConjugate ;
          deltaAlpha = (VlSvmDeltaAlpha) &vl_L2_deltaAlpha ;

        }
        break;

      case opt_random_permutation:
        if (!mxIsLogicalScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "RANDOMPERMUTATION must be a logical scalar.") ;
        }
        last_dca_param = "RANDOMPERMUTATION";
        svm->randomPermutation = (vl_bool) *mxGetLogicals(optarg);
        break;

      case opt_online_setting:
        if (!mxIsLogicalScalar(optarg)) {
          vlmxError(vlmxErrInvalidArgument, "ONLINE must be a logical scalar.") ;
        }
        last_dca_param = "ONLINE";

        svm->onlineSetting = (vl_bool) *mxGetLogicals(optarg);
        break;

    }
  }


  /* "parameters vs. svm type" checking */
  if(svm->type == VL_SVM_DCA && last_sgd_param != NULL) {
          vlmxError(vlmxErrInvalidArgument, "%s is not a valid argument for DCA SVM. (It is a parameter for SGD SVM.)",last_sgd_param) ;
  } else if (svm->type == VL_SVM_SGD && last_dca_param != NULL) {
          vlmxError(vlmxErrInvalidArgument, "%s is not a valid argument for SGD SVM. (It is a parameter for DCA SVM.)",last_dca_param) ;
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



  /*-- Training --*/

  switch (svm->type) {
  case VL_SVM_SGD :
    if (validationDataset == NULL) {
      vl_svm_sgd_train (svm,dataset, numSamples,innerProduct, accumulator,labels) ;
    } else {
      vl_svm_sgd_train_validation_data (svm,dataset, numSamples,innerProduct,
                                        accumulator,labels, validationDataset,
                                        validationNumSamples, validationLabels) ;
    }
    break ;
  case VL_SVM_DCA:
    if (validationDataset == NULL) {
      vl_svm_dca_train(svm,dataset,numSamples,
                     innerProduct, accumulator, lengthSquare, lossFunction, lossConjugateFunction, deltaAlpha, labels);
    } else {
      vl_svm_dca_train_validation_data (svm,dataset,numSamples,
                     innerProduct, accumulator, lengthSquare, lossFunction, lossConjugateFunction, deltaAlpha, labels,
                     validationDataset, validationNumSamples, validationLabels);
    }
    break ;
  }

  /*-- Output --*/

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
    out[OUT_BIAS] = vlmxCreatePlainScalar (svm->bias) ;
  }

  if (nout >= 3) {
    out[OUT_INFO] = createInfoStruct(svm) ;
  }

  if (dataset->map) {
    vl_homogeneouskernelmap_delete(dataset->map);
  }
  vl_svmdataset_delete(dataset) ;
  vl_svm_delete(svm,freeModel) ;

}
