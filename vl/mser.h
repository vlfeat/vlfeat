/** @file     mser.h
 ** @brief    Maximally Stable Extremal Regions (MSER) - Declaration
 ** @author   Andrea Vedaldi
 ** 
 ** Running the MSER filter usually involves the following steps:
 **
 ** - Initialize the MSER filter by ::vl_mser_new(). The
 **   filter can be reused for images of the same size.
 ** - Call ::vl_mser_process() to compute the MSERs.
 ** - Optionally call ::vl_mser_ell_fit() to fit ellipsoids to the MSERs.
 ** - Retrieve the results
 ** - Delete the MSER filter by ::vl_mser_delete().
 **
 ** @section mser-definition Maximally Stable Extremal Regions
 **
 ** An extremal region @f$R_l@f$ of an image is a connected component
 ** of the level set @f$S_l = \{ x : I(x) \leq l \}@f$.
 **
 ** @image html mser-er.png
 **
 ** For each value of @f$l@f$, one has many extremal regions, but they
 ** are disjoint (as they are maximal connected components).  For
 ** simplicity, let @f$l@f$ span a finite number of values (a sampling
 ** of the image range).  One obtains a family of regions @f$R_l@f$;
 ** by connecting two regions @f$R_l@f$and @f$R_{l+1}@f$ if, and only
 ** if, @f$R_l\subset R_{l+1}@f$, one arranges all such extremal
 ** regions in a tree.
 ** 
 ** @image html mser-tree.png
 **
 ** The <em>maximally stable extremal regions</em> are extremal
 ** regions which satisfy a stability criterion. Here we use a
 ** criterion which is similar but not identical to the original
 ** paper. This definition is somewhat simpler both to understand and
 ** code (it also runs faster).
 **
 ** Let @f$B(R_l)=(R_l,R_{l+1},\dots,R_{l+\Delta})@f$ be the branch of
 ** the tree rooted at @f$R_l@f$.  We associate to the branch the
 ** (in)stability score
 **
 ** @f[ 
 **   v(R_l) = \frac{|R_{l+\Delta} - R_l|}{|R_l|}.
 ** @f]
 **
 ** The score is low if the regions along the branch have similar area
 ** (and thus similar shape). We aim to select maximally stable
 ** branches; then a maximally stable region is just a representative
 ** region selected from a maximally stable branch (for simplicity we
 ** select @f$R_l@f$, but one could choose for example
 ** @f$R_{l+\Delta/2}@f$).
 ** 
 ** Roughly speaking, a branch is maximally stable if it is a local
 ** minimum of the stability score. More accurately, we start by
 ** assuming that all branches are maximally stable. Then we consider
 ** each region @f$R_{l}@f$ and its parent region @f$R_{l+1}\supset
 ** R_l@f$. If the two regions are close (in term of shape), we mark
 ** as unstable the one with higher (in)stability score. Formally
 **
 ** - if @f$v(R_l)<\tau@f$
 **   - if @f$v(R_l)<v(R_{l+1})@f$, mark @f$R_{l+1}@f$ as unstable;
 **   - otherwise, mark @f$R_l@f$ as unstable.
 **
 ** Maximal stability measures stability relatively to regions close
 ** by. We optionally refine the selection by running some or all of
 ** the following tests:
 **
 ** - Regions too small or too big are marked unstable.
 **
 ** - Absolutely unstable regions are marked unstable. By this we mean
 **   regions whose (in)stabilty score is above a certain treshold.
 **
 ** - Duplicated regions are marked unstable. These are detected as
 **   follows: for any two regions @f$R_l\subset R_{l+q}@f$ whose
 **   relative area variation @f$|R_{l+q} - R_l|/|R_l|@f$ is
 **   below a treshold, we mark @f$R_{l+q}@f$ as unstable.
 **
 ** @section mser-n-dimension N-dimensional images
 **
 ** The code supports images of arbitrary dimension. For instance, it
 ** is possible to find the MSER regions of volumetric images or time
 ** sequences. See ::vl_mser_new() for further details.
 **
 ** @section mser-algorithm Algorithm
 **
 ** The algorithm is quite efficient. While some details may be tricky, the overall
 ** idea is easy to grasp.
 **
 ** - Pixels are sorted by increasing intensity.
 ** - Adding a pixel per time in this order, pixels are joined into a tree with the
 **   following properties:
 **   - All the descendent of a certain pixels are subset of an extremal region.
 **   - All the extremal regions are the descendents of some pixels.
 ** - Extremal regions are extracted from the tree.
 ** - Stable regions are marked.
 ** - Duplicates and other bad regions are removed.
 **
 **
 **/

/* AUTORIGHTS */

#ifndef VL_MSER
#define VL_MSER

#include "generic.h"

/** @brief MSER image data type 
 ** 
 ** This is the data type of the image pixels. It has to be an
 ** integer.
 **/
typedef vl_uint8 vl_mser_pix ;

/** @brief Maximum value
 **
 ** Maximum value of the integer type ::vl_mser_pix.
 **/
#define VL_MSER_PIX_MAXVAL 256


/** @brief MSER Filter */
typedef struct _VlMserFilt VlMserFilt ;

/** @name Construction and Destruction
 ** @{
 **/
VlMserFilt*      vl_mser_new     (int ndims, int const* dims) ;
void             vl_mser_delete  (VlMserFilt *filt) ;
/** @} */

/** @name Processing 
 ** @{
 **/
void             vl_mser_process (VlMserFilt *filt, 
                                  vl_mser_pix const *im) ;
void             vl_mser_ell_fit (VlMserFilt *filt) ;
/** @} */

/** @name Retrieving data and parameters 
 ** @{
 **/
static vl_mser_pix      vl_mser_get_delta        (VlMserFilt const *filt) ;
static double           vl_mser_get_variation    (VlMserFilt const *filt) ;
static vl_uint          vl_mser_get_regions_num  (VlMserFilt const *filt) ;
static vl_uint const*   vl_mser_get_regions      (VlMserFilt const *filt) ;
static vl_single const* vl_mser_get_ell          (VlMserFilt const *filt) ;
static vl_uint          vl_mser_get_ell_num      (VlMserFilt const *filt) ;
static vl_uint          vl_mser_get_ell_dof      (VlMserFilt const *filt) ;
/** @} */

/** @name Setting parameters
 ** @{
 **/
static void  vl_mser_set_variation (VlMserFilt *filt, double x) ;
static void  vl_mser_set_delta     (VlMserFilt *filt, vl_mser_pix x) ;
/** @} */

/* --------------------------------------------------------------------
 *                                      Inline functions implementation
 * ----------------------------------------------------------------- */

/** @brief MSER accumulator data type 
 ** @internal
 **
 ** This is a large integer type. It should be large enough to contain
 ** a number equal to the area (volume) of the image by the image
 ** widht by the image height (for instance, if the image is a square
 ** of side 256, the maximum value is 256 x 256 x 256).
 **/
typedef vl_single vl_mser_acc ;

/** @brief Basic region flag: null region @internal */
#define VL_MSER_VOID_NODE ((1ULL<<32) - 1)

/* ----------------------------------------------------------------- */
/** @brief MSER: basic region
 ** @internal
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
 ** @internal
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
  vl_uint      shortcut ;
  vl_uint      area ;       /**< area of the region                           */
  vl_uint      area_top ;   /**< area of the region DELTA levels above        */
  vl_uint      area_bot  ;  /**< area of the region DELTA levels below        */
  vl_single    variation ;  /**< rel. area variation                          */
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

  double variation ;

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
/** @brief Get delta
 ** @param f MSER filter.
 ** @return value of @c delta.
 **/
static VL_INLINE vl_mser_pix
vl_mser_get_delta (VlMserFilt const *f) 
{
  return f-> delta ;
}

/* ----------------------------------------------------------------- */
/** @brief Set delta
 ** @param f MSER filter.
 ** @return value of @c delta.
 **/
static VL_INLINE void
vl_mser_set_delta (VlMserFilt *f, vl_mser_pix x)
{
  f-> delta = x ;
}

/* ----------------------------------------------------------------- */
/** @brief Get variation
 ** @param f MSER filter.
 ** @return value of @c delta.
 **/
static VL_INLINE double
vl_mser_get_variation (VlMserFilt const *f) 
{
  return f-> variation ;
}

/* ----------------------------------------------------------------- */
/** @brief Get variation
 ** @param f MSER filter.
 ** @return value of @c delta.
 **/
static VL_INLINE void
vl_mser_set_variation (VlMserFilt *f, double x) 
{
  f-> variation = x ;
}

/* ----------------------------------------------------------------- */
/** @brief Get maximally stable extremal regions
 ** @param f MSER filter.
 ** @return array of MSER pivots.
 **/
static VL_INLINE vl_uint const *
vl_mser_get_regions (VlMserFilt const* f)
{
  return f-> mer ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of maximally stable extremal regions
 ** @param f MSER filter.
 ** @return number of MSERs.
 **/
static VL_INLINE vl_uint
vl_mser_get_regions_num (VlMserFilt const* f)
{
  return f-> nmer ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of degrees of freedom of ellipsoids
 ** @param f MSER filter.
 ** @return number of degrees of freedom.
 **/
static VL_INLINE vl_uint
vl_mser_get_ell_dof (VlMserFilt const* f)
{
  return f-> dof ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of ellipsoids
 ** @param f MSER filter.
 ** @return number of ellipsoids
 **/
static VL_INLINE vl_uint
vl_mser_get_ell_num (VlMserFilt const* f)
{
  return f-> nell ;
}

/* ----------------------------------------------------------------- */
/** @brief Get ellipsoids
 ** @param f MSER filter.
 ** @return ellipsoids.
 **/
static VL_INLINE vl_single const *
vl_mser_get_ell (VlMserFilt const* f)
{
  return f-> ell ;
}

/* VL_MSER */
#endif
