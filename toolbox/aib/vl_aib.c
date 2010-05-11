/** @internal
 ** @file     aib.c
 ** @author   Brian Fulkerson
 ** @author   Andrea Vedaldi
 ** @brief    AIB MEX driver
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <mexutils.h>
#include <vl/mathop.h>
#include <vl/aib.h>

#include <assert.h>
#include <string.h>

/* option codes */
enum {
  opt_cluster_null = 0,
  opt_verbose
} ;

/* options */
vlmxOption  options [] = {
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
 **   single cluster (as they, after all, yield zero information drop).
 **
 ** - we attach this cluster to the root as the very last step (as we
 **   do not want to change other nodes.
 **
 ** In this way,
 **/

static void
cluster_null_nodes (vl_uint32* parents, vl_uint32 nvalues, double *cost)
{
  vl_uint32 nnull = 0 ;
  vl_uint32 n ;
  vl_uint32 first ;
  vl_uint32 last_intermed ;

  vl_uint32 a, b, c, d, e ;
  vl_uint32 dp, ep ;

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

  mexPrintf("a:%u b:%u c:%u d:%u e:%u dp:%u ep:%u\n",
            a,b,c,d,e,dp,ep) ;

  /* search first leaf that has been merged */
  {
    vl_uint32 first_parent = e ;
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

  mexPrintf("nnull:%u\n",nnull) ;
  mexPrintf("nvalues:%u\n",nvalues) ;
  mexPrintf("first:%u\n",first) ;

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

  mexPrintf("first null %u parent seto to last_intermed:%u\n",
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

  mexPrintf("after chaining other nulls last_intermed:%u\n", last_intermed)  ;

  /* make last_intermed point to d */
  parents [last_intermed] = d ;

  /* change parent of first to be last_intermed */
  mexPrintf("parent of %u (first) was %u\n", first, parents[first]) ;
  parents [first] = last_intermed ;
  mexPrintf("parent of %u (first) is now %u\n", first, parents[first]) ;

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
  int            cluster_null = 0 ;

  double   *Pcx     ;
  vl_uint32    nlabels ;
  vl_uint32    nvalues ;

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

  if (!vlmxIsMatrix(in[IN_PCX], -1, -1)) {
    mexErrMsgTxt("PCX must be a real matrix.") ;
  }

  Pcx_cpy = mxDuplicateArray(in[IN_PCX]);
  Pcx     = mxGetPr (Pcx_cpy) ;
  nlabels = mxGetM  (in[IN_PCX]) ;
  nvalues = mxGetN  (in[IN_PCX]) ;

  while ((opt = vlmxNextOption (in, nin, options, &next, &optarg)) >= 0) {

    switch (opt) {

    case opt_verbose :
      ++ verbose ;
      break ;

    case opt_cluster_null :
      cluster_null = 1 ;
      break ;

    }
  }

  if (verbose) {
    mexPrintf("aib: cluster null:    %d", cluster_null) ;
  }

  /* -----------------------------------------------------------------
   *                                                            Do job
   * -------------------------------------------------------------- */

  {
    VlAIB   *aib;
    double* acost = 0, *cost = 0 ;
    vl_uint32 *aparents = 0, *parents = 0 ;
    vl_uint32 n ;

    out[OUT_PARENTS] = mxCreateNumericMatrix(1, 2*nvalues - 1, mxUINT32_CLASS, mxREAL);
    parents = mxGetData(out[OUT_PARENTS]);

    if (nout > 1) {
      out[OUT_COST] = mxCreateNumericMatrix(1, nvalues, mxDOUBLE_CLASS, mxREAL);
      cost = mxGetPr(out[OUT_COST]);
    }

    aib = vl_aib_new (Pcx, nvalues, nlabels) ;
    vl_aib_process (aib);

    aparents = vl_aib_get_parents (aib);
    acost    = vl_aib_get_costs (aib);
    memcpy(parents, aparents, sizeof(vl_uint32)*(2*nvalues-1));
    if (nout > 1)
      memcpy(cost, acost, sizeof(double)*nvalues);

    vl_aib_delete(aib);

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

  }
  mxDestroyArray(Pcx_cpy);
}
