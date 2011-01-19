/** file   getopt_long.h
 ** brief  getopt_long
 ** author Andrea Vedaldi
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_GETOPT_LONG_H
#define VL_GETOPT_LONG_H

#include "generic.h"

VL_EXPORT int    opterr ;   /**< code of the last error occured while parsing an option */
VL_EXPORT int    optind ;   /**< index of the next option to process in @c argv */
VL_EXPORT int    optopt ;   /**< current option */
VL_EXPORT char * optarg ;   /**< argument of the current option */
VL_EXPORT int    optreset ; /**< reset flag */

/** @brief ::getopt_long option */
struct option
{
  const char *name ;  /**< option long name */
  int	      has_arg ; /**< flag indicating whether the option has no, required or optional argument */
  int	     *flag ;    /**< pointer to a variable to set (if @c NULL, the value is returned instead) */
  int	      val ;     /**< value to set or to return */
} ;

#define no_argument       0 /**< ::option with no argument */
#define required_argument 1 /**< ::option with required argument */
#define optional_argument 2 /**< ::option with optional argument */

VL_EXPORT int getopt_long(int argc, char * const argv[],
                          const char * optstring,
                          const struct option * longopts, int * longindex);

/* VL_GETOPT_LONG_H */
#endif

