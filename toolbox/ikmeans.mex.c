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
    {
        int dims[2];

        dims[0] = M;
        dims[1] = K;
        out[OUT_CENTERS] =
            mxCreateNumericArray (2, dims, mxINT32_CLASS, mxREAL);
        dims[0] = 1;
        dims[1] = N;
        out[OUT_ASGN] = mxCreateNumericArray (2, dims, mxUINT32_CLASS, mxREAL);
    }

    centers_pt = (acc_t *) mxGetPr (out[OUT_CENTERS]);
    asgn_pt = (idx_t *) mxGetPr (out[OUT_ASGN]);

    /* ikmeans starts here  */
    /* [centers_pt, asgn_pt] = f(data_pt, M, K, N) */
    /* K is the number of clusters */
    /* M is the dimension of each datapoint */
    /* N is the number of data points */
    /* npasses is hardcoded to 200, max */
    vl_ikmeans(data_pt, M, N, K, centers_pt, asgn_pt);

    int j;
    /* adjust */
    for (j = 0; j < N; ++j)
        ++asgn_pt[j];

}
