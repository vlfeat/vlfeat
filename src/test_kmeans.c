/** @file test_kmeans.c
 ** @brief K-means test
 ** @author David Novotny
 **/


#include <vl/kmeans.h>
#include <vl/host.h>
#include <vl/kdtree.h>
//#include <sys/time.h>


int main(int argc VL_UNUSED, char ** argv VL_UNUSED)
{
  VlRand rand ;

  vl_size numData = 100000;
  vl_size dimension = 256;
  vl_size numCenters = 400;
  vl_size maxiter = 10;
  vl_size maxComp = 100;
  vl_size maxrep = 1;
  vl_size ntrees = 1;

  double * data;

  vl_size dataIdx, d;

  //VlKMeansAlgorithm algorithm = VlKMeansANN ;
  VlKMeansAlgorithm algorithm = VlKMeansLloyd ;
  //VlKMeansAlgorithm algorithm = VlKMeansElkan ;
  VlVectorComparisonType distance = VlDistanceL2 ;
  VlKMeans * kmeans = vl_kmeans_new (VL_TYPE_DOUBLE,distance) ;

  vl_rand_init (&rand) ;
  vl_rand_seed (&rand,  1000) ;

  data = vl_malloc(sizeof(double) * dimension * numData);

  for(dataIdx = 0; dataIdx < numData; dataIdx++) {
    for(d = 0; d < dimension; d++) {
      double randomNum = (double)vl_rand_real3(&rand)+1;
      data[dataIdx*dimension+d] = randomNum;
    }
  }

  vl_kmeans_set_verbosity	(kmeans,1);
  vl_kmeans_set_max_num_iterations (kmeans, maxiter) ;
  vl_kmeans_set_max_num_comparisons (kmeans, maxComp) ;
  vl_kmeans_set_num_repetitions (kmeans, maxrep) ;
  vl_kmeans_set_num_trees (kmeans, ntrees);
  vl_kmeans_set_algorithm (kmeans, algorithm);

  //struct timeval t1,t2;
  //gettimeofday(&t1, NULL);

  vl_kmeans_cluster(kmeans,data,dimension,numData,numCenters);

  //gettimeofday(&t2, NULL);

  //VL_PRINT("elapsed vlfeat: %f s\n",(double)(t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_usec - t1.tv_usec))/1000000.);

  vl_kmeans_delete(kmeans);
  vl_free(data);

  return 0 ;
}
