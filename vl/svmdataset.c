/** @file svm_dataset.c
 ** @brief Generic Svm Dataset
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/
#ifndef VL_SVM_INVARIANTS

#include "svm_dataset.h"
#include <string.h>
#include <math.h>

VL_EXPORT
VlSvmDataset* vl_svmdataset_new (void * data, vl_size dimension)
{
  VlSvmDataset * dataset ;
  dataset = (VlSvmDataset*) vl_malloc(sizeof(VlSvmDataset)) ;

  dataset->data = data ;
  dataset->dimension = dimension ;

  return dataset ;
}

VL_EXPORT
void vl_svmdataset_set_map (VlSvmDataset * data, void * map, VlSvmFeatureMap mapFunc, vl_size order)
{

  data->map  = map ;
  data->mapFunc = mapFunc ;
  data->order = order ;

  return data ;
}

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset)
{
  vl_free(dataset) ;
}




#define FLT VL_TYPE_FLOAT
#define VL_SVM_INVARIANTS
#include "svm_dataset.c"

#define FLT VL_TYPE_DOUBLE
#define VL_SVM_INVARIANTS
#include "svm_dataset.c"

/* VL_SVM_INVARIANTS */

#else

#include "float.th"

VL_EXPORT double
VL_XCAT(vl_svm_innnerproduct_,SFX) (const void* data, const vl_uindex element,
                                    const double* model)
{
  vl_size i, j ;
  T* tData ;
  double res = 0 ;



  tData  = (T*) data->data ;

  if (data->mapFunc)
    {

      double temp[data->order] ;

      for (i = 0; i < data->dimension; i++)
	{
	  data->mapFunc(data->map,temp,1,tData[element*data->dimension + i]);

	  for (j = 0; j < data->order; j++)
	    {
	      res += model[i*data->order + j]*temp[j] ;
	    }
	}
    }
  else
    {
      for (i = 0; i < data->dimension; i++)
	{
	  res += model[i]*(double)(tData[element*data->dimension + i]) ;
	}
    }

  return res ;
}


VL_EXPORT void
VL_XCAT(vl_svm_accumulator_,SFX)(const void* data,
                                 const vl_uindex element,
                                 double * model,
                                 const double multiplier)
{
  vl_size i,j ;
  T* tData ;

  tData  = (T*) data->data ;

  if (data->mapFunc)
    {

      double temp[data->order] ;

      
      for (i = 0; i < data->dimension; i++)
        {
          data->mapFunc(data->map,temp,1,tData[element*data->dimension + i]) ;
          for (j = 0; j < data->order; j++)
            {
              model[i*data->order + j] += multiplier * temp[j] ;
            }
        }
    }
  else
    {
      for (i = 0; i < data->dimension; i++)
        {
          model[i] += multiplier * tData[element*data->dimension + i] ;
        }
    }


}



/* VL_SVM_INVARIANTS */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif
