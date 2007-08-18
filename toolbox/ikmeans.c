/* file:        ikmeans.c
** description: MEX weighted ikmeans function.
** author:      Andrea Vedaldi
**/

#include"mexutils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/ikmeans.h>
#include <vl/generic.h>

/* driver */
void mexFunction (int nout, mxArray * out[], int nin, const mxArray * in[])
{
    idx_t *asgn_pt;
    acc_t *centers_pt;
    data_t *data_pt;

    /*  mxClassID data_class = mxINT8_CLASS ; */
    enum
    { IN_DATA = 0, IN_K };
    enum
    { OUT_CENTERS = 0, OUT_ASGN };
    int M, N;
    int K = 0;
    int verbose = 1;
    idx_t npasses = 200;

  /** -----------------------------------------------------------------
   **                                               Check the arguments
   ** -------------------------------------------------------------- */
    if (nin != 2)
    {
        mexErrMsgTxt ("Two arguments required.");
    }
    else if (nout > 2)
    {
        mexErrMsgTxt ("Too many output arguments.");
    }

    if (mxGetClassID (in[IN_DATA]) != mxUINT8_CLASS)
    {
        mexErrMsgTxt ("DATA must be of class uint8");
    }

    if (uIsRealScalar (in[IN_K]))
    {
        K = (int) *mxGetPr (in[IN_K]);
        if (K > 0)
            goto K_is_ok;
    }
    mexErrMsgTxt ("K must be a positive integer.");
  K_is_ok:

    M = mxGetM (in[IN_DATA]);   /* n of components */
    N = mxGetN (in[IN_DATA]);   /* n of elements */

    if (N < K)
    {
        mexErrMsgTxt ("Numeber of clusters cannot be less than data.");
    }

    data_pt = (data_t *) mxGetPr (in[IN_DATA]);
    out[OUT_CENTERS] = mxCreateNumericMatrix (M, K, mxINT32_CLASS, mxREAL);
    out[OUT_ASGN] = mxCreateNumericMatrix (1, N, mxUINT32_CLASS, mxREAL);

    asgn_pt = (idx_t *) mxGetPr (out[OUT_ASGN]);

    /* K is the number of clusters */
    /* M is the dimension of each datapoint */
    /* N is the number of data points */
    centers_pt = vl_ikmeans(data_pt, M, N, K, asgn_pt);

    /* copy the centers to matlab */
    memcpy(mxGetPr(out[OUT_CENTERS]), centers_pt, M*K*sizeof(vl_int32));
    free(centers_pt);
    
    /* Matlab has 1 based indexing */
    {
      int j ;
      for (j = 0 ; j < N ; ++j)
        ++ asgn_pt [j] ;
    }
}
