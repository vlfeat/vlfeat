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
 **       ::vl_sift_calc_keypoint_orientations() to get the keypoint orientation(s).
 **     - Running the SIFT description ::vl_sift_calc_keypoint_descriptor() to
 **       get the keypoint descriptor.
 ** - Delete the SIFT filter by ::vl_sift_delete().
 **
 ** @section sift-descriptor SIFT descriptor
 **
 ** The SIFT descriptor is a three dimensional histogram <em>h(th,x,y)</em>
 ** of the orientation @e th and position <em>(x,y)</em> of the gradient
 ** inside a patch surronding the keypoint. 
 **
 ** While the descriptor <em>h(th,x,y)</em> is a 3-D array but is
 ** usually presented by stacking the array into a vector. The
 ** dimensions are stacked in the order <em>(th,x,y)</em>, so that @e
 ** th is the fastest varying index and <em>y</em> the slowest.
 **
 ** The keypoint coordinates <em>(x,y)</em> are expressed in the
 ** standard image convention (@e y axis pointing down). This also
 ** establishes the convention for expressing the angle @e th of a
 ** vector @e v (@e v could be either the gradient of the image or the
 ** direction of the keypoint). To slightly complicate the matter,
 ** however, the index @e th of the descrptor <em>h(th,x,y)</em>
 ** follows the opposite convention (this is for compatibility with
 ** Lowe's original SIFT implementation), as shown by the figure:
 **
 ** @image html sift-angle.png
 **
 ** Notice that converting from Lowe's to the standard convention amounts to
 ** changing the sign of the angle.
 **
 ** The histogram uses soft binning, so that bins partially overlap.
 ** There are @e NBP bins along the @e x and @e y directions and @e
 ** NBO along the @e th direction (the standard descriptor uses
 ** <em>NBP=4</em> and <em>NBO=8</em>). In normalized coordinate, the
 ** bin arrangment along the @e x direciton is as follows:
 **
 ** @image html sift-bins.png
 **
 ** The other directions are similar. Each triangle represent the
 ** weighed support of a bin. A sample is assigned to each bin
 ** proportionally to the relative weights (3-linear interoplation).
 **
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
                                                       
