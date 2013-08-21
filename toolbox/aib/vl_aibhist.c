/** @internal
 ** @file    aibhist.c
 ** @brief   Push data down the AIB tree - MEX driver
 ** @author  Andrea Vedaldi
 **/

/*
Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
All rights reserved.

This file is part of the VLFeat library and is made available under
the terms of the BSD license (see the COPYING file).
*/

#include<mexutils.h>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/generic.h>

/* ---------------------------------------------------------------- */
/** @brief MEX driver entry point
 **/

void
mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
  enum {IN_PARENTS = 0, IN_DATA, IN_OPT} ;
  enum {OUT_TREE} ;

  vl_uint32 const *parents ;
  vl_uint32 *tree ;
  double const *data ;

  int nnull = 0 ;
  int histmode = 0 ;

  vl_uint32 i, P, N ;

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if ((nin < 2) || (nin > 3)) {
    mexErrMsgTxt ("Two or three arguments required.") ;
  }

  if (nout > 1) {
    mexErrMsgTxt ("Too many output arguments.") ;
  }

  if (!vlmxIsMatrix(in[IN_DATA], -1, -1)) {
    mexErrMsgTxt ("DATA must be a matrix of DOUBLE");
  }

  if (!vlmxIsVector(in[IN_PARENTS], -1)) {
    mexErrMsgTxt ("PARENTS must be a vector") ;
  }

  if (mxGetClassID(in[IN_PARENTS]) != mxUINT32_CLASS) {
    mexErrMsgTxt ("PARENTS must be UINT32") ;
  }

  N = mxGetNumberOfElements (in[IN_DATA]) ;
  data = mxGetPr (in[IN_DATA]) ;

  P = mxGetNumberOfElements (in[IN_PARENTS]) ;
  parents = mxGetData (in[IN_PARENTS]) ;

  if (nin > 2) {
    enum {buflen = 32} ;
    char buf [buflen] ;
    if (!vlmxIsString(in[IN_OPT], -1)) {
      mexErrMsgTxt("OPT must be a string") ;
    }
    mxGetString(in[IN_OPT], buf, buflen) ;
    buf [buflen - 1] = 0 ;
    if (!vlmxCompareStringsI("hist", buf)) {
      mexErrMsgTxt("OPT must be equal to 'hist'") ;
    }
    histmode = 1 ;
  }

  out[OUT_TREE] = mxCreateNumericMatrix(1, P,mxUINT32_CLASS, mxREAL) ;
  tree = mxGetData (out[OUT_TREE]) ;

  /* -----------------------------------------------------------------
   *                                                        Do the job
   * -------------------------------------------------------------- */

  {
    char buf [1024] ;
    vl_uint32 max_node = 0 ;
    vl_uint32 min_node = 0 ;
    vl_uint32 last_leaf = 0 ;
    /*vl_uint32 root = 0 ;*/

    /* exhamine parents for errors and informations */
    for (i = 0 ; i  < P ; ++i) {
      vl_uint32 node = parents [i] ;

      if ((node != 0) & (node != 1)) {
        max_node = VL_MAX (node, max_node) ;
        min_node = VL_MIN (node, min_node) ;
      }

      /* check no node points outside the tree */
      if (node > P) {
        snprintf(buf, sizeof(buf),
                 "Out of bounds link PARENTS[%d] = %u > %u", i, node, P) ;
        mexErrMsgTxt (buf) ;
      }

      /* check node points to something above him */
      if ((node != 0) & (node != 1) & (node < i)) {
        snprintf(buf, sizeof(buf),
                 "Backward link PARENTS[%d] = %u < %d", i, node, i) ;
        mexErrMsgTxt (buf) ;
      }
      if (node == 0) ++ nnull ;
    }

    /* now
     *
     * min_node = first node which is not a leaf
     * max_node = root node
     * nnull    = number of leaves pointing to the null node
     */

    last_leaf = min_node - 1 ;
    /*root = max_node ;*/

    /* process data */
    for (i = 0 ; i < N ; ++i) {
      /*int w = 1 ;*/
      vl_uint32 x = data [i] ;

      if (histmode) {
        /*w = x ;*/
        x = i ;
      }

      if ((x < 1) | (x > last_leaf)) {
        if (histmode) {
          snprintf(buf, sizeof(buf),
                   "DATA length exceeds number of AIB leaves") ;
        } else {
          snprintf(buf, sizeof(buf),
                   "DATA [%u] = %u is not a leaf", i, x) ;
        }
        mexErrMsgTxt (buf) ;
      }

      while (VL_TRUE) {
        vl_uint32 x_ = parents [x -1] ;
        /*     mexPrintf("%u : x_=%u, x=%u\n", i, x_, x) ; */
        ++ tree [x - 1] ;
        if ((x_ == x) | (x_ == 0) | (x_ == 1)) break ;
        x = x_ ;
      }
    }
  }
}
