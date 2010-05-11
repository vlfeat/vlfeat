/** @file    getopt_long.h
 ** @author  Andrea Vedaldi
 ** @brief   getopt_long
 **
 ** This is a re-implementation of GNU getopt_long meant to be used on
 ** platforms that do not support such functionality.
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_GETOPT_LONG_H
#define VL_GETOPT_LONG_H

#include "generic.h"

extern VL_EXPORT int    opterr ;  /** @brief option parsing last error */
extern VL_EXPORT int    optind ;  /** @brief index of option to process in @c argv */
extern VL_EXPORT int    optopt ;  /** @brief current option */
extern VL_EXPORT char * optarg ;  /** @brief argument of current option */
extern VL_EXPORT int    optreset; /** @brief */

/** @brief ::getopt_long option */
struct option
{
  const char *name ;    /**< option long name */
  int	      has_arg ; /**< no, required or optional argument flag */
  int	     *flag ;    /**< pointer to variable to set (if null, returns) */
  int	      val ;     /**< value to set or to return */
} ;

#define no_argument       0 /**< ::option with no argument */
#define required_argument 1 /**< ::option with required argument */
#define optional_argument 2 /**< ::option with optional argument */

extern VL_EXPORT int getopt_long(int argc, char *const argv[],
			const char *optstring,
			const struct option * longopts, int *longindex);

/* VL_GETOPT_LONG_H */
#endif

