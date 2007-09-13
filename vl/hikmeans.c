#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hikmeans.h"


/** @brief Delete node */
void vl_hikm_delete_node(VlHIKMNode * node)
{
  if(node)
    {
      int c;
      if(node->children)
        {
          for(c=0; c<node->K; c++)
            vl_hikm_delete_node(node->children[c]);
          free(node->children);
        }
      
      if(node->centers) 
        free(node->centers);
      
      free(node);
    }
}

vl_uint8* 
vl_hikm_copy_subset(vl_uint8 * data, vl_uint * ids, int N, int M, vl_uint id, int * N2)
{
  int i;
  int count = 0;
  
  for(i=0; i<N; i++)
    if(ids[i] == id)
      count++;
  *N2 = count;
  
  vl_uint8 * new_data = malloc(sizeof(vl_uint8)*M*count);
  
  count = 0;
  for(i=0; i<N; i++)
    if(ids[i] == id)
      {
        memcpy(&new_data[count*M], &data[i*M], M*sizeof(vl_uint8));
        count++;
      }
  
  *N2 = count;
  return new_data;
}

VlHIKMNode * 
vl_hikm_xmeans(VlHIKMTree * hikm, vl_uint8 *data, 
               int N, int K, int depth)
{
  fprintf(stderr, "xmeans depth=%d\n", depth);
  VlHIKMNode * node = malloc(sizeof(VlHIKMNode));
  node->children = 0;
  /* get the center points */
  vl_uint * ids = malloc(sizeof(vl_uint)*N);
  fprintf(stderr, "ikmeans with M=%d N=%d K=%d ids=%p\n", hikm->M, N, K, ids);
  node->centers = malloc(sizeof(vl_int32) * hikm->M * N) ;
  vl_ikmeans (node->centers, data, hikm->M, N, K, ids, 200);
  node->K = K;
  
  if(depth != 1)
    {
      /* find data assignments to children */
      /* for each child
       *      partition the data
       *      recurse with this partition to get the node
       */
      node->children = malloc(sizeof(VlHIKMNode *)*K);
      int c;
      for(c=0; c < K; c++)
        {
          int N2;
          vl_uint8 * new_data = vl_hikm_copy_subset(data, ids, N, hikm->M, c, 
                                                    &N2);
          node->children[c] = (VlHIKMNode *)vl_hikm_xmeans(hikm, new_data, N2, K, depth-1);
          free(new_data);
        }
    }
  free(ids);
  
  return node;
}

VlHIKMTree * vl_hikm(vl_uint8 * data, int M, int N, int K, int nleaves)
{
  VlHIKMTree * hikm = malloc(sizeof(VlHIKMTree));
  // First, figure out the depth
  hikm->depth = ceil(log(nleaves)/log(K));
  hikm->M = M;
  
  //fprintf(stderr, "Starting tree build, depth = %d\n", hikm->depth);
  // Now, build the tree recursively
  hikm->root = vl_hikm_xmeans(hikm, data, N, K, hikm->depth);
  
  //fprintf(stderr, "Done building tree\n");
  return hikm;
}

void vl_hikm_delete(VlHIKMTree * hikm)
{
  
  /* free all the subtrees */
  vl_hikm_delete_node(hikm->root);
  
  free(hikm);
}

vl_uint * vl_hikm_push(VlHIKMTree * hikm, vl_uint8 * data, int N)
{
  vl_uint * ids = malloc(sizeof(vl_uint)*hikm->depth*N);
  int i,d;
  int depth = hikm->depth;
  int M = hikm->M;
  
  /* For each datapoint */
  for(i=0; i<N; i++)
    {
        VlHIKMNode * node = hikm->root;
        d=0;
        while(node->centers)
          {
            vl_uint best = vl_ikmeans_push_one(node->centers, node->K, &data[i*M], M);
            ids[i*depth+d] = best;
            d++;
            
            if(!node->children) break;
            
            node = node->children[best];
          }
    }
  
  return ids;
}
