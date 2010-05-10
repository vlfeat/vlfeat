/** @internal
 ** @file     sift.c
 ** @author   Andrea Vedaldi
 ** @brief    Scale Invariant Feature Transform (SIFT) - Driver
 **/

/* AUTORIGHTS
Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

This file is part of VLFeat, available in the terms of the GNU
General Public License version 2.
*/

#define VL_SIFT_DRIVER_VERSION 0.1

#include "generic-driver.h"

#include <vl/generic.h>
#include <vl/stringop.h>
#include <vl/pgm.h>
#include <vl/sift.h>
#include <vl/getopt_long.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* ----------------------------------------------------------------- */
/* help message */
char const help_message [] =
  "Usage: %s [options] files ...\n"
  "\n"
  "Options include:\n"
  " --verbose -v    Be verbose\n"
  " --help -h       Print this help message\n"
  " --output -o     Specify output file\n"
  " --frames        Specify frames file\n"
  " --descriptors   Specify descriptors file\n"
  " --meta          Specify meta file\n"
  " --gss           Specify Gaussian scale space files\n"
  " --octaves -O    Number of octaves\n"
  " --levels -S     Number of levels per octave\n"
  " --first-octave  Index of the first octave\n"
  " --edge-thresh   Specify the edge threshold\n"
  " --peak-thresh   Specift the peak threshold\n"
  " --magnif        Specify the magnification factor\n"
  " --read-frames   Specify a file from which to read frames\n"
  " --orientations  Force the computation of the oriantations\n"
  "\n" ;

/* ----------------------------------------------------------------- */
/* long options codes */
enum {
  opt_meta = 1000,
  opt_frames,
  opt_descriptors,
  opt_gss,
  opt_first_octave,
  opt_edge_thresh,
  opt_peak_thresh,
  opt_magnif,
  opt_read_frames,
  opt_orientations
} ;

/* short options */
char const opts [] = "vhO:S:o:" ;

/* long options */
struct option const longopts [] = {
  { "verbose",         no_argument,            0,          'v'              },
  { "help",            no_argument,            0,          'h'              },
  { "octaves",         required_argument,      0,          'O'              },
  { "levels",          required_argument,      0,          'S'              },
  { "output",          optional_argument,      0,          'o'              },
  { "meta",            optional_argument,      0,          opt_meta         },
  { "frames",          optional_argument,      0,          opt_frames       },
  { "descriptors",     optional_argument,      0,          opt_descriptors  },
  { "gss",             optional_argument,      0,          opt_gss          },
  { "first-octave",    required_argument,      0,          opt_first_octave },
  { "edge-thresh",     required_argument,      0,          opt_edge_thresh  },
  { "peak-thresh",     required_argument,      0,          opt_peak_thresh  },
  { "magnif",          required_argument,      0,          opt_magnif       },
  { "read-frames",     required_argument,      0,          opt_read_frames  },
  { "orientations",    no_argument,            0,          opt_orientations },
  { 0,                 0,                      0,          0                }
} ;

/* ----------------------------------------------------------------- */
/** @brief Save octave on disk
 ** @internal
 **/
static int
save_gss (VlSiftFilt * filt, VlFileMeta * fm, const char * basename,
          int verbose)
{
  char tmp [1024] ;
  int S = filt -> S ;
  int i ;
  int s, err = 0 ;
  int w, h ;
  int o = filt -> o_cur ;
  VlPgmImage pim ;
  vl_uint8 *buffer = 0 ;
  vl_size q ;

  if (! fm -> active) {
    return VL_ERR_OK ;
  }

  w = vl_sift_get_octave_width  (filt) ;
  h = vl_sift_get_octave_height (filt) ;

  pim.width     = w ;
  pim.height    = h ;
  pim.max_value = 255 ;
  pim.is_raw    = 1 ;

  buffer = malloc (sizeof(vl_uint8) * w * h) ;
  if (! buffer) {
    err = VL_ERR_ALLOC ;
    goto save_gss_quit ;
  }

  q = vl_string_copy (tmp, sizeof(tmp), basename) ;
  if (q >= sizeof(tmp)) {
    err = VL_ERR_OVERFLOW ;
    goto save_gss_quit ;
  }

  for (s = 0 ; s < S ; ++s) {
    vl_sift_pix * pt = vl_sift_get_octave (filt, s) ;

    /* conversion */
    for (i = 0 ; i < w * h ; ++i) {
      buffer [i] = (vl_uint8) pt [i] ;
    }

    /* save */
    snprintf(tmp + q, sizeof(tmp) - q, "_%02d_%03d", o, s) ;

    err = vl_file_meta_open (fm, tmp, "wb") ;
    if (err) goto save_gss_quit ;

    err = vl_pgm_insert (fm -> file, &pim, buffer) ;
    if (err) goto save_gss_quit ;

    if (verbose) {
      printf("sift: saved gss level to '%s'\n", fm -> name) ;
    }

    vl_file_meta_close (fm) ;
  }

 save_gss_quit : ;
  if (buffer) free (buffer) ;
  vl_file_meta_close (fm) ;
  return err ;
}

/* ----------------------------------------------------------------- */
/** @brief Keypoint ordering
 ** @internal
 **/
int
korder (void const* a, void const* b) {
  double x = ((double*) a) [2] - ((double*) b) [2] ;
  if (x < 0) return -1 ;
  if (x > 0) return +1 ;
  return 0 ;
}

/* ---------------------------------------------------------------- */
/** @brief SIFT driver entry point
 **/
int
main(int argc, char **argv)
{
  /* algorithm parameters */
  double   edge_thresh  = -1 ;
  double   peak_thresh  = -1 ;
  double   magnif       = -1 ;
  int      O = -1, S = 3, omin = -1 ;

  vl_bool  err    = VL_ERR_OK ;
  char     err_msg [1024] ;
  int      n ;
  int      exit_code          = 0 ;
  int      verbose            = 0 ;
  vl_bool  force_output       = 0 ;
  vl_bool  force_orientations = 0 ;

  VlFileMeta out  = {1, "%.sift",  VL_PROT_ASCII, "", 0} ;
  VlFileMeta frm  = {0, "%.frame", VL_PROT_ASCII, "", 0} ;
  VlFileMeta dsc  = {0, "%.descr", VL_PROT_ASCII, "", 0} ;
  VlFileMeta met  = {0, "%.meta",  VL_PROT_ASCII, "", 0} ;
  VlFileMeta gss  = {0, "%.pgm",   VL_PROT_ASCII, "", 0} ;
  VlFileMeta ifr  = {0, "%.frame", VL_PROT_ASCII, "", 0} ;

#define ERRF(msg, arg) {                                        \
    err = VL_ERR_BAD_ARG ;                                      \
    snprintf(err_msg, sizeof(err_msg), msg, arg) ;              \
    break ;                                                     \
  }

#define ERR(msg) {                                              \
    err = VL_ERR_BAD_ARG ;                                      \
    snprintf(err_msg, sizeof(err_msg), msg) ;                   \
    break ;                                                     \
}

  /* -----------------------------------------------------------------
   *                                                     Parse options
   * -------------------------------------------------------------- */

  while (!err) {
    int ch = getopt_long(argc, argv, opts, longopts, 0) ;

    /* If there are no files passed as input, print the help and settings */
    if (ch == -1 && argc - optind == 0)
      ch = 'h';

    /* end of option list? */
    if (ch == -1) break;

    switch (ch) {

    case '?' :
      /* unkown option ............................................ */
      ERRF("Invalid option '%s'.", argv [optind - 1]) ;
      break ;

    case ':' :
      /* missing argument ......................................... */
      ERRF("Missing mandatory argument for option '%s'.",
          argv [optind - 1]) ;
      break ;

    case 'h' :
      /* --help ................................................... */
      printf (help_message, argv [0]) ;
      printf ("SIFT         filespec: `%s'\n", out.pattern) ;
      printf ("Frames       filespec: `%s'\n", frm.pattern) ;
      printf ("Descriptors  filespec: `%s'\n", dsc.pattern) ;
      printf ("Meta         filespec: `%s'\n", met.pattern) ;
      printf ("GSS          filespec: '%s'\n", gss.pattern) ;
      printf ("Read frames  filespec: '%s'\n", ifr.pattern) ;
      printf ("Version: driver %s; libvl %s\n",
              VL_XSTRINGIFY(VL_SIFT_DRIVER_VERSION),
              vl_get_version_string()) ;
      exit (0) ;
      break ;

    case 'v' :
      /* --verbose ................................................ */
      ++ verbose ;
      break ;

    case 'o' :
      /* --output  ................................................ */
      err = vl_file_meta_parse (&out, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      force_output = 1 ;
      break ;

    case opt_frames :
      /* --frames  ................................................ */
      err = vl_file_meta_parse (&frm, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break ;

    case opt_descriptors :
      /* --descriptor ............................................. */
      err = vl_file_meta_parse (&dsc, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break;

    case opt_meta :
      /* --meta ................................................... */
      err = vl_file_meta_parse (&met, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;

      if (met.protocol != VL_PROT_ASCII)
        ERR("meta file supports only ASCII protocol") ;
      break ;

    case opt_read_frames :
      /* --read_frames ............................................ */
      err = vl_file_meta_parse (&ifr, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break ;

    case opt_gss :
      /* --gss .................................................... */
      err = vl_file_meta_parse (&gss, optarg) ;
      if (err)
        ERRF("The arguments of '%s' is invalid.", argv [optind - 1]) ;
      break ;



    case 'O' :
      /* --octaves ............................................... */
      n = sscanf (optarg, "%d", &O) ;
      if (n == 0 || O < 0)
        ERRF("The argument of '%s' must be a non-negative integer.",
            argv [optind - 1]) ;
      break ;

    case 'S' :
      /* --levels ............................................... */
      n = sscanf (optarg, "%d", &S) ;
      if (n == 0 || S < 0)
        ERRF("The argument of '%s' must be a non-negative integer.",
            argv [optind - 1]) ;
      break ;

    case opt_first_octave :
      /* --first-octave ......................................... */
      n = sscanf (optarg, "%d", &omin) ;
      if (n == 0)
        ERRF("The argument of '%s' must be an integer.",
            argv [optind - 1]) ;
      break ;



    case opt_edge_thresh :
      /* --edge-thresh ........................................... */
      n = sscanf (optarg, "%lf", &edge_thresh) ;
      if (n == 0 || edge_thresh < 1)
        ERRF("The argument of '%s' must be not smaller than 1.",
            argv [optind - 1]) ;
      break ;

    case opt_peak_thresh :
      /* --edge-thresh ........................................... */
      n = sscanf (optarg, "%lf", &peak_thresh) ;
      if (n == 0 || peak_thresh < 0)
        ERRF("The argument of '%s' must be a non-negative float.",
            argv [optind - 1]) ;
      break ;

    case opt_magnif :
      /* --magnif  .............................................. */
      n = sscanf (optarg, "%lf", &magnif) ;
      if (n == 0 || magnif < 1)
        ERRF("The argument of '%s' must be a non-negative float.",
            argv [optind - 1]) ;
      break ;


    case opt_orientations :
      /* --orientations ......................................... */
      force_orientations = 1 ;
      break ;

    case 0 :
    default :
      /* should not get here ...................................... */
      abort() ;
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

  /*
     if --output is not specified, specifying --frames or --descriptors
     prevent the aggregate outout file to be produced.
  */
  if (! force_output && (frm.active || dsc.active)) {
    out.active = 0 ;
  }

  if (verbose > 1) {
#define PRNFO(name,fm)                                                  \
    printf("sift: " name) ;                                             \
    printf("%3s ",  (fm).active ? "yes" : "no") ;                       \
    printf("%-6s ", vl_string_protocol_name ((fm).protocol)) ;          \
    printf("%-10s\n", (fm).pattern) ;

    PRNFO("write aggregate . ", out) ;
    PRNFO("write frames .... ", frm) ;
    PRNFO("write descriptors ", dsc) ;
    PRNFO("write meta ...... ", met) ;
    PRNFO("write GSS ....... ", gss) ;
    PRNFO("read  frames .... ", ifr) ;

    if (force_orientations)
      printf("sift: will compute orientations\n") ;
  }

  /* ------------------------------------------------------------------
   *                                         Process one image per time
   * --------------------------------------------------------------- */

  while (argc--) {

    char             basename [1024] ;
    char const      *name = *argv++ ;

    FILE            *in    = 0 ;
    vl_uint8        *data  = 0 ;
    vl_sift_pix     *fdata = 0 ;
    VlPgmImage       pim ;

    VlSiftFilt      *filt = 0 ;
    vl_size          q ;
    int              i ;
    vl_bool          first ;

    double           *ikeys = 0 ;
    int              nikeys = 0, ikeys_size = 0 ;

    /* ...............................................................
     *                                                 Determine files
     * ............................................................ */

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
      printf ("sift: <== '%s'\n", name) ;
    }

    if (verbose > 1) {
      printf ("sift: basename is '%s'\n", basename) ;
    }

    /* open input file */
    in = fopen (name, "rb") ;
    if (!in) {
      err = VL_ERR_IO ;
      snprintf(err_msg, sizeof(err_msg),
               "Could not open '%s' for reading.", name) ;
      goto done ;
    }

    /* ...............................................................
     *                                                       Read data
     * ............................................................ */

    /* read PGM header */
    err = vl_pgm_extract_head (in, &pim) ;

    if (err) {
      switch (vl_get_last_error()) {
      case  VL_ERR_PGM_IO :
        snprintf(err_msg, sizeof(err_msg),
                 "Cannot read from '%s'.", name) ;
        err = VL_ERR_IO ;
        break ;

      case VL_ERR_PGM_INV_HEAD :
        snprintf(err_msg, sizeof(err_msg),
                 "'%s' contains a malformed PGM header.", name) ;
        err = VL_ERR_IO ;
        goto done ;
      }
    }

    if (verbose)
      printf ("sift: image is %d by %d pixels\n",
              pim. width,
              pim. height) ;

    /* allocate buffer */
    data  = malloc(vl_pgm_get_npixels (&pim) *
                   vl_pgm_get_bpp       (&pim) * sizeof (vl_uint8)   ) ;
    fdata = malloc(vl_pgm_get_npixels (&pim) *
                   vl_pgm_get_bpp       (&pim) * sizeof (vl_sift_pix)) ;

    if (!data || !fdata) {
      err = VL_ERR_ALLOC ;
      snprintf(err_msg, sizeof(err_msg),
               "Could not allocate enough memory.") ;
      goto done ;
    }

    /* read PGM body */
    err  = vl_pgm_extract_data (in, &pim, data) ;

    if (err) {
      snprintf(err_msg, sizeof(err_msg), "PGM body malformed.") ;
      err = VL_ERR_IO ;
      goto done ;
    }

    /* convert data type */
    for (q = 0 ; q < (unsigned) (pim.width * pim.height) ; ++q) {
      fdata [q] = data [q] ;
    }

    /* ...............................................................
     *                                     Optionally source keypoints
     * ............................................................ */

#define WERR(name,op)                                           \
    if (err == VL_ERR_OVERFLOW) {                               \
      snprintf(err_msg, sizeof(err_msg),                        \
               "Output file name too long.") ;                  \
      goto done ;                                               \
    } else if (err) {                                           \
      snprintf(err_msg, sizeof(err_msg),                        \
               "Could not open '%s' for " #op, name) ;          \
      goto done ;                                               \
    }

    if (ifr.active) {

      /* open file */
      err = vl_file_meta_open (&ifr, basename, "rb") ;
      WERR(ifr.name, reading) ;

#define QERR                                                            \
      if (err ) {                                                       \
        snprintf (err_msg, sizeof(err_msg),                             \
                  "'%s' malformed", ifr.name) ;                         \
        err = VL_ERR_IO ;                                               \
        goto done ;                                                     \
      }

      while (1) {
        double x, y, s, th ;

        /* read next guy */
        err = vl_file_meta_get_double (&ifr, &x) ;
        if   (err == VL_ERR_EOF) break;
        else QERR ;
        err = vl_file_meta_get_double (&ifr, &y ) ; QERR ;
        err = vl_file_meta_get_double (&ifr, &s ) ; QERR ;
        err = vl_file_meta_get_double (&ifr, &th) ;
        if   (err == VL_ERR_EOF) break;
        else QERR ;

        /* make enough space */
        if (ikeys_size < nikeys + 1) {
          ikeys_size += 10000 ;
          ikeys       = realloc (ikeys, 4 * sizeof(double) * ikeys_size) ;
        }

        /* add the guy to the buffer */
        ikeys [4 * nikeys + 0]  = x ;
        ikeys [4 * nikeys + 1]  = y ;
        ikeys [4 * nikeys + 2]  = s ;
        ikeys [4 * nikeys + 3]  = th ;

        ++ nikeys ;
      }

      /* now order by scale */
      qsort (ikeys, nikeys, 4 * sizeof(double), korder) ;

      if (verbose) {
        printf ("sift: read %d keypoints from '%s'\n", nikeys, ifr.name) ;
      }

      /* close file */
      vl_file_meta_close (&ifr) ;
    }

    /* ...............................................................
     *                                               Open output files
     * ............................................................ */

    err = vl_file_meta_open (&out, basename, "wb") ; WERR(out.name, writing) ;
    err = vl_file_meta_open (&dsc, basename, "wb") ; WERR(dsc.name, writing) ;
    err = vl_file_meta_open (&frm, basename, "wb") ; WERR(frm.name, writing) ;
    err = vl_file_meta_open (&met, basename, "wb") ; WERR(met.name, writing) ;

    if (verbose > 1) {
      if (out.active) printf("sift: writing all ....... to . '%s'\n", out.name);
      if (frm.active) printf("sift: writing frames .... to . '%s'\n", frm.name);
      if (dsc.active) printf("sift: writing descriptors to . '%s'\n", dsc.name);
      if (met.active) printf("sift: writign meta ...... to . '%s'\n", met.name);
    }

    /* ...............................................................
     *                                                     Make filter
     * ............................................................ */

    filt = vl_sift_new (pim.width, pim.height, O, S, omin) ;

    if (edge_thresh >= 0) vl_sift_set_edge_thresh (filt, edge_thresh) ;
    if (peak_thresh >= 0) vl_sift_set_peak_thresh (filt, peak_thresh) ;
    if (magnif      >= 0) vl_sift_set_magnif      (filt, magnif) ;

    if (!filt) {
      snprintf (err_msg, sizeof(err_msg),
                "Could not create SIFT filter.") ;
      err = VL_ERR_ALLOC ;
      goto done ;
    }

    if (verbose > 1) {
      printf ("sift: filter settings:\n") ;
      printf ("sift:   octaves      (O)     = %d\n",
              vl_sift_get_noctaves     (filt)) ;
      printf ("sift:   levels       (S)     = %d\n",
              vl_sift_get_nlevels      (filt)) ;
      printf ("sift:   first octave (o_min) = %d\n",
              vl_sift_get_octave_first (filt)) ;
      printf ("sift:   edge thresh           = %g\n",
              vl_sift_get_edge_thresh  (filt)) ;
      printf ("sift:   peak thresh           = %g\n",
              vl_sift_get_peak_thresh  (filt)) ;
      printf ("sift:   magnif                = %g\n",
              vl_sift_get_magnif       (filt)) ;
      printf ("sift: will source frames? %s\n",
              ikeys ? "yes" : "no") ;
      printf ("sift: will force orientations? %s\n",
              force_orientations ? "yes" : "no") ;
    }

    /* ...............................................................
     *                                             Process each octave
     * ............................................................ */
    i     = 0 ;
    first = 1 ;
    while (1) {
      VlSiftKeypoint const *keys = 0 ;
      int                   nkeys ;

      /* calculate the GSS for the next octave .................... */
      if (first) {
        first = 0 ;
        err = vl_sift_process_first_octave (filt, fdata) ;
      } else {
        err = vl_sift_process_next_octave  (filt) ;
      }

      if (err) {
        err = VL_ERR_OK ;
        break ;
      }

      if (verbose > 1) {
        printf("sift: GSS octave %d computed\n",
               vl_sift_get_octave_index (filt));
      }

      /* optionally save GSS */
      if (gss.active) {
        err = save_gss (filt, &gss, basename, verbose) ;
        if (err) {
          snprintf (err_msg, sizeof(err_msg),
                    "Could not write GSS to PGM file.") ;
          goto done ;
        }
      }

      /* run detector ............................................. */
      if (ikeys == 0) {
        vl_sift_detect (filt) ;

        keys  = vl_sift_get_keypoints     (filt) ;
        nkeys = vl_sift_get_nkeypoints (filt) ;
        i     = 0 ;

        if (verbose > 1) {
          printf ("sift: detected %d (unoriented) keypoints\n", nkeys) ;
        }
      } else {
        nkeys = nikeys ;
      }

      /* for each keypoint ........................................ */
      for (; i < nkeys ; ++i) {
        double                angles [4] ;
        int                   nangles ;
        VlSiftKeypoint        ik ;
        VlSiftKeypoint const *k ;

        /* obtain keypoint orientations ........................... */
        if (ikeys) {
          vl_sift_keypoint_init (filt, &ik,
                                 ikeys [4 * i + 0],
                                 ikeys [4 * i + 1],
                                 ikeys [4 * i + 2]) ;

          if (ik.o != vl_sift_get_octave_index (filt)) {
            break ;
          }

          k          = &ik ;

          /* optionally compute orientations too */
          if (force_orientations) {
            nangles = vl_sift_calc_keypoint_orientations
              (filt, angles, k) ;
          } else {
            angles [0] = ikeys [4 * i + 3] ;
            nangles    = 1 ;
          }
        } else {
          k = keys + i ;
          nangles = vl_sift_calc_keypoint_orientations
            (filt, angles, k) ;
        }

        /* for each orientation ................................... */
        for (q = 0 ; q < (unsigned) nangles ; ++q) {
          vl_sift_pix descr [128] ;

          /* compute descriptor (if necessary) */
          if (out.active || dsc.active) {
            vl_sift_calc_keypoint_descriptor
              (filt, descr, k, angles [q]) ;
          }

          if (out.active) {
            int l ;
            vl_file_meta_put_double (&out, k -> x     ) ;
            vl_file_meta_put_double (&out, k -> y     ) ;
            vl_file_meta_put_double (&out, k -> sigma ) ;
            vl_file_meta_put_double (&out, angles [q] ) ;
            for (l = 0 ; l < 128 ; ++l) {
              vl_file_meta_put_uint8 (&out, (vl_uint8) (512.0 * descr [l])) ;
            }
            if (out.protocol == VL_PROT_ASCII) fprintf(out.file, "\n") ;
          }

          if (frm.active) {
            vl_file_meta_put_double (&frm, k -> x     ) ;
            vl_file_meta_put_double (&frm, k -> y     ) ;
            vl_file_meta_put_double (&frm, k -> sigma ) ;
            vl_file_meta_put_double (&frm, angles [q] ) ;
            if (frm.protocol == VL_PROT_ASCII) fprintf(frm.file, "\n") ;
          }

          if (dsc.active) {
            int l ;
            for (l = 0 ; l < 128 ; ++l) {
              double x = 512.0 * descr[l] ;
              x = (x < 255.0) ? x : 255.0 ;
              vl_file_meta_put_uint8 (&dsc, (vl_uint8) (x)) ;
            }
            if (dsc.protocol == VL_PROT_ASCII) fprintf(dsc.file, "\n") ;
          }
        }
      }
    }

    /* ...............................................................
     *                                                       Finish up
     * ............................................................ */

    if (met.active) {
      fprintf(met.file, "<sift\n") ;
      fprintf(met.file, "  input       = '%s'\n", name) ;
      if (dsc.active) {
        fprintf(met.file, "  descriptors = '%s'\n", dsc.name) ;
      }
      if (frm.active) {
        fprintf(met.file,"  frames      = '%s'\n", frm.name) ;
      }
      fprintf(met.file, ">\n") ;
    }

  done :
    /* release input keys buffer */
    if (ikeys) {
      free (ikeys) ;
      ikeys_size = nikeys = 0 ;
      ikeys = 0 ;
    }

    /* release filter */
    if (filt) {
      vl_sift_delete (filt) ;
      filt = 0 ;
    }

    /* release image data */
    if (fdata) {
      free (fdata) ;
      fdata = 0 ;
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

    vl_file_meta_close (&out) ;
    vl_file_meta_close (&frm) ;
    vl_file_meta_close (&dsc) ;
    vl_file_meta_close (&met) ;
    vl_file_meta_close (&gss) ;
    vl_file_meta_close (&ifr) ;

    /* if bad print error message */
    if (err) {
      fprintf
        (stderr,
         "sift: err: %s (%d)\n",
         err_msg,
         err) ;
      exit_code = 1 ;
    }
  }

  /* quit */
  return exit_code ;
}
