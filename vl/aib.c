#include "aib.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h> /* DBL_MAX */
#include <math.h>

#define BETA_MAX DBL_MAX
typedef struct _VlAIB
{
    vl_node   * nodes; 
    vl_double * beta;
    vl_node * bidx;
    vl_node nnodes;

    vl_node * which;  
    vl_node nwhich;

    vl_prob * Pic;
    vl_prob * Pi;
    vl_prob * Pc;
    vl_node nrows;
    vl_node ncols;
} VlAIB;

void vl_aib_normalizeP(vl_prob * P, vl_node nrows, vl_node ncols)
{
    vl_node i;
    vl_prob sum = 0;
    for(i=0; i<nrows*ncols; i++)
        sum += P[i];
    for(i=0; i<nrows*ncols; i++)
        P[i] /= sum;
}

vl_node * vl_aib_new_nodelist(vl_node nrows)
{
    vl_node * nodelist = malloc(sizeof(vl_node)*nrows);
    vl_node n;
    for(n=0; n<nrows; n++)
        nodelist[n] = n;

    return nodelist;
}

vl_prob * vl_aib_new_Pi(vl_prob * Pic, vl_node nrows, vl_node ncols)
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

vl_prob * vl_aib_new_Pc(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    vl_prob * Pc = malloc(sizeof(vl_prob)*ncols);
    vl_node r, c;
    for(c=0; c<ncols; c++)
    {
        vl_prob sum = 0;
        for(r=0; r<nrows; r++)
            sum += Pic[r*ncols+c];
        Pc[c] = sum;
    }
    return Pc;
}

void vl_aib_min_beta(VlAIB * aib, vl_node * besti, vl_node * bestj, vl_double * minbeta)
{
    *minbeta = aib->beta[0];
    *besti   = 0;
    *bestj   = aib->bidx[0];

    vl_node i;
    for(i=0; i<aib->nnodes; i++)
    {
        if(aib->beta[i] < *minbeta)
        {
            *minbeta = aib->beta[i];
            *besti = i;
            *bestj = aib->bidx[i];
        }
    }
}

void vl_aib_merge_nodes(VlAIB * aib, vl_node i, vl_node j, vl_node new)
{
    vl_node lastnode = aib->nnodes-1;
    vl_node c;
    /* clear which */
    aib->nwhich = 0;

    if(i > j) { vl_node tmp = j; j = i; i = tmp; }

    /* merge i and j */
    for(c=0; c<aib->ncols; c++)    /* Pic */
        aib->Pic[i*aib->ncols + c] += aib->Pic[j*aib->ncols + c];
    aib->Pi[i] += aib->Pi[j]; /* Pi */
    aib->beta[i] = BETA_MAX; /* beta */
    aib->nodes[i] = new; /* nodes */
    /* i will always be added to which */

    /* copy lastnode into j */
    for(c=0; c<aib->ncols; c++)    /* Pic */
        aib->Pic[j*aib->ncols + c] = aib->Pic[lastnode*aib->ncols + c];
    aib->Pi[j] = aib->Pi[lastnode]; /* Pi */
    aib->beta[j] = aib->beta[lastnode]; /* beta */
    aib->bidx[j] = aib->bidx[lastnode]; /* bidx */
    aib->nodes[j] = aib->nodes[lastnode]; /* nodes */

    /* delete a node */
    aib->nnodes--;
    /* fprintf(stderr, "nnodes %d\n", aib->nnodes); */

    vl_node n;
    for(n=0; n < aib->nnodes; n++)
    {
        /* find any bidx = i || bidx == j and add them to which */
        if(aib->bidx[n] == i || aib->bidx[n] == j)
        {
            aib->bidx[n] = 0;
            aib->beta[n] = BETA_MAX;
            aib->which[aib->nwhich++] = n;
        }
        /* find any bidx = lastnode and make it bidx = j */
        else if(aib->bidx[n] == lastnode)
            aib->bidx[n] = j;
    }

}

#define PLOGP(x) (x)*log((x))
void vl_aib_update_beta(VlAIB * aib)
{
    vl_node i;
    vl_prob * Pi  = aib->Pi;
    vl_prob * Pic = aib->Pic;

    fprintf(stderr, "Considering: ");
    for(i=0; i<aib->nwhich; i++)
        fprintf(stderr, "%d ", aib->which[i]);
    fprintf(stderr, "\n");

    for(i=0; i<aib->nwhich; i++)
    {
        vl_node a = aib->which[i];
        vl_node b;
        for(b=0; b<aib->nnodes; b++)
        {
            if(a==b) continue;

            vl_double C1 = 0;

            if (Pi [a] == 0 || Pi [b] == 0) continue;

            C1 =  - PLOGP ((Pi[a] + Pi[b])) ;

            vl_node c;
            for (c = 0 ; c < aib->ncols; ++ c) {
                vl_double Pac = Pic [a*aib->ncols + c] ;
                vl_double Pbc = Pic [b*aib->ncols + c] ;

                if(Pac != 0)
                    C1 += -Pac*log(Pac/Pi[a]); /* tmpa */
                if(Pbc != 0)
                    C1 += -Pbc*log(Pbc/Pi[b]); /* tmpb */

                if (Pac == 0 && Pbc == 0) continue;
                C1 += PLOGP ((Pac + Pbc)) ;
            }

            vl_double beta = -C1;

            if (beta < aib->beta[a])
            {
                aib->beta[a] = beta;
                aib->bidx[a] = b;
            }
            if (beta < aib->beta[b])
            {
                aib->beta[b] = beta;
                aib->bidx[b] = a;
            }
        }
    }
}

void vl_aib_calculate_information(VlAIB * aib, vl_prob * I, vl_prob * H)
{
    *H = 0;
    *I = 0;

    vl_node r, c;
    for(r=0; r<aib->nnodes; r++)
    {
        if(aib->Pi[r] == 0) continue;
        *H += -log(aib->Pi[r])*aib->Pi[r];
        for(c=0; c<aib->ncols; c++)
        {
            if(aib->Pc[c] == 0) continue;
            *I += aib->Pic[r*aib->ncols+c] * 
                  log( aib->Pic[r*aib->ncols+c] / (aib->Pi[r]*aib->Pc[c]));
        }
    }

    fprintf(stderr, "I=%g, H=%g\n", *I, *H);
}

VlAIB * vl_aib_new_aib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    VlAIB * aib = malloc(sizeof(VlAIB));
    aib->Pic = Pic;
    aib->nrows = nrows;
    aib->ncols = ncols;

    vl_aib_normalizeP(aib->Pic, aib->nrows, aib->ncols);
    aib->Pi = vl_aib_new_Pi(aib->Pic, aib->nrows, aib->ncols);
    aib->Pc = vl_aib_new_Pc(aib->Pic, aib->nrows, aib->ncols);

    /* nodelist contains all the remaining nodes. This also has to be modified
     * after a merge, but order doesn't matter (as long as Pi and Pic agree) */
    aib->nnodes = nrows;
    aib->nodes = vl_aib_new_nodelist(aib->nnodes);
    aib->beta   = malloc(sizeof(vl_prob)*aib->nnodes);
    aib->bidx   = malloc(sizeof(vl_node)*aib->nnodes);
    vl_node i;
    for(i=0; i<aib->nnodes; i++)
        aib->beta[i] = BETA_MAX;
    
    /* Initially we must consider all nodes */
    aib->nwhich = nrows;
    aib->which  = vl_aib_new_nodelist(aib->nwhich);

    return aib;
}

void vl_aib_delete_aib(VlAIB * aib)
{
    free(aib->nodes);
    free(aib->beta);
    free(aib->bidx);
    free(aib->which);
    free(aib->Pi);
    free(aib->Pc);
    free(aib);
}

vl_node * vl_aib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    vl_node * parents = malloc(sizeof(vl_node)*(nrows*2-1));
    vl_node n;
     /* Initially, all parents point to a nonexistant node */
    for(n=0; n<nrows*2-1; n++)
        parents[n] = nrows*2; 

    VlAIB * aib = vl_aib_new_aib(Pic, nrows, ncols);

    vl_node i, besti, bestj, newnode;
    vl_double minbeta;
    /* Initially which = all */

    /* For each merge */
    for(i=0; i<nrows-1; i++)
    {
        /* Update those rows which need to be updated */
        vl_aib_update_beta(aib);

        /* Find best merge */
        vl_aib_min_beta(aib, &besti, &bestj, &minbeta);
        if(minbeta == BETA_MAX)
            break; /* All that remain are null rows */

        /* Add the parent pointers for the new node */
        newnode = nrows+i;
        parents[aib->nodes[besti]] = newnode;
        parents[aib->nodes[bestj]] = newnode;

        /* Merge the nodes which produced the minimum beta */
        fprintf(stderr, "Merging %d and %d into %d beta %.4g\n", aib->nodes[besti], aib->nodes[bestj], newnode, aib->beta[besti]);
        vl_aib_merge_nodes(aib, besti, bestj, newnode);
        vl_prob I, H;
        vl_aib_calculate_information(aib, &I, &H);
    }

    vl_aib_delete_aib(aib);

    return parents;
} 
