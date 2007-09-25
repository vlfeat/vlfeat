/* file:        ikmeanspush.c
** description: MEX weighted ikmeanspush function.
** author:      Andrea Vedaldi
**/

/* AUTORIGHTS */

#include"mexutils.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

#include <vl/generic.h>
#include <vl/ikmeans.h>

/* driver */
void
mexFunction(int nout, mxArray *out[],
        int nin, const mxArray *in[])
{
    enum {IN_X=0,IN_C} ;
    enum {OUT_ASGN=0} ;
    vl_uint*  asgn ;
    vl_int32* centers ;
    vl_uint8* data ;
    int M,N,j ;
    int K=0 ;

    /** -----------------------------------------------------------------
     **                                               Check the arguments
     ** -------------------------------------------------------------- */
    if (nin != 2) {
        mexErrMsgTxt("Two arguments required.") ;
    } else if (nout > 2) {
        mexErrMsgTxt("Too many output arguments.") ;
    }

    if(mxGetClassID(in[IN_X]) != mxUINT8_CLASS) {
        mexErrMsgTxt("X must be of class UINT8") ;
    }

    if(mxGetClassID(in[IN_C]) != mxINT32_CLASS) {
        mexErrMsgTxt("C must be of class INT32") ;
    }

    M = mxGetM(in[IN_X]) ;     /* n of components */
    N = mxGetN(in[IN_X]) ;     /* n of elements */
    K = mxGetN(in[IN_C]) ;  /* n of centers */

    if( mxGetM(in[IN_C]) != M ) {
        mexErrMsgTxt("DATA and CENTERS must have the same number of columns.") ;
    }

    out[OUT_ASGN] = mxCreateNumericMatrix(1,N,mxUINT32_CLASS,mxREAL);

    data    = (vl_uint8*) mxGetPr(in[IN_X]) ;
    centers = (vl_int32*) mxGetPr(in[IN_C]) ;
    asgn    = (vl_uint*)  mxGetPr(out[OUT_ASGN]);

    vl_ikmeans_push (asgn, centers, K, data, M, N);

    /* adjust for MATLAB indexing */
    for(j = 0 ; j < N ; ++j) ++ asgn[j] ;
}
