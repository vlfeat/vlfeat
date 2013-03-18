/** @file svm_dca.c
 ** @brief Linear SVM Solver based on Dual Coordinate Descent Method
 ** @author Milan Sulc
 ** @author Andrea Vedaldi
 **/

/*
Copyright (C) 2013 Milan Sulc, Andrea Vedaldi.

All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/


#ifndef VL_DCA_INSTANTIATING

#include "generic.h"
#include "svm_dca.h"
#include <math.h>

#define SFX _validation_data
#define VALIDATION
#define VL_DCA_INSTANTIATING
#include "svm_dca.c"

#define SFX
#define VL_DCA_INSTANTIATING
#include "svm_dca.c"





/** -------------------------------------------------------------------
 ** @brief Random permutation of an vl_int array
 ** @param *A pointer to a @ref vl_int array
 ** @param size @ref vl_size size of array
 ** @param *random pointer to a @ref VlRand random generator
 **
 ** The function randomly orders an array using "Algorithm P" also known as Knuth shuffle
 **/

VL_EXPORT
void vl_int_array_random_permute(vl_int *A, vl_size size, VlRand *random) {
  vl_int ai, i, j;
  for (i = size - 1; i > 0; i--) {

  //random integer, 0 ≤ j ≤ i
    j = (vl_int) vl_rand_uindex (random, i+1);
    ai = A[i];
    A[i] = A[j];
    A[j] = ai;
  }
}



#else

/** -------------------------------------------------------------------
 ** @brief Train an SVM using Dual Coordinate Ascent Method
 ** @param *svm pointer to a @ref VlSvm structure.
 ** @param *dataset pointer to training dataset
 ** @param numSamples number of training samples
 ** @param labels labels for training data (+1/-1)
 **
 ** The function trains an SVM in a way similar to the method presented in @cite{shwartz13a-dual}
 ** @sa ::vl_svm_new().
 **/
VL_EXPORT void
VL_XCAT(vl_svm_dca_train,SFX) (VlSvm * svm,
                    void * dataset,
                    vl_size numSamples,
                    VlSvmDatasetInnerProduct innerProduct,
                    VlSvmDatasetAccumulator accumulator,
                    VlSvmDatasetLengthSquare lengthSquare,
                    VlSvmLossFunction lossFunction,
                    VlSvmLossConjugateFunction lossConjugateFunction,
                    VlSvmDeltaAlpha deltaAlpha,
                    vl_int8 const * labels
#ifdef VALIDATION
                    ,void * validation,
                    vl_size validationNumSamples,
                    vl_int8 const * validationLabels
#endif
				) {


  double * xi_squares;
  vl_int * A;
  vl_size i,p;
  vl_bool inc, doBreak;
  double inner, delta, multiplier;

  /* Init and seed VLFeat random generator */
  VlRand rand;
  vl_rand_init (&rand);
  vl_rand_seed (&rand, clock()) ;


  /*  Start timer */
  vl_tic() ;



  /* Init  A = {0,...,l-1}, alpha = {0,0,...,0};
   Precompute Q_ii */
  svm->alpha = (double*) vl_calloc(numSamples, sizeof(double));
  xi_squares = (double*) vl_calloc(numSamples, sizeof(double));
  A = (vl_int*) vl_calloc(numSamples, sizeof(vl_int));


  for (i=0; i < numSamples; i++) {
    A[i] = i;

    // compute the length square a of data vector (inner product with itself)
    if(lengthSquare == NULL) {
      double * xw = (double*) vl_calloc(svm->dimension, sizeof(double)) ;
      accumulator(dataset,i,xw,1) ;
      xi_squares[i] = innerProduct(dataset,i,xw);
    } else
      xi_squares[i] = lengthSquare(dataset,i);

    xi_squares[i] += svm->biasMultiplier*svm->biasMultiplier; // as not working on homogeneous coordinates
  }

  /* Outer iterations */
  inc = 1;
  if(svm->onlineSetting) inc = 0; // online setting increments in inner cycle, not in outer cycle
  doBreak = 0;
  for( ; !doBreak && svm->iterations < svm->maxIterations; svm->iterations+=inc) {

    if(svm->randomPermutation) vl_int_array_random_permute(A,numSamples,&rand);


    for (p=0; p < numSamples; p++) {
      vl_int i = A[p];

      inner = innerProduct(dataset,i,svm->model);
      inner += svm->bias * svm->biasMultiplier;
      delta = deltaAlpha(labels[i], inner, svm, xi_squares[i], svm->alpha[i], numSamples);

      if(delta!=0) {
        /* update alpha */
        svm->alpha[i] += delta;

        /* update model */
        multiplier = delta/(numSamples*svm->lambda);
        accumulator(dataset,i,svm->model,multiplier);
        svm->bias += multiplier * svm->biasMultiplier;
      }


      if (svm->onlineSetting) {
        svm->iterations++;
        if (svm->iterations % svm->energyFrequency == 0) {
          svm->elapsedTime += vl_toc() ;

          #ifdef VALIDATION
          vl_svm_compute_diagnostic(svm, validation, validationNumSamples, validationLabels, innerProduct, lossFunction, lossConjugateFunction);
          #else
          vl_svm_compute_diagnostic(svm, dataset,numSamples, labels, innerProduct, lossFunction, lossConjugateFunction);
          #endif
          /* Diagnostic */
          if (svm->diagnostic) {
          //  svm->elapsedTime += vl_toc() ;
            svm->diagnostic(svm) ; /* MATLAB callback */
          //  vl_tic() ;
          }

          vl_tic() ;

          if(svm->objective->dualityGap < svm->epsilon) {
            doBreak = 1;
            break ;
          }

        }

        if (svm->iterations >= svm->maxIterations) {
          doBreak = 1;
          break;

        }

      }

    }


    if (!svm->onlineSetting  && (svm->iterations % svm->energyFrequency == 0) ) {
      svm->elapsedTime += vl_toc() ;

      #ifdef VALIDATION
      vl_svm_compute_diagnostic(svm, validation, validationNumSamples, validationLabels, innerProduct, lossFunction, lossConjugateFunction);
      #else
      vl_svm_compute_diagnostic(svm, dataset,numSamples, labels, innerProduct, lossFunction, lossConjugateFunction);
      #endif

      /* Diagnostic */
      if (svm->diagnostic) {
      //  svm->elapsedTime += vl_toc() ;
        svm->diagnostic(svm) ; /* MATLAB callback */
      //  vl_tic() ;
      }

      vl_tic() ;

      if(svm->objective->dualityGap <= svm->epsilon) {
        break ;
      }
    }
  }

  svm->elapsedTime += vl_toc() ;

  vl_svm_compute_diagnostic(svm, dataset,numSamples, labels, innerProduct, lossFunction, lossConjugateFunction);
}

#undef SFX
#undef VALIDATION
#undef VL_DCA_INSTANTIATING
#endif
