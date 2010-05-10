/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <vl/aib.h>

int main()
{
    vl_uint32 nrows = 10;
    vl_uint32 ncols = 3;
    double Pic[3*10] = {     
    0.6813,    0.3028,    0.8216,
    0.3795,    0.5417,    0.6449,
    0.8318,    0.1509,    0.8180,
    0.5028,    0.6979,    0.6602,
    0.7095,    0.3784,    0.3420,
    0.4289,    0.8600,    0.2897,
    0.3046,    0.8537,    0.3412,
    0.1897,    0.5936,    0.5341,
    0.1934,    0.4966,    0.7271,
    0.6822,    0.8998,    0.3093,
    };

    vl_uint32 r,c;
    VlAIB * aib;
    vl_uint * parents;

    printf("Pic = [");
    for(r=0; r<nrows; r++)
    {
        for(c=0; c<ncols; c++)
            printf("%f ", Pic[r*ncols+c]);
        printf("; ...\n");
    }
    printf("];\n");

    printf("AIB starting\n");
    {
      aib = vl_aib_new(Pic, nrows, ncols);
      vl_aib_process(aib);
    
      /*  parents always has size 2*nrows-1 */
      parents = vl_aib_get_parents(aib);
      for(r=0; r<2*nrows-1; r++)
        printf("%d => %d\n", r, parents[r]);      

      vl_aib_delete(aib);
    }
    /* free(Pic); */

    printf("IB done\n");
    return 0;
}

