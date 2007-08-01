/** @internal
 ** @file   aib.c
 ** @author Brian Fulkerson
 ** @brief  Agglomerative Information Bottleneck (AIB) with practical speedups.
 **/

#include "aib.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h> /* DBL_MAX */
#include <math.h>

/** @internal @brief The maximum value which beta may take */
#define BETA_MAX DBL_MAX

/** @internal @brief An internal structure which has pointers to our data */
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

/** ---------------------------------------------------------------- */
/** @internal @brief Normalizes an array of probabilities to sum to 1
 **
 ** @param P        The array of probabilities 
 ** @param nelem    The number of elements in the array
 **
 ** @return Modifies P to contain values which sum to 1
 **/
void vl_aib_normalize_P(vl_prob * P, vl_node nelem)
{
    vl_node i;
    vl_prob sum = 0;
    for(i=0; i<nelem; i++)
        sum += P[i];
    for(i=0; i<nelem; i++)
        P[i] /= sum;
}


/** ---------------------------------------------------------------- */
/** @internal @brief Allocates and creates a list of nodes
 **
 ** @param nnodes   The size of the list which will be created 
 **
 ** @return an array containing elements 0...nnodes
 **/
vl_node * vl_aib_new_nodelist(vl_node nnodes)
{
    vl_node * nodelist = malloc(sizeof(vl_node)*nnodes);
    vl_node n;
    for(n=0; n<nnodes; n++)
        nodelist[n] = n;

    return nodelist;
}

/** ---------------------------------------------------------------- */
/** @internal @brief Allocates and creates the marginal distribution Pi
 **
 ** @param Pic      A two-dimensional array of probabilities
 ** @param nrows    The number of rows in Pic
 ** @param ncols    The number of columns in Pic
 **
 ** @return an array of size @a nrows which contains the marginal distribution
 **         over the rows
 **/
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

/** ---------------------------------------------------------------- */
/** @internal @brief Allocates and creates the marginal distribution Pc
 **
 ** @param Pic      A two-dimensional array of probabilities
 ** @param nrows    The number of rows in Pic
 ** @param ncols    The number of columns in Pic
 **
 ** @return an array of size @a ncols which contains the marginal distribution
 **         over the columns
 **/
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

/** ---------------------------------------------------------------- */
/** @internal @brief Find the two nodes which have minimum beta.
 **
 ** @param aib      A pointer to the internal data structure
 ** @param besti    The index of one member of the pair which has mininum beta
 ** @param bestj    The index of the other member of the pair which 
 **                 minimizes beta
 ** @param minbeta  The minimum beta value corresponding to (@a i, @a j)
 **
 ** Searches @a aib->beta to find the minimum value and fills @a minbeta and 
 ** @a besti and @a bestj with this information.
 **/
void vl_aib_min_beta
(VlAIB * aib, vl_node * besti, vl_node * bestj, vl_double * minbeta)
{
    vl_node i;
    *minbeta = aib->beta[0];
    *besti   = 0;
    *bestj   = aib->bidx[0];

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

/** ---------------------------------------------------------------- */
/** @internal @brief Merges two nodes i,j in the internal datastructure
 **
 ** @param aib      A pointer to the internal data structure
 ** @param i        The index of one member of the pair to merge
 ** @param j        The index of the other member of the pair to merge
 ** @param new      The index of the new node which corresponds to the union of  **                 (@a i, @a j).
 **
 ** Nodes are merged by replacing i with the union of i,j and moving the node in ** last position of the nodelist (called @a lastnode) to the jth position.
 **
 ** After the nodes have been merged, it updates which nodes should be 
 ** considered on the next iteration based on which beta values could 
 ** potentially change. The merged node will always be part of this list.
 **/
void vl_aib_merge_nodes(VlAIB * aib, vl_node i, vl_node j, vl_node new)
{
    vl_node lastnode = aib->nnodes-1;
    vl_node c, n ;

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


/** ---------------------------------------------------------------- */
/** @internal @brief Updates aib->beta and aib->bidx according to aib->which
 **
 ** @param aib      A pointer to the internal data structure
 **
 ** Calculates new @a beta values for the nodes listed in @a aib->which. 
 ** @a beta is the loss of mutual information caused by merging two nodes.
 **
 ** More concretely, for @a (i,j), where X corresponds to the rows of Pic,
 ** Y corresponds to the columns of Pic and M corresponds to the rows of Pic 
 ** after a merge, @a beta is: 
 **
 ** @a beta = I(@a X, @a Y) - I(@a M, @a Y)
 **/
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
            vl_double C1 = 0;
            vl_node c;

            if(a==b) continue;

            if (Pi [a] == 0 || Pi [b] == 0) continue;

            C1 =  - PLOGP ((Pi[a] + Pi[b])) ;

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

            {
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
}

/** ---------------------------------------------------------------- */
/** @internal @brief Calculates the current information and entropy
 **
 ** @param aib      A pointer to the internal data structure
 ** @param I        The current mutual information.
 ** @param H        The current entropy.
 **
 ** Calculates the current mutual information and entropy of Pic and sets
 ** @a I and @a H to these new values.
 **/
void vl_aib_calculate_information(VlAIB * aib, vl_prob * I, vl_prob * H)
{
    vl_node r, c;
    *H = 0;
    *I = 0;

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

/** ---------------------------------------------------------------- */
/** @internal @brief Allocates and initializes the internal data structure
 **
 ** @param Pic      A pointer to a 2D array of probabilities
 ** @param nrows    The number of rows in the array     
 ** @param ncols    The number of columns in the array
 **
 ** Creates a new @a VlAIB struct containing pointers to all the data that 
 ** will be used during the AIB process. 
 **
 ** Allocates memory for the following:
 ** - Pi (nrows*sizeof(vl_prob))
 ** - Pc (ncols*sizeof(vl_prob))
 ** - nodelist (nrows*sizeof(vl_node))
 ** - which (nrows*sizeof(vl_node))
 ** - beta (nrows*sizeof(vl_double))
 ** - bidx (nrows*sizeof(vl_node))
 **
 ** Since it simply copies to pointer to Pic, the total additional memory
 ** requirement is: 
 ** (nrows+ncols)*sizeof(vl_prob) + 3*nrows*sizeof(vl_prob) + 
 ** nrows*sizeof(vl_double)
 **
 ** @returns An allocated and initialized @a VlAIB pointer
 **/
VlAIB * vl_aib_new_aib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    VlAIB * aib = malloc(sizeof(VlAIB));
    vl_node i ;

    aib->Pic = Pic;
    aib->nrows = nrows;
    aib->ncols = ncols;

    vl_aib_normalize_P(aib->Pic, aib->nrows*aib->ncols);
    aib->Pi = vl_aib_new_Pi(aib->Pic, aib->nrows, aib->ncols);
    aib->Pc = vl_aib_new_Pc(aib->Pic, aib->nrows, aib->ncols);

    /* nodelist contains all the remaining nodes. This also has to be modified
     * after a merge, but order doesn't matter (as long as Pi and Pic agree) */
    aib->nnodes = nrows;
    aib->nodes = vl_aib_new_nodelist(aib->nnodes);
    aib->beta   = malloc(sizeof(vl_double)*aib->nnodes);
    aib->bidx   = malloc(sizeof(vl_node)*aib->nnodes);

    for(i=0; i<aib->nnodes; i++)
        aib->beta[i] = BETA_MAX;
    
    /* Initially we must consider all nodes */
    aib->nwhich = nrows;
    aib->which  = vl_aib_new_nodelist(aib->nwhich);

    return aib;
}

/** ---------------------------------------------------------------- */
/** @internal @brief Deletes a VlAIB and frees all memory associated 
 ** with it.
 **
 ** @param aib  The VlAIB to be deleted.
 **/ 
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

/** ---------------------------------------------------------------- */
/** @brief Runs AIB on Pic
 **
 ** @param Pic   The joint probability of P(i,c).
 ** @param nrows The number of rows in Pic
 ** @param ncols The number of columns in Pic
 **
 ** Runs Agglomerative Information Bottleneck on the data defined by
 ** @a Pic. Merges Pic in the i direction until no more merges are possible
 ** and returns these merges as a tree represented by pointers to parents.
 **
 ** @return An array of parent pointers. The array has size nrows*2-1
 **/ 
vl_node * vl_aib(vl_prob * Pic, vl_node nrows, vl_node ncols)
{
    vl_node * parents = malloc(sizeof(vl_node)*(nrows*2-1));
    vl_node n;
    /* Initially, all parents point to a nonexistant node */
    for(n=0; n<nrows*2-1; n++)
        parents[n] = nrows*2; 

    {
      VlAIB * aib = vl_aib_new_aib(Pic, nrows, ncols);
      
      vl_node i, besti, bestj, newnode;
      vl_double minbeta;
      /* Initially which = all */

      /* For each merge */
      for(i=0; i<nrows-1; i++)
        {
          vl_prob I, H;

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
    
          vl_aib_calculate_information(aib, &I, &H);
        }

      vl_aib_delete_aib(aib);
    }

    return parents;
} 
