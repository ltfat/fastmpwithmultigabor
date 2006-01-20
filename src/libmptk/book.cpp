/******************************************************************************/
/*                                                                            */
/*                                 book.cpp                                   */
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
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/******************************************/
/*                                        */
/* book.cpp: methods for class MP_Book_c  */
/*                                        */
/******************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/******************/
/* Constructor    */
MP_Book_c* MP_Book_c::init() {

  const char* func = "MP_Book_c::init()";
  MP_Book_c *newBook = NULL;

  /* Instantiate and check */
  newBook = new MP_Book_c();
  if ( newBook == NULL ) {
    mp_error_msg( func, "Failed to create a new book.\n" );
    return( NULL );
  }

  /* Set the default sampleRate */
  newBook->sampleRate  = MP_SIGNAL_DEFAULT_SAMPLERATE;

  /* Allocate the atom array */
  if ( (newBook->atom = (MP_Atom_c**) malloc( MP_BOOK_GRANULARITY*sizeof(MP_Atom_c*) )) == NULL ) {
    mp_warning_msg( func, "Can't allocate storage space for [%lu] atoms in the new book."
		    " The atom array is left un-initialized.\n", MP_BOOK_GRANULARITY );
    newBook->atom = NULL;
    newBook->maxNumAtoms = 0;
  }
  else newBook->maxNumAtoms = MP_BOOK_GRANULARITY;

  return( newBook );
}

/***********************/
/* NULL constructor    */
MP_Book_c::MP_Book_c() {

  numAtoms    = 0;
  numChans    = 0;
  numSamples  = 0;
  sampleRate  = 0;
  atom = NULL;
  maxNumAtoms = 0;

}

/**************/
/* Destructor */
MP_Book_c::~MP_Book_c() {

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Book_c::~MP_Book_c()",
		"Deleting book...\n" );

  unsigned long int i;

  for ( i=0; i<numAtoms; i++ ) delete( atom[i] );
  free( atom );

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Book_c::~MP_Book_c()",
		"Done.\n" );
}



/***************************/
/* I/O METHODS             */
/***************************/

/********************************/
/* Print some atoms to a stream */
unsigned long int MP_Book_c::print( FILE *fid , const char mode, MP_Mask_c* mask) {
  
  const char* func = "MP_Book_c::print(fid,mask)";
  unsigned long int nAtom = 0;
  unsigned long int i;

  /* determine how many atoms the printed book will contain  */
  if ( mask == NULL ) nAtom = numAtoms;
  else {
    for (i=0; i<numAtoms; i++) {
      if (mask->sieve[i]) nAtom++;
    }
  }

  /* print the book format */
  if ( mode == MP_TEXT ) {
    fprintf( fid, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" );
  }
  else if( mode == MP_BINARY ) {
    fprintf( fid, "bin\n" );
  }
  else {
    mp_error_msg( func, "Unknown write mode.\n" );
    return( 0 );
  }
  /* Print the book header */
  fprintf( fid, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
	   " sampleRate=\"%d\" libVersion=\"%s\">\n",
	   nAtom, numChans, numSamples, sampleRate, VERSION );

  /* print the atoms */
  if ( mask == NULL ) {
    for ( nAtom = 0; nAtom < numAtoms; nAtom++ )
      write_atom( fid, mode, atom[nAtom] ); /*atom[nAtom]->write( fid, mode );*/
  }
  else {
   for ( i = 0, nAtom = 0; i < numAtoms; i++ ) {
      if ( mask->sieve[i] ) {
	write_atom( fid, mode, atom[i] ); /*atom[i]->write( fid, mode );*/
	nAtom++;
      }
    }
  }

  /* print the closing </book> tag */
  fprintf( fid, "</book>\n"); 

  return( nAtom );

}


/******************************/
/* Print some atoms to a file */
unsigned long int MP_Book_c::print( const char *fName , const char mode, MP_Mask_c* mask ) {

  FILE *fid;
  unsigned long int nAtom = 0;

  if ( ( fid = fopen( fName, "w" ) ) == NULL ) {
    mp_error_msg( "MP_Book_c::print(fname,mask)",
		  "Could not open file %s to print a book.\n", fName );
    return( 0 );
  }
  nAtom = print( fid, mode, mask );
  fclose( fid );

  return ( nAtom );
}


/***********************************/
/* Print all the atoms to a stream */
unsigned long int MP_Book_c::print( FILE *fid, const char mode ) {
  return( print( fid, mode, NULL ) );
}


/***********************************/
/* Print all the atoms to a file   */
unsigned long int MP_Book_c::print( const char *fName, const char mode ) {
  return( print( fName, mode, NULL ) );
}


/********************************************************/
/* Load from a stream, either in ascii or binary format */
unsigned long int MP_Book_c::load( FILE *fid ) {
  
  const char* func = "MP_Book_c::load(fid)";
  int fidNumChans, fidSampleRate;
  unsigned long int i, fidNumAtoms, fidNumSamples;
  unsigned long int nRead = 0;
  char mode;
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  MP_Atom_c* newAtom;

  /* Read the format */
  if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) {
    mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
    return( 0 );
  }

  /* Try to determine the format */
  if      ( !strcmp( line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" ) ) mode = MP_TEXT;
  else if ( !strcmp( line, "bin\n" ) ) mode = MP_BINARY;
  else {
    mp_error_msg( func, "The loaded book has an erroneous file format."
		  " This book will remain un-changed.\n" );
    return( 0 );
  }

  /* Read the header */
  if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) ||
       (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
		" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\">\n",
		&fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str ) != 5 )
       ) {
    mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
    return( 0 );
  }

  /* Check or fill the data fields */
  if ( numAtoms == 0 ) {
    numChans    = fidNumChans;
    numSamples  = fidNumSamples;
    sampleRate  = fidSampleRate;
  }
  /* If the atoms have different characteristics from the existing ones, don't append them: */
  else if ( (numChans != fidNumChans) || (numSamples != fidNumSamples) || (sampleRate != fidSampleRate) ) {
    mp_error_msg( func, "Trying to append incompatible atoms. This book will remain un-changed.\n" );
    /* Flush the atoms anyway, in case we are reading from a stream: */
    for ( i=0; i<fidNumAtoms; i++ ) {
      newAtom = read_atom( fid, mode );
      if ( newAtom == NULL ) mp_warning_msg( func, "Failed to flush an atom.\n");
      else nRead++;
    }
    /* Flush the terminating </book> tag */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( strcmp( line, "</book>\n" ) ) ) {
      mp_warning_msg( func, "Could not find the </book> tag."
		      " (%lu atoms flushed, %lu atoms expected.)\n",
		      nRead, fidNumAtoms );
    }
    return( 0 );
  }

  /* Read the atoms */
  for ( i=0; i<fidNumAtoms; i++ ) {
    /* Try to create a new atom */
    newAtom = read_atom( fid, mode );
    if ( newAtom == NULL ) mp_warning_msg( func, "Failed to read an atom. This atom will be skipped.\n");
    /* If the atom is OK, add it */
    else { append( newAtom ); nRead++; }
  }

  /* Read the terminating </book> tag */
  if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
       ( strcmp( line, "</book>\n" ) ) ) {
    mp_warning_msg( func, "Could not find the </book> tag."
		    " (%lu atoms added, %lu atoms expected.)\n",
		    nRead, fidNumAtoms );
  }

  return( nRead );
}


/********************/
/* Load from a file */
unsigned long int MP_Book_c::load( const char *fName ) {
  FILE *fid;
  unsigned long int nAtom = 0;

  if ( ( fid = fopen(fName,"r") ) == NULL ) {
    mp_error_msg( "MP_Book_c::load(fName)",
		  "Could not open file %s to scan for a book.\n", fName );
    return( 0 );
  }
  nAtom = load( fid );
  fclose( fid );

  return ( nAtom );
}


/***************************************************************/
/* Print human readable information about the book to a stream */
int MP_Book_c::info( FILE *fid ) {
  
  unsigned long int i;
  int nChar = 0;

  nChar += mp_info_msg( fid, "BOOK", "Number of atoms              =[%lu] ",   numAtoms );
  nChar += mp_info_msg( fid, "  |-", "(Current atom array size     =[%lu])\n", maxNumAtoms );
  nChar += mp_info_msg( fid, "  |-", "Number of channels           =[%d]\n",    numChans );
  nChar += mp_info_msg( fid, "  |-", "Number of samples per channel=[%lu]\n",   numSamples );
  nChar += mp_info_msg( fid, "  |-", "Sampling rate                =[%d]\n",    sampleRate );
  for ( i=0; i<numAtoms; i++ ) {
    nChar += mp_info_msg( fid, "  |-", "--ATOM [%lu/%lu] info :\n", i+1, numAtoms );
    atom[i]->info( fid );
  }
  nChar += mp_info_msg( fid, "  O-", "End of book.\n",    sampleRate );

  return( nChar );
}



/***************************/
/* MISC METHODS            */
/***************************/

/***********************/
/* Clear all the atoms */
void MP_Book_c::reset( void ) {

  unsigned long int i;

  if ( atom ) {
    for ( i=0; i<numAtoms; i++ ) delete( atom[i] );
    free(atom);
  }
  atom = NULL;
  numAtoms    = 0;
  maxNumAtoms = 0;
}


/******************/
/* Append an atom */
int MP_Book_c::append( MP_Atom_c *newAtom ) {

  const char* func = "MP_Book_c::append(*atom)";

  assert( newAtom != NULL );

  /* If the max storage capacity is attained for the list: */
  if (numAtoms == maxNumAtoms) {
    MP_Atom_c **tmp;
    /* re-allocate the atom array */
    if ( (tmp = (MP_Atom_c**) realloc( atom, (maxNumAtoms+MP_BOOK_GRANULARITY)*sizeof(MP_Atom_c*) )) == NULL ) {
      mp_error_msg( func, "Can't allocate space for [%d] more atoms."
		    " The book is left untouched, the passed atom is not saved.\n",
		    MP_BOOK_GRANULARITY );
      return( 0 );
    }
    else {
      atom = tmp;
      maxNumAtoms += MP_BOOK_GRANULARITY;
    }
  }

  /* If the atom is the first one to be stored, set up the number of channels */
  if (numAtoms == 0) {
    numChans = newAtom->numChans;
  }
  /* Otherwise check that the new atom has the right number of channels */
  else if (newAtom->numChans != numChans) {
    mp_error_msg( func, "Cannot append an atom with [%d] channels"
		  " in a book with [%d] channels\n", newAtom->numChans, numChans );
    
    return( 0 );
  } 

  /* Hook the passed atom */
  atom[numAtoms] = newAtom;
  numAtoms++;

  return(1);
}

/***************************************************************/
/* Substract or add the sum of (some) atoms from / to a signal */
unsigned long int MP_Book_c::substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd, MP_Mask_c* mask ) {

  unsigned long int i;
  unsigned long int n = 0;
  
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) atom[i]->substract_add(sigSub,sigAdd);
    n = numAtoms;
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { atom[i]->substract_add(sigSub,sigAdd); n++; }
    }
  }
  return( n );
}

/***********************************************/
/* Build the sum of (some) atoms into a signal */
unsigned long int MP_Book_c::build_waveform( MP_Signal_c *sig, MP_Mask_c* mask ) {

  unsigned long int i;
  unsigned long int n = 0;
  
  /* allocate the signal at the right size */
  sig->init( numChans, numSamples, sampleRate );

  /* add the atom waveforms */
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) {
      atom[i]->substract_add( NULL, sig );
      n++;
      /* #ifndef NDEBUG */
      /* display a 'progress bar' */
      /* fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]",
	 (int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
	 #endif*/
      /* TODO: make a "progress bar" generic + text function
	 in mp_messaging.{h,cpp} */
    }
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) {
	atom[i]->substract_add( NULL, sig );
	n++;
	/* #ifndef NDEBUG */
	/* display a 'progress bar' */
	/* fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]",
	   (int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
	   #endif */
      }
    }
  }

  /* #ifndef NDEBUG */
  /* terminate the display of the 'progress bar' */
  /* fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]\n",
     (int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
     #endif */

  return( n );
}

/******************************************************/
/* Adds the sum of the pseudo Wigner-Ville distributions
   of some atoms to a time-frequency map */
unsigned long int MP_Book_c::add_to_tfmap(MP_TF_Map_c *tfmap, const char tfmapType, MP_Mask_c* mask ) {

  unsigned long int i;
  unsigned long int n = 0;
  
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) atom[i]->add_to_tfmap( tfmap, tfmapType );
    n = numAtoms;
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { atom[i]->add_to_tfmap( tfmap, tfmapType ); n++; }
    }
  }
  return( n );
}


/***********************************/
/* Check compatibility with a mask */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}
