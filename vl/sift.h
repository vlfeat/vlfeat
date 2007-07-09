/** @file     sift.h
 ** @author   Andrea Vedaldi
 ** @brief    Scale invariant feature transform (SIFT) - Declaration
 **
 ** Running the SIFT filter usually involves the following steps:
 **
 ** - Initialize the SIFT filter by ::vl_sift_new(). The
 **   filter can be reused if the image size does not change.
 ** - For each octave:
 **   - Compute the DOG scale space by either ::vl_sift_process_first_octave() or
 **    ::vl_sift_process_next_octave() (stop if ::VL_ERR_EOF is returned).
 **   - Run the SIFT detector by ::vl_sift_detect() to get the keypoints.
 **   - For each keypoint:
 **     - Use ::vl_sift_calc_keypoint_orientations() to get the keypoint orientation(s).
 **     - For each orientation:
 **       - Use ::vl_sift_calc_keypoint_descriptor()   to get the keypoint descriptor.
 ** - Delete the SIFT filter by ::vl_sift_delete().
 **
 ** @section sift-scale-space SIFT scale space
 **
 ** SIFT keypoints are computed at different scales. To this end, the
 ** image is projected in a Gaussian scale space by convoving it by
 ** isotropic Gaussian kernels of increasing variance. This results in a
 ** somewhat complex structure, illustrated next:
 **
 ** @image html sift-ss.png
 **
 ** - In the scale space, the image at scale @f$\sigma@f$ is simply
 **   the image smoothed with a Gaussian kernel of that variance.  The
 **   input image is assumed to be pre-smoothed at scale
 **   @f$\sigma_n@f$ due to pixel aliasing.
 ** - Scales are sampled at different points @f$\sigma(o,s)@f$ which
 **   are logarithmically spaced.  The levels are indexed by @e o
 **   (octave index) and @e s (level index). There are @e O octaves
 **   and @e S levels per octave. At each octave, the image is
 **   downsampled to save computations. The sampled levels are
 **   represented as black vertical lines in the figure (their length
 **   is proportional to the resolution of the image).
 ** - The octave index starts at @f$o_{\mathrm{min}}@f$ and ends at
 **   @f$o_{\mathrm{min}}+O-1@f$. The level index starts at
 **   @f$s_{\mathrm{min}}=-1@f$ and ends at @f$s_{\mathrm{max}} = S+2@f$.
 **   This means that scale levels are sampled twice, as belonging to
 **   different octaves (this is apparent from the figure).
 ** - The DOG scale space is obtained by subtracting contiguous GSS
 **   levels.  The DOG levels are represented as vertical blue bars.
 ** - The SIFT detector works in between DOG levels, finding keypoints
 **   at the intersection of the green arrows. Notice that, due to the
 **   choice of the GSS and DOG samples, these intersections
 **   consitutite a non-redunant complete sampling of all octaves and
 **   levels.
 **
 **  <table>
 **  <tr>
 **   <td>parameter</td>
 **   <td>alt. name</td>
 **   <td>standard value</td>
 **   <td>set by</td>
 **  </tr>
 **  <tr>
 **    <td>@f$O@f$</td>
 **    <td>@c O</td>
 **    <td>as big as possible</td>
 **    <td>::vl_sift_new()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$o_{\mathrm{min}}@f$</td>
 **    <td>@c o_min</td>
 **    <td>-1</td>
 **    <td>::vl_sift_new()</td>
 **  </tr>
 **  <tr>
 **    <td>@f$S@f$</td>
 **    <td>@c S</td>
 **    <td>3</td>
 **    <td>::vl_sift_new()</td>
 **  </tr>
 ** </table>
 **
 ** @section sift-detector SIFT detector
 **
 ** The SIFT detectors finds peaks of the DOG scale space as follows:
 **
 ** - Peaks are searched in a neighborhood of 3x3x3 pixels (in both
 **   space and scale).  Along the scale index, they are selected at
 **   the intersecton fo the green arrows in the previous figure.
 ** - Peaks are then quadratically interpolated.
 ** - Peaks are filtered by contrast (peak treshold) and
 **   spatial stability (edge treshold).
 **
 ** Each peak passing these test is a SIFT keypoint. SIFT also assigns
 ** up to four orientations to each keypoint by looking at the modes
 ** of the distribution of the gradient orientation in a polling
 ** region supported by a Gaussian windows 1.5 wider than the scale of
 ** the keypoint:
 **
 ** @image html sift-orient.png
 **
 ** In addition to the biggest mode, up to other three modes whose
 ** amplitude is within the 80% of the biggest mode are retained too,
 ** returned as additional orientations.
 **
 ** <table>
 **  <tr>
 **   <td>parameter</td>
 **   <td>standard value</td>
 **   <td>set by</td>
 **  </tr>
 **  <tr>
 **    <td>@c edge_tresh</td>
 **    <td>10.0</td>
 **    <td>::vl_sift_set_edge_tresh()</td>
 **  </tr>
 **  <tr>
 **    <td>@c peak_tresh</td>
 **    <td>0</td>
 **    <td>::vl_sift_set_peak_tresh()</td>
 **  </tr>
 ** </table>
 **
 ** @section sift-descriptor SIFT descriptor
 **
 ** The SIFT descriptor is a three dimensional histogram
 ** @f$h(\theta,x,y)@f$ of the orientation @f$\theta@f$ and position
 ** @f$(x,y)@f$ of the gradient inside a patch surronding the
 ** keypoint. The following picutres illustrates the geometry of the
 ** histogram:
 **
 ** @image html sift-bins.png
 **
 ** While SIFT descriptor @f$h(\theta,x,y)@f$ is a 3-D array but it is
 ** usually presented as a vector. This vector is obtained by stacking
 ** the 3-D array being @f$\theta@f$ is the fastest varying index and
 ** @e y the slowest.
 **
 ** The histogram uses soft binning, so that bins partially overlap.
 ** There are @f$B_p@f$ bins along the @e x and @e y directions and 
 ** @f$B_o@f$ along the @f$\theta@f$ direction.
 **
 ** The actual sizes of the descriptor depend on the size of the
 ** keypoint as follows:
 **
 ** - Let @f$ \sigma  @f$ be the scale of the keypoint.
 ** - A bin extends for @f$m\sigma@f$ pixels where @f$m@f$ is a constant
 **   (the standard descriptor has @f$m=3@f$).
 ** - Overall, the bins cover @f$m\sigma B_p/2@f$ pixels in each
 **   direction.  This is also the deviation of the Gaussian window
 **   which defines the smooth support of the descriptor.
 ** - Due to the smooth binning, the bins actually extend up for
 **   @f$m\sigma (B_p+1)/2@f$.
 ** - Since the descriptor may be rotated, a window of up to 
 **   @f$m\sigma (B_p+1)/2\sqrt{2}@f$ may need to be considered for
 **   the computations.
 **
 ** The following table summarizes the descriptors parameters along
 ** with their standard vale.
 **
 ** <table>
 **  <tr>
 **   <td>parameter</td>
 **   <td>alt. name</td>
 **   <td>standard value</td>
 **  </tr>
 **  <tr>
 **    <td>@f$B_p@f$</td>
 **    <td>@c BP</td>
 **    <td>4</td>
 **  </tr>
 **  <tr>
 **    <td>@f$B_o@f$</td>
 **    <td>@c BO</td>
 **    <td>8</td>
 **  </tr>
 **  <tr>
 **    <td>@f$m@f$</td>
 **    <td>@c magnif</td>
 **    <td>3</td>
 **  </tr>
 ** </table>
 **
 ** The keypoint coordinates <em>(x,y)</em> are expressed in the
 ** standard image convention (@e y axis pointing down). This also
 ** establishes the convention for expressing the angle @e th of a
 ** vector @e v (here @e v could be either the gradient of the image
 ** or the direction of the keypoint). To slightly complicate the
 ** matter, however, the index @e th of the descrptor
 ** <em>h(th,x,y)</em> follows the opposite convention (this is for
 ** compatibility with Lowe's original SIFT implementation), as shown
 ** by the figure:
 **
 ** @image html sift-angle.png
 **/
 
/* AUTORIGHTS */

#include <stdio.h>

#include "generic.h"

/** @brief SIFT filter pixel type */
typedef vl_single vl_sift_pix ;

/** @brief SIFT filter keypoint.
 **
 ** This structure represent a keypoint as extracted by the SIFT
 ** filter ::VLSiftFilt.
 **/
typedef struct _VlSiftKeypoint
{
  int o ;           /**< o coordinate (octave). */
  
  int ix ;          /**< Integer unnormalized x coordinate. */
  int iy ;          /**< Integer unnormalized y coordinate. */
  int is ;          /**< Integer s coordinate. */
  
  vl_single x ;     /**< x coorindate. */
  vl_single y ;     /**< u coordinate. */
  vl_single s ;     /**< x coordinate. */
  vl_single sigma ; /**< scale. */
} VlSiftKeypoint ;

/** @brief SIFT filter
 **
 ** This filter implements the SIFT detector and descriptor.
 **/
typedef struct _VlSiftFilt
{
  double sigman ;       /**< nominal image smoothing. */
  double sigma0 ;       /**< smoothing of pyramid base. */
  double sigmak ;       /**< k-smoothing */ 
  double dsigma0 ;      /**< delta-smoothing. */
  
  int width ;           /**< image width */
  int height ;          /**< image height */
  int O ;               /**< number of octaves */ 
  int S ;               /**< number of levels per octave */
  int o_min ;           /**< minimum octave index */
  int s_min ;           /**< minimum level index */
  int s_max ;           /**< maximum level index */
  int o_cur ;           /**< current octave */
  
  vl_sift_pix *temp ;   /**< temporary pixel buffer. */
  vl_sift_pix *octave ; /**< current GSS data. */
  vl_sift_pix *dog ;    /**< current DoG data. */
  int octave_width ;    /**< current octave width. */
  int octave_height ;   /**< current octave height. */
  
  VlSiftKeypoint* keys ;/**< detected keypoints. */
  int nkeys ;           /**< number of detected keypoints. */
  int keys_res ;        /**< size of the keys buffer. */

  double peak_tresh ;   /**< peak treshold. */
  double edge_tresh ;   /**< edge treshold. */

  vl_sift_pix *grad ;   /**< GSS gradient data. */
  int grad_o ;          /**< GSS gradient data octave. */

} VlSiftFilt ;


/** @name Create and destroy
 ** @{
 **/
VlSiftFilt*  vl_sift_new    (int width, int height,
                             int O, int S,
                             int o_min) ;
void         vl_sift_delete (VlSiftFilt *f) ;
/** @} */

/** @name Process data
 ** @{
 **/
int   vl_sift_process_first_octave       (VlSiftFilt *f, 
                                          vl_sift_pix const *im) ;
int   vl_sift_process_next_octave        (VlSiftFilt *f) ;
void  vl_sift_detect                     (VlSiftFilt *f) ;
int   vl_sift_calc_keypoint_orientations (VlSiftFilt *f, 
                                          double angles [4],
                                          VlSiftKeypoint const*k);
void  vl_sift_calc_keypoint_descriptor   (VlSiftFilt *f,
                                          vl_sift_pix *descr,
                                          VlSiftKeypoint const* k,
                                          double angle) ;
void  vl_sift_keypoint_init              (VlSiftFilt const *f,
                                          VlSiftKeypoint *k, 
                                          double x,
                                          double y,
                                          double sigma) ;
/** @} */

/** @name Retrieve data and parameters
 ** @{
 **/
static int    vl_sift_get_octave_index   (VlSiftFilt const *f) ;
static int    vl_sift_get_octave_num     (VlSiftFilt const *f) ;
static int    vl_sift_get_octave_first   (VlSiftFilt const *f) ;
static int    vl_sift_get_octave_width   (VlSiftFilt const *f) ;
static int    vl_sift_get_octave_height  (VlSiftFilt const *f) ;
static int    vl_sift_get_level_num      (VlSiftFilt const *f) ;
static int    vl_sift_get_keypoints_num  (VlSiftFilt const *f) ;
static double vl_sift_get_peak_tresh     (VlSiftFilt const *f) ;
static double vl_sift_get_edge_tresh     (VlSiftFilt const *f) ;

static vl_sift_pix *vl_sift_get_octave  (VlSiftFilt const *f, int s) ;
static VlSiftKeypoint const *vl_sift_get_keypoints (VlSiftFilt const *f) ;
/** @} */

/** @name Set parameters
 ** @{
 **/
static void vl_sift_set_peak_tresh (VlSiftFilt *f, double t) ;
static void vl_sift_set_edge_tresh (VlSiftFilt *f, double t) ;
/** @} */

/* --------------------------------------------------------------------
 *                                      Inline functions implementation
 * ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- */
/** @brief Get current octave index.
 ** @param f SIFT filter.
 ** @return index of the current octave.
 **/
static VL_INLINE int
vl_sift_get_octave_index (VlSiftFilt const *f) 
{
  return f-> o_cur ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of octaves.
 ** @param f SIFT filter.
 ** @return number of octaves.
 **/
static VL_INLINE int
vl_sift_get_octave_num (VlSiftFilt const *f) 
{
  return f-> O ;
}

/* ----------------------------------------------------------------- */
/** @brief Get first octave.
 ** @param f SIFT filter.
 ** @return index of the first octave.
 **/
static VL_INLINE int
vl_sift_get_octave_first (VlSiftFilt const *f) 
{
  return f-> o_min ;
}


/* ----------------------------------------------------------------- */
/** @brief Get current octave width
 ** @param f SIFT filter.
 ** @return current octave width.
 **/
static VL_INLINE int 
vl_sift_get_octave_width (VlSiftFilt const *f) 
{
  return f-> octave_width ; 
}

/* ----------------------------------------------------------------- */
/** @brief Get current octave height
 ** @param f SIFT filter.
 ** @return current octave height.
 **/
static VL_INLINE int 
vl_sift_get_octave_height (VlSiftFilt const *f) 
{
  return f-> octave_height ;
}

/* ----------------------------------------------------------------- */
/** @brief Get current octave data
 ** @param f SIFT filter.
 ** @param s level index.
 **
 ** The level index @a s ranges in the interval <tt>s_min = -1</tt>
 ** and <tt> s_max = S + 2</tt>, where @c S is the number of levels
 ** per octave.
 **
 ** @return pointer to the octave data for level @a s.
 **/
static VL_INLINE vl_sift_pix *
vl_sift_get_octave (VlSiftFilt const *f, int s) 
{
  int w = vl_sift_get_octave_width  (f) ;
  int h = vl_sift_get_octave_height (f) ;  
  return f->octave + w * h * (s - f->s_min) ;
}

/* ----------------------------------------------------------------- */
/** @brief Get number of levels per octave
 ** @param f SIFT filter.
 ** @return number of leves per octave.
 **/
static VL_INLINE int
vl_sift_get_level_num (VlSiftFilt const *f) 
{
  return f-> S ;
}


/* ----------------------------------------------------------------- */
/** @brief Get number of keypoints.
 ** @param f SIFT filter.
 ** @return number of keypoints.
 **/
static VL_INLINE int 
vl_sift_get_keypoints_num (VlSiftFilt const *f) 
{
  return f-> nkeys ;
}

/* ----------------------------------------------------------------- */
/** @brief Get keypoints.
 ** @param f SIFT filter.
 ** @return pointer to the keypoints list.
 **/
static VL_INLINE VlSiftKeypoint const *
vl_sift_get_keypoints (VlSiftFilt const *f) 
{
  return f-> keys ;
}

/* ----------------------------------------------------------------- */
/** @brief Get peaks treashold
 ** @param f SIFT filter.
 ** @param t treshold.
 **/
static VL_INLINE double
vl_sift_get_peak_tresh (VlSiftFilt const *f)
{
  return f -> peak_tresh ;
}

/* ----------------------------------------------------------------- */
/** @brief Get edges treshold
 ** @param f SIFT filter.
 ** @param t treshold.
 **/
static VL_INLINE double
vl_sift_get_edge_tresh (VlSiftFilt const *f) 
{
  return f -> edge_tresh ;
}

/* ----------------------------------------------------------------- */
/** @brief Set peaks treshold
 ** @param f SIFT filter.
 ** @param t treshold.
 **/
static VL_INLINE void
vl_sift_set_peak_tresh (VlSiftFilt *f, double t) 
{
  f -> peak_tresh = t ;
}

/* ----------------------------------------------------------------- */
/** @brief Set edges treshold
 ** @param f SIFT filter.
 ** @param t treshold.
 **/
static VL_INLINE void
vl_sift_set_edge_tresh (VlSiftFilt *f, double t) 
{
  f -> edge_tresh = t ;
}
