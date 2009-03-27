/** @internal
 ** @file     quickshift.c
 ** @author   Andrea Vedaldi
 ** @author   Brian Fulkerson
 ** @brief    Quick Shift - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

/** @file   quickshift.c
 ** @author Andrea Vedaldi
 ** @author Brian Fulkerson
 ** @brief  Quick Shift

 Quick Shift is a fast mode seeking algorithm, similar to mean shift.
    
**/
  
#include "quickshift.h"
#include "mathop.h"

#include <string.h>
#include <math.h>
#include <stdio.h>


/** -----------------------------------------------------------------
 ** @internal
 ** @brief Computes the accumulated channel L2 distance between
 **        i,j + the distance between i,j
 **
 ** @param I    input image buffer
 ** @param N1   size of the first dimension of the image
 ** @param N2   size of the second dimension of the image
 ** @param K    number of channels
 ** @param i1   first dimension index of the first pixel to compare
 ** @param i2   second dimension of the first pixel
 ** @param j1   index of the second pixel to compare
 ** @param j2   second dimension of the second pixel
 **
 ** Takes the L2 distance between the values in I at pixel i and j, 
 ** accumulating along K channels and adding in the distance
 ** between i,j in the image.
 ** 
 ** @return the distance as described above
 **/

VL_INLINE
vl_qs_type
vl_quickshift_distance(vl_qs_type const * I, 
         int N1, int N2, int K,
         int i1, int i2,
         int j1, int j2) 
{
  vl_qs_type dist = 0 ;
  int d1 = j1 - i1 ;
  int d2 = j2 - i2 ;
  int k ;
  dist += d1*d1 + d2*d2 ;
  /* For k = 0...K-1, d+= L2 distance between I(i1,i2,k) and 
   * I(j1,j2,k) */
  for (k = 0 ; k < K ; ++k) {
    vl_qs_type d = 
      I [i1 + N1 * i2 + (N1*N2) * k] - 
      I [j1 + N1 * j2 + (N1*N2) * k] ;
    dist += d*d ;
  }
  return dist ;
}

/** -----------------------------------------------------------------
 ** @internal
 ** @brief Computes the accumulated channel inner product between i,j + the
 **        distance between i,j
 ** 
 ** @param I    input image buffer
 ** @param N1   size of the first dimension of the image
 ** @param N2   size of the second dimension of the image
 ** @param K    number of channels
 ** @param i1   first dimension index of the first pixel to compare
 ** @param i2   second dimension of the first pixel
 ** @param j1   index of the second pixel to compare
 ** @param j2   second dimension of the second pixel
 **
 ** Takes the channel-wise inner product between the values in I at pixel i and
 ** j, accumulating along K channels and adding in the inner product between i,j in
 ** the image.
 ** 
 ** @return the inner product as described above
 **/

VL_INLINE
vl_qs_type
vl_quickshift_inner(vl_qs_type const * I, 
      int N1, int N2, int K,
      int i1, int i2,
      int j1, int j2) 
{
  vl_qs_type ker = 0 ;
  int k ;
  ker += i1*j1 + i2*j2 ;
  for (k = 0 ; k < K ; ++k) {
    ker += 
      I [i1 + N1 * i2 + (N1*N2) * k] *
      I [j1 + N1 * j2 + (N1*N2) * k] ;
  }
  return ker ;
}


VL_EXPORT
VlQS *
vl_quickshift_new(vl_qs_type const * image, int height, int width,
                       int channels)
{
  VlQS * q = vl_malloc(sizeof(VlQS));

  q->image    = (vl_qs_type *)image;
  q->height   = height;
  q->width    = width;
  q->channels = channels;

  q->medoid   = VL_FALSE;
  q->tau      = VL_MAX(height,width)/50;
  q->sigma    = VL_MAX(2, q->tau/3);

  q->dists    = vl_malloc(sizeof(vl_qs_type)*height*width);
  q->parents  = vl_malloc(sizeof(int)*height*width); 
  q->density  = vl_malloc(sizeof(vl_qs_type)*height*width);

  return q;
}

VL_EXPORT
void vl_quickshift_process(VlQS * q)
{
  vl_qs_type const *I = q->image;
  int        *parents = q->parents;
  vl_qs_type *E = q->density;
  vl_qs_type *dists = q->dists; 
  vl_qs_type *M = 0, *n = 0 ;
  vl_qs_type sigma = q->sigma ;
  vl_qs_type tau = q->tau;
  vl_qs_type tau2 = tau*tau;
  
  int K = q->channels, d;
  int N1 = q->height, N2 = q->width;
  int i1,i2, j1,j2, R, tR;

  d = 2 + K ; /* Total dimensions include spatial component (x,y) */

  if (q->medoid) { /* n and M are only used in mediod shift */
    M = (vl_qs_type *) vl_calloc(N1*N2*d, sizeof(vl_qs_type)) ;
    n = (vl_qs_type *) vl_calloc(N1*N2,   sizeof(vl_qs_type)) ;
  }

  R = (int) ceil (3 * sigma) ;
  tR = (int) ceil (tau) ;
  
  /* -----------------------------------------------------------------
   *                                                                 n 
   * -------------------------------------------------------------- */

  /* If we are doing medoid shift, initialize n to the inner product of the
   * image with itself
   */
  if (n) { 
    for (i2 = 0 ; i2 < N2 ; ++ i2) {
      for (i1 = 0 ; i1 < N1 ; ++ i1) {        
        n [i1 + N1 * i2] = vl_quickshift_inner(I,N1,N2,K,
                                               i1,i2,
                                               i1,i2) ;
      }
    }
  }
  
  /* -----------------------------------------------------------------
   *                                                 E = - [oN'*F]', M
   * -------------------------------------------------------------- */
  
  /* 
     D_ij = d(x_i,x_j)
     E_ij = exp(- .5 * D_ij / sigma^2) ;
     F_ij = - E_ij             
     E_i  = sum_j E_ij
     M_di = sum_j X_j F_ij

     E is the parzen window estimate of the density
     0 = dissimilar to everything, windowsize = identical
  */
  
  for (i2 = 0 ; i2 < N2 ; ++ i2) {
    for (i1 = 0 ; i1 < N1 ; ++ i1) {
      
      int j1min = VL_MAX(i1 - R, 0   ) ;
      int j1max = VL_MIN(i1 + R, N1-1) ;
      int j2min = VL_MAX(i2 - R, 0   ) ;
      int j2max = VL_MIN(i2 + R, N2-1) ;      
      
      /* For each pixel in the window compute the distance between it and the
       * source pixel */
      for (j2 = j2min ; j2 <= j2max ; ++ j2) {
        for (j1 = j1min ; j1 <= j1max ; ++ j1) {
          vl_qs_type Dij = vl_quickshift_distance(I,N1,N2,K, i1,i2, j1,j2) ;          
          /* Make distance a similarity */ 
          vl_qs_type Fij = - exp(- Dij / (2*sigma*sigma)) ;

          /* E is E_i above */
          E [i1 + N1 * i2] -= Fij ;
          
          if (M) {
            /* Accumulate votes for the median */
            int k ;
            M [i1 + N1*i2 + (N1*N2) * 0] += j1 * Fij ;
            M [i1 + N1*i2 + (N1*N2) * 1] += j2 * Fij ;
            for (k = 0 ; k < K ; ++k) {
              M [i1 + N1*i2 + (N1*N2) * (k+2)] += 
                I [j1 + N1*j2 + (N1*N2) * k] * Fij ;
            }
          } 
          
        } /* j1 */ 
      } /* j2 */

    }  /* i1 */
  } /* i2 */
  
  /* -----------------------------------------------------------------
   *                                               Find best neighbors
   * -------------------------------------------------------------- */
  
  if (q->medoid) {
    
    /* 
       Qij = - nj Ei - 2 sum_k Gjk Mik
       n is I.^2
    */
    
    /* medoid shift */
    for (i2 = 0 ; i2 < N2 ; ++i2) {
      for (i1 = 0 ; i1 < N1 ; ++i1) {
        
        vl_qs_type sc_best = 0  ;
        /* j1/j2 best are the best indicies for each i */
        vl_qs_type j1_best = i1 ;
        vl_qs_type j2_best = i2 ; 
        
        int j1min = VL_MAX(i1 - R, 0   ) ;
        int j1max = VL_MIN(i1 + R, N1-1) ;
        int j2min = VL_MAX(i2 - R, 0   ) ;
        int j2max = VL_MIN(i2 + R, N2-1) ;      
        
        for (j2 = j2min ; j2 <= j2max ; ++ j2) {
          for (j1 = j1min ; j1 <= j1max ; ++ j1) {            
            
            vl_qs_type Qij = - n [j1 + j2 * N1] * E [i1 + i2 * N1] ;
            int k ;

            Qij -= 2 * j1 * M [i1 + i2 * N1 + (N1*N2) * 0] ;
            Qij -= 2 * j2 * M [i1 + i2 * N1 + (N1*N2) * 1] ;
            for (k = 0 ; k < K ; ++k) {
              Qij -= 2 * 
                I [j1 + j2 * N1 + (N1*N2) * k] *
                M [i1 + i2 * N1 + (N1*N2) * (k + 2)] ;
            }
            
            if (Qij > sc_best) {
              sc_best = Qij ;
              j1_best = j1 ;
              j2_best = j2 ;
            }
          }
        }

        /* parents_i is the linear index of j which is the best pair (in matlab
         * notation
         * dists_i is the score of the best match
         */
        parents [i1 + N1 * i2] = j1_best + N1 * j2_best + 1 ;
        dists[i1 + N1 * i2] = sc_best ;
      }
    }  

  } else {
    
    /* Quickshift assigns each i to the closest j which has an increase in the
     * density (E). If there is no j s.t. Ej > Ei, then dists_i == inf (a root
     * node in one of the trees of merges).
     */
    for (i2 = 0 ; i2 < N2 ; ++i2) {
      for (i1 = 0 ; i1 < N1 ; ++i1) {
        
        vl_qs_type E0 = E [i1 + N1 * i2] ;
        vl_qs_type d_best = VL_QS_INF ;
        vl_qs_type j1_best = i1   ;
        vl_qs_type j2_best = i2   ; 
        
        int j1min = VL_MAX(i1 - tR, 0   ) ;
        int j1max = VL_MIN(i1 + tR, N1-1) ;
        int j2min = VL_MAX(i2 - tR, 0   ) ;
        int j2max = VL_MIN(i2 + tR, N2-1) ;      
        
        for (j2 = j2min ; j2 <= j2max ; ++ j2) {
          for (j1 = j1min ; j1 <= j1max ; ++ j1) {            
            if (E [j1 + N1 * j2] > E0) {
              vl_qs_type Dij = vl_quickshift_distance(I,N1,N2,K, i1,i2, j1,j2) ;
              if (Dij <= tau2 && Dij < d_best) {
                d_best = Dij ;
                j1_best = j1 ;
                j2_best = j2 ;
              }
            }
          }
        }
        
        /* parents is the index of the best pair */
        /* dists_i is the minimal distance, inf implies no Ej > Ei within
         * distance tau from the point */
        parents [i1 + N1 * i2] = j1_best + N1 * j2_best + 1 ;
        dists[i1 + N1 * i2] = sqrt(d_best) ;
      }
    }  
  }
  
  if (M) vl_free(M) ;
  if (n) vl_free(n) ;
}

void vl_quickshift_delete(VlQS * q)
{
  if (q) {
    if (q->parents) vl_free(q->parents);
    if (q->dists)   vl_free(q->dists);
    if (q->density) vl_free(q->density);
    
    vl_free(q);
  }
}
