/** @file test_gmm.c
 ** @brief GMM test
 ** @author David Novotny
 **/

#include <vl/gmm.h>
#include <vl/host.h>
#include <vl/kmeans.h>
#include <vl/fisher.h>
#include <vl/vlad.h>
#include <stdio.h>
//#include <sys/time.h>

//#define TYPE double
//#define VL_F_TYPE VL_TYPE_DOUBLE

#define TYPE float
#define VL_F_TYPE VL_TYPE_FLOAT

void saveResults(const char * dataFileData, const char * dataFileResults, VlGMM * gmm, void * data, vl_size numData);

int main(int argc VL_UNUSED, char ** argv VL_UNUSED)
{
  VlKMeans * kmeans = 0;
  VlRand rand ;
  vl_size dataIdx, d, cIdx;
  VlGMM * gmm;

  double sigmaLowerBound = 0.000001;

  vl_size numData = 1000;
  vl_size dimension = 3;
  vl_size numClusters = 20;
  vl_size maxiter = 5;
  vl_size maxrep = 1;

  vl_size maxiterKM = 5;
  vl_size ntrees = 3;
  vl_size maxComp = 20;

  typedef enum _init {
    KMeans,
    Rand,
    Custom
  } Init ;

  vl_bool computeFisher = VL_TRUE;
  vl_bool computeVlad = VL_FALSE;

  Init init = KMeans;

  //char * dataFileResults = "/home/dave/vlfeat/data/gmm/gmm-results.mat";
  //char * dataFileData = "/home/dave/vlfeat/data/gmm/gmm-data.mat";

  TYPE * data = vl_malloc(sizeof(TYPE)*numData*dimension);
  TYPE * enc = vl_malloc(sizeof(TYPE)*2*dimension*numClusters);
  vl_uint32 * assign;

  vl_set_num_threads(0) ; /* use the default number of threads */

  vl_rand_init (&rand) ;
  vl_rand_seed (&rand, 49000) ;

  for(dataIdx = 0; dataIdx < numData; dataIdx++) {
    for(d = 0; d < dimension; d++) {
      data[dataIdx*dimension+d] = (TYPE)vl_rand_real3(&rand);
      //VL_PRINT("%f ",data[dataIdx*dimension+d]);
    }
    //VL_PRINT("\n");
  }

  gmm = vl_gmm_new (VL_F_TYPE, dimension, numClusters) ;

  switch(init) {
    case KMeans:
      kmeans = vl_kmeans_new(VL_F_TYPE,VlDistanceL2);
      vl_kmeans_set_verbosity	(kmeans,1);
      vl_kmeans_set_max_num_iterations (kmeans, maxiterKM) ;
      vl_kmeans_set_max_num_comparisons (kmeans, maxComp) ;
      vl_kmeans_set_num_trees (kmeans, ntrees);
      vl_kmeans_set_algorithm (kmeans, VlKMeansANN);
      vl_kmeans_set_initialization(kmeans, VlKMeansRandomSelection);
      vl_gmm_set_initialization (gmm,VlGMMKMeans);
      vl_gmm_set_kmeans_init_object(gmm,kmeans);
      break;

    case Rand:
      vl_gmm_set_initialization (gmm,VlGMMRand);
      break;

    case Custom: {
      TYPE * initSigmas;
      TYPE * initMeans;
      TYPE * initWeights;

      initSigmas = vl_malloc(sizeof(TYPE) * numClusters * dimension);
      initWeights = vl_malloc(sizeof(TYPE) * numClusters);
      initMeans = vl_malloc(sizeof(TYPE) * numClusters * dimension);

      vl_gmm_set_initialization (gmm,VlGMMCustom);

      for(cIdx = 0; cIdx < numClusters; cIdx++) {
        for(d = 0; d < dimension; d++) {
          initMeans[cIdx*dimension+d] = (TYPE)vl_rand_real3(&rand);
          initSigmas[cIdx*dimension+d] = (TYPE)vl_rand_real3(&rand);
        }
        initWeights[cIdx] = (TYPE)vl_rand_real3(&rand);
      }

      vl_gmm_set_priors(gmm,initWeights);
      vl_gmm_set_covariances(gmm,initSigmas);
      vl_gmm_set_means(gmm,initMeans);

      break;
    }

    default:
      abort();
  }

  vl_gmm_set_max_num_iterations (gmm, maxiter) ;
  vl_gmm_set_num_repetitions(gmm, maxrep);
  vl_gmm_set_verbosity(gmm,1);
  vl_gmm_set_covariance_lower_bound (gmm,sigmaLowerBound);

  //struct timeval t1,t2;
  //gettimeofday(&t1, NULL);

  vl_gmm_cluster (gmm, data, numData);

  //gettimeofday(&t2, NULL);
  //VL_PRINT("elapsed vlfeat: %f s\n",(double)(t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_usec - t1.tv_usec))/1000000.);

//    VL_PRINT("posterior:\n");
//    for(cIdx = 0; cIdx < clusterNum; cIdx++){
//        for(dataIdx = 0; dataIdx < Ndata; dataIdx++){
//            VL_PRINT("%f ",((float*)posteriors)[cIdx*Ndata+dataIdx]);
//        }
//        VL_PRINT("\n");
//    }

//  VL_PRINT("mean:\n");
//  for(cIdx = 0; cIdx < numClusters; cIdx++) {
//    for(d = 0; d < dimension; d++) {
//      VL_PRINT("%f ",((TYPE*)means)[cIdx*dimension+d]);
//    }
//    VL_PRINT("\n");
//  }
//
//  VL_PRINT("sigma:\n");
//  for(cIdx = 0; cIdx < numClusters; cIdx++) {
//    for(d = 0; d < dimension; d++) {
//      VL_PRINT("%f ",((TYPE*)sigmas)[cIdx*dimension+d]);
//    }
//    VL_PRINT("\n");
//  }
//
//  VL_PRINT("w:\n");
//  for(cIdx = 0; cIdx < numClusters; cIdx++) {
//    VL_PRINT("%f ",((TYPE*)weights)[cIdx]);
//    VL_PRINT("\n");
//  }

  //saveResults(dataFileData,dataFileResults,gmm,(void*) data, numData);

//  VL_PRINT("fisher:\n");
//  for(cIdx = 0; cIdx < clusterNum; cIdx++) {
//    for(d = 0; d < dimension*2; d++) {
//      VL_PRINT("%f ",enc[cIdx*dimension*2+d]);
//    }
//    VL_PRINT("\n");
//  }

  vl_free(data);
  numData = 2000;
  data = vl_malloc(numData*dimension*sizeof(TYPE));
  for(dataIdx = 0; dataIdx < numData; dataIdx++) {
    for(d = 0; d < dimension; d++) {
      data[dataIdx*dimension+d] = (TYPE)vl_rand_real3(&rand);
    }
  }

  if(computeFisher) {
    vl_fisher_encode
    (enc, VL_F_TYPE,
     vl_gmm_get_means(gmm), dimension, numClusters,
     vl_gmm_get_covariances(gmm),
     vl_gmm_get_priors(gmm),
     data, numData,
     VL_FISHER_FLAG_IMPROVED
     ) ;
  }

  assign = vl_malloc(numData*numClusters*sizeof(vl_uint32));
  for(dataIdx = 0; dataIdx < numData; dataIdx++) {
    for(cIdx = 0; cIdx < numClusters; cIdx++) {
      assign[cIdx*numData+dataIdx] = (vl_uint32)vl_rand_real3(&rand);
    }
  }

  if(computeVlad) {
    vl_free(enc);
    enc = vl_malloc(sizeof(TYPE)*dimension*numClusters);
    vl_vlad_encode
    (enc, VL_F_TYPE,
     vl_gmm_get_means(gmm), dimension, numClusters,
     data, numData,
     assign,
     0) ;
  }

  vl_gmm_delete(gmm);
  vl_free(data);
  if(enc){
    vl_free(enc);
  }
  if(kmeans) {
    vl_kmeans_delete(kmeans);
  }
  return 0 ;
}

void saveResults(const char * dataFileData, const char * dataFileResults, VlGMM * gmm, void * data, vl_size numData)
{
  char *mode = "w";
  FILE * ofp;

  vl_size d, cIdx;
  vl_uindex i_d;

  vl_size dimension = vl_gmm_get_dimension(gmm) ;
  vl_size numClusters = vl_gmm_get_num_clusters(gmm) ;
  vl_type dataType = vl_gmm_get_data_type(gmm) ;
  double const * sigmas = vl_gmm_get_covariances(gmm) ;
  double const * means = vl_gmm_get_means(gmm) ;
  double const * weights = vl_gmm_get_priors(gmm) ;
  double const * posteriors = vl_gmm_get_posteriors(gmm) ;

  ofp = fopen(dataFileData, mode);
  for(i_d = 0; i_d < numData; i_d++) {
    if(vl_gmm_get_data_type(gmm) == VL_TYPE_DOUBLE) {
      for(d = 0; d < vl_gmm_get_dimension(gmm) ; d++) {
        fprintf(ofp, "%f ", ((double*)data)[i_d * vl_gmm_get_dimension(gmm) + d]);
      }
    } else {
      for(d = 0; d < vl_gmm_get_dimension(gmm); d++) {
        fprintf(ofp, "%f ", ((float*) data)[i_d * vl_gmm_get_dimension(gmm) + d]);
      }
    }
    fprintf(ofp, "\n");
  }
  fclose (ofp);

  ofp = fopen(dataFileResults, mode);
  for(cIdx = 0; cIdx < numClusters; cIdx++) {
    if(dataType == VL_TYPE_DOUBLE) {
      for(d = 0; d < vl_gmm_get_dimension(gmm); d++) {
        fprintf(ofp, "%f ", ((double*)means)[cIdx*dimension+d]);
      }
      for(d = 0; d < dimension; d++) {
        fprintf(ofp, "%f ", ((double*)sigmas)[cIdx*dimension+d]);
      }
      fprintf(ofp, "%f ", ((double*)weights)[cIdx]);
      for(i_d = 0; i_d < numData; i_d++) {
        fprintf(ofp, "%f ", ((double*)posteriors)[cIdx*numData + i_d]);
      }
      fprintf(ofp, "\n");
    } else {
      for(d = 0; d < dimension; d++) {
        fprintf(ofp, "%f ", ((float*)means)[cIdx*dimension+d]);
      }
      for(d = 0; d < dimension; d++) {
        fprintf(ofp, "%f ", ((float*)sigmas)[cIdx*dimension+d]);
      }
      fprintf(ofp, "%f ", ((float*)weights)[cIdx]);
      for(i_d = 0; i_d < numData; i_d++) {
        fprintf(ofp, "%f ", ((float*)posteriors)[cIdx*numData + i_d]);

      }
      fprintf(ofp, "\n");
    }
  }
  fclose (ofp);
}
