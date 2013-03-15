/** @internal
 ** @file svms_common.h
 ** @brief Common functions for SVMs in MATLAB
 ** @author Milan Sulc
 ** @author Daniele Perrone
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Milan Sulc.
Copyright (C) 2012 Daniele Perrone.
Copyright (C) 2011-12 Andrea Vedaldi.

All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include "mex.h"
#include <mexutils.h>
#include <vl/svms.h>


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

    const char* names [15] = {"model","bias","dimension", "iterations","maxIterations",
                              "epsilon", "lambda", "biasMultiplier",
                              "elapsedTime","energy","regularizerTerm", "lossPos",
                              "lossNeg", "hardLossPos", "hardLossNeg"};
    output = mxCreateStructArray(1, dims, 15, names);

  }


  /*--MODEL--*/
  mwSize mdims[2] ;
  double * tempBuffer ;
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
    vlmxError(vlmxErrInvalidArgument,"DATA must be a valid TRAINING SET Struct.") ;

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
