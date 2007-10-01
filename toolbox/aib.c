/** @internal
 ** @file     aib.c
 ** @author   Brian Fulkerson
 ** @brief    AIB MEX driver
 **/

/* AUTORIGHTS */

#include "mexutils.h"
#include <vl/mathop.h>
#include <vl/aib.h>

#include <assert.h>

/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_PCX = 0} ;
  enum {OUT_PARENTS = 0} ;

  vl_aib_prob    *Pcx     = mxGetPr (in[IN_PCX]) ;
  vl_aib_node     nlabels = mxGetM  (in[IN_PCX]) ;
  vl_aib_node     nvalues = mxGetN  (in[IN_PCX]) ;

  VL_USE_MATLAB_ENV

  if (!uIsRealMatrix(in[IN_PCX], -1, -1)) {
    mexErrMsgTxt("PCX must be a real matrix.") ;
  }

  { 
    vl_aib_node *parents = vl_aib(Pcx, nlabels, nvalues);
    vl_aib_node *mexparents ;
    int dims [2], n ;

    dims[0] = 2 * nvalues - 1 ;
    dims[1] = 1 ;
    out[OUT_PARENTS] = mxCreateNumericArray (2, dims, mxUINT32_CLASS, mxREAL) ;
    mexparents = (vl_aib_node *) mxGetData(out[OUT_PARENTS]) ;
    
    for (n = 0 ; n < 2 * nvalues - 1 ; n++) {
      if (parents[n] > 2* nvalues - 1) {
        mexparents[n] = 0 ;
      } else {
        /* parents + 1 for matlab indexing */    
        mexparents[n] = parents[n] + 1 ; 
      }
    }
  }
}
