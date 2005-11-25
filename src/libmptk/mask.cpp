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
    maxNumAtoms = 0;
  }
  else {
    numAtoms = setNumAtoms;
    maxNumAtoms = setNumAtoms;
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


/******************************/
/* A useful growing routine   */
unsigned long int MP_Mask_c::grow( unsigned long int nElem ) {

  MP_Bool_t *tmp;
  unsigned long int newSize;

  /* If nElem is small, make a big realloc of MP_MASK_GRANULARITY elements. */
  if ( nElem < MP_MASK_GRANULARITY ) newSize = numAtoms + MP_MASK_GRANULARITY;
  else                               newSize = numAtoms + nElem;
  /* Actual realloc: */
  if ( ( tmp = (MP_Bool_t*) realloc( sieve, newSize * sizeof(MP_Bool_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::grow() - Can't reallocate storage space"
	     " for an array of booleans in the mask. The assignment fails, and the"
	     " mask will remain untouched.\n");
    fflush( stderr );
    return( 0 );
  }
  /* If realloc succeeds: */
  sieve = tmp;
  maxNumAtoms = newSize;

  return( newSize );
}

/******************************/
/* Append some MP_TRUE values */
unsigned long int MP_Mask_c::append_true( unsigned long int nElem ) {

  unsigned long int i;
  unsigned long int newSize = 1;

  assert( sieve != NULL );

  /* If the number of elements to add goes beyond the max sieve size, realloc: */
  if ( (numAtoms + nElem) > maxNumAtoms ) newSize = grow( nElem );
  if ( newSize == 0 ) return( 0 );  
  /* If the realloc succeeded or if there was enough space already, assign the new elements: */
  for ( i = numAtoms; i < (numAtoms + nElem); i++ ) sieve[i] = MP_TRUE;
  numAtoms = (numAtoms + nElem);

  return( numAtoms );
}

/*******************************/
/* Append some MP_FALSE values */
unsigned long int MP_Mask_c::append_false( unsigned long int nElem ) {

  unsigned long int i;
  unsigned long int newSize = 1;

  assert( sieve != NULL );

  /* If the number of elements to add goes beyond the max sieve size, realloc: */
  if ( (numAtoms + nElem) > maxNumAtoms ) newSize = grow( nElem );
  if ( newSize == 0 ) return( 0 );  
  /* If the realloc succeeded or if there was enough space already, assign the new elements: */
  for ( i = numAtoms; i < (numAtoms + nElem); i++ ) sieve[i] = MP_FALSE;
  numAtoms = (numAtoms + nElem);

  return( numAtoms );
}

/*******************************/
/* Append any MP_Bool_t value  */
unsigned long int MP_Mask_c::append( MP_Bool_t val ) {

  unsigned long int newSize = 1;

  assert( sieve != NULL );

  /* If the number of elements to add goes beyond the max sieve size, realloc: */
  if ( (numAtoms + 1) > maxNumAtoms ) newSize = grow( MP_MASK_GRANULARITY );
  if ( newSize == 0 ) return( 0 );  
  /* If the realloc succeeded or if there was enough space already, assign the new element: */
  sieve[numAtoms] = val;
  numAtoms++;

  return( numAtoms );
}


/***********************************/
/* Check compatibility with another mask book */
MP_Bool_t MP_Mask_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}


/***************************/
/* FILE I/O                */
/***************************/

/***********************************/
/* A method to read from a stream. */
unsigned long int MP_Mask_c::read_from_stream( FILE* fid ) {

  unsigned long int nRead = 0;
  unsigned long int expected = 0;
  MP_Bool_t* tmp;
  MP_Bool_On_Disk_t buff;
  unsigned long int i;

  /* Get the simple header announcing the number of atoms */
  if ( ( fread( &expected, sizeof(unsigned long int), 1, fid ) ) == 0 ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::read_from_stream() - Can't read expected number of"
	     " sieve coefficients from stream. Returning 0.\n");
    fflush( stderr );
    return( 0 );
  }

  /* Resize the sieve if needed */
  if ( expected != numAtoms ) {
    if ( ( tmp = (MP_Bool_t*) realloc( sieve, expected * sizeof(MP_Bool_t)) ) == NULL ) {
      fprintf( stderr, "mplib warning -- MP_Mask_c::read_from_stream() - Can't reallocate storage space"
	       " for an array of booleans in the new mask. The assignment fails, and the target"
	       " object will remain untouched.\n");
      fflush( stderr );
      return( 0 );
    }
    else {
      sieve = tmp;
      numAtoms = expected;
      maxNumAtoms = expected;
    }
  }

  /* Read and cast */
  for ( i = 0; i < expected; i++ ) {
    if ( ( fread( &buff, sizeof(MP_Bool_On_Disk_t), 1, fid ) ) == 0 ) {
      fprintf( stderr, "mplib warning -- MP_Mask_c::read_from_stream() - Can't read a new MP_Bool_On_Disk_t"
	       " from stream after %lu reads. Returning number of correctly read MP_Bool_On_Disk_t.\n", nRead );
      fflush( stderr );
      return( nRead );
    }
    else {
      nRead++;
      sieve[i] = (MP_Bool_t)(buff);
    }
  }

  return( nRead );
}

/***********************************/
/* A method to write to a file. */
unsigned long int MP_Mask_c::write_to_stream( FILE* fid ) {

  unsigned long int nWrite = 0;
  MP_Bool_On_Disk_t buff;
  unsigned long int i;

  /* Write the simple header indicating the number of coeffs in the sieve */
  if ( ( fwrite( &numAtoms, sizeof(unsigned long int), 1, fid ) ) == 0 ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::write_to_stream() - Can't write the number of"
	     " sieve coefficients to the stream. Returning 0.\n");
    fflush( stderr );
    return( 0 );
  }
  
  /* Cast and write the sieve */
  for ( i = 0; i < numAtoms; i++ ) {
    buff = (MP_Bool_On_Disk_t)( sieve[i] );
    if ( ( fwrite( &buff, sizeof(MP_Bool_On_Disk_t), 1, fid ) ) == 0 ) {
      fprintf( stderr, "mplib warning -- MP_Mask_c::write_to_stream() - Can't write a new MP_Bool_On_Disk_t"
	       " to the stream after %lu writes. Returning nWrite.\n", nWrite );
      fflush( stderr );
      return( nWrite );
    }
    else nWrite++;
  }

  return( nWrite );
}


/***********************************/
/* A method to read from a file. */
unsigned long int MP_Mask_c::read_from_file( const char* fName ) {

  unsigned long int nRead = 0;
  FILE* fid;

  if ( ( fid = fopen(fName,"r") ) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Mask_c::read_from_file() - Could not open file %s to load a mask.\n",
	     fName );
    return( 0 );
  }
  nRead = read_from_stream( fid );
  fclose( fid );

  return( nRead );
}

/***********************************/
/* A method to write to a file. */
unsigned long int MP_Mask_c::write_to_file( const char* fName ) {

  unsigned long int nWrite = 0;
  FILE* fid;

  if ( ( fid = fopen(fName,"w") ) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Mask_c::write_to_file() - Could not open file %s to write a mask.\n",
	     fName );
    return( 0 );
  }
  nWrite = write_to_stream( fid );
  fclose( fid );

  return( nWrite );
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

    if ( ( tmp = (MP_Bool_t*) realloc( sieve, from.maxNumAtoms * sizeof(MP_Bool_t)) ) == NULL ) {
      fprintf( stderr, "mplib warning -- MP_Mask_c::operator=() - Can't reallocate storage space"
	       " for an array of booleans in the new mask. The assignment fails, and the target"
	       " object will remain untouched.\n");
      fflush( stderr );
      return( *this );
    }
    else {
      sieve = tmp;
      numAtoms = from.numAtoms;
      maxNumAtoms = from.maxNumAtoms;
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

  assert( sieve != NULL );
  assert( m1.sieve != NULL );

  /* Check mask compatibility */
  if ( numAtoms != m1.numAtoms ) {
    fprintf( stderr, "mplib warning -- MP_Mask_c::operator&& - Can't perform AND between masks"
	     " of different lengths. Returning an empty mask.\n");
    fflush( stderr );
    if ( ret.sieve ) free( ret.sieve );
    ret.numAtoms = 0;
    ret.maxNumAtoms = 0;
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
    fprintf( stderr, "mplib warning -- MP_Mask_c::operator|| - Can't perform OR between masks"
	     " of different lengths. Returning an empty mask.\n");
    fflush( stderr );
    if ( ret.sieve ) free( ret.sieve );
    ret.numAtoms = 0;
    ret.maxNumAtoms = 0;
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

/*******************/
/* == (COMPARISON) */
MP_Bool_t MP_Mask_c::operator==( const MP_Mask_c& m1 ) {

  unsigned long int i;

  assert( sieve != NULL );
  assert( m1.sieve != NULL );

  if ( numAtoms != m1.numAtoms ) return ( (MP_Bool_t)( MP_FALSE ) );
  /* Browse until different values are found */
  for ( i = 0;
	(i < numAtoms) && (sieve[i]==m1.sieve[i]);
	i++ );
  /* Then check where the loop stopped */
  if ( i == numAtoms ) return( (MP_Bool_t)( MP_TRUE ) );
  else                 return( (MP_Bool_t)( MP_FALSE ) );

}

/***********************/
/* != (NEG COMPARISON) */
MP_Bool_t MP_Mask_c::operator!=( const MP_Mask_c& m1 ) {

  assert( sieve != NULL );
  assert( m1.sieve != NULL );

  return( !( (*this) == m1 ) );
}

