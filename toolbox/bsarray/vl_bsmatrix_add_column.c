/** @internal
 ** @file     vl_bsarray_add_block.c
 ** @author   Daniele Perrone
 ** @brief    Block Sparse Array - Add Block - MEX
 **/

/*
Copyright Statement
*/
#include <mexutils.h>
#include <assert.h>

#include <vl/bsmatrix.h>



/** ------------------------------------------------------------------
 ** @brief MEX entry point
 **/

void
mexFunction(int nout, mxArray *out[],
            int nin, const mxArray *in[])
{
  enum {IN_BSARRAY, IN_BLOCK, IN_END} ;
  enum {OUT_DATA=0} ;


}
