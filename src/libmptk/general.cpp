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
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

/**********************************************/
/*                                            */
/* general.cpp: general purpose functions     */
/*                                            */
/**********************************************/

#include "mptk.h"
#include "mp_system.h"
#include <fstream>

/** A generic byte_swapping function */
inline void mp_swap( void *buf, size_t s, size_t n ) {

  char c, *p1, *p2, *p = (char *)buf;
  size_t i, j, s2 = s >> 1;
  
  for ( i = 0; i < n; i++, p += s ) {
    p1 = p; p2 = p+s-1;
    for ( j = 0; j < s2; j++, p1++, p2-- ) {
      c = *p1;
      *p1 = *p2;
      *p2 = c;
    }
  }

}

/** A wrapper for fread which byte-swaps if the machine is BIG_ENDIAN. */
size_t mp_fread( void *buf, size_t size, size_t n, FILE *fid ) {
  size_t r;

  r = fread( buf, size, n, fid );
#ifdef WORDS_BIGENDIAN
  mp_swap( buf, size, n );
#endif

  return( r );
}

/** A wrapper for fwrite which byte-swaps if the machine is BIG_ENDIAN. */
size_t mp_fwrite( void *buf, size_t size, size_t n, FILE *fid ) {

  size_t r;

#ifdef WORDS_BIGENDIAN
  /* Swap before writing */
  mp_swap( buf, size, n );
#endif

  r = fwrite( buf, size, n, fid );

#ifdef WORDS_BIGENDIAN
  /* Return the buffer to its previous state */
  mp_swap( buf, size, n );
#endif

  return( r );

}



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
    *phase = MP_PI; /* corresponds to the '-' sign exp(i\pi) */
  }
  return;
}

/** Compute an inner product between two signals */
double inner_product ( MP_Real_t *in1, MP_Real_t *in2, unsigned long int size ) {
  unsigned long int t;
  double ip = 0.0;
  MP_Real_t *p1,*p2;

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

/* Append function for the MP_Var_Array_c class */
template <class TYPE>
int MP_Var_Array_c<TYPE>::append( TYPE newElem ) {

  if ( nElem == maxNElem ) 
  {
    TYPE* tmp;
    tmp = (TYPE*) realloc( elem, (maxNElem+blockSize)*sizeof(TYPE) );
    if ( tmp == NULL ) 
		return( 0 );
    else 
	{
      elem = tmp;
      memset( elem+maxNElem, 0, blockSize*sizeof( TYPE ) );
      maxNElem += blockSize;
    }
  }
  elem[nElem] = newElem;
  nElem++;

  return( 1 );
}

// Assignment operator
template <class TYPE>
MP_Var_Array_c<TYPE>& MP_Var_Array_c<TYPE>::operator=(const MP_Var_Array_c<TYPE>& cSource){
    TYPE* tmp;
    // check for self-assignment
    if (this == &cSource)
        return *this;
	// First we need to copy the elements
	nElem = cSource.nElem;
	maxNElem = cSource.maxNElem;
	blockSize = cSource.blockSize;
	// Second we need to allocate memory for our copy
	tmp = (TYPE*) malloc((maxNElem+blockSize)*sizeof(TYPE));
	if(tmp == NULL)
		throw bad_alloc();
    // Third we need to deallocate any value that elem is holding!
    free(elem);
    // allocate memory for our copy
	elem = tmp;
	// Copy the parameter the newly allocated memory
	memcpy(elem, cSource.elem, nElem*sizeof(TYPE));
	return *this;
}

/* Save function for the MP_Var_Array_c class */
template <class TYPE>
unsigned long int MP_Var_Array_c<TYPE>::save( const char* fName ) {

  FILE *fid;
  unsigned long int nWrite = 0;

  if ( (fid = fopen( fName, "w" )) == NULL ) {
    mp_error_msg( "MP_Var_Array_c::save(fName)",
		  "Failed to open the file [%s] for writing.\n",
		  fName );
    return( 0 );
  }
  nWrite = mp_fwrite( elem, sizeof(TYPE), nElem, fid );
  fclose( fid );
  return( nWrite );
}
/*
template <class TYPE>
unsigned long int MP_Var_Array_c<TYPE>::save_to_text( const char* fName ) {

  FILE *fid;
  unsigned long int nWrite = 0;

  if ( (fid = fopen( fName, "w" )) == NULL ) {
    mp_error_msg( "MP_Var_Array_c::save(fName)",
		  "Failed to open the file [%s] for writing.\n",
		  fName );
    return( 0 );
  }
  mp_fwrite( elem, sizeof(TYPE), nElem, fid );
  fclose( fid );
  return( nWrite );
}*/




template <class TYPE>
unsigned long int MP_Var_Array_c<TYPE>::save_ui_to_text( const char* fName ) {

  //FILE *fid;
  ofstream fid(fName);
  unsigned long int i;
  //if ( (fid = fopen( fName, "w" )) == NULL ) {
  if(!fid.is_open()){
    mp_error_msg( "MP_Var_Array_c::save(fName)",
		  "Failed to open the file [%s] for writing.\n",
		  fName );
    return( 0 );
  }
  for ( i = 0 ; i< nElem; i++) //fprintf (fid, "Iteration %lu Source [%lu]\n",i, elem[i]);
    fid << "Iteration " << i << " Source [" << elem[i] << "]\n";
  //fclose( fid );
  fid.close();
  return( i );
}
/* Specify the MP_Var_Array template for double */
template class MP_Var_Array_c<double>;

#if defined __GNUC__ && __GNUC__ < 3
template
#endif
int append ( MP_Var_Array_c<double> );
#if defined __GNUC__ && __GNUC__ < 3
template
#endif
unsigned long int save ( MP_Var_Array_c<double> );
unsigned long int save_ui_to_text ( MP_Var_Array_c<double> );
/* Specify the MP_Var_Array template for unsigned short int */
template class MP_Var_Array_c<unsigned short int>;

#if defined __GNUC__ && __GNUC__ < 3
template
#endif
int append ( MP_Var_Array_c<unsigned short int> );
#if defined __GNUC__ && __GNUC__ < 3
template
#endif
unsigned long int save ( MP_Var_Array_c<unsigned short int> );
unsigned long int save_ui_to_text ( MP_Var_Array_c<unsigned short int> );
