/** @file   hikmeans.c
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Heirachical Integer K-Means Clustering - Declaration
 **/

/** @file hikmeans.h
 ** @brief Hierachical integer K-Means clustering
 ** 
 ** Hierarichial integer K-Means clustering (HIKM) is a simple
 ** hierarchical version of integer K-Means (@ref ikmeans.h
 ** "IKM"). The algorithm recusrively apply integer K-means to create
 ** more refined partitions of the data.
 **
 ** Use the ::vl_hikm() function to partition the data and create a
 ** HIKM tree (and ::vl_hikm_delete() to dispose of it).  Use
 ** ::vl_hikm_push() to project new data down a HIKM tree.
 **
 ** @section hikm-tree HIKM tree
 **
 ** The HIKM tree is a simple structure, represented by ::VlHIKMTree
 ** and ::VlHIKMNode. The tree as a 
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hikmeans.h"

/** @brief Delete node 
 **
 ** @param node to delete.
 **
 ** The function deletes recursively @a node and all its descendent.
 **/
void 
vl_hikm_delete_node (VlHIKMNode * node)
{
  if(node)
    {
      int c;
      if (node->children)
        {
          for(c = 0 ; c < node->K ; ++c)
            vl_hikm_delete_node (node->children[c]) ;
          vl_free (node->children) ;
        }
      
      if (node->centers) 
        vl_free (node->centers);
      
      vl_free(node);
    }
}

/** @brief Copy a subset of the data to a buffer
 **
 ** @param data  Data.
 ** @param ids   Data labels.
 ** @param N     Number of indeces.
 ** @param M     Data dimensionality. 
 ** @param id    Label of data to copy.
 ** @param N2    Number of data copied (out).
 **
 ** @return a new buffer with a copy of the selected data.
 **/
vl_uint8* 
vl_hikm_copy_subset (vl_uint8 * data, 
                     vl_uint * ids, int N, int M, 
                     vl_uint id, int *N2)
{
  int i ;
  int count = 0;

  /* count how many data with this label there are */
  for (i = 0 ; i < N ; i++)
    if (ids[i] == id)
      count ++ ;
  *N2 = count ;
  
  /* copy each datum to the buffer */
  {
    vl_uint8 * new_data = vl_malloc (sizeof(vl_uint8) * M * count);    
    count = 0;
    for (i = 0 ; i < N ; i ++)
      if (ids[i] == id)
        {
          memcpy(new_data + count * M, 
                 data     + i     * M, 
                 sizeof(vl_uint8) * M);
          count ++ ;
        }    
    *N2 = count ;
    return new_data ;
  }  
}


/** @brief Compute hierarchical integer K-means clustering.
 **
 ** @param hikm   HIKM tree to fill.
 ** @param data   Data to cluster.
 ** @param N      Number of data points.
 ** @param K      Number of clusters for this node.
 ** @param depth  Tree depth.
 **
 ** @return a new HIKM node representing a sub-clustering.
 **/
VlHIKMNode * 
vl_hikm_xmeans (VlHIKMTree *hikm, 
                vl_uint8 *data, 
                int N, int K, int depth)
{
  VlHIKMNode *node = vl_malloc (sizeof(VlHIKMNode)) ;
  vl_uint     *ids = vl_malloc (sizeof(vl_uint) * N) ;

  node-> centers  = vl_malloc (sizeof(vl_int32) * hikm->M * N) ;
  node-> K        = K ;
  node-> children = (depth == 1) ? 0 : vl_malloc (sizeof(VlHIKMNode*) * K) ;

  vl_ikmeans (node->centers, ids, 
              data, hikm->M, N, K,
              hikm->miter) ;
  
  if (depth != 1)
    {
      /* For each child
       * - partition the data
       * - recurse with this partition to get the node
       */
      int c ;
      for (c = 0 ; c < K ; c ++)
        {
          int N2 ;
          vl_uint8 * new_data = vl_hikm_copy_subset
            (data, ids, N, hikm->M, c, &N2) ;

          node->children[c] = (VlHIKMNode *) vl_hikm_xmeans
            (hikm, new_data, N2, VL_MIN(K, N2), depth - 1) ;

          vl_free (new_data);
        }
    }
  vl_free(ids) ;
  
  return node ;
}

/** @brief Initialize HIKM tree.
 **
 ** @param data    Data to cluster.
 ** @param M       Data dimensionality.
 ** @param N       Number of data.
 ** @param K       Number of clusters per node.
 ** @param nleaves Minimum number of leaves.
 **
 ** @return a new HIKM tree representing the clustering.
 **/ 
VlHIKMTree * 
vl_hikm (vl_uint8 * data, int M, int N, int K, int nleaves)
{
  VlHIKMTree * hikm = vl_malloc (sizeof(VlHIKMTree)) ;

  /* Make the tree deep enough to get at least the target number of
   * leaves. x*/
  hikm->depth = ceil(log(nleaves) / log(K)) ;
  hikm->M     = M ;
  hikm->K     = K ;
  hikm->miter = 200 ;
  hikm->root  = vl_hikm_xmeans (hikm, data, N, VL_MIN(K, N), hikm->depth) ;
  return hikm ;
}

/** @brief Delete HIKM tree.
 ** @param hikm tree to delete.
 **/
void vl_hikm_delete(VlHIKMTree *hikm)
{
  vl_hikm_delete_node (hikm->root) ;  
  vl_free(hikm);
}

/** @biref Project data down HIKM tree
 **
 ** @param hikm HIKM tree.
 ** @param data Data to project.
 ** @param N    Number of data.
 **
 ** @return a new vector with the data to node assignments.
 **/
vl_uint * 
vl_hikm_push (VlHIKMTree *hikm, vl_uint8 *data, int N)
{
  int M         = hikm->M ;
  int depth     = hikm->depth ;
  vl_uint * ids = vl_malloc (sizeof(vl_uint) * depth * N) ;
  int i, d ;
  
  /* for each datum */
  for(i = 0 ; i < N ; i++)
    {
      VlHIKMNode *node = hikm->root ;
      d = 0 ;
      while(node->centers)
        {
          vl_uint best = vl_ikmeans_push_one
            (node->centers, node->K, data + i * M, M) ;
          ids[i*depth+d] = best ;
          d ++ ;
          
          if (!node->children) break ;
          
          node = node->children [best] ;
        }
    }
  
  return ids ;
}
