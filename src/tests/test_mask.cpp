/******************************************************************************/
/*                                                                            */
/*                              test_mask.cpp                                 */
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
 * $Date: 2005-11-25 17:02:47 +0100 (Fri, 25 Nov 2005) $
 * $Revision: 132 $
 *
 */

#include <mptk.h>

int main( void ) {

  unsigned long int i;

#define MASK_SIZE 16

  MP_Mask_c mask1(MASK_SIZE);
  MP_Mask_c mask2(MASK_SIZE);
  MP_Mask_c mask3(MASK_SIZE);
  MP_Mask_c mask4(MASK_SIZE - 4);
  MP_Mask_c mask5(MASK_SIZE - 4);

  /*********************************/
  fprintf( stdout, "==== INITIALIZATION:\n" );

  fprintf( stdout, "MASK1 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask1.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "MASK2 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask2.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "MASK3 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  if (mask1 == mask2) fprintf( stdout, "MASK1 == MASK2 is TRUE.\n" );
  else                fprintf( stdout, "MASK1 == MASK2 is FALSE.\n" );

  if (mask1 != mask2) fprintf( stdout, "MASK1 != MASK2 is TRUE.\n" );
  else                fprintf( stdout, "MASK1 != MASK2 is FALSE.\n" );


  /*********************************/
  fprintf( stdout, "==== SETTING:\n" );

  mask1.set_false( 2 );
  mask1.set_false( 4 );

  mask2.set_false( 2 );
  mask2.set_false( 5 );

  fprintf( stdout, "MASK1 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask1.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "MASK2 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask2.sieve[i] );
  fprintf( stdout, "\n" );

  if (mask1 == mask2) fprintf( stdout, "MASK1 == MASK2 is TRUE.\n" );
  else                fprintf( stdout, "MASK1 == MASK2 is FALSE.\n" );

  if (mask1 != mask2) fprintf( stdout, "MASK1 != MASK2 is TRUE.\n" );
  else                fprintf( stdout, "MASK1 != MASK2 is FALSE.\n" );


  /*********************************/
  fprintf( stdout, "==== COPY:\n" );

  mask3 = mask1;

  fprintf( stdout, "COPY1 " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  /*********************************/
  fprintf( stdout, "==== COPY/RESIZE:\n" );

  fprintf( stdout, "BEFOR " );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );

  mask4 = mask1;

  fprintf( stdout, "AFTER " );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "+4TRU " );
  mask4.append_true( 4 );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "+3FLS " );
  mask4.append_false( 3 );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "+1TRU " );
  mask4.append( 1==1 );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "+1FLS " );
  mask4.append( (1==0) );
  for ( i = 0; i < mask4.numAtoms; i++ ) fprintf( stdout, "%1d", mask4.sieve[i] );
  fprintf( stdout, "\n" );


  /*********************************/
  fprintf( stdout, "==== OPERATORS:\n" );

  /* AND */
  mask3 = ( mask1.operator&&(mask2) );
  fprintf( stdout, "AND   " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  /* OR */
  mask3 = ( mask1 || mask2 );
  fprintf( stdout, "OR    " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  /* NOT */
  mask3 = !(mask1);
  fprintf( stdout, "NOT1  " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  /* NOR */
  mask3 = !(mask1 || mask2);
  fprintf( stdout, "NOR   " );
  for ( i = 0; i < MASK_SIZE; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );


  /*********************************/
  fprintf( stdout, "==== FILE I/O:\n" );

  i = mask3.write_to_file( "mask3.bin" );
  fprintf( stdout, "Wrote %lu elements. mask5 has %lu elements.\n",
	   i, mask5.numAtoms );

  i = mask5.read_from_file( "mask3.bin" );
  fprintf( stdout, "Read %lu elements. mask5 now has %lu elements.\n",
	   i, mask5.numAtoms );

  if (mask3 == mask5) fprintf( stdout, "MASK3 == MASK5 is TRUE.\n" );
  else                fprintf( stdout, "MASK3 == MASK5 is FALSE.\n" );

  fprintf( stdout, "MASK3: " );
  for ( i = 0; i < mask3.numAtoms; i++ ) fprintf( stdout, "%1d", mask3.sieve[i] );
  fprintf( stdout, "\n" );

  fprintf( stdout, "MASK5: " );
  for ( i = 0; i < mask5.numAtoms; i++ ) fprintf( stdout, "%1d", mask5.sieve[i] );
  fprintf( stdout, "\n" );

  return(0);
}
