/** @file test_svd2.c
 ** @brief Test SVD on 2x2 matrices (@ref mathop.h)
 ** @author Andrea Vedaldi
 **/

/*
 Copyright (C) 2007-12 Andrea Vedaldi and Brian Fulkerson.
 All rights reserved.

 This file is part of the VLFeat library and is made available under
 the terms of the BSD license (see the COPYING file).
 */

#include <vl/mathop.h>
#include <stdio.h>

void
print_matrix(char const * name, double *M)
{
  printf("%s = \n[ %10f %10f ]\n[ %10f %10f ]\n", name, M[0], M[2], M[1], M[3]) ;
}

void
prod2 (double R [4], double A [4], double B [4])
{
  R[0] = A[0]*B[0] + A[2]*B[1] ;
  R[1] = A[1]*B[0] + A[3]*B[1] ;
  R[2] = A[0]*B[2] + A[2]*B[3] ;
  R[3] = A[1]*B[2] + A[3]*B[3] ;
}

void
transp2 (double R [4], double A [4])
{
  R[0] = A[0] ;
  R[1] = A[2] ;
  R[2] = A[1] ;
  R[3] = A[3] ;
}

double
det2 (double A [4])
{
  return A[0]*A[3] - A[1]*A[2];
}

void
check_svd (double *M , double * U, double * S, double *V)
{
  double T1 [4] ;
  double T2 [4] ;

  print_matrix("M",M) ;
  print_matrix("U",U) ;
  print_matrix("S",S) ;
  print_matrix("V",V) ;

  transp2(T1, V) ;
  prod2(T2, S, T1) ;
  prod2(T1, U, T2) ;
  print_matrix("USV'",T1) ;

  transp2(T1, U) ;
  prod2(T2, T1, U) ;
  print_matrix("U'U",T2) ;

  transp2(T1, V) ;
  prod2(T2, T1, V) ;
  print_matrix("V'V",T2) ;

  printf("det(M) = %f\n", det2(M)) ;
  printf("det(U) = %f\n", det2(U)) ;
  printf("det(V) = %f\n", det2(V)) ;
  printf("det(S) = %f\n", det2(S)) ;
  printf("\n") ;
}

int
main (int argc VL_UNUSED, char ** argv VL_UNUSED)
{
  double M [] = {
    0.864397318249258,
    0.094202610858281,
    -0.851909224508774,
    0.873504449150106 } ;
  double S [4] ;
  double U [4] ;
  double V [4] ;

  vl_svd2(S, U, V, M) ;
  check_svd(M, U, S, V) ;

  M[1] = 0 ;

  vl_svd2(S, U, V, M) ;
  check_svd(M, U, S, V) ;

  vl_lapack_dlasv2(S+3, S,
                   V+1, V,
                   U+1, U,
                   M[0], M[2], M[3]) ;
  V[2] = -V[1] ;
  V[3] = V[0] ;
  U[2] = -U[1] ;
  U[3] = U[0] ;
  check_svd(M, U, S, V) ;
  return 0 ;
}
