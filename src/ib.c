#include <stdio.h>
#include <stdlib.h>
#include <vl/ib.h>

int main()
{
    vl_node nrows = 30;
    vl_node ncols = 3;
    vl_prob * Pic = malloc(sizeof(vl_prob)*nrows*ncols);
    int r,c;
    printf("Pic = [");
    for(r=0; r<nrows; r++)
    {
        for(c=0; c<ncols; c++)
        {
            Pic[r*ncols+c] = rand() % 100;
            printf("%f ", Pic[r*ncols+c]);
        }
        printf("; ...\n");
    }
    printf("];\n");

    printf("IB starting\n");
    // parents always has size 2*nrows-1
    vl_node * parents = vl_ib(Pic, nrows, ncols);
    for(r=0; r<2*nrows-1; r++)
        printf("%d => %d\n", r, parents[r]);

    free(parents);
    free(Pic);

    printf("IB done\n");
    return 0;
}

