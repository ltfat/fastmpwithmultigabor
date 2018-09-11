/******************************************************************************/
/*                                                                            */
/*                             regression.cpp                                 */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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
 * $Author: broy $
 * $Date: 2007-01-22 12:12:05 +0100 (Mon, 22 Jan 2007) $
 * $Revision: 832 $
 *
 */

/**********************************************/
/*                                            */
/* regression.cpp: regression functions       */
/*                                            */
/**********************************************/

#include "mptk.h"
#include "mp_system.h"
#include "regression_constants.h"

/** A function implementing parabolic regression. */
int parabolic_regression( MP_Real_t *Al, MP_Real_t *Phil,
			  const int L,
			  MP_Real_t *a, MP_Real_t *b, MP_Real_t *d, MP_Real_t *e ) {

  assert( L > 0 );
  assert( L < MP_MAX_REGRESSION_SIZE );

  switch ( L ) {

  case 1:
    {
      static MP_Real_t v0, v1, v2;
      /* Al */
      v0 = Al[0];
      v1 = Al[1];
      v2 = Al[2];
      *a = MP_C0_1 * ( v0 + v2 ) + MP_minus_C1_1 * ( v0 + v1 + v2 );
      *b = MP_C2_1 * ( - v0 + v2 );
      /* Phil */
      v0 = Phil[0];
      v1 = Phil[1];
      v2 = Phil[2];
      *d = MP_C0_1 * ( v0 + v2 ) + MP_minus_C1_1 * ( v0 + v1 + v2 );
      *e = MP_C2_1 * ( - v0 + v2 );
    }
    break;
    
  case 2:
    {
      static MP_Real_t v0, v1, v2, v3, v4;
      /* Al */
      v0 = Al[0];
      v1 = Al[1];
      v2 = Al[2];
      v3 = Al[3];
      v4 = Al[4];
      *a = MP_C0_2 * ( 4*v0 + v1 + v3 + 4*v4 ) + MP_minus_C1_2 * ( v0 + v1 + v2 + v3 + v4 );
      *b = MP_C2_2 * ( - 2*v0 - v1 + v3 + 2*v4 );
      /* Phil */
      v0 = Phil[0];
      v1 = Phil[1];
      v2 = Phil[2];
      v3 = Phil[3];
      v4 = Phil[4];
      *d = MP_C0_2 * ( 4*v0 + v1 + v3 + 4*v4 ) + MP_minus_C1_2 * ( v0 + v1 + v2 + v3 + v4 );
      *e = MP_C2_2 * ( - 2*v0 - v1 + v3 + 2*v4 );
    }
    break;

  case 3:
    {
      static MP_Real_t v0, v1, v2, v3, v4, v5, v6;
      /* Al */
      v0 = Al[0];
      v1 = Al[1];
      v2 = Al[2];
      v3 = Al[3];
      v4 = Al[4];
      v5 = Al[5];
      v6 = Al[6];
      *a = MP_C0_3 * ( 9*v0 + 4*v1 + v2 + v4 + 4*v5 + 9*v6 )
	+ MP_minus_C1_3 * ( v0 + v1 + v2 + v3 + v4 + v5 + v6 );
      *b = MP_C2_3 * ( - 3*v0 - 2*v1 - v2 + v4 + 2*v5 + 3*v6 );
      /* Phil */
      v0 = Phil[0];
      v1 = Phil[1];
      v2 = Phil[2];
      v3 = Phil[3];
      v4 = Phil[4];
      v5 = Phil[5];
      v6 = Phil[6];
      *d = MP_C0_3 * ( 9*v0 + 4*v1 + v2 + v4 + 4*v5 + 9*v6 )
	+ MP_minus_C1_3 * ( v0 + v1 + v2 + v3 + v4 + v5 + v6 );
      *e = MP_C2_3 * ( - 3*v0 - 2*v1 - v2 + v4 + 2*v5 + 3*v6 );
    }
    break;

  default:
    {
      static int l;
      static MP_Real_t *pAl, *pPhil;
      static MP_Real_t C0[MP_MAX_REGRESSION_SIZE] = MP_C0_Table;
      static MP_Real_t minus_C1[MP_MAX_REGRESSION_SIZE] = MP_minus_C1_Table;
      static MP_Real_t C2[MP_MAX_REGRESSION_SIZE] = MP_C2_Table;
      static MP_Real_t accAC0, accAC1, accAC2;
      static MP_Real_t accPC0, accPC1, accPC2;
      static MP_Real_t val;
      for ( l = -L, pAl = Al, pPhil = Phil;
	    l <= L;
	    l++ ) {
	/* Accumulate for Al */
	val = (*pAl++);
	accAC1 += val;
	accAC2 += (l * val);
	accAC0 += (l * l * val);
	/* Accumulate for Phil */
	val = (*pPhil++);
	accPC1 += val;
	accPC2 += (l * val);
	accPC0 += (l * l * val);
      }
      *a = C0[L] * accAC0 + minus_C1[L] * accAC1;
      *b = C2[L] * accAC2;
      *d = C0[L] * accPC0 + minus_C1[L] * accPC1;
      *e = C2[L] * accPC2;
    }
    break;
  }

  return( 0 );
}
