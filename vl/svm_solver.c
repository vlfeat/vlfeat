/** @file svm_solver.c
 ** @brief General Tools for Svm Solvers
 ** @author Daniele Perrone
 **/

/*
Copyright Statement
*/
#ifndef VL_SVM_INVARIANTS

#include "svm_solver.h"
#include <string.h>
#include <math.h>

VL_EXPORT
VlSvm* initialiseSvm (vl_size dimension, double regularizer, double biasMultiplier , double *preConditioner, vl_size maxIterations)
{
  VlSvm* svm ; 
  vl_size extDimension ; 

  extDimension = dimension ; 

  if (biasMultiplier)
    extDimension++ ;


  svm = (VlSvm*) vl_malloc(sizeof(VlSvm)) ;
  
  svm->model = (double*) vl_calloc(extDimension,sizeof(double)) ;

  svm->dimension = dimension ;
  svm->regularizer = regularizer ; 
  svm->biasMultiplier = biasMultiplier ; 
  svm->maxIterations = maxIterations ; 

  svm->iterationsSoFar = 0 ;
  
  

  if (!preConditioner)
    {
      svm->preConditioner = (double*) vl_calloc(extDimension,sizeof(double)) ;
      
      memcpy (svm->preConditioner, preConditioner,extDimension*sizeof(double)) ; 
    } 

  return svm ; 
}

VL_EXPORT
void deleteSvm (VlSvm* svm) 
{
  vl_free(svm->model) ;

  if (svm->preConditioner != NULL)
    vl_free(svm->preConditioner) ;

  vl_free(svm) ; 
}


VL_EXPORT void
vlSvmComputeDiagnostics(VlSvm *svm, VlSvmStatus* status, void const * data,vl_size dataDimension, vl_size numSamples, const vl_int8 * labels, vlSvmInnerProductFunction innerProduct, vlSvmFeatureMap mapFunc, const void * map )
{
  vl_size i, k ;
  vl_size numPos = 0 ;
  vl_size numNeg = 0 ;
  double pd ; 
  status->regularizer = 0.0 ;

  for (i = 0; i < svm->dimension; i++)
    {
      status->regularizer += svm->model[i] * svm->model[i] ; 
    }

  status->regularizer *= svm->regularizer * 0.5 ; 

  status->lossPos = 0 ;
  status->lossNeg = 0 ;
  status->hardLossPos = 0 ;
  status->hardLossNeg = 0 ; 

  for (k = 0; k < numSamples; k++)
    {
      pd = innerProduct(svm->model,svm->dimension,data,dataDimension,k,mapFunc,map) ;
      if (svm->biasMultiplier)
	{
	  pd += svm->model[svm->dimension]*svm->biasMultiplier ;
	}
      
      pd = VL_MAX(1 - labels[k]*pd, 0.0) ; 

      if (labels[k] < 0)
	{
	  status->lossNeg += pd ;
	  status->hardLossNeg += (pd > 0) ;
	  numNeg++ ;
	}
      else
	{
	  status->lossPos += pd ;
	  status->hardLossPos += (pd > 0) ;
	  numPos++ ;
	}
    }

  status->lossNeg /= numNeg ;
  status->hardLossNeg /= numNeg ;

  status->lossPos /= numPos ;
  status->hardLossPos /= numPos ; 

  status->energy = status->regularizer + status->lossPos + status->lossNeg ; 
}

#define FLT VL_TYPE_FLOAT
#define VL_SVM_INVARIANTS
#include "svm_solver.c"

#define FLT VL_TYPE_DOUBLE
#define VL_SVM_INVARIANTS
#include "svm_solver.c"

/* VL_SVM_INVARIANTS */

#else

#include "float.th"

VL_EXPORT double
VL_XCAT(vlSvmInnerProductFunction_,SFX) (const double* model, vl_size modelDimension,  const void* data, vl_size dataDimension, vl_uindex element, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_size i, j ; 
  T* tData ; 
  double res = 0;

 

  tData  = (T*) data ;
  
  if (mapFunc)
    {
      vl_size order = modelDimension / dataDimension ; 

      double temp[order] ; 

      for (i = 0; i < dataDimension; i++) 
	{
	  mapFunc(map,temp,1,tData[element*dataDimension + i]); 
      
	  for (j = 0; j < order; j++)
	    { 
	      res += model[i*order + j]*temp[j] ;
	    }
	}
    }
  else
    {
      for (i = 0; i < dataDimension; i++) 
	{
	  res += model[i]*(double)(tData[element*dataDimension + i]) ;
	}
    }

  return res ; 
}


VL_EXPORT void
VL_XCAT(vlSvmAccumulatorFunction_,SFX)(VlSvm* svm,  vl_size modelDimension, const void* data, vl_size dataDimension, vl_uindex element, double multiplier, vlSvmFeatureMap mapFunc, const void * map)
{
  vl_size i,j ; 
  T* tData ; 

  tData  = (T*) data ;
  
  if (mapFunc)
    {
      vl_size order = modelDimension / dataDimension ; 

      double temp[order] ; 
      
      
      
      if (svm->preConditioner)
	for (i = 0; i < dataDimension; i++) 
	  {
	    mapFunc(map,temp,1,tData[element*dataDimension + i]);
	    for (j = 0; j < order; j++)
	      {
		svm->model[i*order + j] += multiplier * svm->preConditioner[i*order + j] * temp[j] ;
	      }
	  }	
      else
	for (i = 0; i < dataDimension; i++) 
	  {
	    mapFunc(map,temp,1,tData[element*dataDimension + i]);
	    for (j = 0; j < order; j++)
	      {
		svm->model[i*order + j] += multiplier * temp[j] ;
	      }
	  }
    }
  else
    {
      if (svm->preConditioner)
	for (i = 0; i < dataDimension; i++) 
	  {
	    svm->model[i] += multiplier * svm->preConditioner[i] * tData[element*dataDimension + i] ;
	  }	
      else
	for (i = 0; i < dataDimension; i++) 
	  {
	    svm->model[i] += multiplier * tData[element*dataDimension + i] ;
	  }
    }
  
	
}



/* VL_SVM_INVARIANTS */
#undef FLT
#undef VL_PEGAOS_INSTANTIATING
#endif
