/** @file svmdataset.c
 ** @brief Svm Dataset - Definition
 ** @author Daniele Perrone
 **/

/*
Copyright (C) 2012 Daniele Perrone.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

/**
@file svmdataset.h

Any SVM solver requires a set of training data in order to learn a
correspondent classifier.

Regardless the kind of SVM solver, the training data is used and
accessed in a similar manner.

In @ref svmdataset.h we define an abstraction that separates the
particular SVM algorithm from the representation of the data. Such
abstraction is defined via two type of functions:

- Inner Product ::VlSvmDatasetInnerProduct .
- Accumulation ::VlSvmDatasetAccumulator .

The above type of functions define the two typical operations
performed by SVM solvers on the training data. Any particular
application can define its own data representation and implement the
above functions. 

For double and float arrays we provide the implementation of the
aforementioned functions.

We define also an utility struct @ref VlSvmDataset that implements the
online expansions of data points. This allows the use of techniques
such homogeneous kernel maps @cite{vedaldi11efficient} inside the SVM
solver.
**/

#ifndef VL_SVMDATASET_INSTANTIATING

#include "svmdataset.h"
#include <string.h>
#include <math.h>

/** ------------------------------------------------------------------
 ** @brief Create a new SVM dataset structure
 ** @param data      pointer to data.
 ** @param dimension data point dimension.
 ** @return the new SVM dataset structure.
 **
 ** The function allocates and returns a new SVM dataset structure.
 **
 ** The data pointed by @data is not replicated, so the caller should
 ** keep the data allocated for the full usage of the structure.
 **
 ** @sa ::vl_svmdataset_delete().
 **/

VL_EXPORT
VlSvmDataset* vl_svmdataset_new (void * data, vl_size dimension)
{
  VlSvmDataset * dataset ;
  dataset = (VlSvmDataset*) vl_malloc(sizeof(VlSvmDataset)) ;
  dataset->data = data ;
  dataset->dimension = dimension ;
  dataset->map = NULL ;
  dataset->mapFunc = NULL ;
  dataset->order = 1 ;
  return dataset ;
}

/** ------------------------------------------------------------------
 ** @brief Set feature map
 ** @param data SVM dataset Structure
 ** @param map pointer to Feature Map Object
 ** @param mapFunc function that perform the feature map
 ** @param order order of the map
 **
 ** The function sets a feature map to the SVM dataset @data. The
 ** dimension of the extended data points is /f$2*order + 1/f$.
 **/

VL_EXPORT
void vl_svmdataset_set_map (VlSvmDataset * data, void * map,
                            VlSvmDatasetFeatureMap mapFunc, vl_size order)
{
  data->map  = map ;
  data->mapFunc = mapFunc ;
  data->order = order ;
}

/** -------------------------------------------------------------------
 ** @brief Delete a SVM dataset structure
 ** @param dataset SVM dataset to delete.
 **
 ** The function frees the resources allocated by
 ** ::vl_svmdataset_new(). Notice that only the structure will be
 ** freed and not the raw data referred by the pointer @data.
 **/

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset)
{
  vl_free(dataset) ;
}

#define FLT VL_TYPE_FLOAT
#define VL_SVMDATASET_INSTANTIATING
#include "svmdataset.c"

#define FLT VL_TYPE_DOUBLE
#define VL_SVMDATASET_INSTANTIATING
#include "svmdataset.c"

/* VL_SVMDATASET_INSTANTIATING */
#else

#include "float.th"

/** @fn vl_svmdataset_innerproduct_d (const void*, const vl_uindex,const double*)
 ** @param data Pointer to the training data.
 ** @param element Index of data point.
 ** @param model SVM model.
 **
 ** The function performs the inner product between the @a element
 ** data point and the model @a model.
 **
 ** @see @ref svmdataset-overview
 **/

/** @fn  vl_svmdataset_innerproduct_f (const void*, const vl_uindex,const double*)
 ** @see ::vl_svmdataset_innerproduct_d
 **/

VL_EXPORT double
VL_XCAT(vl_svmdataset_innerproduct_,SFX) (const void* data, const vl_uindex element,
                                          const double* model)
{
  vl_size i, j ;
  T* tData ;
  double res = 0 ;
  VlSvmDataset* sdata = (VlSvmDataset*) data ;

  tData  = (T*) sdata->data ;

  if (sdata->mapFunc) {
    double *temp = vl_malloc(sizeof(double) * sdata->order) ;
    for (i = 0; i < sdata->dimension; i++) {
      sdata->mapFunc(sdata->map,temp,1,tData[element*sdata->dimension + i]);

      for (j = 0; j < sdata->order; j++) {
        res += model[i*sdata->order + j]*temp[j] ;
      }
    }
    vl_free(temp) ;
  } else {
    for (i = 0; i < sdata->dimension; i++) {
      res += model[i]*(double)(tData[element*sdata->dimension + i]) ;
    }
  }
  return res ;
}

/** @fn vl_svmdataset_accumulator_d (const void*, const vl_uindex,double*, const double)
 ** @param data Pointer to the training data.
 ** @param element Index of data point.
 ** @param model SVM model.
 ** @param multiplier Constant that multiplies the data point.
 **
 ** The function adds to the model @a model the data point selected by
 ** @element multiplied by @multiplier.
 **
 ** @see @ref svmdataset-overview
 **/

/** @fn  vl_svmdataset_accumulator_f (const void*, const vl_uindex,double*, const double)
 ** @see ::vl_svmdataset_accumulator_d
 **/

VL_EXPORT void
VL_XCAT(vl_svmdataset_accumulator_,SFX)(const void* data,
                                        const vl_uindex element,
                                        double * model,
                                        const double multiplier)
{
  vl_size i,j ;
  T* tData ;
  VlSvmDataset* sdata = (VlSvmDataset*) data ;
  tData  = (T*) sdata->data ;

  if (sdata->mapFunc) {
    double * temp = vl_malloc(sizeof(double)*sdata->order) ;
    for (i = 0; i < sdata->dimension; i++) {
      sdata->mapFunc(sdata->map,temp,1,tData[element*sdata->dimension + i]) ;
      for (j = 0; j < sdata->order; j++){
        model[i*sdata->order + j] += multiplier * temp[j] ;
      }
    }
    vl_free(temp) ;
  } else {
    for (i = 0; i < sdata->dimension; i++) {
      model[i] += multiplier * tData[element*sdata->dimension + i] ;
    }
  }
}

/* VL_SVMDATASET_INSTANTIATING */
#undef FLT
#undef VL_SVMDATASET_INSTANTIATING
#endif
