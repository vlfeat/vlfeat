#include <stdio.h>
#include <stdlib.h>
#include <vl/aib.h>

int main()
{
    vl_aib_node nrows = 10;
    vl_aib_node ncols = 3;
    vl_aib_prob Pic[3*10] = {     
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

    /* vl_prob * Pic = malloc(sizeof(vl_prob)*nrows*ncols); */
    int r,c;

    printf("Pic = [");
    for(r=0; r<nrows; r++)
    {
        for(c=0; c<ncols; c++)
        {
            /*Pic[r*ncols+c] = rand() % 100;*/
            printf("%f ", Pic[r*ncols+c]);
        }
        printf("; ...\n");
    }
    printf("];\n");

    printf("AIB starting\n");
    /*  parents always has size 2*nrows-1 */
    {
      vl_aib_node * parents = vl_aib(Pic, nrows, ncols, 0);
      for(r=0; r<2*nrows-1; r++)
        printf("%d => %d\n", r, parents[r]);      
      free(parents);
    }
    /* free(Pic); */

    printf("IB done\n");
    return 0;
}

