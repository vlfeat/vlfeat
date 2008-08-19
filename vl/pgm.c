/** @file    pgm.c
 ** @author  Andrea Vedaldi
 ** @brief   Portable graymap format (PGM) parser - Definition
 **/

/* AUTORIGHTS
Copyright 2007 (c) Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

/** 
 @file pgm.h
 
 To extract a PGM image from an input stream, 
 first call ::vl_pgm_extract_head() to extract
 the image meta data (size and bit-depth). Then allocate the
 appropriate buffer to hold the image pixels and then 
 call ::vl_pgm_extract_data().
 
 To insert a PGM image to a file stream use ::vl_pgm_insert().
 
 To quickly read/write a PGM image from/to a given file, use
 ::vl_pgm_read_new() and ::vl_pgm_write(). To to the same
 from a buffer in floating point format use ::vl_pgm_read_new_f() and 
 ::vl_pgm_write_f().
 **/

#include "pgm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Remove all characters to the next new-line
 ** @param f file to strip.
 ** @return number of characters removed.
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

/** ------------------------------------------------------------------
 ** @internal
 ** @brief Remove white-spaces and comments
 ** @param f file to strip.
 ** @return number of characters removed.
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

/** ------------------------------------------------------------------
 ** @brief Get PGM image number of pixels.
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

VL_EXPORT
int
vl_pgm_get_npixels (VlPgmImage const *im)
{
  return im->width * im->height ;
}

/** ------------------------------------------------------------------
 ** @brief Get PGM image bytes per pixel
 **
 ** @param im PGM image descriptor.
 **
 ** The function returns the number of bytes for each pixel of the
 ** PGM image @a im.
 **
 ** @return number of bytes per pixel.
 **/

VL_EXPORT
int
vl_pgm_get_bpp (VlPgmImage const *im)
{
  return (im->max_value >= 256) + 1 ;
}

/** ------------------------------------------------------------------
 ** @brief Extract PGM header from stream
 **
 ** @param f  input file.
 ** @param im image structure to fill.
 **
 ** The function extracts from the file @a f the meta-data section of
 ** an image encoded in PGM format. The function fills the structure
 ** VlPgmImage according.
 **
 ** @return error code. The function sets ::vl_err_no to
 ** ::VL_PGM_INV_HEAD or ::VL_PGM_INV_META depending whether the error
 ** occurred in decoding the header or meta section of the PGM file.
 **/

VL_EXPORT
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
  

  /* -----------------------------------------------------------------
   *                                                check magic number
   * -------------------------------------------------------------- */
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

  /* -----------------------------------------------------------------
   *                                    parse width, height, max_value
   * -------------------------------------------------------------- */  
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

/** ------------------------------------------------------------------
 ** @brief Extract PGM data from stream
 **
 ** @param f    input file.
 ** @param im   PGM image descriptor.
 ** @param data data buffer to fill.
 **
 ** The function extracts from the file @a f the data section of an
 ** image encoded in PGM format. The function fills the buffer @a data
 ** according. The buffer @a data should be ::vl_pgm_get_npixels() by
 ** ::vl_pgm_get_bpp() bytes large.
 **
 ** @return error code.
 **/

VL_EXPORT
int
vl_pgm_extract_data (FILE* f, VlPgmImage const *im, void *data)
{
  int bpp       = vl_pgm_get_bpp       (im) ;
  int data_size = vl_pgm_get_npixels (im) ;
  int c ;
  vl_bool good = 1 ;

  /* -----------------------------------------------------------------
   *                                                         read data
   * -------------------------------------------------------------- */  

  /* 
     In RAW mode we read directly an array of bytes or shorts.  In
     the latter case, however, we must take care of the
     endianess. PGM files are sorted in big-endian format. If our
     architecture is little endian, we must do a conversion.
  */  
  if (im->is_raw) {

    c = fread( data,
               bpp,
               data_size,
               f ) ;
    good = (c == data_size) ;
    
    /* adjust endianess */
#if defined(VL_ARCH_LITTLE_ENDIAN)
    if (bpp == 2) {
      int i ;
      vl_uint8 *pt = (vl_uint8*) data ;
      for(i = 0 ; i < 2 * data_size ; i += 2) {
        vl_uint8 tmp = pt [i] ;
        pt [i]   = pt [i+1] ;
        pt [i+1] = tmp ;
      }      
    }
#endif
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

/** ------------------------------------------------------------------
 ** @brief Insert a PGM image into a stream
 **
 ** @param f output file.
 ** @param im   PGM image meta-data.
 ** @param data image data.
 ** @return error code.
 **/

VL_EXPORT
int
vl_pgm_insert(FILE* f, VlPgmImage const *im, void const *data)
{
  int bpp = vl_pgm_get_bpp (im) ;
  int data_size = vl_pgm_get_npixels (im) ;
  int c ; 
  
  /* write preamble */
  fprintf(f,
          "P5\n%d\n%d\n%d\n",
          im->width,
          im->height,
          im->max_value) ;

  /* take care of endianness */
#if defined(VL_ARCH_LITTLE_ENDIAN)
  if (bpp == 2) {
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
#endif
    c = fwrite(data, bpp, data_size, f) ;
#if defined(VL_ARCH_LITTLE_ENDIAN)  
  }
#endif
  
  if(c != data_size) {
    vl_err_no = VL_ERR_PGM_IO ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "Error writing PGM data") ;
    return vl_err_no ;
  }
  return 0 ;
}

/** ------------------------------------------------------------------
 ** @brief Read a PGM file
 **
 ** @param name file name.
 ** @param im a pointer to the PGM image structure to fill.
 ** @param data a pointer to the pointer to the allocated buffer.
 **
 ** The function reads a PGM image from file @a name and initializes the
 ** structure @a im and the buffer @a data accordingly. 
 **
 ** The ownership of the buffer @a data is transfered to the caller.
 ** @a data should be freed by means of ::vl_free().
 **
 ** @bug Only PGM files with 1 BPP are supported.
 **
 ** @return error code.
 **/

VL_EXPORT
int vl_pgm_read_new (char const *name, VlPgmImage *im, vl_uint8** data)
{
  int err = 0 ;

  FILE *f = fopen (name, "rb") ;
  
  if (! f) {
    vl_err_no = VL_ERR_PGM_IO ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "Error opening PGM file `%s' for reading", name) ;
    return vl_err_no ;
  }
  
  err = vl_pgm_extract_head(f, im) ;
  if (err) {
    fclose (f) ;
    return err ;
  }
  
  if (vl_pgm_get_bpp(im) > 1) {
    vl_err_no = VL_ERR_BAD_ARG ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "vl_pgm_read(): PGM with BPP > 1 not supported") ;
    return vl_err_no ;
  }
  
  *data = vl_malloc (vl_pgm_get_npixels(im) * sizeof(vl_uint8)) ;
  err = vl_pgm_extract_data(f, im, *data) ;
  
  if (err) {
    vl_free (data) ;
    fclose (f) ;
  }
  
  fclose (f) ;  
  return err ;
}

/** ------------------------------------------------------------------
 ** @brief Read floats from a PGM file
 **
 ** @param name file name.
 ** @param im a pointer to the PGM image structure to fill.
 ** @param data a pointer to the pointer to the allocated buffer.
 **
 ** The function reads a PGM image from file @a name and initializes the
 ** structure @a im and the buffer @a data accordingly. The buffer
 ** @a data is an array of floats in the range [0, 1].
 **
 ** The ownership of the buffer @a data is transfered to the caller.
 ** @a data should be freed by means of ::vl_free().
 **
 ** @bug Only PGM files with 1 BPP are supported.
 **
 ** @return error code.
 **/

VL_EXPORT
int vl_pgm_read_new_f (char const *name,  VlPgmImage *im, float** data)
{
  int err = 0 ;
  size_t npixels ;
  vl_uint8 *idata ;
  
  err = vl_pgm_read_new (name, im, &idata) ;
  if (err) {
    return err ;
  }
  
  npixels = vl_pgm_get_npixels(im) ;
  *data = vl_malloc (sizeof(float) * npixels) ;
  {
    size_t k ;
    float scale = 1.0f / im->max_value ;
    for (k = 0 ; k < npixels ; ++ k) (*data)[k] = scale * idata[k] ;
  }
  
  vl_free (idata) ;
  return err ;
}

/** ------------------------------------------------------------------
 ** @brief Write bytes to a PGM file
 **
 ** @param name file name.
 ** @param data data to write.
 ** @param width width of the image.
 ** @param height height of the image.
 **
 ** The function dumps the image @a data to the PGM file of the specified
 ** name. This is an helper function simplifying the usage of
 ** vl_pgm_insert().
 **
 ** @return error code.
 **/

VL_EXPORT
int vl_pgm_write (char const *name, vl_uint8 const* data, int width, int height)
{
  int err = 0 ;
  VlPgmImage pgm ;
  
  FILE *f = fopen (name, "wb") ;
  
  if (! f) {
    vl_err_no = VL_ERR_PGM_IO ;
    snprintf(vl_err_msg, VL_ERR_MSG_LEN,
             "Error opening PGM file for writing") ;
    return vl_err_no ;
  }
    
  pgm.width = width ;
  pgm.height = height ;
  pgm.is_raw = 1 ;
  pgm.max_value = 255 ;
  
  err = vl_pgm_insert (f, &pgm, data) ;
  fclose (f) ;
  
  return err ;
}

/** -------------------------------------------------------------------
 ** @brief Write floats to PGM file
 **
 ** @param name file name.
 ** @param data data to write.
 ** @param width width of the image.
 ** @param height height of the image.
 **
 ** The function dumps the image @a data to the PGM file of the specified
 ** name. The data is re-scaled to fit in the range 0-255. 
 ** This is an helper function simplifying the usage of
 ** vl_pgm_insert().
 **
 ** @return error code.
 **/

VL_EXPORT
int vl_pgm_write_f (char const *name, float const* data, int width, int height)
{
  int err = 0 ;
  int k ;
  float min = + VL_INFINITY_F ;
  float max = - VL_INFINITY_F ;
  float scale ;
  
  vl_uint8 * buffer = vl_malloc (sizeof(float) * width * height) ;
  
  for (k = 0 ; k < width * height ; ++k) {
    min = VL_MIN(min, data [k]) ;
    max = VL_MAX(max, data [k]) ;
  }
  
  scale = 255 / (max - min + VL_EPSILON_F) ;
  
  for (k = 0 ; k < width * height ; ++k) {
    buffer [k] = (vl_uint8) ((data [k] - min) * scale) ;
  }
  
  err = vl_pgm_write (name, buffer, width, height) ;
  
  vl_free (buffer) ;  
  return err ;
}
