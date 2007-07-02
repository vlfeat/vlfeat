/** @file     generic-driver.h
 ** @author   Andrea Vedaldi
 ** @brief    Support for command line drivers - Definition.
 ** @internal
 **
 ** This file contains support code which is shared by the command
 ** line drivers.
 **/

/* AUTORIGHTS */

#ifndef VL_GENERIC_DRIVER
#define VL_GENERIC_DRIVER

#include <vl/generic.h>
#include <vl/stringop.h>

#include <stdio.h>

/** @brief File meta information
 **/
struct _VlFileMeta
{
  vl_bool active ;          /**< Is the file active? */
  char    pattern [1024] ;  /**< File name pattern */
  int     protocol ;        /**< File protocol */

  char    name [1024] ;     /**< Current file name */
  FILE *  file ;            /**< Current file stream */
} ;


/** @brief File meta information type
 ** @see ::_VlFileMeta
 **/
typedef struct _VlFileMeta VlFileMeta ;

/* ----------------------------------------------------------------- */
/** @brief Parse argument for file meta information
 **
 ** @param optarg  argument to parse.
 ** @param out     ::vl_driver_out structure to fill.
 ** @param int     error code.
 **
 ** The function parses the string @a optarg to fill the structure @a
 ** fm. @a optarg is supposed to be composed of two parts: a file
 ** protocol specification and a file pattern. Then the function:
 **
 ** - Sets VlFileMeta::active to true.
 ** - Sets VlFileMeta::protocol to the file protocol id (if any).
 ** - Sets VlFileMeta::pattern  to the file pattern.
 **
 ** @return error code. The funciton may fail either because the file
 ** protocol is not recognized (::VL_ERR_BAD_ARG) or because the file
 ** pattern is too long to be stored (::VL_ERR_OVERFLOW).
 **/
static int
vl_file_meta_parse (VlFileMeta * fm, char const * optarg) 
{
  int q ;

  fm -> active = 1 ;

  if (optarg) {
    int protocol ;
    char const * arg = vl_string_parse_protocol (optarg, &protocol) ;
        
    /* parse the (optional) protocol part */
    switch (protocol) {
    case VL_PROT_UNKNOWN :
      return VL_ERR_BAD_ARG  ;

    case VL_PROT_ASCII  :
    case VL_PROT_BINARY :
      fm -> protocol = protocol ;
      break ;

    case VL_PROT_NONE :
      break ;
    }

    q = vl_string_copy 
      (fm -> pattern, sizeof (fm -> pattern), arg) ;
    
    if (q >= sizeof (fm -> pattern)) {
      return VL_ERR_OVERFLOW ;
    }
    
  }
  return VL_ERR_OK ;
}

/* ----------------------------------------------------------------- */
/** @brief Open the file associated to meta information
 **
 ** @param fm        File meta information.
 ** @param basename  Basename.
 ** @param mode      Opening mode (as in @c fopen).
 **
 ** @return error code.
 **/
static int
vl_file_meta_open (VlFileMeta * fm, char const * basename, char const * mode) 
{
  int q ;
  
  if (! fm -> active) {
    return VL_ERR_OK ;
  }

  q = vl_string_replace_wildcard (fm -> name, 
                                  sizeof(fm -> name),
                                  fm -> pattern, 
                                  '%', '\0', 
                                  basename) ;

  if (q >= sizeof(fm -> name)) {
    return VL_ERR_OVERFLOW ;
  }
  
  if (fm -> active) {
    fm -> file = fopen (fm -> name, mode) ;
    if (! fm -> file) {
      return VL_ERR_IO ;
    }
  }
  return VL_ERR_OK ;
}

/* ----------------------------------------------------------------- */
/** @brief Close the file associated to meta information
 **
 ** @param fm File meta information.
 **/
static void
vl_file_meta_close (VlFileMeta * fm) 
{
  if (fm -> file) {
    fclose (fm -> file) ;
    fm -> file = 0 ;
  }
}

/* VL_GENERIC_DRIVER */
#endif
