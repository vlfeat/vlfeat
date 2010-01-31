/** @file   check.h
 ** @brief  Unit test utilties
 ** @author Andrea Vedaldi
 **/

#ifndef __CHECK_H__
#define __CHECK_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void
check_impl (int condition,
            const char * file,
            int line,
            const char * format, ...)
{
  va_list args;
  va_start(args, format) ;
  if (! condition) {
    fprintf(stderr, "%s:%d: check failed: ", file, line) ;
    vfprintf(stderr, format, args) ;
    fprintf(stderr, "\n") ;
    exit (1) ;
  }
  va_end(args) ;
}

#define check(condition, ...) \
check_impl(condition, __FILE__, __LINE__, "" __VA_ARGS__)

#define check_signoff() \
fprintf(stdout, "%s passed\n", __FILE__)

/* __CHECK_H__ */
#endif
