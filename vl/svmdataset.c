/** @file svm_dataset.c
 ** @brief Generic Svm Dataset
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/
#ifndef VL_SVM_INVARIANTS

#include "svmdataset.h"
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

}

VL_EXPORT
void vl_svmdataset_delete (VlSvmDataset * dataset)
{
  vl_free(dataset) ;
}




#define FLT VL_TYPE_FLOAT
#define VL_SVM_INVARIANTS
#include "svmdataset.c"

#define FLT VL_TYPE_DOUBLE
#define VL_SVM_INVARIANTS
#include "svmdataset.c"

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

  VlSvmDataset* sdata = (VlSvmDataset*) data ;

  tData  = (T*) sdata->data ;

  if (sdata->mapFunc)
    {

      double temp[sdata->order] ;

      for (i = 0; i < sdata->dimension; i++)
	{
	  sdata->mapFunc(sdata->map,temp,1,tData[element*sdata->dimension + i]);

	  for (j = 0; j < sdata->order; j++)
	    {
	      res += model[i*sdata->order + j]*temp[j] ;
	    }
	}
    }
  else
    {
      for (i = 0; i < sdata->dimension; i++)
	{
	  res += model[i]*(double)(tData[element*sdata->dimension + i]) ;
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


  VlSvmDataset* sdata = (VlSvmDataset*) data ;

  tData  = (T*) sdata->data ;

  if (sdata->mapFunc)
    {

      double temp[sdata->order] ;


      for (i = 0; i < sdata->dimension; i++)
        {
          sdata->mapFunc(sdata->map,temp,1,tData[element*sdata->dimension + i]) ;
          for (j = 0; j < sdata->order; j++)
            {
              model[i*sdata->order + j] += multiplier * temp[j] ;
            }
        }
    }
  else
    {
      for (i = 0; i < sdata->dimension; i++)
        {
          model[i] += multiplier * tData[element*sdata->dimension + i] ;
        }
    }


}



/* VL_SVM_INVARIANTS */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif
