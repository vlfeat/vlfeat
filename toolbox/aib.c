/** @file     aib.mex.c
 ** @author   Brian Fulkerson
 ** @brief    AIB MEX driver
 ** @internal
 **/

#include "mexutils.h"
#include <vl/mathop.h>
#include <vl/aib.h>
#include <assert.h>


/** @brief MEX entry point */
void
mexFunction(int nout, mxArray *out[], 
            int nin, const mxArray *in[])
{
  enum {IN_PIC=0} ;
  enum {OUT_PARENTS=0} ;

  vl_prob * P = mxGetPr(in[IN_PIC]);
  vl_node rows = mxGetM(in[IN_PIC]);
  vl_node cols = mxGetN(in[IN_PIC]);
  vl_prob * Pic = malloc(sizeof(vl_prob)*rows*cols);
  int r,c;
  fprintf(stderr, "transposing\n");
  for(r=0; r<rows; r++)
      for(c=0; c<cols; c++)
          Pic[r*cols+c] = P[c*rows+r];

  /*
  fprintf(stderr, "dumping Pic\n");
  for(r=0; r<rows; r++)
  {
      for(c=0; c<cols; c++)
      {
          fprintf(stderr, "%f ", Pic[r*cols+c]);
      }
      fprintf(stderr, "\n");
  }
  */

  {
    vl_node *parents = vl_aib(Pic, rows, cols);
    vl_node *mexparents ;
    int dims [2], n ;
    /* Create an empty array */
    dims[0] = 2*rows-1;
    dims[1] = 1;
    out[OUT_PARENTS] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
    mexparents = (vl_node *)mxGetData(out[OUT_PARENTS]);
    
    for(n=0; n<2*rows-1; n++)
      if(parents[n] == 2*rows)
        mexparents[n] = 0;
      else
        mexparents[n] = parents[n]+1; /* parents + 1 for matlab indexing */
    
    /* Fill it with our stuff
       dims[0] = rows*2-1;
       dims[1] = 1;
       mxSetDimensions(out[OUT_PARENTS], dims, 2);
       mxSetData(out[OUT_PARENTS], parents);
    */
  }
}
