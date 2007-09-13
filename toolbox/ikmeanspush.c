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
    vl_uint*  asgn ;
    vl_int32* centers ;
    vl_uint8* data ;
    /*  mxClassID data_class = mxINT8_CLASS ;*/
    enum {IN_DATA=0,IN_CENTERS} ;
    enum {OUT_ASGN} ;
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

    if(mxGetClassID(in[IN_DATA]) != mxUINT8_CLASS) {
        mexErrMsgTxt("X must be of class UINT8") ;
    }

    if(mxGetClassID(in[IN_CENTERS]) != mxINT32_CLASS) {
        mexErrMsgTxt("C must be of class INT32") ;
    }

    M = mxGetM(in[IN_DATA]) ;     /* n of components */
    N = mxGetN(in[IN_DATA]) ;     /* n of elements */
    K = mxGetN(in[IN_CENTERS]) ;  /* n of centers */

    if( mxGetM(in[IN_CENTERS]) != M ) {
        mexErrMsgTxt("DATA and CENTERS must have the same number of columns.") ;
    }

    out[OUT_ASGN] = mxCreateNumericMatrix(1,N,mxUINT32_CLASS,mxREAL);

    data    = (vl_uint8*) mxGetPr(in[IN_DATA]) ;
    centers = (vl_int32*) mxGetPr(in[IN_CENTERS]) ;
    asgn    = (vl_uint*)  mxGetPr(out[OUT_ASGN]);

    vl_ikmeans_push (asgn, centers, K, data, M, N);

    /* adjust for MATLAB indexing */
    for(j = 0 ; j < N ; ++j) ++ asgn[j] ;
}
