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
 **    ::vl_sift_process_next_octave().
 **   - Run the SIFT detector by ::vl_sift_detect() to get the keypoints.
 **   - For each keypoint:
 **     - Running the SIFT keypoint oriantation assignment 
 **       ::vl_sift_find_keypoint_orientations() to get the keypoint orientation(s).
 **     - Running the SIFT description ::vl_sift_get_keypoint_descriptor() to
 **       get the keypoint descriptor.
 ** - Delete the SIFT filter by ::vl_sift_delete().
 **/
 
/* AUTORIGHTS */

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


VlSiftFilt*        vl_sift_new                (int width, int height,
                                               int O, int S,
                                               int o_min) ;
void               vl_sift_delete              (VlSiftFilt *f) ;
int                vl_sift_process_first_octave(VlSiftFilt *f, 
                                                vl_sift_pix const *im) ;
int                vl_sift_process_next_octave (VlSiftFilt *f) ;

int                vl_sift_get_octave_width    (VlSiftFilt const *f) ;
int                vl_sift_get_octave_height   (VlSiftFilt const *f) ;

vl_sift_pix       *vl_sift_get_octave (VlSiftFilt const *f, int s) ;

void               vl_sift_detect             (VlSiftFilt *f) ;

VlSiftKeypoint const * vl_sift_get_keypoints  (VlSiftFilt const *f) ;
int                vl_sift_get_keypoints_num  (VlSiftFilt const *f) ;

int                vl_sift_calc_keypoint_orientations (VlSiftFilt *f, 
                                                       double angles [4],
                                                       VlSiftKeypoint const*k);
void               vl_sift_calc_keypoint_descriptor (VlSiftFilt *f,
                                                     vl_sift_pix *descr,
                                                     VlSiftKeypoint const* k,
                                                     double angle) ;
                                                       
