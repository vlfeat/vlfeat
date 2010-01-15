/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <stdio.h>
#include <assert.h>

#include <vl/stringop.h>

int
main (int argc, char *argv[])
{    
  int q ;
  int err ;
  char const *repl = "vision" ;
  char const *str  = "*: the string '*' shold be equal to 'vision' but not to '\\**\\*'" ;
  char const *t1   = "a/b/c/d.tar.gz" ;
  char const *t2   = "ascii:bin:ascii:bin:bho:bha:bin:/which remains/" ;
  char const *t3   = "t" ;
  char const *next ;

  enum { buf_len = 1024 } ;
  char buf [buf_len] ;

  enum { sml_len = 2 } ;
  char sml [sml_len] ;

  /* -------------------------------------------------------------- */
  printf ("testing vl_string_copy\n") ;

  q = vl_string_copy (sml, sml_len, t3) ;
  
  err = (q >= sml_len) ;

  printf ("%s", sml) ;
  printf ("\nwrote %d error %d\n", q, err) ;

  q = vl_string_copy (sml, sml_len, t1) ;
  
  err = (q >= sml_len) ;

  printf ("%s", sml) ;
  printf ("\nwrote %d error %d\n", q, err) ;


  /* -------------------------------------------------------------- */
  printf ("testing vl_string_replace_wildcard\n") ;

  q = vl_string_replace_wildcard (buf, buf_len,
                                  str, '*', '\\', repl) ;
  
  err = (q >= buf_len) ;
  
  printf ("%s", buf) ;
  printf ("\nwrote %d error %d\n", q, err) ;

  q = vl_string_replace_wildcard (sml, sml_len,
                                  str, '*', '\\', repl) ;

  err = (q >= sml_len) ;
  
  printf ("%s", sml) ;
  printf ("\nwrote %d error %d\n", q, err) ;

  q = vl_string_replace_wildcard (0,0,
                                  str, '*', '\\', repl) ;

  err = (q >= 0) ;

  printf ("\nwrote %d error %d\n", q, err) ;

  /* -------------------------------------------------------------- */
  printf ("\ntesting vl_string_basename\n") ;

  q = vl_string_basename (buf, buf_len, t1, 1) ;

  err = (q >= buf_len) ;

  printf("'%s' -> '%s' (wrote %d, error %d)\n", t1, buf, q, err) ;

  q = vl_string_basename (sml, sml_len, t1, 1) ;

  err = (q >= buf_len) ;

  printf("'%s' -> '%s' (wrote %d, error %d)\n", t1, sml, q, err) ;


  /* -------------------------------------------------------------- */
  printf ("\ntesting vl_string_parse_protocol\n") ;

  next = t2 ;
  while(next = vl_string_parse_protocol(next, &q), q != VL_PROT_NONE) {
    printf("protocol: '%s'\n", vl_string_protocol_name (q)) ;
  }
  
  printf("%s\n", next) ;
  return 0 ;
}

