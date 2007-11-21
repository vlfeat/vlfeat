/** @internal
 ** @file     aib.c
 ** @author   Brian Fulkerson
 ** @author   Andrea Vedaldi
 ** @brief    AIB MEX driver
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "mexutils.h"
#include <vl/mathop.h>
#include <vl/aib.h>

#include <assert.h>
#include <string.h>

/* option codes */
enum {
  opt_cost = 0,
  opt_cluster_null,
  opt_verbose 
} ;

/* options */
uMexOption options [] = {
  {"Cost",       1,   opt_cost       },
  {"ClusterNull",0,   opt_cluster_null}, 
  {"Verbose",    0,   opt_verbose    },
  {0,            0,   0              }
} ;

#define CLUSTER_NULL_NODES 1

/** ------------------------------------------------------------------
 **
 ** Null nodes are nodes with null probability and are not merged by AIB.
 ** It is convenient, however, to treat them as follows:
 ** 
 ** - we pretend that AIB merged those nodes at the very beginning into a
 **   single cluster (as they, after hall, yield zero information drop).
 **
 ** - we attach this cluster to the root as the very last step (as we
 **   do not want to change other nodes.
 **
 ** In this way, 
 **/

static void
cluster_null_nodes (vl_uint32* parents, int nvalues, double *cost)
{
  int nnull = 0 ;
  int count = 0 ;
  int free_slot = nvalues ;
  int n ;

  cost = 0 ;

  /* count null nodes so far */
  for (n = 0 ; n < nvalues ; ++ n) {
    if (parents[n] >= 2 * nvalues - 1) {
      ++ nnull ;
    }
  }

  /* Move all internal nodes nnull places to the right. To do this, we
   * need to update all the parents pointer of the nodes (except the
   * root) and to physically move the internal nodes to the right (the root will
   * to, so that it will be moved to 2 * nvalues -1 -1).
   */
     
  for (n = 0 ; n < 2 * nvalues - 1 - nnull  -1 ; ++ n) {
    if (parents[n] < 2 * nvalues - 1) parents[n] += nnull ;
  }
  
  for (n = 2 * nvalues - 1 - 1 ; n > nvalues - 1 + nnull ; --n) {
    parents [n] = parents [n - nnull] ;
    if (cost) {
      cost [n - nvalues] = cost [n - nvalues - nnull] ;
    }
  }

  if (cost) {
    for (n = nvalues ; n < nvalues + nnull ; ++ n){
      cost [n] = cost [nvalues - nvalues + nnull] ;
    }    
  }
  
  /* Now chain the null nodes */
  for (n = 0 ; n < nvalues ; ++ n) {
    if (parents[n] >= 2 * nvalues - 1) {
      ++ count ;
      if (count < nnull) {
        parents [n]         = free_slot ;
        parents [free_slot] = free_slot + 1 ;
        ++ free_slot ;
      } else {
        parents [n]         = free_slot ;
        parents [free_slot] = 2 * nvalues - 1 - 1 ;
      }
    }
  }
}


/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_PCX = 0, IN_END} ;
  enum {OUT_PARENTS = 0, OUT_COST} ;
  enum {INFORMATION, EC} ;

  int            verbose = 0 ;
  int            opt ;
  int            next = IN_END ;
  mxArray const *optarg ;
  int            cost_type = INFORMATION ;
  int            cluster_null = 0 ;

  vl_aib_prob   *Pcx     ;
  vl_aib_node    nlabels ;
  vl_aib_node    nvalues ;

  mxArray *Pcx_cpy ;

  VL_USE_MATLAB_ENV ; 

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */
  
  if (nin < 1) {
    mexErrMsgTxt("One argument required.") ;
  } else if (nout > 2) {
    mexErrMsgTxt("Too many output arguments.");
  }
       
  if (!uIsRealMatrix(in[IN_PCX], -1, -1)) {
    mexErrMsgTxt("PCX must be a real matrix.") ;
  }

  Pcx_cpy = mxDuplicateArray(in[IN_PCX]);
  Pcx     = mxGetPr (Pcx_cpy) ;
  nlabels = mxGetM  (in[IN_PCX]) ;
  nvalues = mxGetN  (in[IN_PCX]) ;
  
  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;
    
    switch (opt) {

    case opt_verbose :
      ++ verbose ;
      break ;

    case opt_cluster_null :
      cluster_null = 1 ;
      break ;
      
    case opt_cost :
      if (!uIsString (optarg, -1)) {
        mexErrMsgTxt("'Cost' must be a string.") ;        
      }
      if (mxGetString (optarg, buf, sizeof(buf))) {
        mexErrMsgTxt("Option argument too long.") ;
      }
      if (strcmp("i", buf) == 0) {
        cost_type = INFORMATION ;
      } else if (strcmp("ec", buf) == 0) {
        cost_type = EC ;
      } else {
        mexErrMsgTxt("Unknown cost type.") ;
      }
    }
  }

  if (verbose) {
    char const * cost_name = 0 ;
    switch (cost_type) {
    case INFORMATION: cost_name = "information" ; break ;
    case EC:          cost_name = "entropy-constrained information" ; break ;
    }
    mexPrintf("aib: signal null:    %d", cluster_null) ;
    mexPrintf("aib: cost minimized: %s", cost_name) ;    
  }
  
  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */
  
  { 
    double* cost ;
    vl_aib_node *parents = 0 ;
    int n ;
    
    switch (cost_type) {
    case INFORMATION :
      parents = vl_aib (Pcx, nlabels, nvalues, (nout == 0) ? 0 : &cost) ;
      break ;
    case EC :
      mexErrMsgTxt("Not implemented") ;
      break ;
    default:
      assert (0) ;
    }
    
    if (cluster_null) {
      cluster_null_nodes (parents, nvalues, (nout == 0) ? 0 : cost) ;
    }

    /* save back parents */
    for (n = 0 ; n < 2 * nvalues - 1 ; ++n) {
      if (parents [n] > 2 * nvalues - 1) {
        /* map ingored nodes to zero */
        parents [n] = 0 ;
      } else {
        /* MATLAB starts counting from 1 */
        ++ parents [n]  ;
      }
    }
    
    out[OUT_PARENTS] = uCreateNumericMatrix 
      (1, 2 * nvalues - 1, mxUINT32_CLASS, parents) ;
    
    if (nout > 0) {
      out[OUT_COST] = uCreateNumericMatrix 
        (1, nvalues, mxDOUBLE_CLASS, cost) ;
    }
  }
  mxDestroyArray(Pcx_cpy);
}
