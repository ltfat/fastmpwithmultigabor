/******************************************************************************/
/*                                                                            */
/*                      test_parabolic_regression.cpp                         */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2005-07-25 21:40:37 +0200 (Mon, 25 Jul 2005) $
 * $Revision: 23 $
 *
 */

/** \file test_parabolic_regression.cpp
 * A file with some code that serves to test if the parabolic
 * regression routine is properly working.
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

int main(void) {

#define A 2.0
#define B 3.0
#define C 1.0
#define D 5.5
#define E 6.3
#define F 7.3
#define BUF_SIZE 128
#define L 10

  int i;
  MP_Real_t x;
  MP_Real_t buff1[BUF_SIZE];
  MP_Real_t buff2[BUF_SIZE];
  MP_Real_t a, b, d, e;

  for ( i = -L; i <= L; i++ ) {
    x = (MP_Real_t)(i);
    buff1[i+L] = A*x*x + B*x + C;
    buff2[i+L] = D*x*x + E*x + F;
  }

  parabolic_regression( buff1, buff2, L,
			&a, &b, &d, &e );
			
  if (A-a > 1e-5) return( -1 );
  if (B-b > 1e-5) return( -1 );
  if (D-d > 1e-5) return( -1 );
  if (E-e > 1e-5) return( -1 );
  
  fprintf( stdout, "A before: %f A estimated: %f DIFF: %f\n", A, a, A-a );
  fprintf( stdout, "B before: %f B estimated: %f DIFF: %f\n", B, b, B-b );
  fprintf( stdout, "D before: %f D estimated: %f DIFF: %f\n", D, d, D-d );
  fprintf( stdout, "E before: %f E estimated: %f DIFF: %f\n", E, e, E-e );

  return( 0 );
}
