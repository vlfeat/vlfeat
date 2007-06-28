/* file:        mser.c
** description: Maximally Stable Extremal Regions
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS
Copyright (C) 2006 Regents of the University of California
All rights reserved

Written by Andrea Vedaldi (UCLA VisionLab).

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the University of California, Berkeley nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/** @file
 ** @brief Maximally Stable Extremal Regions - MEX implementation
 **/

#include<mexutils.c>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))

#define USE_BUCKET_SORT
#define USE_RANK_UNION

typedef char unsigned val_t ;
typedef int  unsigned idx_t ;
typedef long long int unsigned acc_t ;

/* pairs are used to sort the pixels */
typedef struct 
{
  val_t value ; 
  idx_t  index ;
} pair_t ;

/* forest node */
typedef struct
{
  idx_t parent ;   /**< parent pixel         */
  idx_t shortcut ; /**< shortcut to the root */
  idx_t region ;   /**< index of the region  */
  int   area ;     /**< area of the region   */
#ifdef USE_RANK_UNION
  int   height ;    /**< node height         */
#endif
} node_t ;

/* extremal regions */
typedef struct
{
  idx_t parent ;     /**< parent region                           */
  idx_t index ;      /**< index of root pixel                     */
  val_t value ;      /**< value of root pixel                     */
  int   area ;       /**< area of the region                      */
  int   area_top ;   /**< area of the region DELTA levels above   */
  int   area_bot  ;  /**< area of the region DELTA levels below   */
  float variation ;  /**< variation                               */
  int   maxstable ;  /**< max stable number (=0 if not maxstable) */
} region_t ;

/* predicate used to sort pixels by increasing intensity */
int 
cmp_pair(void const* a, void const* b) 
{
  pair_t* pa = (pair_t*) a;
  pair_t* pb = (pair_t*) b;
  return pa->value - pb->value ;
}

/* advance N-dimensional subscript */
void
adv(int const* dims, int ndims, int* subs_pt)
{
  int d = 0 ;
  while(d < ndims) {
    if( ++subs_pt[d]  < dims[d] ) return ;
    subs_pt[d++] = 0 ;
  }
}

/* driver */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_I=0, IN_DELTA} ;
  enum {OUT_REGIONS=0, OUT_ELL, OUT_PARENTS, OUT_AREA} ;

  idx_t i ;
  idx_t rindex = 0 ;
  int k ; 

  /* configuration */
  int verbose = 1 ;      /* be verbose                              */
  int small_cleanup= 1 ; /* remove very small regions               */
  int big_cleanup  = 1 ; /* remove very big regions                 */
  int bad_cleanup  = 0 ; /* remove very bad regions                 */
  int dup_cleanup  = 1 ; /* remove duplicates                       */
  val_t delta ;          /* stability delta                         */

  /* node value denoting a void node */
  idx_t const node_is_void = 0xffffffff ;

  int*   subs_pt ;       /* N-dimensional subscript                 */
  int*   nsubs_pt ;      /* diff-subscript to point to neigh.       */
  idx_t* strides_pt ;    /* strides to move in image array          */
  idx_t* visited_pt ;    /* flag                                    */

  int nel ;              /* number of image elements (pixels)       */
  int ner = 0 ;          /* number of extremal regions              */
  int nmer = 0 ;         /* number of maximally stable              */
  int ndims ;            /* number of dimensions                    */
  int const* dims ;      /* dimensions                              */
  int njoins = 0 ;       /* number of join ops                      */

  val_t const* I_pt ;    /* source image                            */
  pair_t*   pairs_pt ;   /* scratch buffer to sort pixels           */
  node_t*   forest_pt ;  /* the extremal regions forest             */
  region_t* regions_pt ; /* list of extremal regions found          */ 

  /* ellipses fitting */
  acc_t* acc_pt ;        /* accumulator to integrate region moments */
  acc_t* ell_pt ;        /* ellipses parameters                     */
  int    gdl ;           /* number of parameters of an ellipse      */
  idx_t* joins_pt ;      /* sequence of joins                       */
  
  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
  if (nin != 2) {
    mexErrMsgTxt("Two arguments required.") ;
  } else if (nout > 4) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  if(mxGetClassID(in[IN_I]) != mxUINT8_CLASS) {
    mexErrMsgTxt("I must be of class UINT8") ;
  }

  if(!uIsScalar(in[IN_DELTA])) {
    mexErrMsgTxt("DELTA must be scalar") ;
  }

  delta = 0 ;
  switch(mxGetClassID(in[IN_DELTA])) {
  case mxUINT8_CLASS :
    delta = * (val_t*) mxGetData(in[IN_DELTA]) ;
    break ;
    
  case mxDOUBLE_CLASS :
    {
      double x = *mxGetPr(in[IN_DELTA])  ;
      if(x < 0.0) {
        mexErrMsgTxt("DELTA must be non-negative") ;
      }
      delta = (val_t) x ;      
    }
    break ;
    
  default :
    mexErrMsgTxt("DELTA must be of class DOUBLE or UINT8") ;
  }

  /* get dimensions */
  nel   = mxGetNumberOfElements(in[IN_I]) ;
  ndims = mxGetNumberOfDimensions(in[IN_I]) ;
  dims  = mxGetDimensions(in[IN_I]) ;
  I_pt  = mxGetData(in[IN_I]) ;

  /* allocate stuff */
  subs_pt    = mxMalloc( sizeof(int)      * ndims ) ;
  nsubs_pt   = mxMalloc( sizeof(int)      * ndims ) ;
  strides_pt = mxMalloc( sizeof(idx_t)    * ndims ) ;
  visited_pt = mxMalloc( sizeof(idx_t)    * nel   ) ;
  regions_pt = mxMalloc( sizeof(region_t) * nel   ) ;
  pairs_pt   = mxMalloc( sizeof(pair_t)   * nel   ) ;
  forest_pt  = mxMalloc( sizeof(node_t)   * nel   ) ;
  joins_pt   = mxMalloc( sizeof(idx_t)    * nel   ) ;
  
  /* compute strides to move into the N-dimensional image array */
  strides_pt [0] = 1 ;
  for(k = 1 ; k < ndims ; ++k) {
    strides_pt [k] = strides_pt [k-1] * dims [k-1] ;
  }

  /* sort pixels by increasing intensity*/
  verbose && mexPrintf("Sorting pixels ... ") ;

#ifndef USE_BUCKETSORT
  for(i = 0 ; i < nel ; ++i) {
    pairs_pt [i].value = I_pt [i] ;
    pairs_pt [i].index = i ;
  }
  qsort(pairs_pt, nel, sizeof(pair_t), cmp_pair) ;
#else
  {
    int unsigned buckets [256] ;
    memset(buckets, 0, sizeof(int unsigned)*256) ;
    for(i = 0 ; i < nel ; ++i) {
      val_t v = I_pt [i] ;
      ++ buckets[v] ;
    }
    for(i = 1 ; i < 256 ; ++i) {
      buckets[i] += buckets[i-1] ;
    }
    for(i = nel ; i >= 1 ; ) {
      val_t v = I_pt [--i] ;
      idx_t j = -- buckets [v] ;
      pairs_pt [j].value = v ;
      pairs_pt [j].index = i ;
    }
  }
#endif
  verbose && mexPrintf("done\n") ;

  /* initialize the forest with all void nodes */
  for(i = 0 ; i < nel ; ++i) {
    forest_pt [i].parent = node_is_void  ;
  }
  
  /* number of ellipse free parameters */
  gdl = ndims*(ndims+1)/2 + ndims ;

  /* -----------------------------------------------------------------
   *                                     Compute extremal regions tree
   * -------------------------------------------------------------- */
  verbose && mexPrintf("Computing extremal regions ... ") ;
  for(i = 0 ; i < nel ; ++i) {
    
    /* pop next node xi */
    idx_t index = pairs_pt [i].index ;    
    val_t value = pairs_pt [i].value ;

    /* this will be needed later */
    rindex = index ;
    
    /* push it into the tree */
    forest_pt [index] .parent   = index ;
    forest_pt [index] .shortcut = index ;
    forest_pt [index] .area     = 1 ;
#ifdef USE_RANK_UNION
    forest_pt [index] .height   = 1 ;
#endif
    
    /* convert index into a subscript sub; also initialize nsubs 
       to (-1,-1,...,-1) */
    {
      idx_t temp = index ;
      for(k = ndims-1 ; k >=0 ; --k) {
        nsubs_pt [k] = -1 ;
        subs_pt  [k] = temp / strides_pt [k] ;
        temp         = temp % strides_pt [k] ;
      }
    }
    
    /* process neighbors of xi */
    while( true ) {
      int good = true ;
      idx_t nindex = 0 ;
      
      /* compute NSUBS+SUB, the correspoinding neighbor index NINDEX
         and check that the pixel is within image boundaries. */
      for(k = 0 ; k < ndims && good ; ++k) {
        int temp = nsubs_pt [k] + subs_pt [k] ;
        good &= 0 <= temp && temp < dims[k] ;
        nindex += temp * strides_pt [k] ;
      }      
      
      /* keep going only if
         1 - the neighbor is within image boundaries;
         2 - the neighbor is indeed different from the current node
             (this happens when nsub=(0,0,...,0));
         3 - the nieghbor is already in the tree, meaning that
             is a pixel older than xi.
      */
      if(good && 
         nindex != index &&
         forest_pt[nindex].parent != node_is_void ) {
        
        idx_t nrindex = 0, nvisited ;
        val_t nrvalue = 0 ;

#ifdef USE_RANK_UNION
        int height  = forest_pt [ rindex] .height ;
        int nheight = forest_pt [nrindex] .height ;
#endif
        
        /* RINDEX = ROOT(INDEX) might change as we merge trees, so we
           need to update it after each merge */
        
        /* find the root of the current node */
        /* also update the shortcuts */
        nvisited = 0 ;
        while( forest_pt[rindex].shortcut != rindex ) {          
          visited_pt[ nvisited++ ] = rindex ;
          rindex = forest_pt[rindex].shortcut ;
        }      
        while( nvisited-- ) {
          forest_pt [ visited_pt[nvisited] ] .shortcut = rindex ;
        }
        
        /* find the root of the neighbor */
        nrindex  = nindex ;
        nvisited = 0 ;
        while( forest_pt[nrindex].shortcut != nrindex ) {          
          visited_pt[ nvisited++ ] = nrindex ;
          nrindex = forest_pt[nrindex].shortcut ;
        }      
        while( nvisited-- ) {
          forest_pt [ visited_pt[nvisited] ] .shortcut = nrindex ;
        }
        
        /*
          Now we join the two subtrees rooted at
          
            RINDEX = ROOT(INDEX) and NRINDEX = ROOT(NINDEX).
          
          Only three things can happen:
          
          a - ROOT(INDEX) == ROOT(NRINDEX). In this case the two trees
              have already been joined and we do not do anything.
          
          b - I(ROOT(INDEX)) == I(ROOT(NRINDEX)). In this case index
              is extending an extremal region with the same
              value. Since ROOT(NRINDEX) will NOT be an extremal
              region of the full image, ROOT(INDEX) can be safely
              addedd as children of ROOT(NRINDEX) if this reduces
              the height according to union rank.
               
          c - I(ROOT(INDEX)) > I(ROOT(NRINDEX)) as index is extending
              an extremal region, but increasing its level. In this
              case ROOT(NRINDEX) WILL be an extremal region of the
              final image and the only possibility is to add
              ROOT(NRINDEX) as children of ROOT(INDEX).
        */

        if( rindex != nrindex ) {
          /* this is a genuine join */
          
          nrvalue = I_pt [nrindex] ;
          if( nrvalue == value 
#ifdef USE_RANK_UNION             
              && height < nheight
#endif
              ) {
            /* ROOT(INDEX) becomes the child */
            forest_pt[rindex] .parent   = nrindex ;
            forest_pt[rindex] .shortcut = nrindex ;          
            forest_pt[nrindex].area    += forest_pt[rindex].area ;

#ifdef USE_RANK_UNION
            forest_pt[nrindex].height = MAX(nheight, height+1) ;
#endif

            joins_pt[njoins++] = rindex ;

          } else {
            /* ROOT(index) becomes parent */
            forest_pt[nrindex] .parent   = rindex ;
            forest_pt[nrindex] .shortcut = rindex ;
            forest_pt[rindex]  .area    += forest_pt[nrindex].area ;

#ifdef USE_RANK_UNION
            forest_pt[rindex].height = MAX(height, nheight+1) ;
#endif
            if( nrvalue != value ) {            
              /* nrindex is extremal region: save for later */
              forest_pt[nrindex].region = ner ;
              regions_pt [ner] .index      = nrindex ;
              regions_pt [ner] .parent     = ner ;
              regions_pt [ner] .value      = nrvalue ;
              regions_pt [ner] .area       = forest_pt [nrindex].area ;
              regions_pt [ner] .area_top   = nel ;
              regions_pt [ner] .area_bot   = 0 ;            
              ++ner ;
            }
            
            /* annote join operation for post-processing */
            joins_pt[njoins++] = nrindex ;
          }
        }
        
      } /* neighbor done */
      
      /* move to next neighbor */      
      k = 0 ;
      while(++ nsubs_pt [k] > 1) {
        nsubs_pt [k++] = -1 ;
        if(k == ndims) goto done_all_neighbors ;
      }
    } /* next neighbor */
  done_all_neighbors : ;
  } /* next pixel */    
    
  /* the root of the last processed pixel must be a region */
  forest_pt [rindex].region = ner ;
  regions_pt [ner] .index      = rindex ;
  regions_pt [ner] .parent     = ner ;
  regions_pt [ner] .value      = I_pt      [rindex] ;
  regions_pt [ner] .area       = forest_pt [rindex] .area ;
  regions_pt [ner] .area_top   = nel ;
  regions_pt [ner] .area_bot   = 0 ;            
  ++ner ;

  verbose && mexPrintf("done\nExtremal regions: %d\n", ner) ;
  
  /* -----------------------------------------------------------------
   *                                            Compute region parents
   * -------------------------------------------------------------- */
  for( i = 0 ; i < ner ; ++i) {
    idx_t index  = regions_pt [i].index ;    
    val_t value  = regions_pt [i].value ;
    idx_t j      = i ;

    while(j == i) {
      idx_t pindex = forest_pt [index].parent ;
      val_t pvalue = I_pt [pindex] ;

      /* top of the tree */
      if(index == pindex) {
        j = forest_pt[index].region ;
        break ;
      }

      /* if index is the root of a region, either this is still
         i, or it is the parent region we are looking for. */
      if(value < pvalue) {
        j = forest_pt[index].region ;
      }

      index = pindex ;
      value = pvalue ;
    }
    regions_pt[i]. parent = j ;
  }
  
  /* -----------------------------------------------------------------
   *                                 Compute areas of tops and bottoms
   * -------------------------------------------------------------- */

  /* We scan the list of regions from the bottom. Let x0 be the current
     region and be x1 = PARENT(x0), x2 = PARENT(x1) and so on.
     
     Here we do two things:
     
     1) Look for regions x for which x0 is the BOTTOM. This requires
        VAL(x0) <= VAL(x) - DELTA < VAL(x1).
        We update AREA_BOT(x) for each of such x found.
        
     2) Look for the region y which is the TOP of x0. This requires
          VAL(y) <= VAL(x0) + DELTA < VAL(y+1)
          We update AREA_TOP(x0) as soon as we find such y.
        
  */

  for( i = 0 ; i < ner ; ++i) {
    /* fix xi as the region, then xj are the parents */
    idx_t parent = regions_pt [i].parent ;
    int   val0   = regions_pt [i].value ;
    int   val1   = regions_pt [parent].value ;
    int   val    = val0 ;
    idx_t j = i ;

    while(true) {
      int valp = regions_pt [parent].value ;

      /* i is the bottom of j */
      if(val0 <= val - delta && val - delta < val1) {
        regions_pt [j].area_bot  =
          MAX(regions_pt [j].area_bot, regions_pt [i].area) ;
      }
      
      /* j is the top of i */
      if(val <= val0 + delta && val0 + delta < valp) {
        regions_pt [i].area_top = regions_pt [j].area ;
      }
      
      /* stop if going on is useless */
      if(val1 <= val - delta && val0 + delta < val)
        break ;

      /* stop also if j is the root */
      if(j == parent)
        break ;
      
      /* next region upward */
      j      = parent ;
      parent = regions_pt [j].parent ;
      val    = valp ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                 Compute variation
   * -------------------------------------------------------------- */
  for(i = 0 ; i < ner ; ++i) {
    int area     = regions_pt [i].area ;
    int area_top = regions_pt [i].area_top ;
    int area_bot = regions_pt [i].area_bot ;    
    regions_pt [i].variation = 
      (float)(area_top - area_bot) / (float)area ;

    /* initialize .mastable to 1 for all nodes */
    regions_pt [i].maxstable = 1 ;
  }

  /* -----------------------------------------------------------------
   *                     Remove regions which are NOT maximally stable
   * -------------------------------------------------------------- */
  nmer = ner ;
  for(i = 0 ; i < ner ; ++i) {
    idx_t parent = regions_pt [i]      .parent ;
    float var    = regions_pt [i]      .variation ;
    float pvar   = regions_pt [parent] .variation ;
    idx_t loser ;

    /* decide which one to keep and put that in loser */
    if(var < pvar) loser = parent ; else loser = i ;
    
    /* make loser NON maximally stable */
    if(regions_pt [loser].maxstable) --nmer ;
    regions_pt [loser].maxstable = 0 ;
  }

  verbose && mexPrintf("Maximally stable regions: %d (%.1f%%)\n", 
                       nmer, 100.0 * (double) nmer / ner) ;

  /* -----------------------------------------------------------------
   *                                               Remove more regions
   * -------------------------------------------------------------- */

  /* it is critical for correct duplicate detection to remove regions
     from the bottom (smallest one first) */

  if( big_cleanup || small_cleanup || bad_cleanup || dup_cleanup ) {
    int nbig   = 0 ;
    int nsmall = 0 ;
    int nbad   = 0 ;
    int ndup   = 0 ;

    /* scann all extremal regions */
    for(i = 0 ; i < ner ; ++i) {
      
      /* process only maximally stable extremal regions */
      if(! regions_pt [i].maxstable) continue ;
      
      if( bad_cleanup   && regions_pt[i].variation >= 1.0f  ) {
        ++nbad ;
        goto remove_this_region ;
      }

      if( big_cleanup   && regions_pt[i].area      >  nel/2 ) {
        ++nbig ;
        goto remove_this_region ;
      }

      if( small_cleanup && regions_pt[i].area      <  25    ) {
        ++nsmall ;
        goto remove_this_region ;
      }
      
      /* 
       * Remove duplicates 
       */
      if( dup_cleanup ) {
        idx_t parent = regions_pt [i].parent ;
        int area, parea ;
        float change ;
        
        /* the search does not apply to root regions */
        if(parent != i) {
          
          /* search for the maximally stable parent region */
          while(! regions_pt[parent].maxstable) {
            idx_t next = regions_pt[parent].parent ;
            if(next == parent) break ;
            parent = next ;
          }
          
          /* compare with the parent region; if the current and parent
             regions are too similar, keep only the parent */
          area   = regions_pt [i].area ;
          parea  = regions_pt [parent].area ;
          change = (float)(parea - area)/area ;

          if(change < 0.5)  {
            ++ndup ;
            goto remove_this_region ;
          }

        } /* drop duplicates */ 
      }
      continue ;
    remove_this_region :
      regions_pt[i].maxstable = false ;
      --nmer ;      
    } /* next region to cleanup */

    if(verbose) {
      mexPrintf("  Bad regions:        %d\n", nbad   ) ;
      mexPrintf("  Small regions:      %d\n", nsmall ) ;
      mexPrintf("  Big regions:        %d\n", nbig   ) ;
      mexPrintf("  Duplicated regions: %d\n", ndup   ) ;
    }
  }

  verbose && mexPrintf("Cleaned-up regions: %d (%.1f%%)\n", 
                       nmer, 100.0 * (double) nmer / ner) ;

  /* -----------------------------------------------------------------
   *                                                      Fit ellipses
   * -------------------------------------------------------------- */
  ell_pt = 0 ;
  if (nout >= 1) {
    int midx = 1 ;
    int d, index, j ;
    
    verbose && mexPrintf("Fitting ellipses...\n") ;

    /* enumerate maxstable regions */
    for(i = 0 ; i < ner ; ++i) {      
      if(! regions_pt [i].maxstable) continue ;
      regions_pt [i].maxstable = midx++ ;
    }
    
    /* allocate space */
    acc_pt = mxMalloc(sizeof(acc_t) * nel) ;
    ell_pt = mxMalloc(sizeof(acc_t) * gdl * nmer) ; 
    
    /* clear accumulators */
    memset(ell_pt, 0, sizeof(int) * gdl * nmer) ;
    
    /* for each gdl */
    for(d = 0 ; d < gdl ; ++d) {
      /* initalize parameter */
      memset(subs_pt, 0, sizeof(int) * ndims) ;
      
      if(d < ndims) {
        verbose && mexPrintf(" mean %d\n",d) ;
        for(index = 0 ; index < nel ; ++ index) {
          acc_pt[index] = subs_pt[d] ;
          adv(dims, ndims, subs_pt) ;
        }

      } else {

        /* decode d-ndims into a (i,j) pair */
        i = d-ndims ; 
        j = 0 ;
        while(i > j) {
          i -= j + 1 ;
          j ++ ;
        }

        verbose && mexPrintf(" corr (%d,%d)\n",i,j) ;

        /* add x_i * x_j */
        for(index = 0 ; index < nel ; ++ index){
          acc_pt[index] = subs_pt[i]*subs_pt[j] ;
          adv(dims, ndims, subs_pt) ;
        }
      }

      /* integrate parameter */
      for(i = 0 ; i < njoins ; ++i) {      
        idx_t index  = joins_pt[i] ;
        idx_t parent = forest_pt [ index ].parent ;
        acc_pt[parent] += acc_pt[index] ;
      }
    
      /* save back to ellpises */
      for(i = 0 ; i < ner ; ++i) {
        idx_t region = regions_pt [i].maxstable ;

        /* skip if not extremal region */
        if(region-- == 0) continue ;
        ell_pt [d + gdl*region] = acc_pt [ regions_pt[i].index ] ;
      }

      /* next gdl */
    }
    mxFree(acc_pt) ;
  }
  
  /* -----------------------------------------------------------------
   *                                                Save back and exit
   * -------------------------------------------------------------- */

  /*
   * Save extremal regions
   */
  {
    int dims[2] ;
    int unsigned * pt ;
    dims[0] = nmer ;
    out[OUT_REGIONS] = mxCreateNumericArray(1,dims,mxUINT32_CLASS,mxREAL);
    pt = mxGetData(out[OUT_REGIONS]) ;
    for (i = 0 ; i < ner ; ++i) {
      if( regions_pt[i].maxstable ) {
        /* adjust for MATLAB index compatibility */
        *pt++ = regions_pt[i].index + 1 ;
      }
    }
  }
  
  /*
   * Save fitted ellipses
   */
  if(nout >= 2) {
    int dims[2], d, j, index ;
    double * pt ;
    dims[0] = gdl ;
    dims[1] = nmer ;
     
    out[OUT_ELL] = mxCreateNumericArray(2,dims,mxDOUBLE_CLASS,mxREAL) ;
    pt = mxGetData(out[OUT_ELL]) ;

    for(index = 0 ; index < nel ; ++index) {

      idx_t region = regions_pt [index] .maxstable ;      
      int   N      = regions_pt [index] .area ;

      if(region-- == 0) continue ;

      for(d = 0  ; d  < gdl ; ++d) {

        pt[d] = (double) ell_pt[gdl*region  + d] / N ;

        if(d < ndims) {
          /* adjust for MATLAB coordinate frame convention */
          pt[d] += 1 ; 
        } else {
          /* remove squared mean from moment to get variance */
          i = d - ndims ; 
          j = 0 ;
          while(i > j) {
            i -= j + 1 ;
            j ++ ;
          }
          pt[d] -= (pt[i]-1)*(pt[j]-1) ;
        }
      }
      pt += gdl ;
    }
    mxFree(ell_pt) ;
  }
  
  if(nout >= 3)  {
    int unsigned * pt ;
    out[OUT_PARENTS] = mxCreateNumericArray(ndims,dims,mxUINT32_CLASS,mxREAL) ;
    pt = mxGetData(out[OUT_PARENTS]) ;
    for(i = 0 ; i < nel ; ++i) {
      *pt++ = forest_pt[i].parent ;
    }
  }

  if(nout >= 4) {
    int dims[2] ;
    int unsigned * pt ;
    dims[0] = 3 ;
    dims[1]= ner ;
    out[OUT_AREA] = mxCreateNumericArray(2,dims,mxUINT32_CLASS,mxREAL);
    pt = mxGetData(out[OUT_AREA]) ;
    for( i = 0 ; i < ner ; ++i ) {
      *pt++ = regions_pt [i]. area_bot ;
      *pt++ = regions_pt [i]. area ;
      *pt++ = regions_pt [i]. area_top ;      
    }
  }

  /* free stuff */
  mxFree( forest_pt  ) ;
  mxFree( pairs_pt   ) ;
  mxFree( regions_pt ) ;
  mxFree( visited_pt ) ;
  mxFree( strides_pt ) ;
  mxFree( nsubs_pt   ) ;
  mxFree( subs_pt    ) ;
}
