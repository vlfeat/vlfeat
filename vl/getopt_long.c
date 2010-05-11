/** @file    getopt_long.c
 ** @author  Andrea Vedaldi
 ** @brief   getopt_long - Definition
 **/

/* AUTORIGHTS
Copyright (C) 2007-10 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available under the terms of the
GNU GPLv2, or (at your option) any later version.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "generic.h"
#include "getopt_long.h"

VL_EXPORT int    opterr = 1 ;
VL_EXPORT int    optind = 1 ; 
VL_EXPORT int    optopt ; 
VL_EXPORT char * optarg ; 
VL_EXPORT int    optreset;

#define BADCH	'?'
#define BADARG	':'
#define EEND    -1
#define EMSG	""

/** @brief Parse long options - BSD style
 **
 ** @param argc
 ** @param argv
 ** @param optstring  abbreviated options 
 ** @param longopts   list of long options.
 ** @param longindex  index of current option in @a longopts.
 **
 ** This function extract long and short options from the argument
 ** list @a argv of @a argc entries.
 **
 ** A short options sequence is introduced by a singe dash @c -.  Each
 ** short option is described by a single character in the @a
 ** optstring, possibly followed by a @c : character to denote a
 ** (mandatory) argument of the short option. A short option with an
 ** argument cannot appear in the middle of a short option sequence,
 ** but only at the end.
 **
 ** A long option is introduced by a double dash @c --. Each long
 ** option is described by an instance of the ::option structure in the
 ** @a longopts table (the last entry must be filled with zeroes to
 ** denote the end).
 ** 
 **
 ** @return the code of the next option. Illegal options and missing
 ** arguments cause the function to skip the option and return '?'. If
 ** ::opterr is true (default), the function prints an error message
 ** to @a stderr. Finally, if @a optstring has a leading @c :, then
 ** error messages are suppressed and a missing argument causes @a :
 ** to be returned.

 **/

VL_EXPORT
int
getopt_long(int argc, char *const argv[],
            const char *optstring,
            const struct option * longopts,
            int *longindex)
{
  static char *place = EMSG;	/* option letter processing */
  static int   optbegin  = 0 ;
  static int   optend    = 0 ;
  char        *oli;		/* option letter list index */
  int          has_colon = 0 ;
  int          ret_val   = 0 ;

  /* 
     A semicolon at the beginning of optstring has a special meaning.
     If we find one, we annote and remove it.
  */
  has_colon = optstring && optstring[0] == ':' ;
  if (has_colon) ++ optstring ;

  /*
   Here we are either processing a short option sequence or 
   we start processing a new option. This is indicated by optreset.   
  */

  if (optreset || *place == '\0') {
    
    /* --------------------------------------------------------------
     *                                Look for next short/long option
     * ----------------------------------------------------------- */
    optreset = 0 ;
    
    /* no more arguments ? */
    if (optind >= argc) {
      place = EMSG ;
      return -1 ;
    }
    
    /* next argument that may hold an option */
    optbegin = optind ;

    /* --------------------------------------------------------------
     *                                    look for an option to parse
     * ----------------------------------------------------------- */
    
  parse_option_at_optbegin :    

    /* place points to the candidate option */
    place = argv [optbegin] ;

    /* an option is introduced by '-' */
    if (place [0] != '-') {
      /* this argument is not an option: try next argument */
      ++ optbegin ;
      if (optbegin >= argc) {
        /* no more arguments to look for options */
        place = EMSG ;
        return -1 ;
      }      
      goto parse_option_at_optbegin ;
    }

    /* consume leading `-' */
    ++ place ;

    /* assume the option is composed of one argument only */
    optend = optbegin + 1 ;

    /* assume no argument */
    optarg = 0 ;

    /* --------------------------------------------------------------
     *                                                    option `--'
     * ----------------------------------------------------------- */
    
    /* this special option (void long option) ends the option processing */
    if (place[0]        && 
        place[0] == '-' && 
        place[1] == '\0') {

      optind  = optend ;
      place   = EMSG ;
      ret_val = -1 ;
      goto done_option ;
    }

    /* --------------------------------------------------------------
     *                                                    long option
     * ----------------------------------------------------------- */

    if (place[0]        && 
        place[0] == '-' && 
        place[1] ) {

      size_t namelen ;
      int i ;
      
      /* consume second `-' */
      ++ place ;
      
      /* count characters before `=' */
      namelen = strcspn(place, "=") ;
      
      /* scan longopts for this option */
      for (i = 0 ; longopts[i].name != NULL ; ++ i) {
        
        if (strlen  (       longopts[i].name) == namelen &&
            strncmp (place, longopts[i].name, namelen) == 0 ) {
          
          /* save back long option index */
          if (longindex) *longindex = i ;
          
          /* process long option argument */
          if (longopts[i].has_arg == required_argument ||
              longopts[i].has_arg == optional_argument) {
            
            /* --option=value style */
            if (place[namelen] == '=') {
              optarg = place + namelen + 1 ;
            }
            
            /* --option value style (only required_argument) */
            else if (longopts[i].has_arg == required_argument) {
              /* missing argument ? */
              if (optbegin >= argc - 1) {
                if (! has_colon && opterr)
                  fprintf(stderr,
                          "%s: option requires an argument -- %s\n",
                          argv[0], place);
                place   = EMSG ;
                ret_val = has_colon ? BADARG : BADCH ;
                goto done_option ;
              }              
              optarg = argv [optend] ;
              ++ optend ;
            }
          }
        
          /* determine return value */ 
          if (longopts[i].flag == NULL) {
            ret_val = longopts[i].val ;
          }
          else {
            *longopts[i].flag = longopts[i].val;
            ret_val = 0 ;
          }
          
          /* mark sequence closed */
          place = EMSG ;
          goto done_option ;          
        } /* if match */

      } /* scan longoptions */
      
      /* no matching option found */
      if (! has_colon && opterr)
        fprintf(stderr,
                "%s: illegal option -- %s\n", argv[0], place) ;
      place   = EMSG ;
      ret_val = BADCH ;
      goto done_option ;
    }
  } /* end new option */
  
    /* --------------------------------------------------------------
     *                                   finish short option sequence
     * ----------------------------------------------------------- */        
  optopt = (int) *place++ ;
  
  /* search charcater in option list */
  oli = strchr(optstring, optopt);
    
  /* short option not found */    
  if (!oli) {
    
    if (! has_colon && opterr)
      fprintf(stderr,
              "%s: illegal option -- %c\n", 
              argv[0], optopt);
    
    if (*place) {
      /* more short options in the list */
      return BADCH ;
    }
    
    else {
      /* error occured as last option in the list */
      place   = EMSG ;
      ret_val = BADCH ;
      goto done_option ;
    }
  } /* end short option not found */
  
  if (oli[1] != ':') {
    /* short option with no argument */
    
    if (*place) {
      /* more short options in the list */
      return optopt ;
    }
    else {
      /* last option in the list */
      place   = EMSG ;
      ret_val = optopt ;
      goto done_option ;
    }
  
  } else {
    /* short option with argument */
    
    /* -ovalue style */
    if (*place) {
      optarg  = place ;
      place   = EMSG ;
      ret_val = optopt ;
      goto done_option ;
    }
    /* -o value style: missing argument */
    else if (optbegin >= argc - 1) {
      if (! has_colon && opterr)
        fprintf(stderr,
                "%s: option requires an argument -- %c\n",
                argv[0], optopt);
      place   = EMSG ;
      ret_val = has_colon ? BADARG : BADCH ;
      goto done_option ;
    }
    
    /* -o value style: process argument */
    optarg = argv [optend] ;
    ++ optend ;
    place   = EMSG ;
    ret_val = optopt ;
    goto done_option ;
  } /* short with argument */
  
 done_option :  
  {
    int pos = optend - optbegin ;  /* n of circular shifts */
    int c   = pos ;
    
    while (c --) {
      int i ;
      char *tmp = argv [optend - 1] ;
      for (i = optend - 1 ; i > optind ; -- i) {
        ((char**)argv) [i] = argv [i-1] ;
      }
      ((char**)argv) [optind] = tmp ;
    }
    optind += pos ;
  }

  return ret_val ;
}
