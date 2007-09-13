/** @file    rodrigues.h
 ** @author  Andrea Vedaldi
 ** @brief   Rodrigues formulas - Declaration
 **
 ** @section rodrigues Rodrigues formulas
 **
 ** - Use vl_rodrigues() to compute the Rodrigues formula and its derivative.
 ** - Use vl_irodrigues() to compute the inverse Rodrigues formula and
 **   its derivative.
 **/

/* AUTORIGHTS */

#ifndef VL_RODRIGUES
#define VL_RODRIGUES

void vl_rodrigues  (double* R_pt,  double* dR_pt, const double* om_pt) ;
void vl_irodrigues (double* om_pt, double* dom_pt, const double* R_pt) ;

/* VL_RODRIGUES */
#endif

