/******************************************************************************/
/*                                                                            */
/*                               general.cpp                                  */
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
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 13:38:02 $
 * $Revision: 1.2 $
 *
 */

/**********************************************/
/*                                            */
/* general.cpp: general purpose functions     */
/*                                            */
/**********************************************/

#include "mptk.h"
#include "system.h"


/** Finds out which frames intersect a given support */
void support2frame( MP_Support_t support, 
		    unsigned long int fLen ,
		    unsigned long int fShift,
		    unsigned long int *fromFrame,
		    unsigned long int *toFrame ) {

  unsigned long int fromSample = support.pos;
  unsigned long int toSample;

  fromSample = support.pos;
  *fromFrame = len2numFrames( fromSample, fLen, fShift );
  
  toSample = ( fromSample + support.len - 1 );
  *toFrame  = toSample / fShift ;

#ifndef NDEBUG
  assert ( *fromFrame < *toFrame );
#endif
}


/* Computes the phase of the projection of a real valued signal on
 *  the space spanned by a complex atom and its conjugate transpose. */
void complex2amp_and_phase( double re, double im,
			    double reCorrel, double imCorrel,
			    double *amp, double *phase ) { 

  double energy = re*re+im*im;
  double real, imag;
#ifndef NDEBUG
  assert( (reCorrel*reCorrel + imCorrel*imCorrel) <= 1.0 );
#endif

  /* It is very simple when the complex inner product is zero : the phase does not matter ! */
  if (energy == 0.0) {
    *amp   = 0.0;
    *phase = 0.0;
    return;
  } 

  /* Cf. explanations in general.h */
  if ( (reCorrel*reCorrel + imCorrel*imCorrel) < 1.0 ) {  
    real = (1-reCorrel)*re + imCorrel*im;
    imag = (1+reCorrel)*im + imCorrel*re;
    *amp   = 2*sqrt( real*real +imag*imag );
    *phase = atan2( imag, real ); /* the result is between -M_PI and MP_PI */
    return;
  }

  /* When the atom and its conjugate are aligned, they should be real 
   * and the phase is simply the sign of the inner product (re,im) = (re,0) */
#ifndef NDEBUG
  assert( reCorrel == 1.0 );
  assert( imCorrel == 0.0 );
  assert( im == 0 );
#endif
  
  *amp = sqrt(energy);
  if (re >= 0) { /* note that the case re==0 is impossible since we would have energy==0
		    which is already dealt with */
    *phase = 0.0; /* corresponds to the '+' sign */
  } else {
    *phase = M_PI; /* corresponds to the '-' sign exp(i\pi) */
  }
  return;
}

/** Compute an inner product between two signals */
double inner_product ( MP_Sample_t *in1, MP_Sample_t *in2, unsigned long int size ) {
  unsigned long int t;
  double ip = 0.0;
  MP_Sample_t *p1,*p2;

#ifndef NDEBUG
  assert( (in1!=NULL) && (in2!=NULL) );
#endif

  for (t=0, p1 = in1, p2 = in2; t < size; t++, p1++, p2++) {
    ip += ( (double)(*p1) * (double)(*p2) );
  }
  return(ip);
}


/* Ramoval of any blank character from a string */
char* deblank( char *str ) {

  char *pfrom = str;
  char *pto = str;

  while ( (*pfrom) != '\0' ) {

    /* Skip the blank chars */
    switch (*pfrom) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      pfrom++;
      break;
    default:
      (*pto++) = (*pfrom++);
      break;
    }

  }

  /* Close the target string */
  *pto = '\0';

  return( str );
}
