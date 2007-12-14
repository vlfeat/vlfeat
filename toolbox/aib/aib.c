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

#include <mexutils.h>
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
  int n ;
  int first ;
  int last_intermed ;

  int a, b, c, d, e ;
  int dp, ep ;

  /* count null nodes so far */
  for (n = 0 ; n < nvalues ; ++ n) {
    if (parents[n] >= 2 * nvalues - 1) {
      ++ nnull ;
    }
  }
  
  if (nnull == 0) return ;
  
  /* = : leaves
   * 0 : null leaves
   * i : internal node
   * * : padding
   * x : intermediate nodes to link null nodes
   *
   * Input:
   *
   * ====== 0000 ======= iiiiiii ****
   * |                   |     |
   * 0                   dp    ep
   *
   *
   * Output:
   *
   * ====== ==== ======= xxxx iiiiiii
   * |                   | || |     |
   * 0                   a bc d     e
   */
  
  a = nvalues ;
  b = nvalues + nnull - 1 - 1 ;
  c = b + 1 ;
  d = c + 1 ;
  e = 2 * nvalues - 2 ;

  dp = nvalues ;
  ep = 2 * nvalues - 2 - nnull ; 

  mexPrintf("a:%d b:%d c:%d d:%d e:%d dp:%d ep:%d\n",
            a,b,c,d,e,dp,ep) ;

  /* search first leaf that has been merged */
  {
    int first_parent = e ;
    first = 0 ;
    for (n = 0 ; n < nvalues ; ++ n) {
      if ((parents[n] <= e) & (parents[n] != 1)) {
        if (first_parent >= parents [n]) {
          first_parent = parents [n] ;
          first = n ;
        }
      }
    }
  }

  mexPrintf("nnull:%d\n",nnull) ;
  mexPrintf("nvalues:%d\n",nvalues) ;
  mexPrintf("first:%d\n",first) ;
    
  /* move internal node block [dp:ep] to [d:e] */
  for (n = 0 ; n < e ; ++ n) {
    if ((parents [n] <= e) & (parents [n] != 0)) {
      parents [n] += (e - ep) ;
    }
  }
  for (n = e ; n >= d ; -- n) {
    parents [n] = parents [n - (e - ep)] ;
  }
 
  /* find first null node and connect it to a */
  last_intermed = a ;
  for (n = 0 ; n < a ; ++ n) {
    if (parents[n] > e) {
      parents [n] = last_intermed ;
      break ;
    }
  }
  
  mexPrintf("first null %d parent seto to last_intermed:%d\n", 
            n,
            last_intermed)  ;

  /* chain rest of intermediate nodes */
  for (; n < a ; ++ n) {
    if (parents[n] > e) {
      parents [n] = last_intermed ;
      parents [last_intermed] = last_intermed + 1 ;
      ++ last_intermed ;
    }
  }

  mexPrintf("after chaining other nulls last_intermed:%d\n", last_intermed)  ;

  /* make last_intermed point to d */
  parents [last_intermed] = d ;
  
  /* change parent of first to be last_intermed */
  mexPrintf("parent of %d (first) was %d\n", first, parents[first]) ;
  parents [first] = last_intermed ;
  mexPrintf("parent of %d (first) is now %d\n", first, parents[first]) ;

  /* fix cost too (reall that the fist entry is the cost before
   any merge) */
  if (cost) {
    cost -= nvalues - 1 ;
    for (n = e ; n >= d ; --n) {
      cost [n] = cost [n - (e - ep)] ;
    }
    for (n = c ; n >= a ; --n) {
      cost [n] = cost [d] ;
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
