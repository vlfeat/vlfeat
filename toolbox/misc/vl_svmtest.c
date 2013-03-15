/** @file vl_svmtest.c
 ** @brief vl_svmtest MEX definition
 ** @author Milan Sulc
 **/

/*
Copyright (C) 2013 Milan Sulc
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include <mexutils.h>
#include <vl/svms.h>

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
 ** @brief Get testing data from testing dataset struct
 **
 ** @param trainingData Matlab training dataset struct.
 ** @param data pointer to variable where to save the data pointer.
 ** @param dataDimension pointer to variable where to save data dimension.
 ** @param dataType pointer to variable where to save the data type.
 ** @param numSamples pointer to variable where to save the number of samples.
 **
 ** The function extracts the information provided in the Matlab
 ** testing set struct.
 **
 **/

void  getTestingData(const mxArray* testingData, void** data, vl_size* dataDimension, vl_type* dataType, vl_size* numSamples)
{
  mxClassID dataClass ;
  mxArray* field = NULL ;

  if (! mxIsStruct(testingData))
    vlmxError(vlmxErrInvalidArgument,"DATA must be a valid TRAINING SET Struct.") ;

  /* Get data */
  field = mxGetField(testingData, 0, "data") ;
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

}

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Set a point map to the training dataset
 ** @param trainingData Matlab training dataset struct.
 ** @param dataset      dataset struct to be passed to SVM.
 ** @param n            pointer to a variable where the order of the
 ** map is saved.
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

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{


  VL_USE_MATLAB_ENV ;


  enum {IN_DATA, IN_MODEL, IN_BIAS, IN_BIAS_MULTIPLIER} ;
  enum {OUT_LABELS} ;


	/* Check number of input and output parameters */

  if (nin < 2) {
    vlmxError(vlmxErrInvalidArgument,
              "At least two arguments are required.") ;
  } else if (nin > 4) {
    vlmxError(vlmxErrInvalidArgument,
              "Maximum number of input arguments is 4.") ;
  } else if (nout > 1) {
    vlmxError(vlmxErrInvalidArgument,
              "Too many output arguments.");
  }


	/* Load data */

	void * data = NULL ;
	vl_size dataDimension ;
	vl_type dataType ;
	vl_size numSamples = 0 ;

	getTestingData(IN(DATA),&data,&dataDimension,&dataType,&numSamples) ;

  VlSvmDataset * dataset = vl_svmdataset_new(data,dataDimension) ;

  int mapDim = 1 ;
  setMap(IN(DATA),dataset,&mapDim) ;


  /* Load model */
  //dimension: mapDim*dataDimension

  if (mxGetM(IN(MODEL)) != mapDim*dataDimension) {
    vlmxError(vlmxErrInvalidArgument,
              "MODEL dimension does not correspond to DATA dimension (w.r.t. possible feature mapping).") ;
  }

  double * model = (double*) mxGetData(IN(MODEL));
  double bias;
  if (nin < 3) {
    bias = 0;
  } else {
    bias = (double) *mxGetPr(IN(BIAS)) ;
  }

  double biasMultiplier;
  if (nin<4) {
    biasMultiplier = 1;
  } else {
    biasMultiplier = (double) *mxGetPr(IN(BIAS_MULTIPLIER)) ;
  }


  VlSvmDatasetInnerProduct innerProduct = NULL ;

  switch (dataType) {
  case VL_TYPE_FLOAT :
    innerProduct = (VlSvmDatasetInnerProduct)&vl_svmdataset_innerproduct_f ;
    break ;
  case VL_TYPE_DOUBLE:
    innerProduct = (VlSvmDatasetInnerProduct)&vl_svmdataset_innerproduct_d ;
    break ;
  }


  vl_int8 * labels = vl_calloc(numSamples, sizeof(vl_int8)) ;


  /* Evaluate */
  double score;
  for (vl_size i=0; i<numSamples; i++) {
    score =innerProduct(dataset,i,model) + biasMultiplier*bias;
    if (score>0) {
      labels[i] = 1;
    } else {
      labels[i] = -1;
    }
  }


  mwSize dims[2] ;
  double * tempBuffer ;
  dims[0] = numSamples ;
  dims[1] = 1 ;

  out[OUT_LABELS] = mxCreateNumericArray(2, dims,
                                          mxINT8_CLASS, mxREAL) ;

  tempBuffer = (vl_int8*) mxGetData(out[OUT_LABELS]) ;
  memcpy(tempBuffer,labels,numSamples * sizeof(vl_int8)) ;

  vl_svmdataset_delete(dataset) ;

}
