#include "ikmeans.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memset */

/* pairs are used to generate random permutations of data */
typedef struct
{
    int w;
    int j;
} pair_t;

int cmp_pair (void const *a, void const *b)
{
    pair_t *pa = (pair_t *) a;
    pair_t *pb = (pair_t *) b;

    return pa->w - pb->w;
}


void vl_ikmeans(data_t * data_pt, int M, int N, idx_t K,
             acc_t * centers_pt, idx_t * asgn_pt)
{

    /* random permutation */
    idx_t i, j, k, pass;
    int done;
    int npasses = 200;
    vl_bool verbose = 1 ;
    acc_t *counts_pt = malloc (sizeof (acc_t) * K);
    pair_t *pairs_pt = (pair_t *) malloc (sizeof (pair_t) * N);

    for (j = 0; j < N; ++j)
    {
        pairs_pt[j].j = j;
        pairs_pt[j].w = rand ();
    }
    qsort (pairs_pt, N, sizeof (pair_t), cmp_pair);

    /* initialize centers */
    for (k = 0; k < K; ++k)
        for (i = 0; i < M; ++i)
            centers_pt[k * M + i] = data_pt[pairs_pt[k].j * M + i];

    /* do passes */
    if (verbose)
        printf ("ikmeans: passes:");

    for (pass = 0; 1; ++pass)
    {

        if (verbose)
            printf (" %d", pass + 1);

        /* assign data to centers */
        done = 1;
        for (j = 0; j < N; ++j)
        {
            acc_t best_dist = 0;
            idx_t best = (idx_t) - 1;

            for (k = 0; k < K; ++k)
            {
                acc_t dist = 0;

                /* compute distance with this center */
                for (i = 0; i < M; ++i)
                {
                    acc_t delta =
                        data_pt[j * M + i] - centers_pt[k * M + i];
                    dist += delta * delta;
                }

                /* compare with current best */
                if (best == (idx_t) - 1 || dist < best_dist)
                {
                    best = k;
                    best_dist = dist;
                }
            }
            if (asgn_pt[j] != best)
            {
                asgn_pt[j] = best;
                done = 0;
            }
        }

        /* stopping condition */
        if (done || pass == npasses)
        {
            if (verbose)
                printf ("\n");
            break;
        }

        /* re-compute centers */
        memset (centers_pt, 0, sizeof (acc_t) * M * K);
        memset (counts_pt, 0, sizeof (acc_t) * K);
        for (j = 0; j < N; ++j)
        {
            int this_center = asgn_pt[j];

            ++counts_pt[this_center];

            for (i = 0; i < M; ++i)
                centers_pt[this_center * M + i] += data_pt[j * M + i];
        }

        for (k = 0; k < K; ++k)
        {
            acc_t n = counts_pt[k];

            if (n > 0xffffff)
                fprintf(stderr, "ikmeans: Possible overflow of variable");
            if (n > 0)
                for (i = 0; i < M; ++i)
                    centers_pt[k * M + i] /= n;
            else
                for (i = 0; i < M; ++i)
                    centers_pt[k * M + i] = data_pt[pairs_pt[k].j * M + i];
        }
    }
    free (pairs_pt);
    free (counts_pt);

}


void vl_ikmeans_push(data_t * data_pt, acc_t * centers_pt, int M, int N, idx_t K,
                     idx_t * asgn_pt)
{
    int i,j,k ;

    /* assign data to centers */
    for(j=0 ; j < N ; ++j)
    {
        acc_t best_dist = 0 ;
        idx_t best = (idx_t)-1 ;

        for(k = 0 ; k < K ; ++k)
        {
            acc_t dist = 0 ;

            /* compute distance with this center */
            for(i = 0 ; i < M ; ++i)
            {
                acc_t delta = data_pt[ j*M + i ] - centers_pt[ k*M + i ] ;
                dist += delta*delta ;
            }

            /* compare with current best */
            if( best == (idx_t)-1 || dist < best_dist )
            {
                best = k  ;
                best_dist = dist ;
            }
        }
        asgn_pt[j] = best ;
    }
}
