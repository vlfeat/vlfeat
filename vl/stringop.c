/** @file    stringop.c
 ** @author  Andrea Vedaldi
 ** @brief   String Operations - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

/** @file   stringop.h
 ** @author Andrea Vedaldi
 ** @brief  String Operations

This module implements common strings operations. All functions that
write to strings use range checking, which makes them safer and more
robust than standard POSIX equivalent (see @ref vl-stringop-err).

@section vl-stringop-err Detecting overflow

Functions of this module may write a string as a result of their
operation.  The string is written to a buffer passed to the function
along with its length @c n and is @c NUL terminated. The function
never writes more than @c n characters, thus preventing overflow (this
includes the @c NUL character terminating the string). The function
also returns the total number @c r of characters that would have been
written if the buffer was big enough (not counting the terminating @c
NUL). Thus:

- overflow errors are be detected by testing whether @c r &gt;= @c n;
- @c r can be used to re-allocate a sufficiently large buffer and repeat the operation;
- as long as @a n &gt;= 0 the result is always @c NUL terminated;
- if @a n = 0 nothing is actually written.

**/

#include "stringop.h"

#include <string.h>
#include <ctype.h>

/** ------------------------------------------------------------------
 ** @brief Parse string prefix for file protocol
 **
 ** @param str  string.
 ** @param prot protocol code (output).
 **
 ** The function extracts the prefix of the string @a str terminated
 ** by the first occurrence of the <tt>`:'</tt> character (if any). It
 ** then matches the suffix to the supported @ref vl-file-protocols
 ** "protocols".  The corresponding protocol code is wrote to @a prot.
 **
 ** @return A pointer to the first character after the protocol
 ** prefix.  The function writes to @a prot the value ::VL_PROT_NONE
 ** if no suffix is detected and ::VL_PROT_UNKNOWN if there is a
 ** suffix but it cannot be recognized.
 **/

VL_EXPORT char *
vl_string_parse_protocol (char const *str, int *prot)
{
  char const* cpt ;
  int dummy ;

  /* handle the case prot = 0 */
  if (prot == 0)
    prot = &dummy ;

  /* look for :// */
  cpt = strstr(str, "://") ;

  if (cpt == 0) {
    *prot = VL_PROT_NONE ;
    cpt = str ;
  }
  else {
    if      (strncmp(str, "ascii", cpt - str) == 0) {
      *prot = VL_PROT_ASCII ;
    }
    else if (strncmp(str, "bin",   cpt - str) == 0) {
      *prot = VL_PROT_BINARY ;
    }
    else {
      *prot = VL_PROT_UNKNOWN ;
    }
    cpt += 3 ;
  }
  return (char*) cpt ;
}

/** ------------------------------------------------------------------
 ** @brief Get protocol name.
 **
 ** @param prot protocol id.
 **
 ** The function returns a pointer to the symbolic name of the protocol
 ** @a prot (see the @a vl-file-protocols "protocols" list).
 **
 ** @return pointer to the protocol name. The function returns a null
 ** pointer if the protocol code is unknown.
 **/

VL_EXPORT char const *
vl_string_protocol_name (int prot)
{
  switch (prot) {
  case VL_PROT_ASCII:
    return "ascii" ;
  case VL_PROT_BINARY:
    return "bin" ;
  case VL_PROT_NONE :
    return "" ;
  default:
    return 0 ;
  }
}


/** ------------------------------------------------------------------
 ** @brief Extract base of file name.
 **
 ** @param dst   destination buffer.
 ** @param n     size of destination buffer.
 ** @param src   string to process.
 ** @param n_ext maximum number of extensions to strip.
 **
 ** The function removes leading path and (at most) @c n_ext trailing
 ** extensions from the string @a src and writes the result to the
 ** buffer @a dst of length @a n.
 **
 ** The leading path is the suffix that ends with the last occurrence
 ** of the <tt>`\'</tt> or <tt>`/'</tt> characters. A trailing extension
 ** is the shortest suffix starting with the <tt>`.'</tt> character.
 **
 ** @return length of the result.
 ** @sa vl-stringop-err.
 **/

VL_EXPORT int
vl_string_basename (char *dst, int n, char const *src, int n_ext)
{

  char c ;
  int  k = 0 ;
  int  beg, end ;

  /* find beginning */
  beg = 0 ;
  for (k = 0 ; (c = src[k]) ; ++ k) {
    if (c == '\\' || c == '/') beg = k + 1 ;
  }

  /* find ending */
  end = strlen(src) ;
  for (k = end ; k > beg ; --k) {
    if (src [k - 1] == '.' && n_ext > 0) {
      -- n_ext ;
      end = k - 1 ;
    }
  }

  return vl_string_copy_sub (dst, n, src + beg, src + end) ;
}

/** -------------------------------------------------------------------
 ** @brief Replace wildcard by string
 **
 ** @param dst   output buffer.
 ** @param n     size of output buffer.
 ** @param src   input string.
 ** @param wild  widcard character.
 ** @param esc   escape character.
 ** @param repl  replacement string.
 **
 ** The function replaces the occurrence of the specified wildcard
 ** character @a wild by the string @a repl. The result is written to
 ** the buffer @a dst of size @a n.
 **
 ** Wildcard characters may be escaped by preceding them by the @a
 ** esc character. More in general, anything following an occurrence of
 ** @a esc character is copied verbatim. To disable the escape
 ** characters simply set @a esc to 0.
 **
 ** @return length of the result.
 ** @sa @ref vl-stringop-err.
 **/

VL_EXPORT int
vl_string_replace_wildcard (char *dst, int n,
                            char const *src,
                            char wild,
                            char esc,
                            char const *repl)
{
  char    c ;
  int     k = 0 ;
  vl_bool escape = 0 ;

  while (( c = *src++ )) {

    /* enter escape mode ? */
    if (! escape && c == esc) {
      escape = 1 ;
      continue ;
    }

    /* wildcard or regular? */
    if (! escape && c == wild) {
      char const *repl_ = repl ;
      while (( c = *repl_++ )) {
        if (dst && k < n - 1) dst [k]  = c ;
        ++ k ;
      }
    }
    /* regular character */
    else {
      if (dst && k < n - 1) dst [k] = c ;
      ++ k ;
    }

    escape = 0 ;
  }

  /* add trailing 0 */
  if (n > 0) dst [VL_MIN(k, n - 1)] = 0 ;
  return  k ;
}

/** -------------------------------------------------------------------
 ** @brief Copy string
 **
 ** @param dst output buffer.
 ** @param n   size of output buffer.
 ** @param src string to copy.
 **
 ** The function copies the string @a src to the buffer @a dst of size
 ** @a n.
 **
 ** @return length of @a src.
 ** @sa vl-stringop-err.
 **/

VL_EXPORT int
vl_string_copy (char *dst, int n, char const *src)
{

  char c ;
  int  k = 0 ;

  while (( c = *src++ )) {
    if (dst && k < n - 1) dst [k] = c ;
    ++ k ;
  }

  /* finalize */
  if (n > 0) dst [VL_MIN(k, n - 1)] = 0 ;
  return  k ;
}

/** -------------------------------------------------------------------
 ** @brief Copy substring
 **
 ** @param dst output buffer.
 ** @param n   size of output buffer.
 ** @param beg start of the source string.
 ** @param end end of the source string.
 **
 ** The function copies the substring from at @a beg to @a end to the
 ** buffer @a dst of size @a n. The substring terminates when either a
 ** @c NUL is found or @a end is reached (the character pointed by @a
 ** end is <em>not</em> included).
 **
 ** @return length of the result.
 ** @sa vl-stringop-err.
 **/

VL_EXPORT int
vl_string_copy_sub (char *dst, int n,
                    char const *beg,
                    char const *end)
{
  char c ;
  int  k = 0 ;

  while (beg < end && (c = *beg++)) {
    if (dst && k < n - 1) dst [k] = c ;
    ++ k ;
  }

  /* finalize */
  if (n > 0) dst [VL_MIN(k, n - 1)] = 0 ;
  return  k ;
}

/** ------------------------------------------------------------------
 ** @brief Search character in reversed order
 **
 ** @param beg substring beginning.
 ** @param end substring ending.
 ** @param c   character to search for.
 **
 ** The function searches for the last occurrence of the character @a c
 ** in the substring from @a beg to @a end (the latter not being included).
 **
 ** @return pointer to last occurrence of @a c, or 0 if none.
 **/

VL_EXPORT char *
vl_string_find_char_rev (char const *beg, char const* end, char c)
{
  while(end -- != beg) {
    if(*end == 'c')
      return (char*) end ;
  }
  return 0 ;
}


/** -------------------------------------------------------------------
 ** @brief Calculate string length
 **
 ** @param str string.
 ** @return string length.
 **/

VL_EXPORT int
vl_string_length (char const *str)
{
  int i ;
  for(i = 0 ; str [i] ; ++i) ;
  return i ;
}

/** -------------------------------------------------------------------
 ** @brief Compare strings case-insensitive
 **
 ** @param s1 fisrt string.
 ** @param s2 second string.
 **
 ** @return 0 if the strings are equal, >0 if the first string is
 ** greater (in lexicographical order) and <0 otherwise.
 **/

VL_EXPORT int
vl_string_casei_cmp (const char *s1, const char *s2)
{
  while (tolower((unsigned char)*s1) ==
         tolower((unsigned char)*s2))
  {
    if (*s1 == 0)
      return 0;
    s1++;
    s2++;
  }
  return
    (int)tolower((unsigned char)*s1) -
    (int)tolower((unsigned char)*s2) ;
}
