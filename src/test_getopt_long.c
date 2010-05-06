/* AUTORIGHTS
 Copyright (C) 2007-09 Andrea Vedaldi and Brian Fulkerson

 This file is part of VLFeat, available in the terms of the GNU
 General Public License version 2.
 */

#include <stdio.h>
#include <assert.h>

#include <vl/getopt_long.h>

int daggerset;

/* options descriptor */
static struct option longopts[] = {
  { "help",         no_argument,            0,           'h' },
  { "buffy",        no_argument,            0,           'b' },
  { "fluoride",     required_argument,      0,           'f' },
  { "daggerset",    no_argument,            &daggerset,   1000  },
  { "daggerunset",  no_argument,            &daggerset,   1001  },
  { "spike",        optional_argument,      0,            1002  },
  { NULL,           0,                      0,            0  }
};

int
main (int argc, char *argv[])
{
  int ch ;

  printf("*** parsing options: start\n") ;

  while ((ch = getopt_long(argc, argv, "hbf:", longopts, 0)) != -1) {

    switch (ch) {

    case 'h':
        printf("test_getopt_long: test VLFeat implementation of this function.\n"
               " --help -h      this message\n"
               " --buffy -b     option with no arguments\n"
               " --flouride -f  option with required argumetn\n"
               " --daggerset    auto option with no arguments\n"
               " --daggerrunset auto option with no argument\n"
               " --spike        long option with optional argument\n") ;
        break;

    case 'b':
      printf("option `b' or `buffy'\n") ;
      break;

    case 'f':
      printf("option `f' or `flouride' with arg `%s'\n", optarg) ;
      break ;

    case 1002 :
      printf("option `spike' with arg `%s'\n", optarg) ;
      break ;

    case 0:
      printf("automatic longoption (daggerset=%d)\n", daggerset) ;
      break ;

    case  '?' :
      printf("illegal option or missing argument\n") ;
      break ;

    case ':'  :
      printf("illegal option\n") ;
      break ;

    default:
      abort() ;
    }
  }

  printf("*** parsing options: end\n");
  {
    int i ;
    for(i = optind ; i < argc ; ++i)
		printf("non-option: '%s'\n", argv[i]) ;
  }

  return 0 ;
}
