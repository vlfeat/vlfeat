#include <vl/kmeans.h>
#include <vl/host.h>
#include <vl/kdtree.h>
#include <sys/time.h>


int main(int argc VL_UNUSED, char ** argv VL_UNUSED)
{

  VlRand rand ;

  vl_size numData = 10000;
  vl_size dimension = 128;
  vl_size numCenters = 400;
  vl_size maxiter = 10;
  vl_size maxComp = 100;
  vl_size maxrep = 1;
  vl_size ntrees = 1;

  struct timeval tim;

  double * data;

  vl_size dataIdx, d, cIdx;

  //

  VlKMeansAlgorithm algorithm = VlKMeansANN ;
  //VlKMeansAlgorithm algorithm = VlKMeansLloyd ;
  //VlKMeansAlgorithm algorithm = VlKMeansElkan ;
  VlVectorComparisonType distance = VlDistanceL2 ;
  VlKMeans * kmeans = vl_kmeans_new (VL_TYPE_DOUBLE,distance) ;

  unsigned int iseed = (unsigned int)time(NULL);
  srand (iseed);

  vl_rand_init (&rand) ;
  vl_rand_seed (&rand,  1000) ;


  data = vl_malloc(sizeof(double) * dimension * numData);

  for(dataIdx = 0; dataIdx < numData; dataIdx++) {
    for(d = 0; d < dimension; d++) {
      double randomNum = (double)vl_rand_real3(&rand)+1;
      data[dataIdx*dimension+d] = randomNum;
      //VL_PRINT("%f ",data[dataIdx*dimensions+d]);
    }
    //VL_PRINT("\n");
  }

  vl_kmeans_set_verbosity	(kmeans,1);
  vl_kmeans_set_max_num_iterations (kmeans, maxiter) ;
  vl_kmeans_set_max_num_comparisons (kmeans, maxComp) ;
  vl_kmeans_set_num_trees (kmeans, ntrees);
  vl_kmeans_set_algorithm (kmeans, algorithm);
  vl_kmeans_set_multithreading (kmeans,VlKMeansParallel);

  gettimeofday(&tim, NULL);
  double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

  vl_kmeans_cluster(kmeans,data,dimension,numData,numCenters);

  gettimeofday(&tim, NULL);
  double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
  printf("Vlfeat wall time: %.6lf seconds\n", t2-t1);

  for(cIdx = 0; cIdx < numCenters; cIdx++) {
    for(d = 0; d < dimension; d++) {
      //VL_PRINT("%f ",((double*)kmeans->centers)[cIdx*dimensions+d]);
    }
    //VL_PRINT("\n");
  }

  vl_kmeans_delete(kmeans);
  vl_free(data);

  return 0 ;
}
