/** @internal
 ** @file     aib.c
 ** @author   Brian Fulkerson
 ** @author   Andrea Vedaldi
 ** @brief    Agglomerative Information Bottleneck (AIB) - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

/** @file aib.h
 ** @author Brian Fulkerson
 ** @author Andrea Vedaldi
 ** @brief  Agglomerative Information Bottleneck (AIB)

 This provides an implementation of Agglomerative Information
 Bottleneck (AIB) as first described in:
 
 [Slonim] <em>N. Slonim and N. Tishby. Agglomerative information
 bottleneck.  In Proc. NIPS, 1999</em>

 AIB takes a discrete valued feature \f$x\f$ and a label \f$c\f$ and
 gradually compresses \f$x\f$ by merging values while preserving as
 mush as possible the mutual information \f$I(x,c)\f$.
 
 While the algorithm is equivalent to the one described in [Slonim],
 it uses some speedups that enable handling much larger datasets. Let
 <em>N</em> be the number of feature values and <em>C</em> the number
 of labels.  [Slonim] algorithm is \f$O(N^2)\f$ space and \f$O(C
 N^3)\f$ time. This algorithm is \f$O(N)\f$ space and \f$O(C N^2)\f$
 time in common cases (\f$O(N^3C)\f$ in the worst case).

 @section aib-overview Overview

 Given a discrete feature @f$x \in \mathcal{X} = \{x_1,\dots,x_N\}@f$
 and a category label @f$c = 1,\dots,C@f$ with joint probability
 @f$p(x,c)@f$, AIB computes a compressed feature @f$[x]_{ij}@f$ by
 merging two values @f$x_i@f$ and @f$x_j@f$.  Among all the pairs
 @f$ij@f$, AIB chooses the one that yields the smallest loss in the
 mutual information

 @f[
    D_{ij} = I(x,c) - I([x]_{ij},c) = 
    \sum_c p(x_i) \log \frac{p(x_i,c)}{p(x_i)p(c)}   +
    \sum_c p(x_i) \log \frac{p(x_i,c)}{p(x_i)p(c)}   -    
    \sum_c (p(x_i)+p(x_j)) \log \frac {p(x_i,c)+p(x_i,c)}{(p(x_i)+p(x_j))p(c)}
 @f]

 AIB iterates this procedure as long as the desired level of
 compression is achieved.

 @subsection aib-overview-ec Entropy constrained AIB

 Here we suggest an `entropy constrained' version of AIB. This version
 of AIB optimizes the information-entropy trade-off
 \f[
   F(x,c|\beta) = I(x,c) - \beta H(x), \quad \beta > 0.
 \f]
 Joining \f$ij\f$ yields an improvement
 \f[
    \Delta_{ij} =  F([x]_{ij},c|\beta) - F(x,c|\beta) 
 \f]
 of this goal function. For small values of \f$\beta\f$, there is no
 convenience in performing any merge (i.e. \f$\Delta_{ij} < 0\f$ for
 all pairs). However for \f$\beta\f$ big enough a merge will yield a
 positive improvement \f$\Delta_{ij} \geq 0\f$. The minimum vale of
 \f$\beta\f$ for which this happens is
 \f[
   \beta = \min_{ij} \left(- \frac{I(x,c) - I([x]_{ij},c)}{H(x) - H([x]_{ij})} \right).
 \f]
 This also identifies the pair \f$ij\f$ that we shoudl merge. Entropy
 constrained AIB is therefore the same as AIB, except that it works
 with the adjusted matrix
 \f[
   D_{ij} = - \frac{I(x,c) - I([x]_{ij},c)}{H(x) - H([x]_{ij})}.
 \f]

 @subsection aib-overview-volume Volume regularized AIB

 Blha

 @section aib-algorithm Algorithm details

 Computing \f$D_{ij}\f$ requires \f$O(C)\f$ operations. For example,
 in standard AIB we need to calculate
 @f[
    D_{ij} = I(x,c) - I([x]_{ij},c) = 
    \sum_c p(x_i) \log \frac{p(x_i,c)}{p(x_i)p(c)}   +
    \sum_c p(x_i) \log \frac{p(x_i,c)}{p(x_i)p(c)}   -    
    \sum_c (p(x_i)+p(x_j)) \log \frac {p(x_i,c)+p(x_i,c)}{(p(x_i)+p(x_j))p(c)}
 @f]

 Thus in a basic implementation of AIB, finding the optimal pair
 \f$ij\f$ of feature values requires \f$O(CN^2)\f$ operations in
 total. In order to join all the \f$N\f$ values, we repeat this
 procedure \f$O(N)\f$ times, yielding \f$O(N^3 C)\f$ time and
 \f$O(1)\f$ space complexity (this does not account for the space need
 to store the input).

 The complexity can be improved by reusing computations. For instance,
 we can store the matrix \f$D = [ D_{ij} ]\f$ (which requires
 \f$O(N^2)\f$ space). Then, after joining \f$ij\f$, all of the matrix
 <em>D</em> except the rows and columns (the matrix is symmetric) of
 indexes <em>i</em> and <em>j</em> is unchanged. These two rows and
 columns are deleted and a new row and column, whose computation
 requires \f$O(NC)\f$ operations, are added for the merged value
 \f$x_{ij}\f$.  Finding the minimal element of the matrix still requires
 \f$O(N^2)\f$ operations, so the complexity of this algorithm is
 \f$O(N^2C + N^3)\f$ time and \f$O(N^2)\f$ space.

 We can obtain a much better expected complexity as follows. First,
 instead of storing the whole matrix <em>D</em>, we store the smallest
 element (index and value) of each row as \f$(q_i, D_i)\f$ (notice
 that this is also the best element of each column, being <em>D</em>
 symmetric). This requires \f$O(N)\f$ space only and finding the
 minimal element of the marix requires \f$O(N)\f$ operations
 only. After joining \f$ij\f$, we have to update efficiently this
 representation. This is easily done as follows:
 
 - The entries \f$(q_i,D_i)\f$ and \f$(q_j,D_j)\f$ are deleted.
 - A new entry \f$(q_{ij},D_{ij})\f$ for the joint value \f$x_{ij}\f$
   is added. This requires \f$O(CN)\f$ operations.
 - We test which among the other entries \f$(q_{k},D_{k})\f$ need to
   be updated. Recall that \f$(q_{k},D_{k})\f$ means that, before the
   merge, the value
   closest to \f$x_k\f$ was \f$x_{q_k}\f$ at a distance \f$D_k\f$. Then
   - If \f$q_k \not = i\f$, \f$q_k \not = j\f$ and \f$D_{k,ij} \geq D_k\f$, then
     \f$q_k\f$ is still the closest element and we do not do anything.
   - If \f$q_k \not = i\f$, \f$q_k \not = j\f$ and \f$D_{k,ij} <
     D_k\f$, then the closest element is \f$ij\f$ and we update the
     entry in constant time.
   - If \f$q_k = i\f$ or \f$q_k = j\f$, then we need to re-compute
     the closest element in \f$O(CN)\f$ operations.

  This algorithm requires only \f$O(N)\f$ space and \f$O(\gamma(N) C
  N^2)\f$ time, where \f$\gamma(N)\f$ is the expected number of times
  we fall in the last case. In common cases one has \f$\gamma(N)
  \approx \mathrm{const.}\f$, so the time saving is significant.
 
**/

#include "aib.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h> /* DBL_MAX */
#include <math.h>

/** @internal @brief The maximum value which beta may take */
#define BETA_MAX DBL_MAX

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief AIB algorithm data
 **
 ** The implementation is quite straightforward, but the way feature
 ** values are handled in order to support efficiently joins,
 ** deletions and re-arrangement needs to be explaiend. This is
 ** achieved by a layer of indrirection:
 ** - Call each feature value (either original or obtained by a join
 **   operation) <em>node</em>. Nodes are idenitfied by numbers.
 ** - Call the elements of various array (such as VlAIB::Px)
 **    <em>entry</em>.
 ** - Entry are dynamically associated to nodes as specified by
 **   VlAIB::nodes. So @c Px[i] actually refers to the node @c
 **   nodes[i].
 **/

typedef struct _VlAIB
{
  vl_aib_node   *nodes ;    /**< Entires to nodes */
  vl_aib_node    nentries ; /**< Total number of entries (= # active nodes) */
  vl_double     *beta ;     /**< Minimum distance to an entry  */
  vl_aib_node   *bidx ;     /**< Closest entry */


  vl_aib_node   *which ;    /**< List of entries to update */
  vl_aib_node    nwhich ;   /**< Number of entries to update */
  
  vl_aib_prob   *Pcx;       /**< Joint probability table */
  vl_aib_prob   *Px;        /**< Marginal. */
  vl_aib_prob   *Pc;        /**< Marginal. */
  vl_uint       nvalues;    /**< Number of feature values */
  vl_uint       nlabels;    /**< Number of labels */
} VlAIB;

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Normalizes an array of probabilities to sum to 1
 **
 ** @param P        The array of probabilities 
 ** @param nelem    The number of elements in the array
 **
 ** @return Modifies P to contain values which sum to 1
 **/

void vl_aib_normalize_P (vl_aib_prob * P, vl_aib_node nelem)
{
    vl_aib_node i;
    vl_aib_prob sum = 0;
    for(i=0; i<nelem; i++)
        sum += P[i];
    for(i=0; i<nelem; i++)
        P[i] /= sum;
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Allocates and creates a list of nodes
 **
 ** @param nentries   The size of the list which will be created 
 **
 ** @return an array containing elements 0...nentries
 **/

vl_aib_node *vl_aib_new_nodelist (vl_aib_node nentries)
{
    vl_aib_node * nodelist = vl_malloc(sizeof(vl_aib_node)*nentries);
    vl_aib_node n;
    for(n=0; n<nentries; n++)
        nodelist[n] = n;

    return nodelist;
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Allocates and creates the marginal distribution Px
 **
 ** @param Pcx   A two-dimensional array of probabilities
 ** @param nvalues The number of rows in Pcx
 ** @param nlabels The number of columns in Pcx
 **
 ** @return an array of size @a nvalues which contains the marginal
 **         distribution over the rows.
 **/

vl_aib_prob * vl_aib_new_Px(vl_aib_prob * Pcx, vl_aib_node nvalues, vl_aib_node nlabels)
{
    vl_aib_prob * Px = vl_malloc(sizeof(vl_aib_prob)*nvalues);
    vl_aib_node r,c;
    for(r=0; r<nvalues; r++)
    {
        vl_aib_prob sum = 0;
        for(c=0; c<nlabels; c++)
            sum += Pcx[r*nlabels+c];
        Px[r] = sum;
    }
    return Px;
}

/** ------------------------------------------------------------------
 ** @internal @brief Allocates and creates the marginal distribution Pc
 **
 ** @param Pcx      A two-dimensional array of probabilities
 ** @param nvalues    The number of rows in Pcx
 ** @param nlabels    The number of columns in Pcx
 **
 ** @return an array of size @a nlabels which contains the marginal distribution
 **         over the columns
 **/

vl_aib_prob * vl_aib_new_Pc(vl_aib_prob * Pcx, vl_aib_node nvalues, vl_aib_node nlabels)
{
    vl_aib_prob * Pc = vl_malloc(sizeof(vl_aib_prob)*nlabels);
    vl_aib_node r, c;
    for(c=0; c<nlabels; c++)
    {
        vl_aib_prob sum = 0;
        for(r=0; r<nvalues; r++)
            sum += Pcx[r*nlabels+c];
        Pc[c] = sum;
    }
    return Pc;
}

/** ------------------------------------------------------------------
 ** @internal @brief Find the two nodes which have minimum beta.
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
(VlAIB * aib, vl_aib_node * besti, vl_aib_node * bestj, vl_double * minbeta)
{
    vl_aib_node i;
    *minbeta = aib->beta[0];
    *besti   = 0;
    *bestj   = aib->bidx[0];

    for(i=0; i<aib->nentries; i++)
    {
        if(aib->beta[i] < *minbeta)
        {
            *minbeta = aib->beta[i];
            *besti = i;
            *bestj = aib->bidx[i];
        }
    }
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Merges two nodes i,j in the internal datastructure
 **
 ** @param aib  A pointer to the internal data structure
 ** @param i    The index of one member of the pair to merge
 ** @param j    The index of the other member of the pair to merge
 ** @param new  The index of the new node which corresponds to the union of 
 **             (@a i, @a j).
 **
 ** Nodes are merged by replacing the entry @a i with the union of @c
 ** ij, moving the node stored in last position (called @c lastnode)
 ** back to jth position and the entry at the end.
 **
 ** After the nodes have been merged, it updates which nodes should be
 ** considered on the next iteration based on which beta values could
 ** potentially change. The merged node will always be part of this
 ** list.
 **/

void 
vl_aib_merge_nodes (VlAIB * aib, vl_aib_node i, vl_aib_node j, vl_aib_node new)
{
  vl_aib_node last_entry = aib->nentries - 1 ;
  vl_aib_node c, n ;
  
  /* clear the list of nodes to update */
  aib->nwhich = 0;
  
  /* make sure that i is smaller than j */
  if(i > j) { vl_aib_node tmp = j; j = i; i = tmp; }
  
  /* -----------------------------------------------------------------
   *                    Merge entries i and j, storing the result in i
   * -------------------------------------------------------------- */

  aib-> Px   [i] += aib->Px[j] ; 
  aib-> beta [i]  = BETA_MAX ;
  aib-> nodes[i]  = new ; 

  for (c = 0; c < aib->nlabels; c++) 
    aib-> Pcx [i*aib->nlabels + c] += aib-> Pcx [j*aib->nlabels + c] ;
    
  /* -----------------------------------------------------------------
   *                                              Move last entry to j
   * -------------------------------------------------------------- */
  
  aib-> Px    [j]  = aib-> Px    [last_entry]; 
  aib-> beta  [j]  = aib-> beta  [last_entry];
  aib-> bidx  [j]  = aib-> bidx  [last_entry]; 
  aib-> nodes [j]  = aib-> nodes [last_entry];

  for (c = 0 ;  c < aib->nlabels ; c++)
    aib-> Pcx[j*aib->nlabels + c] = aib-> Pcx [last_entry*aib->nlabels + c] ;
  
  /* delete last entry */
  aib-> nentries -- ;
  
  /* -----------------------------------------------------------------
   *                                        Scan for entries to update
   * -------------------------------------------------------------- */

  /*
   * After mergin entries i and j, we need to update all other entries
   * that had one of these two as closest match. We also need to
   * update the renewend entry i. This is added by the loop below
   * since bidx [i] = j exactly because i was merged.
   *
   * Additionaly, since we moved the last entry back to the entry j,
   * we need to adjust the valeus of bidx to reflect this.
   */
  
  for (n = 0 ; n < aib->nentries; n++) {
    if(aib->bidx[n] == i || aib->bidx[n] == j) {
        aib->bidx  [n] = 0;
        aib->beta  [n] = BETA_MAX;
        aib->which [aib->nwhich++] = n ;
      }
    else if(aib->bidx[n] == last_entry) {
      aib->bidx[n] = j ;
    }
  }
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Updates @c aib->beta and @c aib->bidx according to @c aib->which
 **
 ** @param aib AIB data structure.
 **
 ** The function calculates @c beta[i] and @c bidx[i] for the nodes @c
 ** i listed in @c aib->which.  @c beta[i] is the minimal variation of mutual
 ** information (or other score) caused by merging entry @c i with another entry
 ** and @c bidx[i] is the index of this best matching entry.
 ** 
 ** Notice that for each entry @c i that we need to update, a full
 ** scan of all the other entries must be performed.
 **/

void 
vl_aib_update_beta (VlAIB * aib)
{

#define PLOGP(x) ((x)*log((x)))

  vl_aib_node i;
  vl_aib_prob * Px  = aib->Px;
  vl_aib_prob * Pcx = aib->Pcx;
  vl_aib_prob * tmp = vl_malloc(sizeof(vl_aib_prob)*aib->nentries);
  vl_aib_node a, b, c ; 
  
  /* 
   * T1 = I(x,c) - I([x]_ij) = A + B - C
   *
   * A  = \sum_c p(xa,c)           \log ( p(xa,c)          /  p(xa)       ) 
   * B  = \sum_c p(xb,c)           \log ( p(xb,c)          /  p(xb)       ) 
   * C  = \sum_c (p(xa,c)+p(xb,c)) \log ((p(xa,c)+p(xb,c)) / (p(xa)+p(xb)))
   *
   * C  = C1 + C2
   * C1 = \sum_c (p(xa,c)+p(xb,c)) \log (p(xa,c)+p(xb,c))
   * C2 = - (p(xa)+p(xb) \log (p(xa)+p(xb))
   */
 
  /* precalculate A and B */
  for (a = 0; a < aib->nentries; a++) {
    tmp[a] = 0;
    for (c = 0; c < aib->nlabels; c++) {
        vl_double Pac = Pcx [a*aib->nlabels + c] ;       
        if(Pac != 0) tmp[a] += Pac * log (Pac / Px[a]) ;
    }
  }
  
  /* for each entry listed in which */
  for (i = 0 ; i < aib->nwhich; i++) {    
    a = aib->which[i];

    /* for each other entry */
    for(b = 0 ; b < aib->nentries ; b++) {
      vl_double T1 = 0 ;
      
      if (a == b || Px [a] == 0 || Px [b] == 0) continue ;


      T1 = PLOGP ((Px[a] + Px[b])) ;                  /* - C2 */
      T1 += tmp[a] + tmp[b] ;                         /* + A + B */

      for (c = 0 ; c < aib->nlabels; ++ c) {
        vl_double Pac = Pcx [a*aib->nlabels + c] ;
        vl_double Pbc = Pcx [b*aib->nlabels + c] ;
        if (Pac == 0 && Pbc == 0) continue;
        T1 += - PLOGP ((Pac + Pbc)) ;                 /* - C1 */
      }
      
      /* 
       * Now we have beta(a,b). We check wether this is the best beta
       * for entries a and b.
       */
      {
        vl_double beta = T1 ;
        
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
  vl_free(tmp);
}

/** ------------------------------------------------------------------
 ** @internal @brief Calculates the current information and entropy
 **
 ** @param aib      A pointer to the internal data structure
 ** @param I        The current mutual information (out).
 ** @param H        The current entropy (out).
 **
 ** Calculates the current mutual information and entropy of Pcx and sets
 ** @a I and @a H to these new values.
 **/
void vl_aib_calculate_information(VlAIB * aib, vl_aib_prob * I, vl_aib_prob * H)
{
    vl_aib_node r, c;
    *H = 0;
    *I = 0;

    /* 
     * H(x)   = - sum_x p(x)    \ log p(x)
     * I(x,c) =   sum_xc p(x,c) \ log (p(x,c) / p(x)p(c))
     */
    
    /* for each entry */
    for(r = 0 ; r< aib->nentries ; r++) {
      
      if (aib->Px[r] == 0) continue ;
      *H += -log(aib->Px[r]) * aib->Px[r] ;
      
      for(c=0; c<aib->nlabels; c++) {
        if (aib->Pcx[r*aib->nlabels+c] == 0) continue;
        *I += aib->Pcx[r*aib->nlabels+c] * 
          log (aib->Pcx[r*aib->nlabels+c] / (aib->Px[r]*aib->Pc[c])) ;
      }
    }   
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Allocates and initializes the internal data structure
 **
 ** @param Pcx      A pointer to a 2D array of probabilities
 ** @param nvalues    The number of rows in the array     
 ** @param nlabels    The number of columns in the array
 **
 ** Creates a new @a VlAIB struct containing pointers to all the data that 
 ** will be used during the AIB process. 
 **
 ** Allocates memory for the following:
 ** - Px (nvalues*sizeof(vl_aib_prob))
 ** - Pc (nlabels*sizeof(vl_aib_prob))
 ** - nodelist (nvalues*sizeof(vl_aib_node))
 ** - which (nvalues*sizeof(vl_aib_node))
 ** - beta (nvalues*sizeof(vl_double))
 ** - bidx (nvalues*sizeof(vl_aib_node))
 **
 ** Since it simply copies to pointer to Pcx, the total additional memory
 ** requirement is: 
 ** (nvalues+nlabels)*sizeof(vl_aib_prob) + 3*nvalues*sizeof(vl_aib_prob) + 
 ** nvalues*sizeof(vl_double)
 **
 ** @returns An allocated and initialized @a VlAIB pointer
 **/
VlAIB * vl_aib_new_aib(vl_aib_prob * Pcx, vl_aib_node nvalues, vl_aib_node nlabels)
{
    VlAIB * aib = vl_malloc(sizeof(VlAIB));
    vl_aib_node i ;

    aib->Pcx   = Pcx ;
    aib->nvalues = nvalues ;
    aib->nlabels = nlabels ;

    vl_aib_normalize_P (aib->Pcx, aib->nvalues * aib->nlabels) ;

    aib->Px = vl_aib_new_Px (aib->Pcx, aib->nvalues, aib->nlabels) ;
    aib->Pc = vl_aib_new_Pc (aib->Pcx, aib->nvalues, aib->nlabels) ;

    aib->nentries = nvalues ;
    aib->nodes    = vl_aib_new_nodelist(aib->nentries) ;
    aib->beta     = vl_malloc(sizeof(vl_double) * aib->nentries) ;
    aib->bidx     = vl_malloc(sizeof(vl_aib_node)   * aib->nentries) ;

    for(i = 0 ; i < aib->nentries ; i++)
      aib->beta [i] = BETA_MAX ;
    
    /* Initially we must consider all nodes */
    aib->nwhich = nvalues;
    aib->which  = vl_aib_new_nodelist (aib->nwhich) ;
    
    return aib ;
}

/** ------------------------------------------------------------------
 ** @internal 
 ** @brief Deletes AIB data structure
 ** @param aib data structure to delete.
 **/

void 
vl_aib_delete_aib (VlAIB * aib)
{
  if (aib) {
    if (aib-> nodes) vl_free (aib-> nodes);
    if (aib-> beta)  vl_free (aib-> beta);
    if (aib-> bidx)  vl_free (aib-> bidx);
    if (aib-> which) vl_free (aib-> which);
    if (aib-> Px)    vl_free (aib-> Px);
    if (aib-> Pc)    vl_free (aib-> Pc);
  }
  vl_free (aib) ;
}

/** ------------------------------------------------------------------
 ** @brief Runs AIB on Pcx
 **
 ** @param Pcx     joint probability table (column major).
 ** @param nlabels number of rows in @a Pcx (number of labels).
 ** @param nvalues number of columns in @a Pcx (number of feature values).
 ** @param cost    cost of each node (out).
 **
 ** The function runs Agglomerative Information Bottleneck (AIB) on
 ** the joint probabilit talbe @a Pcx which has labels along the
 ** columns and feature valeus along the rows. AIB iteratively merges
 ** the two vlaues of the feature @c x that casuses the smallest
 ** decrease in mutual information between the random variables @c x
 ** and @c c.
 **
 ** Merge operations are arranged in a binary tree. The nodes of the
 ** tree correspond to the original feature values and any other value
 ** obtained as a result of a merge operation. The nodes are indexed
 ** in breadth-first order, starting from the leaves. The first index
 ** is zero. In this way, the leaves correspond directly to the
 ** original feature values.  In total there are @c 2*nvalues-1 nodes.
 **
 ** The function returns an array whit one element per tree node. Each
 ** element is the index the parent node. The root parent is equal to
 ** zero.
 **
 ** Feature values with null probability are ignored by the algorithm
 ** and their nodes have parents indexing a non-existant tree node (a
 ** value bigger than @c 2*nvalues-1).
 **
 ** If @a cost is not equal to NULL, then the function will also
 ** return a vector with the information level after each merge. @a
 ** cost has @c nvalues entries: The first is the value of the cost
 ** funcitonal before any merge, and the other are the cost after the
 ** @c nvalues-1 merges.
 ** 
 ** @return the array of parents representing the tree. The array has
 ** @c 2*nvalues-1 elements.
 **/ 

vl_aib_node *
vl_aib (vl_aib_prob * Pcx, vl_uint nlabels, vl_uint nvalues,
        double ** cost)
{
  vl_aib_node * parents = vl_malloc(sizeof(vl_aib_node)*(nvalues*2-1));
  vl_aib_node n;
  
  /* Initially, all parents point to a nonexistant node */
  for (n = 0 ; n < 2 * nvalues - 1 ; n++)
    parents [n] = 2 * nvalues ; 

  /* Allocate cost outut vector */
  if (cost) *cost = vl_malloc (sizeof(double) * (nvalues - 1 + 1)) ;
  
  {
    VlAIB * aib = vl_aib_new_aib (Pcx, nvalues, nlabels) ;    
    vl_aib_node i, besti, bestj, newnode, nodei, nodej;
    vl_aib_prob I, H;
    vl_double minbeta;

    /* Caluclate initial value of cost functiion */
    vl_aib_calculate_information (aib, &I, &H) ;
    if (cost) (*cost)[0] = I ;
    
    /* Initially which = all */
    
    /* For each merge */
    for(i = 0 ; i < nvalues - 1 ; i++) {
      
      /* update entries in aib-> which */
      vl_aib_update_beta(aib);
      
      /* find best pair of nodes to merge */
      vl_aib_min_beta (aib, &besti, &bestj, &minbeta);

      if(minbeta == BETA_MAX)
        /* only null-probability entries remain */
        break;
      
      /* Add the parent pointers for the new node */
      newnode = nvalues + i ;
      nodei = aib->nodes[besti];
      nodej = aib->nodes[bestj];

      parents [nodei] = newnode ;
      parents [nodej] = newnode ;
      parents [newnode] = 0 ;
      
      /* Merge the nodes which produced the minimum beta */
      vl_aib_merge_nodes (aib, besti, bestj, newnode) ;
      vl_aib_calculate_information (aib, &I, &H) ;

      if (cost && *cost) {
        (*cost) [i+1] = I ;
      }
      
      VL_PRINTF ("aib: (%5d,%5d)=%5d dE: %10.3g I: %6.4g H: %6.4g updt: %5d\n", 
                 nodei, 
                 nodej, 
                 newnode,                  
                 minbeta,
                 I,
                 H,
                 aib->nwhich) ;
    }

    /* fill ignored entries with NaNs */
    for(; i < nvalues - 1 ; i++) {
      if (cost && *cost) {
        (*cost) [i+1] = VL_NAN_D ;
      }
    }

    vl_aib_delete_aib(aib) ;
  }  
  return parents ;
} 
