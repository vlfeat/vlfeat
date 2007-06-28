/** @file     mser.c
 ** @brief    Maximally Stable Extremal Regions (MSER) - Definition
 ** @author   Andrea Vedaldi
 ** @internal
 **/

/* AUTORIGHTS */

#include "mser.h"

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<assert.h>

/** @brief Maximum value
 **
 ** Maximum value of the integer type ::vl_mser_pix.
 **/
#define VL_MSER_PIX_MAXVAL 256

/** @brief MSER accumulator data type 
 **
 ** This is a large integer type. It should be large enough to contain
 ** a number equal to the area (volume) of the image by the image
 ** widht by the image height (for instance, if the image is a square
 ** of side 256, the maximum value is 256 x 256 x 256).
 **/
typedef vl_single vl_mser_acc ;

/** @brief Basic region flag: null region */
#define VL_MSER_VOID_NODE ((1ULL<<32) - 1)

/* ----------------------------------------------------------------- */
/** @brief MSER: basic region
 **
 ** Extremal regions and maximally stable extremal regions are
 ** instances of image regions.
 **
 ** There is an image region for each pixel of the image. Each region
 ** is represented by an instance of this structure.  Regions are
 ** stored into an array in pixel order.
 **
 ** Regions are arranged into a forest. VlMserReg::parent points to
 ** the parent node, or to the node itself if the node is a root.
 ** VlMserReg::parent is the index of the node in the node array
 ** (which therefore is also the index of the corresponding
 ** pixel). VlMserReg::height is the distance of the fartest leaf. If
 ** the node itself is a leaf, then VlMserReg::height is zero.
 **
 ** VlMserReg::area is the area of the image region corresponding to
 ** this node.
 **
 ** VlMserReg::region is the extremal region identifier. Not all
 ** regions are extremal regions however; if the region is NOT
 ** extremal, this field is set to .... 
 **/

typedef struct _VlMserReg
{
  vl_uint parent ;   /**< points to the parent region.            */
  vl_uint shortcut ; /**< points to a region closer to a root.    */
  vl_uint height ;   /**< region height in the forest.            */
  vl_uint area ;     /**< area of the region.                     */
} VlMserReg ;

/* ----------------------------------------------------------------- */
/** @brief MSER: extremal region
 **
 ** Extermal regions (ER) are extracted from the region forest. Each
 ** region is represented by an instance of this structure. The
 ** structures are stored into an array, in arbitrary order.
 **
 ** ER are arranged into a tree. @a parent points to the parent ER, or
 ** to iself if the ER is the root.
 **
 ** The image region corresponding to an ER is identified to a single
 ** pixel, whose index is VlMserExtrReg::value. The region is the maximal
 ** connected component containing that pixel and all the pixel with
 ** intensity lower or equal to it.
 **
 ** VlMserExtrReg::area, VlMserExtrReg::area_top and
 ** VlMserExtrReg::area_bot are the areas of the region, the region
 ** `above' and the region 'below' respectively.  
 **
 ** VlMserExtrReg::variation is the relative area variation.
 **
 ** VlMserExtrReg::max_stable is the identifier of the corresponding
 ** Maxmally Stable Extremal Region (MSER). However not all ER are
 ** MSER too; in this case this value is equal to zero.
 **/

typedef struct _VlMserExtrReg
{
  int          parent ;     /**< index of the parent region                   */
  int          index ;      /**< index of pivot pixel                         */
  vl_mser_pix  value ;      /**< value of pivot pixel                         */
  vl_uint      area ;       /**< area of the region                           */
  vl_uint      area_top ;   /**< area of the region DELTA levels above        */
  vl_uint      area_bot  ;  /**< area of the region DELTA levels below        */
  vl_single    variation ;  /**< area variation                               */
  vl_uint      max_stable ; /**< max stable number (=0 if not maxstable)      */
} VlMserExtrReg ;

/* ----------------------------------------------------------------- */
/** @brief MSER filter
 **
 ** The MSER filter computes the Maximally Stable Extremal Regions of
 ** an image.
 **
 ** The filter is designed as an <em>opaque</em> object. As such, one
 ** should avoid accessing directly the data fields but use the
 ** appropriate functions instead.
 **
 ** An image is a N-dimensional array of integers (of class
 ** ::vl_mser_pix). Given the image geometry (number of dimensions and
 ** dimensions), a MSER filter is created by ::vl_mser_new().  The
 ** regions are extracted from the image data by ::vl_mser_process().
 **
 ** Once computed, the number of MSERs found is returned by
 ** ::vl_mser_get_num_regions(). The actual regions can be obtained by
 ** means of ::vl_mser_get_regions().
 **
 ** Optionally, it is possible to fit N-dimensional ellipsoids to the
 ** MSERs by calling ::vl_mser_fit_ell(). The number of ellipsoids @c
 ** K fitted can be retrieved by ::vl_mser_get_num_ell(). The number
 ** of degrees of freedom @c D of an N-dimensional ellipsoids is
 ** returned by ::vl_mser_get_dof_ell().
 **
 ** A filter should be disposed by ::vl_mser_delete().
 **
 ** @section internals Internals
 **
 ** This filter implements the MSER detector.
 **/

struct _VlMserFilt
{  
  
  /** @name Image data and meta data */
  /*@{*/
  int                ndims ;   /**< number of dimensions                    */
  int               *dims ;    /**< dimensions                              */
  int                nel ;     /**< number of image elements (pixels)       */
  int               *subs ;    /**< N-dimensional subscript                 */
  int               *dsubs ;   /**< another subscript                       */
  int               *strides ; /**< strides to move in image data           */
  /*@}*/

  vl_uint           *perm ;    /**< pixel ordering                          */
  vl_uint           *joins ;   /**< sequence of join ops                    */
  int                njoins ;  /**< number of join ops                      */

  /** @name Regions */
  /*@{*/
  VlMserReg         *r ;       /**< basic regions                           */
  VlMserExtrReg     *er ;      /**< extremal tree                           */
  vl_uint           *mer ;     /**< maximally stalbe extremal regions       */
  int                ner ;     /**< number of extremal regions              */
  int                nmer ;    /**< number of maximally stable extr. reg.   */
  /*@}*/

  /** @name Ellipsoids fitting */
  /*@{*/
  vl_single         *acc ;     /**< moment accumulator.                    */
  vl_single         *ell ;     /**< ellipsoids list.                       */
  int                ell_sz ;  /**< reserved space in ell. list            */
  int                nell ;    /**< number of ellipsoids extracted         */
  int                dof ;     /**< number of dof of ellipsoids.           */

  /*@}*/

  /** @name Configuration */
  /*@{*/
  vl_bool   verbose ;          /**< be verbose                             */
  vl_bool   cleanup_big ;      /**< cleanup regions too big                */
  vl_bool   cleanup_small ;    /**< cleanup regions too small              */
  vl_bool   cleanup_bad ;      /**< cleanup regions too bad                */
  vl_bool   cleanup_dup ;      /**< cleanup regions too similar to others  */
  int       delta ;            /**< DELTA                                  */
  /*@}*/
} ;


/* ----------------------------------------------------------------- */
/** @brief Advance N-dimensional subscript
 **
 ** The function increments by one the subscript @a subs indexing an
 ** array the @a ndims dimensions @a dims.
 **
 ** @param ndims number of dimensions. 
 ** @param dims dimensions.
 ** @param subs subscript to advance.
 **/

void
adv(int ndims, int const *dims, int *subs)
{
  int d = 0 ;
  while(d < ndims) {
    if( ++subs[d]  < dims[d] ) return ;
    subs[d++] = 0 ;
  }
}

/* ----------------------------------------------------------------- */
/** @brief Climb the region forest to reach aa root 
 **
 ** The function climbs the regions forest @a r starring from the node
 ** @a idx to the correspoding root.
 **
 ** To speed-up the operation, the function uses the
 ** VlMserReg::shortcut field to quickly jump to the root. After the
 ** root is reached, all the used shortcut are updated.
 **
 ** @param r regions' forest.
 ** @param idx stating node.
 ** @return index of the reached root.
 **/

VL_INLINE
vl_uint
climb (VlMserReg* r, vl_uint idx) 
{
  
  vl_uint prev_idx = idx ;
  vl_uint next_idx ;
  vl_uint root_idx ;

  /* move towards root to find it */
  while (1) {
    
    /* next jump to the root */
    next_idx = r [idx] .shortcut ;
    
    /* recycle shortcut to remember how we came here */
    r [idx] .shortcut = prev_idx ;
    
    /* stop if the root is found */
    if( next_idx == idx ) break ;

    /* next guy */
    prev_idx = idx ;
    idx      = next_idx ;
  }

  root_idx = idx ;

  /* move backward to update shortcuts */
  while (1) {
    
    /* get previously visited one */
    prev_idx = r [idx] .shortcut ;
    
    /* update shortcut to point to the new root */
    r [idx] .shortcut = root_idx ;

    /* stop if the first visited node is reached */
    if( prev_idx == idx ) break ;
    
    /* next guy */
    idx = prev_idx ;
  }

  return root_idx ;
}        

/* ----------------------------------------------------------------- */
/** @brief Create a new MSER filter
 **
 ** Initializes a new MSER filter for images of the specified
 ** dimensions. Images are @a ndims -dimensional arrays of dimensions
 ** @a dims.
 **
 ** @param ndims number of dimensions.
 ** @param dims  dimensions.
 **/
VlMserFilt*
vl_mser_new (int ndims, int const* dims)
{
  VlMserFilt* f ;
  int *strides, k ;

  f = calloc(sizeof(VlMserFilt), 1) ;

  f-> ndims   = ndims ;
  f-> dims    = malloc( sizeof(int) * ndims ) ;
  f-> subs    = malloc( sizeof(int) * ndims ) ;
  f-> dsubs   = malloc( sizeof(int) * ndims ) ;
  f-> strides = malloc( sizeof(int) * ndims ) ;

  /* shortcuts */
  strides = f-> strides ;

  /* copy dims to f->dims */
  for(k = 0 ; k < ndims ; ++k) {
    f-> dims [k] = dims [k] ;
  }
  
  /* compute strides to move into the N-dimensional image array */
  strides [0] = 1 ;
  for(k = 1 ; k < ndims ; ++k) {
    strides [k] = strides [k-1] * dims [k-1] ;
  }
  
  /* total number of pixels */
  f-> nel = strides [ndims-1] * dims [ndims-1] ;
  
  /* dof of ellipsoids */
  f-> dof = ndims * (ndims + 1) / 2 + ndims ;

  /* more buffers */
  f-> perm   = malloc( sizeof(vl_uint)   * f-> nel ) ;
  f-> joins  = malloc( sizeof(vl_uint)   * f-> nel ) ;
  f-> r      = malloc( sizeof(VlMserReg) * f-> nel ) ;


  /* other parameters */
  f-> verbose = 1 ;
  f-> cleanup_small = 1 ;
  f-> cleanup_big   = 1 ;
  f-> cleanup_bad   = 1 ;
  f-> cleanup_dup   = 1 ;
  f-> delta         = 5 ;

  return f ;
}

/* ----------------------------------------------------------------- */
/** @brief Delete MSER filter
 **
 ** The function releases the MSER filter @a f and all its resources.
 **
 ** @param f MSER filter to be deleted.
 **/
void
vl_mser_delete (VlMserFilt* f)
{
  if(f) {
    if(f-> acc   )  free( f-> acc    ) ;
    if(f-> ell   )  free( f-> ell    ) ;

    if(f-> er    )  free( f-> er     ) ;
    if(f-> r     )  free( f-> r      ) ;
    if(f-> joins )  free( f-> joins  ) ;
    if(f-> perm  )  free( f-> perm   ) ;
    
    if(f-> strides) free( f-> strides) ;
    if(f-> dsubs  ) free( f-> dsubs  ) ;
    if(f-> subs   ) free( f-> subs   ) ;
    if(f-> dims   ) free( f-> dims   ) ;
    free(f) ;
  }
}

/* ----------------------------------------------------------------- */
/** @brief Get maximally stable extremal regions
 **
 ** A region is represented by the index of the corresponding pivot
 ** pixel.
 **
 ** Therefore the list of regions is an array of
 ** vl_mser_get_num_regions() indeces.
 **
 ** @param f MSER filter.
 **/
vl_uint const *
vl_mser_get_regions (VlMserFilt const* f)
{
  return f-> mer ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of maximally stable extremal regions
 ** @param f MSER filter.
 **/
vl_uint
vl_mser_get_num_regions (VlMserFilt const* f)
{
  return f-> nmer ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of degrees of freedom of ellipsoids
 ** @param f MSER filter.
 **/
vl_uint
vl_mser_get_dof_ell (VlMserFilt const* f)
{
  return f-> dof ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of fitted ellipses
 ** @param f MSER filter.
 **/
vl_uint
vl_mser_get_num_ell (VlMserFilt const* f)
{
  return f-> nell ;
}

/* ----------------------------------------------------------------- */
/** @brief Get fitted ellipses
 ** @param f MSER filter.
 **/
vl_single const *
vl_mser_get_ell (VlMserFilt const* f)
{
  return f-> ell ;
}


/* ----------------------------------------------------------------- */
/** @brief Process image
 ** 
 ** The functions calculates the Maximally Stable Extremal Regions
 ** (MSERs) of image @a im using the MSER filter @a f.
 **
 ** The filter @a f must have been initialized to be compatible with
 ** the dimensions of @a im.
 **
 ** @param f MSER filter.
 ** @param im image data.
 **/
void
vl_mser_process (VlMserFilt* f, vl_mser_pix const* im)
{
  /* shortcuts */
  vl_bool        verbose = f-> verbose ;
  vl_uint        nel     = f-> nel  ;
  vl_uint       *perm    = f-> perm ;
  vl_uint       *joins   = f-> joins ;
  int            ndims   = f-> ndims ;
  int           *dims    = f-> dims ;
  int           *subs    = f-> subs ;
  int           *dsubs   = f-> dsubs ;
  int           *strides = f-> strides ;
  VlMserReg     *r       = f-> r ;
  VlMserExtrReg *er      = f-> er ;
  vl_uint       *mer     = f-> mer ;
  int            delta   = f-> delta ;
 
  int i, j, k, idx ;
  int njoins = 0 ;
  int ner    = 0 ;
  int nmer   = 0 ;

  /* delete any previosuly computed ellipsoid */
  f-> nell = 0 ;

  /* -----------------------------------------------------------------
   *                                          Sort pixels by intensity
   * -------------------------------------------------------------- */
  
  verbose && printf("mser: sorting image pixels ...") ;  

  {
    vl_uint buckets [ VL_MSER_PIX_MAXVAL ] ;

    /* clear buckets */
    bzero(buckets, sizeof(vl_uint) * VL_MSER_PIX_MAXVAL ) ;

    /* compute bucket size (how many pixels for each intensity
       value) */
    for(i = 0 ; i < nel ; ++i) {
      vl_mser_pix v = im [i] ;
      ++ buckets [v] ;
    }

    /* cumulatively add bucket sizes */
    for(i = 1 ; i < VL_MSER_PIX_MAXVAL ; ++i) {
      buckets [i] += buckets [i-1] ;
    }
    
    /* empty buckets computing pixel ordering */
    for(i = nel ; i >= 1 ; ) {
      vl_mser_pix v = im [ --i ] ;
      vl_uint j = -- buckets [v] ;
      perm [j] = i ;
    }
  }

  verbose && printf(" done\n") ;

  /* initialize the forest with all void nodes */
  for(i = 0 ; i < nel ; ++i) {
    r [i] .parent = VL_MSER_VOID_NODE ;
  }

  /* -----------------------------------------------------------------
   *                        Compute regions and count extremal regions
   * -------------------------------------------------------------- */
  /* 
     In the following:

     idx    : index of the current pixel
     val    : intensity of the current pixel
     r_idx  : index of the root of the current pixel
     n_idx  : index of the neighbors of the current pixel
     nr_idx : index of the root of the neighbor of the current pixel

  */

  verbose && printf("mser: computing basic regions ... ") ;
  
  /* process each pixel by increasing intensity */
  for(i = 0 ; i < nel ; ++i) {
    
    /* pop next node xi */
    vl_uint     idx = perm [i] ;  
    vl_mser_pix val = im [idx] ;
    vl_uint     r_idx ;
    
    /* add the pixel to the forest as a root for now */
    r [idx] .parent   = idx ;
    r [idx] .shortcut = idx ;
    r [idx] .area     = 1 ;
    r [idx] .height   = 1 ;

    r_idx = idx ;
   
    /* convert the index IDX into the subscript SUBS; also initialize
       DSUBS to (-1,-1,...,-1) */
    {
      vl_uint temp = idx ;
      for(k = ndims - 1 ; k >= 0 ; --k) {
        dsubs [k] = -1 ;
        subs  [k] = temp / strides [k] ;
        temp      = temp % strides [k] ;
      }
    }
    
    /* examine the neighbors of the current pixel */
    while (1) {
      vl_uint n_idx = 0 ;
      vl_bool good = 1 ;

      /* 
         Compute the neighbor subscript as NSUBS+SUB, the
         correspoinding neighbor index NINDEX and check that the
         neighbor is within the image domain.
      */
      for(k = 0 ; k < ndims && good ; ++k) {
        int temp  = dsubs [k] + subs [k] ;
        good     &= (0 <= temp) && (temp < dims [k]) ;
        n_idx    += temp * strides [k] ;
      }

      /* 
         The neighbor should be processed if the following conditions
         are met:

         1. The neighbor is within image boundaries.

         2. The neighbor is indeed different from the current node
            (the opposite happens when DSUB=(0,0,...,0)).

         3. The nieghbor is already in the forest, meaning that it has
            already been processed.
      */
      if (good && 
          n_idx != idx &&
          r [n_idx] .parent != VL_MSER_VOID_NODE ) {

        vl_mser_pix nr_val = 0 ;
        vl_uint     nr_idx = 0 ;
        int         hgt   = r [ r_idx] .height ;
        int         n_hgt = r [nr_idx] .height ;
        
        /*
          Now we join the two subtrees rooted at
          
           R_IDX = ROOT(  IDX) 
          NR_IDX = ROOT(N_IDX).
          
          Note that R_IDX = ROOT(IDX) might change as we process more
          neighbors, so we need keep updating it. 
        */
        
         r_idx = climb(r,   idx) ;
        nr_idx = climb(r, n_idx) ;
        
        /*  
          At this point we have three possibilities:
          
          (A) ROOT(IDX) == ROOT(NR_IDX). In this case the two trees
              have already been joined and we do not do anything.
          
          (B) I(ROOT(IDX)) == I(ROOT(NR_IDX)). In this case the pixel
              IDX is extending an extremal region with the same
              intensity value. Since ROOT(NR_IDX) will NOT be an
              extremal region of the full image, ROOT(IDX) can be
              safely addedd as children of ROOT(NR_IDX) if this
              reduces the height according to the union rank
              heuristic.
               
          (C) I(ROOT(IDX)) > I(ROOT(NR_IDX)). In this case the pixel
              IDX is starting a new extremal region. Thus ROOT(NR_IDX)
              WILL be an extremal region of the final image and the
              only possibility is to add ROOT(NR_IDX) as children of
              ROOT(IDX), which becomes parent.
        */

        if( r_idx != nr_idx ) { /* skip if (A) */

          nr_val = im [nr_idx] ;

          if( nr_val == val && hgt < n_hgt ) {

            /* ROOT(IDX) becomes the child */
            r [r_idx]  .parent   = nr_idx ;
            r [r_idx]  .shortcut = nr_idx ;          
            r [nr_idx] .area    += r [r_idx] .area ;
            r [nr_idx] .height   = VL_MAX(n_hgt, hgt+1) ;

            joins [njoins++] = r_idx ;

          } else {

            /* cases ROOT(IDX) becomes the parent */
            r [nr_idx] .parent   = r_idx ;
            r [nr_idx] .shortcut = r_idx ;
            r [r_idx]  .area    += r [nr_idx] .area ;
            r [r_idx]  .height   = VL_MAX(hgt, n_hgt+1) ;

            joins [njoins++] = nr_idx ; 

            /* count if extremal */
            if(nr_val != val) ++ ner ;

          } /* check b vs c */
        } /* check a vs b or c */        
      } /* neighbor done */

      /* move to next neighbor */      
      k = 0 ;
      while(++ dsubs [k] > 1) {
        dsubs [k++] = -1 ;
        if(k == ndims) goto done_all_neighbors ;
      }
    } /* next neighbor */
  done_all_neighbors : ;
  } /* next pixel */    

  /* the last root is extremal too */
  ++ ner ;

  /* save back */
  f-> njoins = njoins ;

  verbose && printf("done\n") ;

  /* -----------------------------------------------------------------
   *                                          Extract extremal regions
   * -------------------------------------------------------------- */

  /* 
     Extremal regions are extracted and stored into the array ER.  The
     structure R is also updated so that .SHORTCUT index the
     correspoinding extremal region if any (otherwise it is set to
     VOID).
  */
  
  verbose && printf("mser: computing extremal regions ...") ;

  /* make room */
  if( f-> ner < ner ) {
    if(er) free(er) ;
    f->er  = er = malloc( sizeof(VlMserExtrReg) * ner) ;
    f->ner = ner ;      
  } ;

  /* count again */
  ner = 0 ;

  /* scan all regions Xi */
  for(idx = 0 ; idx < nel ; ++idx) {

    vl_mser_pix val   = im [idx] ;
    vl_uint     p_idx = r  [idx] .parent ;
    vl_mser_pix p_val = im [p_idx] ;

    /* is extremal ? */
    vl_bool is_extr = (p_val > val) || idx == p_idx ;
    
    if( is_extr ) {

      /* if so, add it */      
      er [ner] .index      = idx ;
      er [ner] .parent     = ner ;
      er [ner] .value      = im [idx] ;
      er [ner] .area       = r  [idx] .area ;
      er [ner] .area_top   = nel ;
      er [ner] .area_bot   = 0 ;
      
      /* link this region to this extremal region */
      r [idx] .shortcut = ner ;

      /* increase count */
      ++ ner ;
    } else {      
      /* link this region to void */
      r [idx] .shortcut =   VL_MSER_VOID_NODE ;
    }
  }

  /* save back */
  f-> ner    = ner ;

  verbose && printf("done (%d found)\n", ner) ;

  /* -----------------------------------------------------------------
   *                                   Link extremal regions in a tree
   * -------------------------------------------------------------- */

  for(i = 0 ; i < ner ; ++i) {

    vl_uint idx = er [i] .index ;

    do {
      idx = r [idx] .parent ;
    } while ( r [idx] .shortcut == VL_MSER_VOID_NODE ) ;

    er [i] .parent = r [idx] .shortcut ;
  }

  /* -----------------------------------------------------------------
   *                                 Compute areas of tops and bottoms
   * -------------------------------------------------------------- */

  /* 
     Now we have computed the list of extremal regions. We are left
     with computing the stability score. To this end, we need to
     estimate the area variation as the region intesity level is
     varied by +DELTA or -DELTA.

     For each region Xi, we call BOTTOM region any extremal region
     that is contained in Xi and has intenisty immediately below
     DELTA; similarly the TOP region is the extremal region that
     contains Xi and has intensity immediately above DELTA.

     For each region Xi, we compute the (cumlative) area of the BOTTOM
     regions and of the top region.

     To this  end, let Xj be  an extremal region and  denote X_{j+1} =
     PARENT(Xj),  X_{j+2}  =  PARENT(X_{j+1})  the parents.  For  each
     region Xj we
          
     1) Look for regions Xj for which X0 is the BOTTOM. This requires
        VAL(X0) <= VAL(Xj) - DELTA < VAL(X1).  We update AREA_BOT(Xj)
        for each of such Xj found.
        
     2) Look for the region Xi which is the TOP of Xj. This requires
        VAL(Xi) <= VAL(Xj) + DELTA < VAL(X_{i+1}) We update
        AREA_TOP(X0) as soon as we find such Xi.
  */

  verbose && printf("mser: computing area variation ...") ;

  for(i = 0 ; i < ner ; ++i) {

    /* Xj is the current region the region and Xj are the parents */
    vl_uint parent = er [i] .parent ;
    int     val0   = er [i] .value ;
    int     val1   = er [parent] .value ;
    int     val    = val0 ;
    
    j = i ;

    /* examine all parents */
    while (1) {
      int valp = er [parent] .value ;

      /* Xi is the bottom of Xj */
      if(val0 <= val - delta && val - delta < val1) {
        er [j] .area_bot  =
          VL_MAX(er [j] .area_bot, 
                 er [i] .area) ;
      }
      
      /* Xj is the top of Xi */
      if(val <= val0 + delta && val0 + delta < valp) {
        er [i] .area_top = er [j].area ;
      }
      
      /* stop if going on is useless */
      if(val1 <= val - delta && val0 + delta < val)
        break ;

      /* stop also if j is the root */
      if(j == parent)
        break ;
      
      /* next region upward */
      j      = parent ;
      parent = er [j] .parent ;
      val    = valp ;
    }
  }

  /* -----------------------------------------------------------------
   *                                                 Compute variation
   * -------------------------------------------------------------- */

  for(i = 0 ; i < ner ; ++i) {
    int area     = er [i] .area ;
    int area_top = er [i] .area_top ;
    int area_bot = er [i] .area_bot ;    
    er [i] .variation = 
      (float)(area_top - area_bot) / (float)area ;

    /* assume all regions are maximally stable */
    er [i] .max_stable = 1 ;
  }

  verbose && printf(" done.\n") ;

  /* -----------------------------------------------------------------
   *                     Remove regions which are NOT maximally stable
   * -------------------------------------------------------------- */

  verbose && printf("mser: selecting maximally stable ...") ;

  nmer = ner ;
  for(i = 0 ; i < ner ; ++i) {
    vl_uint   parent = er [i]      .parent ;
    vl_single   var  = er [i]      .variation ;
    vl_single  pvar  = er [parent] .variation ;
    vl_uint   loser ;

    /* decide which one to keep and put that in loser */
    if(var < pvar) loser = parent ; else loser = i ;
    
    /* make loser NON maximally stable */
    if(er [loser] .max_stable) -- nmer ;
    er [loser] .max_stable = 0 ;
  }

  verbose && printf("done (%d left, %.1f%%)\n", 
                    nmer, 100.0 * (double) nmer / ner) ;

  /* -----------------------------------------------------------------
   *                                                 Further filtering
   * -------------------------------------------------------------- */

  /* 
     It is critical for correct duplicate detection to remove regions
     from the bottom (smallest one first).
  */

  if( f-> cleanup_big   || 
      f-> cleanup_small ||
      f-> cleanup_bad   ||
      f-> cleanup_dup   ) {

    int nbig   = 0 ;
    int nsmall = 0 ;
    int nbad   = 0 ;
    int ndup   = 0 ;

    /* scann all extremal regions */
    for(i = 0L ; i < ner ; ++i) {
      
      /* process only maximally stable extremal regions */
      if (! er [i] .max_stable) continue ;
      
      if (f->cleanup_bad   && er [i] .variation >= 1.0f ) {
        ++ nbad ;
        goto remove_this_region ;
      }

      if (f->cleanup_big   && er [i] .area      >  nel/2) {
        ++ nbig ;
        goto remove_this_region ;
      }

      if (f->cleanup_small && er [i] .area      <  25   ) {
        ++ nsmall ;
        goto remove_this_region ;
      }
      
      /* 
       * Remove duplicates 
       */
      if (f->cleanup_dup) {
        vl_uint parent = er [i] .parent ;
        int area, p_area ;
        float change ;
        
        /* check all but the root mser */
        if(parent != i) {
          
          /* search for the maximally stable parent region */
          while(! er [parent] .max_stable) {
            vl_uint next = er [parent] .parent ;
            if(next == parent) break ;
            parent = next ;
          }
          
          /* compare with the parent region; if the current and parent
             regions are too similar, keep only the parent */
          area    = er [i]      .area ;
          p_area  = er [parent] .area ;
          change  = (float)(p_area - area) / area ;

          if(change < 0.5)  {
            ++ ndup ;
            goto remove_this_region ;
          }
          
        } /* drop duplicates */ 
      }
      continue ;
    remove_this_region :
      er [i] .max_stable = 0 ;
      -- nmer ;      
    } /* next region to cleanup */
    
    if(verbose) {
      printf("mser:  bad regions:        %d\n", nbad   ) ;
      printf("mser:  small regions:      %d\n", nsmall ) ;
      printf("mser:  big regions:        %d\n", nbig   ) ;
      printf("mser:  duplicated regions: %d\n", ndup   ) ;
    }
  }

  verbose && printf("mser: cleaned-up regions: %d (%.1f%%)\n", 
                    nmer, 100.0 * (double) nmer / ner) ;


  /* -----------------------------------------------------------------
   *                                                   Save the result
   * -------------------------------------------------------------- */

  /* make room */
  if( f-> nmer < nmer ) {
    if(mer) free(mer) ;
    f->mer  = mer = malloc( sizeof(vl_uint) * nmer) ;
    f->nmer = nmer ;      
  }

  /* save back */
  f-> nmer = nmer ;

  j = 0 ;
  for (i = 0 ; i < ner ; ++i) {
    if (er [i] .max_stable) mer [j++] = er [i] .index ;
  }
}

/* ----------------------------------------------------------------- */
/** @brief Fit ellipsoids
 **
 ** The function fits ellipsoids to the extracted maximally stable
 ** extremal regions. It should be called after compuint the maximally
 ** stable extremal regions by :: vl_mser_process() and before
 ** retrieving the ellipsoids by means of ::vl_mser_get_ell().
 **/

void
vl_mser_fit_ell (VlMserFilt* f)
{
  /* shortcuts */
  int                nel = f-> nel ;
  int                dof = f-> dof ;
  int              *dims = f-> dims ;
  int              ndims = f-> ndims ;
  int              *subs = f-> subs ;
  int             njoins = f-> njoins ;
  vl_uint         *joins = f-> joins ;
  VlMserReg           *r = f-> r ;
  vl_uint           *mer = f-> mer ;
  int               nmer = f-> nmer ;
  vl_mser_acc       *acc = f-> acc ;
  vl_mser_acc       *ell = f-> ell ; 
  vl_bool        verbose = f-> verbose ;

  int d, index, i, j ;
  
  /* already fit ? */
  if (f->nell == f->nmer) return ;
  
  /* make room */
  if (f->ell_sz < f->nmer) {
    if (f->ell) free(f->ell) ;
    f->ell    = malloc(sizeof(vl_single) * f->nmer * f->dof) ;
    f->ell_sz = f->nmer ;
  }

  if (f->acc == 0) {
    f->acc = malloc(sizeof(vl_single) * f->nel) ;
  }
 
  acc = f-> acc ;
  ell = f-> ell ; 
     
  /* -----------------------------------------------------------------
   *                                                 Integrate moments
   * -------------------------------------------------------------- */
    
  verbose && printf("mser: fitting ellipsoids ...\n") ;
  
  /* for each dof */
  for(d = 0 ; d < f->dof ; ++d) {

    /* start from the upper-left pixel (0,0,...,0) */
    bzero(subs, sizeof(int) * ndims) ;
        
    /* step 1: fill acc pretending that each region has only one pixel */
    if(d < ndims) {
      /* 1-order ................................................... */
      verbose && printf("mser:    E[x_%d]\n",d) ;
      for(index = 0 ; index < nel ; ++ index) {
        acc [index] = subs [d] ;
        adv(ndims, dims, subs) ;
      }      
    }
    else {      
      /* 2-order ................................................... */

      /* map the dof d to a second order moment E[x_i x_j] */      
      i = d - ndims ; 
      j = 0 ;
      while(i > j) {
        i -= j + 1 ;
        j ++ ;
      }      
      verbose && printf("mser:   E[x_%d x_%d]\n",i,j) ;

      /* initialize acc with  x_i * x_j */
      for(index = 0 ; index < nel ; ++ index){
        acc [index] = subs [i] * subs [j] ;
        adv(ndims, dims, subs) ;
      }
    }
        
    /* step 2: integrate */
    for(i = 0 ; i < njoins ; ++i) {      
      vl_uint index  = joins [i] ;
      vl_uint parent = r [index] .parent ;
      acc [parent] += acc [index] ;
    }
    
    /* step 3: save back to ellpises */
    for(i = 0 ; i < nmer ; ++i) {      
      vl_uint idx = mer [i] ;
      ell [d + dof*i] = acc [idx] ;
    }
    
  }  /* next dof */

  /* -----------------------------------------------------------------
   *                                           Compute central moments
   * -------------------------------------------------------------- */

  for(index = 0 ; index < nmer ; ++index) {
    vl_single  *pt  = ell + index * dof ;
    vl_uint    idx  = mer [index] ;
    vl_single  area = r [idx] .area ;
    
    for(d = 0 ; d < dof ; ++d) {

      pt [d] /= area ; 

      if(d >= ndims) {
        /* remove squared mean from moment to get variance */
        i = d - ndims ; 
        j = 0 ;
        while(i > j) {
          i -= j + 1 ;
          j ++ ;
        }
        pt [d] -= pt [i] * pt [j] ;
      }

    }
  }

  /* save back */
  f-> nell = nmer ;
}

