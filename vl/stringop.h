/** @file   stringop.h
 ** @author Andrea Vedaldi
 ** @brief  String Operations
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#ifndef VL_STRINGOP_H
#define VL_STRINGOP_H

#include "generic.h"

/* file protocols */
enum {
  VL_PROT_UNKNOWN = -1, /**< unknown protocol */
  VL_PROT_NONE    =  0, /**< no protocol      */
  VL_PROT_ASCII,        /**< ASCII protocol   */
  VL_PROT_BINARY        /**< Binary protocol  */
} ;

VL_EXPORT vl_size vl_string_copy (char * destination, vl_size destinationSize, char const * source) ;
VL_EXPORT vl_size vl_string_copy_sub (char * destination, vl_size destinationSize,
                                      char const * beginning, char const * end) ;
VL_EXPORT char* vl_string_parse_protocol (char const * string, int * protocol) ;
VL_EXPORT char const* vl_string_protocol_name (int prot) ;
VL_EXPORT vl_size vl_string_basename (char * destination, vl_size destinationSize,
                                      char const * source, vl_size maxNumStrippedExtension) ;
VL_EXPORT vl_size vl_string_replace_wildcard (char * destination, vl_size destinationSize,
                                              char const * src, char wildcardChar, char escapeChar,
                                              char const * replacement) ;
VL_EXPORT char*  vl_string_find_char_rev (char const * beginning, char const * end, char c) ;
VL_EXPORT vl_size vl_string_length (char const * string) ;
VL_EXPORT int vl_string_casei_cmp (const char * string1, const char * string2) ;

/* VL_STRINGOP_H */
#endif
