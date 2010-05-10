/** @author   Andrea Vedaldi
 ** @brief    Support for command line drivers - Definition.
 ** @internal
 **
 ** This file contains support code which is shared by the command
 ** line drivers.
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#ifndef VL_GENERIC_DRIVER
#define VL_GENERIC_DRIVER

#include <vl/generic.h>
#include <vl/stringop.h>

#include <stdio.h>
#include <assert.h>

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
 ** @param self      structure to initalize.
 **
 ** The function parses the string @a optarg to fill the structure @a
 ** self. @a optarg is supposed to be composed of two parts: a file
 ** protocol specification and a file pattern. Then the function:
 **
 ** - Sets VlFileMeta::active to true.
 ** - Sets VlFileMeta::protocol to the file protocol id (if any).
 ** - Sets VlFileMeta::pattern  to the file pattern (if any).
 **
 ** @return error code. The funciton may fail either because the file
 ** protocol is not recognized (::VL_ERR_BAD_ARG) or because the file
 ** pattern is too long to be stored (::VL_ERR_OVERFLOW).
 **/
static int
vl_file_meta_parse (VlFileMeta * self, char const * optarg)
{
  vl_size q ;
  self->active = 1 ;

  if (optarg) {
    int protocol ;
    char const * arg = vl_string_parse_protocol (optarg, &protocol) ;

    /* parse the (optional) protocol part */
    switch (protocol) {
    case VL_PROT_UNKNOWN :
      return VL_ERR_BAD_ARG  ;

    case VL_PROT_ASCII  :
    case VL_PROT_BINARY :
      self->protocol = protocol ;
      break ;

    case VL_PROT_NONE :
      break ;
    }

    if (vl_string_length (arg) > 0) {
      q = vl_string_copy
        (self->pattern, sizeof (self->pattern), arg) ;

      if (q >= sizeof(self->pattern)) {
        return VL_ERR_OVERFLOW ;
      }
    }

  }
  return VL_ERR_OK ;
}

/* ----------------------------------------------------------------- */
/** @brief Open the file associated to meta information
 **
 ** @param self        File meta information.
 ** @param basename  Basename.
 ** @param mode      Opening mode (as in @c fopen).
 **
 ** @return error code. The error may be either either
 ** ::VL_ERR_OVERFLOW if the file name is too long or to ::VL_ERR_IO
 ** if the file cannot be opened.
 **/

static int
vl_file_meta_open (VlFileMeta * self, char const * basename, char const * mode)
{
  vl_size q ;

  if (! self->active) {
    return VL_ERR_OK ;
  }

  q = vl_string_replace_wildcard (self->name,
                                  sizeof(self->name),
                                  self -> pattern,
                                  '%', '\0',
                                  basename) ;

  if (q >= sizeof(self->name)) {
    vl_get_thread_specific_state()->lastError = VL_ERR_OVERFLOW ;
    return -1 ;
  }

  if (self->active) {
    self->file = fopen (self->name, mode) ;
    if (! self->file) {
      vl_get_thread_specific_state()->lastError = VL_ERR_IO ;
      return -1 ;
    }
  }
  return 0 ;
}

/* ----------------------------------------------------------------- */
/** @brief Close the file associated to meta information
 **
 ** @param self File meta information.
 **/
static void
vl_file_meta_close (VlFileMeta * self)
{
  if (self -> file) {
    fclose (self -> file) ;
    self -> file = 0 ;
  }
}

/* ----------------------------------------------------------------- */
/** @brief Write double to file
 **
 ** @param self   File meta information.
 ** @param x    Datum to write.
 **
 ** @return error code. The function returns ::VL_ERR_ALLOC if the
 ** datum cannot be written.
 **/

VL_INLINE int
vl_file_meta_put_double (VlFileMeta * self, double x)
{
  int err ;
  size_t n ;
  double y ;

  switch (self -> protocol) {

  case VL_PROT_ASCII :
    err = fprintf (self -> file, "%g ", x) ;
    break ;

  case VL_PROT_BINARY :
    vl_swap_host_big_endianness_8 (&y, &x) ;
    n = fwrite (&y, sizeof(double), 1, self -> file) ;
    err = n < 1 ;
    break ;

  default :
    abort() ;
  }

  return err ? VL_ERR_ALLOC : VL_ERR_OK ;
}

/* ----------------------------------------------------------------- */
/** @brief Write uint8 to file
 **
 ** @param self   File meta information.
 ** @param x    Datum to write.
 **
 ** @return error code. The function returns ::VL_ERR_ALLOC if the
 ** datum cannot be written.
 **/

VL_INLINE int
vl_file_meta_put_uint8 (VlFileMeta *self, vl_uint8 x)
{
  size_t n ;
  int err ;

  switch (self -> protocol) {

  case VL_PROT_ASCII :
    err = fprintf (self -> file, "%d ", x) ;
    if (err) return VL_ERR_ALLOC ;
    break ;

  case VL_PROT_BINARY :
    n = fwrite (&x, sizeof(vl_uint8), 1, self -> file) ;
    if (n < 1) return VL_ERR_ALLOC ;
    break ;

  default :
    abort() ;
  }

  return VL_ERR_OK ;
}

/* ----------------------------------------------------------------- */
/** @brief Read double from file
 **
 ** @param self  File meta information.
 ** @param x   Datum read.
 **
 ** @return error code. The function returns ::VL_ERR_EOF if the
 ** end-of-file is reached and ::VL_ERR_BAD_ARG if the file is
 ** malformed.
 **/

VL_INLINE int
vl_file_meta_get_double (VlFileMeta *self, double *x)
{
  int err ;
  size_t n ;
  double y ;

  switch (self -> protocol) {

  case VL_PROT_ASCII :
    err = fscanf (self -> file, " ") ;
    if (err == EOF) return VL_ERR_EOF ;
    err = fscanf (self -> file, "%lg", x) ;
    if (err == EOF) return VL_ERR_EOF ;
    if (err <  1  ) return VL_ERR_BAD_ARG ;
    break ;

  case VL_PROT_BINARY :
    n = fread (&y, sizeof(double), 1, self -> file) ;
    if (n < 1) return VL_ERR_BAD_ARG ;
    vl_swap_host_big_endianness_8 (x, &y) ;
    break ;

  default :
    abort() ;
  }

  return VL_ERR_OK ;
}



/* VL_GENERIC_DRIVER */
#endif
