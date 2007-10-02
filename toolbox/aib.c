/** @internal
 ** @file     aib.c
 ** @author   Brian Fulkerson
 ** @author   Andrea Vedaldi
 ** @brief    AIB MEX driver
 **/

/* AUTORIGHTS */

#include "mexutils.h"
#include <vl/mathop.h>
#include <vl/aib.h>

#include <assert.h>
#include <string.h>

/* option codes */
enum {
  opt_cost = 0,
  opt_verbose 
} ;

/* options */
uMexOption options [] = {
  {"Cost",      1,   opt_cost },
  {"Verbose",         0,   opt_verbose    },
  {0,                 0,   0              }
} ;


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

  vl_aib_prob   *Pcx     ;
  vl_aib_node    nlabels ;
  vl_aib_node    nvalues ;

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

  Pcx     = mxGetPr (in[IN_PCX]) ;
  nlabels = mxGetM  (in[IN_PCX]) ;
  nvalues = mxGetN  (in[IN_PCX]) ;


  while ((opt = uNextOption(in, nin, options, &next, &optarg)) >= 0) {
    char buf [1024] ;
    
    switch (opt) {

    case opt_verbose :
      ++ verbose ;
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
        (1, 2 * nvalues - 1, mxDOUBLE_CLASS, cost) ;
    }
  }
}
