/******************************************************************************/
/*                                                                            */
/*                                 mask.cpp                                   */
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
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

/**************************************************************/
/*                                                            */
/* mask.cpp: methods for the MP_Mask_c class.                 */
/*                                                            */
/**************************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/****************************/
/* Size-setting constructor */
MP_Mask_c::MP_Mask_c( unsigned long int setNumAtoms ) {

  /* Allocate the sieve array: */
  if ( ( sieve = (MP_Bool_t*) malloc( setNumAtoms * sizeof(MP_Bool_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c() - Can't allocate storage space for an array"
	     " of booleans in the new mask. The array will stay NULL.\n");
    fflush( stderr );
    numAtoms = 0;
  }
  else {
    numAtoms = setNumAtoms;
    /* By default, let all the atoms pass through: */
    reset_all_true();
  }

}


/**************/
/* Destructor */
MP_Mask_c::~MP_Mask_c() {

  if ( sieve ) free ( sieve );

}


/***************************/
/* FACTORY METHOD          */
/***************************/
MP_Mask_c* MP_Mask_c::init( unsigned long int setNumAtoms ) {

  MP_Mask_c *mask = NULL;

  /* Make a new mask */
  if ( (mask = new MP_Mask_c( setNumAtoms )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::init() - Can't create a new mask."
	     " Returning a NULL mask.\n");
    fflush( stderr );
    return( NULL );
  }
  /* Check the new mask */
  if ( mask->sieve == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::init() - Sieve array in mask is NULL."
	     " Returning a NULL mask.\n");
    fflush( stderr );
    return( NULL );
  }

  return( mask );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/*******************************/
/* Set one element to MP_TRUE. */
void MP_Mask_c::set_true( unsigned long int i ) {
  assert( sieve != NULL );
  sieve[i] = MP_TRUE;
}

/********************************/
/* Set one element to MP_FALSE. */
void MP_Mask_c::set_false( unsigned long int i ) {
  assert( sieve != NULL );
  sieve[i] = MP_FALSE;
}

/************************/
/* Reset all to MP_TRUE */
void MP_Mask_c::reset_all_true( void ) {

  unsigned long int i;

  assert( sieve != NULL );
  for ( i = 0; i < numAtoms; i++ ) sieve[i] = MP_TRUE;

}

/************************/
/* Reset all to MP_FALSE */
void MP_Mask_c::reset_all_false( void ) {

  unsigned long int i;

  assert( sieve != NULL );
  for ( i = 0; i < numAtoms; i++ ) sieve[i] = MP_FALSE;

}

/***********************************/
/* Check compatibility with another mask book */
int MP_Mask_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}

/***********************************/
/* Check compatibility with a book */
int MP_Mask_c::is_compatible_with( MP_Book_c book ) {
  return( numAtoms == book.numAtoms );
}


/***************************/
/* OPERATORS               */
/***************************/

/********************************/
/* Assignment operator          */
MP_Mask_c& MP_Mask_c::operator=( const MP_Mask_c& from ) {

  MP_Bool_t *tmp;

  //fprintf( stdout, "COPYING...\n"); fflush( stdout );

  /* If sizes are different, reallocate the sieve array: */
  if ( numAtoms != from.numAtoms ) {

    if ( ( tmp = (MP_Bool_t*) realloc( sieve, from.numAtoms * sizeof(MP_Bool_t)) ) == NULL ) {
      fprintf( stderr, "mplib warning -- MP_Mask_c::operator=() - Can't reallocate storage space"
	       " for an array of booleans in the new mask. The assignment fails, and the target"
	       " object will remain untouched.\n");
      fflush( stderr );
      return( *this );
    }
    else {
      sieve = tmp;
      numAtoms = from.numAtoms;
    }
  }

  /* Once size is OK, copy the sieve */
  memcpy( sieve, from.sieve, numAtoms*sizeof(MP_Bool_t) );

  return( *this );
}

/*******/
/* AND */
MP_Mask_c MP_Mask_c::operator&&( const MP_Mask_c& m1 ) {

  MP_Mask_c ret( numAtoms );
  unsigned long int i;

  assert( sieve != NULL );
  assert( m1.sieve != NULL );

  /* Check mask compatibility */
  if ( numAtoms != m1.numAtoms ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::operator& - Can't perform AND between masks"
	     " of different lengths. Returning an empty mask.\n");
    fflush( stderr );
    if ( ret.sieve ) free( ret.sieve );
    ret.numAtoms = 0;
  }
  /* If masks are compatible, perform the and */
  else {
    unsigned long int i;
    for (i = 0; i < ret.numAtoms; i++ ) {
      ret.sieve[i] = ( sieve[i] && m1.sieve[i] );
      //fprintf( stderr, "%d && %d = %d\n", sieve[i], m1.sieve[i], ret.sieve[i] ); fflush(stderr);
    }
  }

  return( ret );
}


/******/
/* OR */
MP_Mask_c MP_Mask_c::operator||( const MP_Mask_c& m1 ) {

  MP_Mask_c ret( numAtoms );

  assert( sieve != NULL );
  assert( m1.sieve != NULL );

  /* Check mask compatibility */
  if ( numAtoms != m1.numAtoms ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::operator& - Can't perform AND between masks"
	     " of different lengths. Returning an empty mask.\n");
    fflush( stderr );
    if ( ret.sieve ) free( ret.sieve );
    ret.numAtoms = 0;
  }
  /* If masks are compatible, perform the and */
  else {
    unsigned long int i;
    for (i = 0; i < ret.numAtoms; i++ ) {
      ret.sieve[i] = ( sieve[i] || m1.sieve[i] );
      //fprintf( stderr, "%d || %d = %d\n", sieve[i], m1.sieve[i], ret.sieve[i] ); fflush(stderr);
    }
  }

  return( ret );
}

/*************/
/* Unary NOT */
MP_Mask_c MP_Mask_c::operator!( void ) {

  unsigned long int i;
  MP_Mask_c ret( numAtoms );

  assert( sieve != NULL );
  for (i = 0; i < ret.numAtoms; i++ ) {
    ret.sieve[i] = !( sieve[i] );
    //fprintf( stderr, "!%d = %d\n", sieve[i], ret.sieve[i] ); fflush(stderr);
  }

  return( ret );
}

