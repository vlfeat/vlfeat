#include "ib.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h> /* DBL_MAX */
#include <math.h>

#define BETA_MAX DBL_MAX
typedef struct _VlIB
{
    vl_node   * nodes; 
    vl_double * beta;
    vl_node * bidx;
    vl_node nnodes;

    vl_node * which;  
    vl_node nwhich;

    vl_prob * Pic;
    vl_prob * Pi;
    vl_node nrows;
    vl_node ncols;
} VlIB;

void vl_ib_normalizeP(vl_prob * P, vl_node nrows, vl_node ncols)
{
    vl_node i;
    vl_prob sum = 0;
    for(i=0; i<nrows*ncols; i++)
        sum += P[i];
    for(i=0; i<nrows*ncols; i++)
        P[i] /= sum;
}

vl_node * vl_ib_new_nodelist(vl_node nrows)
{
    vl_node * nodelist = malloc(sizeof(vl_node)*nrows);
    vl_node n;
    for(n=0; n<nrows; n++)
        nodelist[n] = n;

    return nodelist;
}

vl_prob * vl_ib_new_Pi(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    vl_prob * Pi = malloc(sizeof(vl_prob)*nrows);
    vl_node r,c;
    for(r=0; r<nrows; r++)
    {
        vl_prob sum = 0;
        for(c=0; c<ncols; c++)
            sum += Pic[r*ncols+c];
        Pi[r] = sum;
    }
    return Pi;
}

void vl_ib_min_beta(VlIB * ib, vl_node * besti, vl_node * bestj, vl_double * minbeta)
{
    *minbeta = ib->beta[0];
    *besti   = 0;
    *bestj   = ib->bidx[0];

    vl_node i;
    for(i=0; i<ib->nnodes; i++)
        if(*minbeta < ib->beta[i])
        {
            *minbeta = ib->beta[i];
            *besti = i;
            *bestj = ib->bidx[i];
        }
}

void vl_ib_merge_nodes(VlIB * ib, vl_node i, vl_node j, vl_node new)
{
    vl_node lastnode = ib->nnodes-1;
    vl_node c;
    /* clear which */
    ib->nwhich = 0;

    /* merge i and j */
    for(c=0; c<ib->ncols; c++)    /* Pic */
        ib->Pic[i*ib->ncols + c] += ib->Pic[j*ib->ncols + c];
    ib->Pi[i] += ib->Pi[j]; /* Pi */
    ib->beta[i] = BETA_MAX; /* beta */
    ib->bidx[i] = 0; /* bidx */
    ib->nodes[i] = new; /* nodes */
    ib->which[ib->nwhich++] = i; /* add i to which */

    /* copy lastnode into j */
    for(c=0; c<ib->ncols; c++)    /* Pic */
        ib->Pic[j*ib->ncols + c] = ib->Pic[lastnode*ib->ncols + c];
    ib->Pi[j] = ib->Pi[lastnode]; /* Pi */
    ib->beta[j] = ib->beta[lastnode]; /* beta */
    ib->bidx[j] = ib->bidx[lastnode]; /* bidx */
    ib->nodes[j] = ib->nodes[lastnode]; /* nodes */

    vl_node n;
    for(n=0; n < ib->nnodes; n++)
    {
        /* find any bidx = i || bidx == j and add them to which */
        if(ib->bidx[n] == i || ib->bidx[n] == j)
        {
            ib->bidx[n] = 0;
            ib->beta[n] = BETA_MAX;
            ib->which[ib->nwhich++] = n;
        }
        /* find any bidx = lastnode and make it bidx = j */
        else if(ib->bidx[n] == lastnode)
            ib->bidx[n] = j;
    }

    /* delete a node */
    ib->nnodes--;
}

#define PLOGP(x) (x)*log((x))
void vl_ib_update_beta(VlIB * ib)
{
    vl_node i;
    vl_prob * Pi  = ib->Pi;
    vl_prob * Pic = ib->Pic;

    fprintf(stderr, "Which is: ");
    for(i=0; i<ib->nwhich; i++)
        fprintf(stderr, "%d ", ib->which[i]);
    fprintf(stderr, "\n");

    for(i=0; i<ib->nwhich; i++)
    {
        vl_node a = ib->which[i];
        vl_node b;
        for(b=0; b<ib->nnodes; b++)
        {
            if(a==b) continue;

            vl_double C1 = 0;

            if (Pi [a] == 0 || Pi [b] == 0) continue;

            C1 = 
/*                + ib->tmp [a] 
                  + ib->tmp [b] */
                - PLOGP ((Pi[a] + Pi[b])) ;

            vl_node c;
            for (c = 0 ; c < ib->ncols; ++ c) {
                vl_double Pac = Pic [a*ib->ncols + c] ;
                vl_double Pbc = Pic [b*ib->ncols + c] ;

                if(Pac != 0)
                    C1 += -Pac*log(Pac/Pi[a]); /* tmpa */
                if(Pbc != 0)
                    C1 += -Pbc*log(Pbc/Pi[b]); /* tmpb */

                if (Pac == 0 && Pbc == 0) continue;
                C1 += PLOGP ((Pac + Pbc)) ;
            }

            /* TODO: Does it matter if we pick min beta or max C1 - Beta*C2? */
            vl_double beta = -C1;
            fprintf(stderr, "beta %f a %d b %d\n", beta, a, b);

            if (beta < ib->beta[a])
            {
                ib->beta[a] = beta;
                ib->bidx[a] = b;
                /* fprintf(stderr, "minbeta %f\n", ib->beta[a]); */
            }
            if (beta < ib->beta[b])
            {
                ib->beta[b] = beta;
                ib->bidx[b] = a;
                /* fprintf(stderr, "minbeta %f\n", ib->beta[b]); */
            }
        }
    }
}

VlIB * vl_ib_new_ib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    VlIB * ib = malloc(sizeof(VlIB));
    ib->Pic = Pic;
    ib->nrows = nrows;
    ib->ncols = ncols;

    vl_ib_normalizeP(ib->Pic, ib->nrows, ib->ncols);
    ib->Pi = vl_ib_new_Pi(ib->Pic, ib->nrows, ib->ncols);

    /* nodelist contains all the remaining nodes. This also has to be modified
     * after a merge, but order doesn't matter (as long as Pi and Pic agree) */
    ib->nnodes = nrows;
    ib->nodes = vl_ib_new_nodelist(ib->nnodes);
    ib->beta   = malloc(sizeof(vl_prob)*ib->nnodes);
    ib->bidx   = malloc(sizeof(vl_node)*ib->nnodes);
    vl_node i;
    for(i=0; i<ib->nnodes; i++)
        ib->beta[i] = BETA_MAX;
    
    /* Have to manipulate the which list (delete) */
    /* order of which doesn't matter */
    ib->nwhich = nrows;
    ib->which  = vl_ib_new_nodelist(ib->nwhich);

    return ib;
}

void vl_ib_delete_ib(VlIB * ib)
{
    free(ib->nodes);
    free(ib->beta);
    free(ib->bidx);
    free(ib->which);
    free(ib->Pi);
    free(ib);
}

vl_node * vl_ib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    vl_node * parents = malloc(sizeof(vl_node)*(nrows*2-1));
    vl_node n;
     /* Initially, all parents point to a nonexistant node */
    for(n=0; n<nrows*2-1; n++)
        parents[n] = nrows*2; 

    VlIB * ib = vl_ib_new_ib(Pic, nrows, ncols);

    vl_node i, besti, bestj, newnode;
    vl_double minbeta;
    /* Initially which = all */

    /* For each merge */
    for(i=0; i<nrows-1; i++)
    {
        /* Update those rows which need to be updated */
        vl_ib_update_beta(ib);

        /* Find best merge */
        vl_ib_min_beta(ib, &besti, &bestj, &minbeta);

        /* Add the parent pointers for the new node */
        newnode = nrows+i;
        parents[ib->nodes[besti]] = newnode;
        parents[ib->nodes[bestj]] = newnode;

        /* Merge the nodes which produced the minimum beta */
        fprintf(stderr, "Merging %d and %d into %d beta %f\n", ib->nodes[besti], ib->nodes[bestj], newnode, ib->beta[besti]);
        vl_ib_merge_nodes(ib, besti, bestj, newnode);
        /* Remove node from:
         *  o which
         *  o nodelist 
         *  o Pic
         *  o Pi
         **/
    }

    vl_ib_delete_ib(ib);
    ib = 0;

    return parents;
} 
