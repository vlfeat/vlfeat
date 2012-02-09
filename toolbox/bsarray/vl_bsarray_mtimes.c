/** @internal
 ** @file     vl_bsarray_mtimes.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Array - Returns mtimes between bsarray and full array - MEX
 **/

/*
Copyright Statement
*/

#include <mexutils.h>

#include <vl/bsarray.h>

/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_BSARRAY=0,IN_FULL_ARRAY=1, IN_END} ;
  enum {OUT_MTIMES=0} ;
    
  VlBlockSparseArrayHeader* bsarray ;
  float* fullArray ; 

  double* acc ; 

  /* -----------------------------------------------------------------
   *                                               Check the arguments
   * -------------------------------------------------------------- */

  if (nin > 2) 
    {
      mexErrMsgTxt("One argument required.") ;
    }
 
  if (nout > 1) 
    {
      mexErrMsgTxt("One output required.") ;
    }

  bsarray = (VlBlockSparseArrayHeader*) mxGetData(in[IN_BSARRAY]) ;
  fullArray = (float*) mxGetData(in[IN_FULL_ARRAY]) ;

   /* ...............................................................
   *                                                   Define output
   * ............................................................ */
  mwSize dims [2] ;

  /* create an empty array */
  dims [0] = 1 ;
  dims [1] = 1 ;
  out[OUT_MTIMES] = mxCreateNumericArray
    (2, dims, mxDOUBLE_CLASS, mxREAL) ;

  acc = (double*) mxGetData(out[OUT_MTIMES]) ;

  /* ...............................................................
   *                                                     Fill output
   * ............................................................ */

  *acc = vl_bsarray_mtimes (bsarray, fullArray ) ;  
  
}
