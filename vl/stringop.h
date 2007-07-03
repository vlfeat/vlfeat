/** @file   stringop.h
 ** @author Andrea Vedaldi
 ** @brief  String Operatons - Declaration
 **
 ** This module supports common strings operations. All functions that
 ** write to strings use range checking, which makes them safer and
 ** more robust than standard POSIX equivalent (see @ref
 ** vl-stringop-err).
 **
 ** @section vl-stringop-err Detecting overflow
 **
 ** Functions of this module may write a string as a result of their
 ** operation.  The string is written to a buffer passed to the
 ** function along with its length @c n and is @c NUL terminated. The
 ** function never writes more than @c n characters, thus preventing
 ** overflow (this includes the @c NUL character terminating the
 ** string). The function also returns the total number @c r of
 ** chatarcters that would have been written if the buffer was big
 ** enough (not counting the terminating @c NUL). Thus:
 ** 
 ** - overflow errors are be detected by testing whether @c r &gt;= @c n; 
 ** - @c r can be used to re-allocate a sufficiently large buffer and repeat the operation;
 ** - as long as @a n &gt;= 0 the result is always @c NUL terminated;
 ** - if @a n = 0 nothing is actually written.
 **   
 **/

#ifndef VL_STRINGOP_H
#define VL_STRINGOP_H

#include "generic.h"

/** @brief File protocols
 ** @anchor vl-file-protocols
 **
 ** The following is a list of the supported protocols with their
 ** symbolic names:
 **
 ** - ASCII protocol (::VL_PROT_ASCII). Symbolic name: <tt>ascii</tt>.
 ** - Binary protocol (::VL_PROT_BINARY). Symbolic name:  <tt>binary</tt>.
 **/

enum {
  VL_PROT_UNKNOWN = -1, /**< unknown protocol */
  VL_PROT_NONE    =  0, /**< no protocol      */ 
  VL_PROT_ASCII,        /**< ASCII protocol   */
  VL_PROT_BINARY        /**< Binary protocol  */
} ;


int    vl_string_copy             (char *dst, int n, char const *src) ;
int    vl_string_copy_sub         (char *dst, int n, char const *beg, char const *end) ;
char*  vl_string_parse_protocol   (char const *str, int *prot) ;
char const* vl_string_protocol_name (int prot) ;
int    vl_string_basename         (char *dst, int n, char const *src, int n_ext) ;
int    vl_string_replace_wildcard (char *dst, int n, char const *src, char wild, char esc, char const* repl) ;
char*  vl_string_find_char_rev    (char const *beg, char const* end, char c) ;
int    vl_string_length           (char const *str) ;

/* VL_STRINGOP_H */
#endif
