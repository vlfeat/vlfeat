/** @file    pgm.c
 ** @author  Andrea Vedaldi
 ** @brief   Portable graymap format (PGM) parser - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#include "pgm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** -------------------------------------------------------------------
 ** @internal
 ** @brief Reremove all characters to the next new-line
 ** @param f file to strip.
 ** @return number of characted removed.
 **/

static int
remove_line(FILE* f) 
{
  int count = 0 ;
  int c ;
  
  while (1) {
    c = fgetc(f) ;
    ++ count ;

    switch(c) {
    case '\n' :
      goto quit_remove_line ;
      
    case EOF :
      -- count ;
      goto quit_remove_line ;
    }
  }
 quit_remove_line :
  return count ;
}

/** -------------------------------------------------------------------
 ** @internal
 ** @brief Reremove white-spaces and comments
 ** @param f file to strip.
 ** @return number of characted removed.
 **/

static int
remove_blanks(FILE* f) 
{
  int count = 0 ;
  int c ;
  
  while (1) {
    c = fgetc(f) ;

    switch(c) {
      
    case '\t' : case '\n' : 
    case '\r' : case ' '  :
      ++ count ;
      break ;

    case '#' :
      count += 1 + remove_line(f) ;
      break ;
      
    case EOF :
      goto quit_remove_blanks ;
      
    default:
      ungetc(c, f) ;
      goto quit_remove_blanks ;
    }
  }
 quit_remove_blanks:
  return count ;
}

/** -------------------------------------------------------------------
 ** @brief Get PGM image data size
 **
 ** @param im PGM image descriptor.
 **
 ** The functions returns the number of pixels of the PGM image @a im.
 **
 ** To calculate the image data size in bytes, this value must be
 ** multiplied by the number of byte per pixels (see
 ** ::vl_pgm_get_bpp()).
 **
 ** @return number of pixels of the image.
 **/

int
vl_pgm_get_data_size (VlPgmImage const *im)
{
  return im->width * im->height ;
}

/** -------------------------------------------------------------------
 ** @brief Get PGM image bytes per pixel
 **
 ** @param im PGM image descriptor.
 **
 ** The functions returns the number of bytes for each pixel of the
 ** PGM image @a im.
 **
 ** @return number of bytes per pixel.
 **/

int
vl_pgm_get_bpp (VlPgmImage const *im)
{
  return (im->max_value >= 256) + 1 ;
}

/** ---------------------------------------------------------------- */ 
/** @brief Read PGM header
 **
 ** @param f  input file.
 ** @param im image structure to fill.
 **
 ** The function extracts from the file @a f the meta-data section of
 ** an image encoded in PGM format. The function fills the structure
 ** VlPgmImage according.
 **
 ** @return error code. The function sets ::vl_err_no to
 ** ::VL_PGM_INV_HEAD or ::VL_PGM_INV_META depending wether the error
 ** occurred in decoding the header or meta section of the PGM file.
 **/

int
vl_pgm_extract_head (FILE* f, VlPgmImage *im)
{
  char     magic [2] ;
  int      c ;
  int      is_raw ;
  int      width ;
  int      height ;
  int      max_value ;
  int      sz ;
  vl_bool  good ;
  

  /* ------------------------------------------------------------------
   *                                                 check magic number
   * --------------------------------------------------------------- */
  sz = fread(magic, 1, 2, f) ;
  
  if (sz < 2) {
    vl_err_no  = VL_ERR_PGM_INV_HEAD ;
    return -1 ;
  }

  good = magic [0] == 'P' ;
  
  switch (magic [1]) {
  case '2' : /* ASCII format */
    is_raw = 0 ;
    break ;
    
  case '5' : /* RAW format */
    is_raw = 1 ;
    break ;
    
  default :
    good = 0 ;
    break ;
  }

  if( ! good ) {
    vl_err_no = VL_ERR_PGM_INV_HEAD ;
    return -1 ;
  }

  /* ------------------------------------------------------------------
   *                                     parse width, height, max_value
   * --------------------------------------------------------------- */  
  good = 1 ;

  c = remove_blanks(f) ;
  good &= c > 0 ;

  c = fscanf(f, "%d", &width) ;
  good &= c == 1 ;

  c = remove_blanks(f) ;
  good &= c > 0 ;
  
  c = fscanf(f, "%d", &height) ;
  good &= c == 1 ;

  c = remove_blanks(f) ;
  good &= c > 0 ;

  c = fscanf(f, "%d", &max_value) ;
  good &= c == 1 ;

  /* must end with a single blank */
  c = fgetc(f) ;
  good &= 
    c == '\n' ||
    c == '\t' ||
    c == ' '  ||
    c == '\r' ;
  
  if(! good) {
    vl_err_no = VL_ERR_PGM_INV_META ;
    return vl_err_no ;
  }

  if(! max_value >= 65536) {
    vl_err_no = VL_ERR_PGM_INV_META ;
    return vl_err_no ;
  }

  /* exit */
  im-> width     = width ;
  im-> height    = height ;
  im-> max_value = max_value ;
  im-> is_raw    = is_raw ;
  return 0 ;
}

/** -------------------------------------------------------------------
 ** @brief Read PGM data
 **
 ** @param f    input file.
 ** @param im   PGM image descriptor.
 ** @param data data buffer to fill.
 **
 ** The function extracts from the file @a f the data section of an
 ** image encoded in PGM format. The function fills the buffer @a data
 ** according.
 **
 ** @return error code.
 **/
  
int
vl_pgm_extract_data (FILE* f, VlPgmImage const *im, void *data)
{
  int bpp       = vl_pgm_get_bpp       (im) ;
  int data_size = vl_pgm_get_data_size (im) ;
  int c ;
  vl_bool good = 1 ;

  /* ------------------------------------------------------------------
   *                                                          read data
   * --------------------------------------------------------------- */  

  /* 
     In RAW mode we read directly an array of bytes or shorts.  In
     the latter case, however, we must take care of the
     endianess. PGM files are sotred in big-endian format. If our
     architecture is little endian, we must do a conversion.
  */  
  if (im->is_raw) {

    c = fread( data,
               bpp,
               data_size,
               f ) ;
    good = (c == data_size) ;
    
    /* adjust endianess */
    if( bpp == 2 && ! vl_get_endianness() == VL_BIG_ENDIAN ) {
      int i ;
      vl_uint8 *pt = (vl_uint8*) data ;
      for(i = 0 ; i < 2 * data_size ; i += 2) {
        vl_uint8 tmp = pt [i] ;
        pt [i]   = pt [i+1] ;
        pt [i+1] = tmp ;
      }      
    }    
  }
  /* 
     In ASCII mode we read a sequence of decimal numbers separated
     by whitespaces.
  */  
  else {    
    int i ;
    int unsigned v ;
    for(good = 1, i = 0 ; 
        i < data_size && good ; 
        ++i) {
      c = fscanf(f, " %ud", &v) ;
      if (bpp == 1) {
        * ((vl_uint8* )  data + i) = (vl_uint8)  v ;
      } else {
        * ((vl_uint16*)  data + i) = (vl_uint16) v ;
      }
      good &= c == 1 ;
    }
  }
  
  if(! good ) {
    vl_err_no = VL_ERR_PGM_INV_DATA ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "Invalid PGM data") ;
    return vl_err_no ;
  }
  return 0 ;
}

/** -------------------------------------------------------------------
 ** @brief Write a PGM image
 **
 ** @param f output file.
 ** @param im   PGM image meta-data.
 ** @param data image data.
 ** @return error code.
 **/

int
vl_pgm_insert(FILE* f, VlPgmImage const *im, void *data)
{
  int bpp       = vl_pgm_get_bpp       (im) ;
  int data_size = vl_pgm_get_data_size (im) ;
  int c ; 
  
  /* write preamble */
  fprintf(f,
          "P5\n%d\n%d\n%d\n",
          im->width,
          im->height,
          im->max_value) ;

  /* take care of endianness */
  if (bpp == 2 && ! vl_get_endianness() == VL_BIG_ENDIAN) {
    int i ;
    vl_uint8* temp = vl_malloc (2 * data_size) ;
    memcpy(temp, data, 2 * data_size) ;
    for(i = 0 ; i < 2 * data_size ; i += 2) {
      vl_uint8 tmp = temp [i] ;
      temp [i]   = temp [i+1] ;
      temp [i+1] = tmp ;
    }
    c = fwrite(temp, 2, data_size, f) ;
    vl_free (temp) ;
  }
  else {
    c = fwrite(data, bpp, data_size, f) ;
  }
  
  if(c != data_size) {
    vl_err_no = VL_ERR_PGM_IO ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "Error writing PGM data") ;
    return vl_err_no ;
  }
  return 0 ;
}
