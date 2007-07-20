/** @file     mser-driver.c
 ** @author   Andrea Vedaldi
 ** @brief    MSER command line driver - Definition
 ** @internal
 **/

/* AUTORIGHTS */

#define VL_MSER_DRIVER_VERSION_STRING "alpha-1"

#include "generic-driver.h"

#include <vl/generic.h>
#include <vl/stringop.h>
#include <vl/pgm.h>
#include <vl/mser.h>
#include <vl/getopt_long.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* ----------------------------------------------------------------- */
/* help message */
char const help_message [] =
  "Usage: %s [options] files ...\n"
  "\n"
  "Where options include:\n"
  " --verbose -v    Be verbose\n"
  " --help -h       Print this help message\n"
  " --output -o     Specify output file\n"
  " --pivot         Specify pivots file\n"
  " --frame         Specify frames file\n"
  " --meta          Specify meta file\n"
  " --no-frame      Do not output frames\n"
  " --delta -d      MSER stability\n"
  "\n" ;

/* ----------------------------------------------------------------- */
/* long options codes */
enum { 
  opt_pivot    = 1000, 
  opt_frame, 
  opt_no_frame,
  opt_meta, 
  opt_version 
} ;

/* short options */
char const opts [] = "vhd:" ;

/* long options */
struct option const longopts [] = {
  { "verbose",         no_argument,            0,          'v'          },
  { "help",            no_argument,            0,          'h'          },
  { "delta",           required_argument,      0,          'd'          },
  { "pivot",           optional_argument,      0,          opt_pivot    },
  { "frame",           optional_argument,      0,          opt_frame    },
  { "no-frame",        no_argument,            0,          opt_no_frame },
  { "meta",            optional_argument,      0,          opt_meta     },
  { "version",         no_argument,            0,          opt_version  },
  { 0,                 0,                      0,          0            }
} ;


/* ----------------------------------------------------------------- */
/** @brief MSER driver entry point 
 **/
int
main(int argc, char **argv)
{  
  /* algorithm parameters */ 
  double   delta  = 2.0 ;  
  vl_bool  err    = VL_ERR_OK ;
  char     err_msg [1024] ;
  int      n ;
  int      exit_code = 0 ;
  int      verbose = 0 ;

  VlFileMeta frm  = {1, "%.frame", VL_PROT_ASCII, "", 0} ;
  VlFileMeta piv  = {0, "%.piv",   VL_PROT_ASCII, "", 0} ;
  VlFileMeta met  = {0, "%.meta",  VL_PROT_ASCII, "", 0} ;
  
#define ERR(args...) {                                          \
    err = VL_ERR_BAD_ARG ;                                      \
    snprintf(err_msg, sizeof(err_msg), args) ;                  \
    break ;                                                     \
  }
  
  /* ------------------------------------------------------------------
   *                                                      Parse options
   * --------------------------------------------------------------- */
  while (!err) {
    int ch = getopt_long(argc, argv, "vhd:p", longopts, 0) ;

    /* end of option list? */
    if (ch == -1) break;

    /* process options */
    switch (ch) {

    case '?' :
      /* unkown option ............................................ */
      ERR("Invalid option '%s'.", argv [optind - 1]) ;
      break ;
   
    case ':' :
      /* missing argument ......................................... */
      ERR("Missing mandatory argument for option '%s'.", 
          argv [optind - 1]) ;
      break ;
   
    case 'h' :
      /* --help ................................................... */
      printf (help_message, argv [0]) ;
      exit (0) ;
      break ;

    case 'v' :
      /* --verbose ................................................ */
      ++ verbose ;
      break ;
      
    case 'd' :
      /* --delta .................................................. */
      n = sscanf (optarg, "%lf", &delta) ;
      if (n == 0 || delta < 0)
        ERR("The argument of '%s' must be a non-negative number.",
            argv [optind - 1]) ;
      break ;
      
    case opt_frame :
      /* --frame  /................................................ */
      err = vl_file_meta_parse (&frm, optarg) ;
      if (err) 
        ERR("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break ;

    case opt_no_frame :
      /* --no-frame ............................................... */
      frm.active = 0 ;
      break ;
 
    case opt_pivot :
      /* --pivot .................................................. */
      err = vl_file_meta_parse (&piv, optarg) ;
      if (err) 
        ERR("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break ;

    case opt_meta :
      /* --meta ................................................... */
      err = vl_file_meta_parse (&met, optarg) ;      
      if (err) 
        ERR("The arguments of '%s' is invalid.", argv [optind - 1]) ;

      if (met.protocol != VL_PROT_ASCII)
        ERR("meta file supports only ASCII protocol") ;
      break ;

    case 0 :
    default :
      /* should not get here ...................................... */
      assert (0) ;
      break ;
    }
  }  
  
  /* check for parsing errors */
  if (err) {
    fprintf(stderr, "%s: error: %s (%d)\n", 
            argv [0], 
            err_msg, err) ;
    exit (1) ;
  }

  /* parse other arguments (filenames) */
  argc -= optind ;
  argv += optind ;
  
  if (verbose > 1) {
    printf("mser: frames output\n") ;
    printf("mser:    active   %d\n",  frm.active ) ;
    printf("mser:    pattern  %s\n",  frm.pattern) ;
    printf("mser:    protocol %s\n",  vl_string_protocol_name (frm.protocol)) ;
    printf("mser: pivots output\n") ;
    printf("mser:    active   %d\n",  piv.active ) ;
    printf("mser:    pattern  %s\n",  piv.pattern) ;
    printf("mser:    protocol %s\n",  vl_string_protocol_name (piv.protocol)) ;
    printf("mser: meta output\n") ;
    printf("mser:    active   %d\n",  met.active ) ;
    printf("mser:    pattern  %s\n",  met.pattern) ;
    printf("mser:    protocol %s\n",  vl_string_protocol_name (met.protocol)) ;
  }

  /* ------------------------------------------------------------------
   *                                         Process one image per time
   * --------------------------------------------------------------- */

  while (argc--) {

    char             basename [1024] ;
    char const      *name = *argv++ ;
    VlMserFilt      *filt = 0 ;
    vl_uint8        *data = 0 ;
    VlPgmImage       pim ;
    vl_uint const   *regions ;
    vl_single const *frames ;
    enum            {ndims = 2} ;
    int              dims [ndims] ;
    int              i, j, dof, nregions, nframes, q ;
    FILE            *in = 0 ;

    /* Open files  ------------------------------------------------ */
    
    /* get basenmae from filename */
    q = vl_string_basename (basename, sizeof(basename), name, 1) ;
    err = (q >= sizeof(basename)) ;
    if (err) {
      snprintf(err_msg, sizeof(err_msg), 
               "Basename of '%s' is too long", name);
      err = VL_ERR_OVERFLOW ;
      goto done ;
    }
    
    if (verbose) {
      printf("mser: processing '%s'\n", name) ;
    }
    
    if (verbose > 1) {
      printf("mser:    basename is '%s'\n", basename) ;
    }
    
#define WERR(name)                                              \
    if (err == VL_ERR_OVERFLOW) {                               \
      snprintf(err_msg, sizeof(err_msg),                        \
               "Output file name too long.") ;                  \
      goto done ;                                               \
    } else if (err) {                                           \
      snprintf(err_msg, sizeof(err_msg),                        \
               "Could not open '%s' for writing.", name) ;      \
      goto done ;                                               \
    }

    /* open input file */
    in = fopen (name, "r") ;
    if (!in) {
      err = VL_ERR_IO ;
      snprintf(err_msg, sizeof(err_msg), 
               "Could not open '%s' for reading.", name) ;
      goto done ;
    }

    /* open output files */
    err = vl_file_meta_open (&piv, basename, "w") ; WERR(piv.name) ;    
    err = vl_file_meta_open (&frm, basename, "w") ; WERR(frm.name) ;   
    err = vl_file_meta_open (&met, basename, "w") ; WERR(met.name) ;

    if (verbose > 1) {
      if (piv.active) printf("mser:  writing pivots to '%s'\n", piv.name); 
      if (frm.active) printf("mser:  writing pivots to '%s'\n", frm.name); 
      if (met.active) printf("mser:  writign metat  to '%s'\n", met.name);
    }
    
    /* Read image data -------------------------------------------- */

    /* read source image header */
    err = vl_pgm_extract_head (in, &pim) ;
    if (err) {
      err = VL_ERR_IO ;
      snprintf(err_msg, sizeof(err_msg), 
               "PGM header corrputed.") ;
      goto done ;
    }
    
    if (verbose) {
      printf("mser:   image is %d by %d pixels\n",
             pim. width,
             pim. height) ;
    }
    
    /* allocate buffer */
    data = malloc(vl_pgm_get_data_size (&pim) * 
                  vl_pgm_get_bpp       (&pim)) ;
    
    if (!data) {
      err = VL_ERR_ALLOC ;
      snprintf(err_msg, sizeof(err_msg), 
               "Could not allocate enough memory.") ;
      goto done ;
    } 
    
    /* read PGM */
    err  = vl_pgm_extract_data (in, &pim, data) ;
    if (err) {
      snprintf(err_msg, sizeof(err_msg), 
               "PGM body corrputed.") ;
      goto done ;
    }
    
    /* Process data  ---------------------------------------------- */
    dims[0] = pim.width ;
    dims[1] = pim.height ;

    filt = vl_mser_new (ndims, dims) ;
    
    if (!filt) {
      snprintf(err_msg, sizeof(err_msg), 
              "Could not initialize MSER filter.") ;
      goto done ;
    } 

    vl_mser_process (filt, (vl_mser_pix*) data) ;

    /* Save result  ----------------------------------------------- */
    nregions = vl_mser_get_regions_num (filt) ;
    regions  = vl_mser_get_regions     (filt) ;
    
    if (piv.active) {
      for (i = 0 ; i < nregions ; ++i) {
        fprintf(piv.file, "%d ", regions [i]) ;
      }
    }

    if (frm.active) {
      vl_mser_ell_fit (filt) ;

      nframes = vl_mser_get_ell_num (filt) ;
      dof     = vl_mser_get_ell_dof (filt) ;
      frames  = vl_mser_get_ell     (filt) ;
      for (i = 0 ; i < nframes ; ++i) {
        for (j = 0 ; j < dof ; ++j) {
          fprintf(frm.file, "%f ", *frames++) ;
        }
        fprintf(frm.file, "\n") ;
      }
    }
    
    if (met.active) {
      fprintf(met.file, "<mser\n") ;
      fprintf(met.file, "  input = '%s'\n", name) ;
      if (piv.active) {
        fprintf(met.file, "  pivots = '%s'\n", piv.name) ;
      }
      if (frm.active) {
        fprintf(met.file,"  frames = '%s'\n", frm.name) ;
      }
      fprintf(met.file, ">\n") ;
    }

    /* Next guy  ----------------------------------------------- */
  done :
    /* release filter */
    if (filt) {
      vl_mser_delete (filt) ;
      filt = 0 ;
    }
  
    /* release image data */
    if (data) {
      free (data) ;
      data = 0 ;
    }

    /* close files */
    if (in) {
      fclose (in) ;
      in = 0 ;
    }

    vl_file_meta_close (&frm) ;
    vl_file_meta_close (&piv) ;
    vl_file_meta_close (&met) ;
        
    /* if bad print error message */
    if (err) {
      fprintf
        (stderr,
         "mser: err: %s (%d)\n",
         err_msg,
         err) ;
      exit_code = 1 ;
    }
  }

  /* quit */
  return exit_code ;
}
