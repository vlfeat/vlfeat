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
 ** For each intenstiy @f$l@f$, one has multiple disjoint extremal
 ** regions in the level set @f$S_l@f$. Let @f$l@f$ span a finite
 ** number of values @f$\mathcal{L}=\{0,\dots,M-1\}@f$ (a sampling of
 ** the image range).  One obtains a family of regions @f$R_l@f$; by
 ** connecting two regions @f$R_l@f$and @f$R_{l+1}@f$ if, and only if,
 ** @f$R_l\subset R_{l+1}@f$, regions form a tree:
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
 ** minimum of the (in)stability score. More accurately, we start by
 ** assuming that all branches are maximally stable. Then we consider
 ** each branch @f$B(R_{l})@f$ and its parent branch
 ** @f$B(R_{l+1}):R_{l+1}\supset R_l@f$. If the two representative
 ** regions @f$B(R_{l})@f$ and @f$B(R_{l+1})@f$ are close (in term of
 ** shape), we mark as unstable the one with higher (in)stability
 ** score. Formally:
 **
 ** - if @f$|R_{l+1}-R_{l}|/|R_l|<\epsilon@f$
 **   - if @f$v(R_l)<v(R_{l+1})@f$, mark @f$R_{l+1}@f$ as unstable;
 **   - otherwise, mark @f$R_l@f$ as unstable.
 **
 ** This criterion selects among nearby regions the one that are more
 ** stable. We optionally refine the selection by running some or all
 ** of the following tests:
 **
 ** - @f$a_- \leq |R_{l}|/|R_{\infty}| \leq a_+@f$: exclude extremal
 **   regions too small or too big (@f$|R_{\infty}|@f$ is the area of
 **   the image).
 **
 ** - @f$v(R_{l}) < v_+@f$: test for absolute stability.
 **
 ** - For any two regions @f$R_l\subset R_{l+q}@f$ whose
 **   relative area variation @f$|R_{l+q} - R_l|/|R_l|@f$ is
 **   below a treshold, mark @f$R_{l}@f$ as unstable: remove duplicated regions.
 **
 **  <table>
 **  <tr>
 **   <td>parameter</td>
 **   <td>alt. name</td>
 **   <td>standard value</td>
 **   <td>set by</td>
 **  </tr>
 **  <tr>
 **    <td>@f$\Delta@f$</td>
 **    <td>@c delta</td>
 **    <td>5</td>
 **    <td>::vl_mser_set_delta()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$\epsilon@f$</td>
 **    <td>@c epsilon</td>
 **    <td>0.2</td>
 **    <td>::vl_mser_set_epsilon()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$a_+@f$</td>
 **    <td>@c max_area</td>
 **    <td>0.75</td>
 **    <td>::vl_mser_set_max_area()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$a_-@f$</td>
 **    <td>@c min_area</td>
 **    <td>3.0/@f$|R_\infty|@f$</td>
 **    <td>::vl_mser_set_min_area()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$v_+@f$</td>
 **    <td>@c max_var</td>
 **    <td>0.25</td>
 **    <td>::vl_mser_set_max_var()</td>
 **  </tr>
 **  <tr>
 **    <td></td>
 **    <td>@c no_dups</td>
 **    <td>@c true (1)</td>
 **    <td>::vl_mser_set_no_dups()</td>
 **  </tr>
 ** </table>
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
 ** - Pixels are added to a forest by increasing intensity. The forest has the
 **   followin properties:
 **   - All the descendent of a certain pixels are subset of an extremal region.
 **   - All the extremal regions are the descendents of some pixels.
 ** - Extremal regions are extracted from the region tree and the extremal regions tree is
 **   calculated.
 ** - Stable regions are marked.
 ** - Duplicates and other bad regions are removed.
 **
 ** @remark The extremal region tree which is calculated is a subset
 ** of the actual extremal region tree. In particular, it does not
 ** contain redundant entries extremal regions that coincde as
 ** sets. So, for example, in the calculated extremal region tree, the
 ** parent @f$R_q@f$ of an extremal region @f$R_{l}@f$ may or may
 ** <em>not</em> correspond to @f$R_{l+1}@f$, depending wether @f$q\leq
 ** l+1@f$ or not. These subtelties are important when caluclating the
 ** stability tests.
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

/** @brief MSER filter statistics */
typedef struct _VlMserStats VlMserStats ;

/** @brief MSER filter statistics definition */
struct _VlMserStats 
{
  int num_extremal ;      /**< number of extremal regions                                */
  int num_unstable ;      /**< number of unstable extremal regions                       */
  int num_abs_unstable ;  /**< number of regions that failed the absolute stability test */
  int num_too_big ;       /**< number of regions that failed the maximum size test       */
  int num_too_small ;     /**< number of regions that failed the minimum size test       */
  int num_duplicates ;    /**< number of regions that failed the duplicate test          */
} ;

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
static double           vl_mser_get_epsilon      (VlMserFilt const *filt) ;

static vl_bool          vl_mser_get_no_dups      (VlMserFilt const *filt) ;
static double           vl_mser_get_min_area     (VlMserFilt const *filt) ;
static double           vl_mser_get_max_area     (VlMserFilt const *filt) ;
static double           vl_mser_get_max_var      (VlMserFilt const *filt) ;

static vl_uint          vl_mser_get_regions_num  (VlMserFilt const *filt) ;
static vl_uint const*   vl_mser_get_regions      (VlMserFilt const *filt) ;
static vl_single const* vl_mser_get_ell          (VlMserFilt const *filt) ;
static vl_uint          vl_mser_get_ell_num      (VlMserFilt const *filt) ;
static vl_uint          vl_mser_get_ell_dof      (VlMserFilt const *filt) ;

static VlMserStats const*  vl_mser_get_stats     (VlMserFilt const *filt) ;
/** @} */

/** @name Setting parameters
 ** @{
 **/
static void  vl_mser_set_epsilon   (VlMserFilt *filt, double      x) ;
static void  vl_mser_set_delta     (VlMserFilt *filt, vl_mser_pix x) ;
static void  vl_mser_set_no_dups   (VlMserFilt *filt, vl_bool     x) ;
static void  vl_mser_set_min_area  (VlMserFilt *filt, double      x) ;
static void  vl_mser_set_max_area  (VlMserFilt *filt, double      x) ;
static void  vl_mser_set_max_var   (VlMserFilt *filt, double      x) ;
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
 ** An instance of the structure represents the extremal region of the
 ** level set of intensity VlMserExtrReg::value and containing the
 ** pixel VlMserExtReg::index.
 **
 ** VlMserExtrReg::area is the are of the extremal region and
 ** VlMserExtrReg::area_top is the area of the extremal region
 ** containing this region in the level set of intenisty
 ** VlMserExtrReg::area + @c delta.
 **
 ** VlMserExtrReg::variation is the relative area variation @c
 ** (area_top-area)/area.
 **
 ** VlMserExtrReg::max_stable is a flag signaling wether this extremal
 ** region is also maximally stable.
 **/

typedef struct _VlMserExtrReg
{
  int          parent ;     /**< index of the parent region                   */
  int          index ;      /**< index of pivot pixel                         */
  vl_mser_pix  value ;      /**< value of pivot pixel                         */
  vl_uint      shortcut ;   /**< shortcut used when building a tree           */
  vl_uint      area ;       /**< area of the region                           */
  vl_single    variation ;  /**< rel. area variation                          */
  vl_uint      max_stable ; /**< max stable number (=0 if not maxstable)      */
} VlMserExtrReg ;

/* ----------------------------------------------------------------- */
/** @brief MSER filter
 **
 ** The MSER filter computes the Maximally Stable Extremal Regions of
 ** an image.
 **
 ** The filter is designed as an <em>opaque</em> object: one should
 ** avoid to access directly the data fields and use the appropriate
 ** access functions instead.
 **
 ** @sa mser
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
  int                rer ;     /**< size of er buffer                       */
  int                rmer ;    /**< size of mer buffer                      */
  /*@}*/

  /** @name Ellipsoids fitting */
  /*@{*/
  vl_single         *acc ;     /**< moment accumulator.                    */
  vl_single         *ell ;     /**< ellipsoids list.                       */
  int                rell ;    /**< size of ell buffer                     */
  int                nell ;    /**< number of ellipsoids extracted         */
  int                dof ;     /**< number of dof of ellipsoids.           */

  /*@}*/

  /** @name Configuration */
  /*@{*/
  vl_bool   verbose ;          /**< be verbose                             */
  int       delta ;            /**< delta filter paramter                  */
  double    epsilon ;          /**< epsilon filter parameter               */
  double    max_area ;         /** badness test parameter                  */
  double    min_area ;         /** badness test parameter                  */
  double    max_var ;          /** badness test parameter                  */
  vl_bool   no_dups  ;         /** badness test parameter                  */
  /*@}*/

  VlMserStats stats ;          /** run statistic                           */
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
/** @brief Get epsilon
 ** @param f MSER filter.
 ** @return value of @c epsilon.
 **/
static VL_INLINE double
vl_mser_get_epsilon (VlMserFilt const *f) 
{
  return f-> epsilon ;
}

/** @brief Get epsilon
 ** @param f MSER filter.
 ** @return value of @c epsilon.
 **/
static VL_INLINE void
vl_mser_set_epsilon (VlMserFilt *f, double x) 
{
  f-> epsilon = x ;
}

/* ----------------------------------------------------------------- */
/** @brief Get statistics
 ** @param f MSER filter.
 ** @return statistics.
 **/
static VL_INLINE VlMserStats const*
vl_mser_get_stats (VlMserFilt const *f) 
{
  return & f-> stats ;
}

/* ----------------------------------------------------------------- */
/** @brief Get maximum region area
 ** @param f MSER filter.
 ** @return maximum region area.
 **/
static VL_INLINE double
vl_mser_get_max_area (VlMserFilt const *f) 
{
  return f-> max_area ;
}

/** @brief Set maximum region area
 ** @param f MSER filter.
 ** @param x maximum region area.
 **/
static VL_INLINE void
vl_mser_set_max_area (VlMserFilt *f, double x) 
{
  f-> max_area = x ;
}

/* ----------------------------------------------------------------- */
/** @brief Get minimum region area
 ** @param f MSER filter.
 ** @return minimum region area.
 **/
static VL_INLINE double
vl_mser_get_min_area (VlMserFilt const *f) 
{
  return f-> min_area ;
}

/** @brief Set minimum region area
 ** @param f MSER filter.
 ** @param x minimum region area.
 **/
static VL_INLINE void
vl_mser_set_min_area (VlMserFilt *f, double x) 
{
  f-> min_area = x ;
}

/* ----------------------------------------------------------------- */
/** @brief Get maximum region variation
 ** @param f MSER filter.
 ** @return maximum region variation.
 **/
static VL_INLINE double
vl_mser_get_max_var (VlMserFilt const *f) 
{
  return f-> max_var ;
}

/** @brief Set maximum region variation
 ** @param f MSER filter.
 ** @return maximum region variation.
 **/
static VL_INLINE void
vl_mser_set_max_var (VlMserFilt *f, double x) 
{
  f-> max_var = x ;
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
/** @brief Get remove duplicates switch
 ** @param f MSER filter.
 ** @return duplicates switch.
 **/
static VL_INLINE vl_bool 
vl_mser_get_no_dups (VlMserFilt const* f)
{
  return f-> no_dups ;
}

/** @brief Set duplicates switch
 ** @param f MSER filter.
 ** @param duplicates switch.
 **/
static VL_INLINE void
vl_mser_set_no_dups (VlMserFilt *f, vl_bool x)
{
  f-> no_dups = x  ;
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

/** @brief Get number of degrees of freedom of ellipsoids
 ** @param f MSER filter.
 ** @return number of degrees of freedom.
 **/
static VL_INLINE vl_uint
vl_mser_get_ell_dof (VlMserFilt const* f)
{
  return f-> dof ;
}

/** @brief Get number of ellipsoids
 ** @param f MSER filter.
 ** @return number of ellipsoids
 **/
static VL_INLINE vl_uint
vl_mser_get_ell_num (VlMserFilt const* f)
{
  return f-> nell ;
}

/* VL_MSER */
#endif
