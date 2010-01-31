/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include "check.h"
#include <vl/stringop.h>
#include <string.h>

int
main (int argc VL_UNUSED, char *argv[] VL_UNUSED)
{
  vl_size q ;
  int err, protocol ;
  char const *repl  = "vision" ;
  char const *str   = "*: * '\\*' *" ;
  char const *subst = "vision: vision '*' vision" ;
  char const *t1   = "a/b/c/d.tar.gz" ;
  char const *t3   = "t" ;
  char const *next ;

  enum { buf_len = 1024 } ;
  char buf [buf_len] ;

  enum { sml_len = 2 } ;
  char sml [sml_len] ;

  /* -------------------------------------------------------------- */

  /* t3 fits in the destination buffer */
  q = vl_string_copy (sml, sml_len, t3) ;
  err = (q >= sml_len) ;
  check (! err, "vl_string_copy") ;
  check (q == strlen(t3), "vl_string_copy") ;

  /* t1 does not fit in the destination buffer */
  q = vl_string_copy (sml, sml_len, t1) ;
  err = (q >= sml_len) ;
  check (err, "vl_string_copy") ;
  check (q == strlen(t1), "vl_string_copy") ;

  /* -------------------------------------------------------------- */

  /* check substitution with widcard */
  q = vl_string_replace_wildcard (buf, buf_len,
                                  str, '*', '\\', repl) ;
  err = (q >= buf_len) ;
  check (! err, "vl_string_replace_wildcard") ;
  check (strcmp(buf, subst) == 0, "vl_string_replace_wildcard") ;
  check (strlen(subst) == q, "vl_string_replace_wildcard") ;


  /* and the case in which the destination buffer is too small */
  q = vl_string_replace_wildcard (sml, sml_len,
                                  str, '*', '\\', repl) ;
  err = (q >= sml_len) ;
  check (err, "vl_string_replace_wildcard") ;
  check (strlen(subst) == q, "vl_string_replace_wildcard") ;

  /* and without output string */
  q = vl_string_replace_wildcard (0,0,
                                  str, '*', '\\', repl) ;
  err = (q >= sml_len) ;
  check (err, "vl_string_replace_wildcard") ;
  check (strlen(subst) == q, "vl_string_replace_wildcard") ;

  /* -------------------------------------------------------------- */
  q = vl_string_basename (buf, buf_len, "a/b/c/d.tar.gz", 0) ;
  err = (q >= buf_len) ;
  check (! err, "vl_string_basename") ;
  check (strcmp(buf, "d.tar.gz") == 0, "vl_string_basename") ;

  q = vl_string_basename (buf, buf_len, "a/b/c/d.tar.gz", 1) ;
  err = (q >= buf_len) ;
  check (! err, "vl_string_basename") ;
  check (strcmp(buf, "d.tar") == 0, "vl_string_basename") ;

  q = vl_string_basename (buf, buf_len, "a/b/c/d.tar.gz", 2) ;
  err = (q >= buf_len) ;
  check (! err, "vl_string_basename") ;
  check (strcmp(buf, "d") == 0, "vl_string_basename") ;

  q = vl_string_basename (buf, buf_len, "a/b/c/d.tar.gz", 3) ;
  err = (q >= buf_len) ;
  check (! err, "vl_string_basename") ;
  check (strcmp(buf, "d") == 0, "vl_string_basename") ;

  /* -------------------------------------------------------------- */
  next = "ascii://bin://ascii://bin://unkown1://unknown2://bin:///which remains/" ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_ASCII,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_BINARY,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_ASCII,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_BINARY,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_UNKNOWN,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_UNKNOWN,) ;
  next = vl_string_parse_protocol(next, &protocol) ;
  check (protocol == VL_PROT_BINARY,) ;
  check (strcmp(next, "/which remains/") == 0,"%s",next) ;

  check_signoff() ;
  return 0 ;
}

