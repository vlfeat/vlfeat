/** @file     sift.h
 ** @author   Andrea Vedaldi
 ** @brief    Scale invariant feature transform (SIFT) - Declaration
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
 ** @return treshold ;
 **/
static VL_INLINE double
vl_sift_get_peak_tresh (VlSiftFilt const *f)
{
  return f -> peak_tresh ;
}

/* ----------------------------------------------------------------- */
/** @brief Get edges treshold
 ** @param f SIFT filter.
 ** @return treshold.
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
