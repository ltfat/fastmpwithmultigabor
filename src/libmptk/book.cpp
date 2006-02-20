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

  /* Allocate the atom array */
  if ( (newBook->atom = (MP_Atom_c**) calloc( MP_BOOK_GRANULARITY, sizeof(MP_Atom_c*) )) == NULL ) {
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

  if ( atom ) {
    for ( i=0; i<numAtoms; i++ ) {
      if ( atom[i] ) delete( atom[i] );
    }
    free( atom );
  }

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
  unsigned int fidNumChans;
  int fidSampleRate;
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

  /* Read the atoms */
  for ( i=0; i<fidNumAtoms; i++ ) {
    /* Try to create a new atom */
    newAtom = read_atom( fid, mode );
    if ( newAtom == NULL ) mp_warning_msg( func, "Failed to read an atom. This atom will be skipped.\n");
    /* If the atom is OK, add it */
    else { append( newAtom ); nRead++; }
  }

  /* Check the global data fields */
  if ( numChans != fidNumChans ) {
    mp_warning_msg( func, "The book object contains a number of channels [%i] different from"
		    " the one read in the stream [%i] (probably more, from a previous initialization).\n",
		    numChans, fidNumChans );
  }
  if ( numSamples != fidNumSamples ) {
    mp_warning_msg( func, "The book object contains a number of samples [%lu] different from"
		    " the one read in the stream [%lu] (probably more, from a previous initialization).\n",
		    numSamples, fidNumSamples );
  }
  if ( (sampleRate != 0) && (sampleRate != fidSampleRate) ) {
    mp_warning_msg( func, "The book object contains a sample rate [%i] different from"
		    " the one read in the stream [%i]. Keeping the new sample rate [%i].\n",
		    sampleRate, fidSampleRate, fidSampleRate );
  }
  sampleRate = fidSampleRate;

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
  
  int nChar = 0;
  FILE* bakStream;
  void (*bakHandler)( void );

  /* Backup the current stream/handler */
  bakStream = get_info_stream();
  bakHandler = get_info_handler();
  /* Redirect to the given file */
  set_info_stream( fid );
  set_info_handler( MP_FLUSH );
  /* Launch the info output */
  nChar += info();
  /* Reset to the previous stream/handler */
  set_info_stream( bakStream );
  set_info_handler( bakHandler );

  return( nChar );
}


/*******************************************************************************/
/* Print human readable information about the book to the default info handler */
int MP_Book_c::info() {
  
  unsigned long int i;
  int nChar = 0;

  nChar += mp_info_msg( "BOOK", "Number of atoms              =[%lu]  (Current atom array size =[%lu])\n",
			numAtoms, maxNumAtoms );
  nChar += mp_info_msg( "  |-", "Number of channels           =[%d]\n",    numChans );
  nChar += mp_info_msg( "  |-", "Number of samples per channel=[%lu]\n",   numSamples );
  nChar += mp_info_msg( "  |-", "Sampling rate                =[%d]\n",    sampleRate );
  for ( i=0; i<numAtoms; i++ ) {
    nChar += mp_info_msg( "  |-", "--ATOM [%lu/%lu] info :\n", i+1, numAtoms );
    atom[i]->info();
  }
  nChar += mp_info_msg( "  O-", "End of book.\n",    sampleRate );

  return( nChar );
}


/*******************************************************************************/
/* Print human readable information about the book to the default info handler */
int MP_Book_c::short_info() {
  
  int nChar = 0;

  nChar += mp_info_msg( "BOOK", "[%lu] atoms (current atom array size = [%lu])\n",
			numAtoms, maxNumAtoms );
  nChar += mp_info_msg( "  |-", "[%lu] samples on [%d] channels; sample rate [%d]Hz.\n",
			numSamples, numChans, sampleRate );
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
    for ( i=0; i<numAtoms; i++ ) {
      if ( atom[i] ) delete( atom[i] );
    }
  }
  numAtoms = 0;

}


/******************/
/* Append an atom */
int MP_Book_c::append( MP_Atom_c *newAtom ) {

  const char* func = "MP_Book_c::append(*atom)";
  unsigned long int newLen;
  int numChansAtom;

  /* If the passed atom is NULL, silently ignore (but return 0 as the number of added atoms) */
  if( newAtom == NULL ) return( 0 );
  /* Else: */
  else {

    /* Re-allocate if the max storage capacity is attained for the list: */
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
    
    /* Hook the passed atom */
    atom[numAtoms] = newAtom;
    numAtoms++;
    
    /* Set the number of channels to the max among all the atoms */
    numChansAtom = newAtom->numChans;
    if ( numChans < numChansAtom ) numChans = numChansAtom;
    
    /* Rectify the numSamples if needed */
    newLen = newAtom->numSamples;
    if ( numSamples < newLen ) numSamples = newLen;

  }

  return( 1 );
}


/***********************************/
/* Re-check the number of samples  */
int MP_Book_c::recheck_num_samples() {
  unsigned long int i;
  unsigned long int checkedNumSamples = 0;
  MP_Bool_t ret = MP_TRUE;

  for ( i = 0; i < numAtoms; i++ ) {
    if ( checkedNumSamples < atom[i]->numSamples ) checkedNumSamples = atom[i]->numSamples;
  }
  ret = ( checkedNumSamples == numSamples );
  numSamples = checkedNumSamples;

  return( ret );
}


/***********************************/
/* Re-check the number of channels */
int MP_Book_c::recheck_num_channels() {
  unsigned long int i;
  int checkedNumChans = 0;
  MP_Bool_t ret = MP_TRUE;

  for ( i = 0; i < numAtoms; i++ ) {
    if ( checkedNumChans < atom[i]->numChans ) checkedNumChans = atom[i]->numChans;
  }
  ret = ( checkedNumChans == numChans );
  numChans = checkedNumChans;

  return( ret );
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

/******************************************************/
/*  Returns the atom which is the closest to a given 
 *  time-frequency location, as well as its index in the book->atom[] array
 */
MP_Atom_c* MP_Book_c::get_closest_atom(MP_Real_t time, MP_Real_t freq, int chanIdx, MP_Mask_c* mask, unsigned long int *nClosest ) {

  unsigned long int i;
  MP_Atom_c *atomClosest = NULL;
  MP_Real_t dist, distClosest;
  
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) {
      dist = atom[i]->dist_to_tfpoint( time, freq , chanIdx );
      if (NULL == atomClosest || dist < distClosest) {
	atomClosest = atom[i];
	*nClosest = i;
	distClosest = dist;
      }
    }
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { 
	dist = atom[i]->dist_to_tfpoint( time, freq , chanIdx );
	if (NULL == atomClosest || dist < distClosest) {
	  atomClosest = atom[i];
	  *nClosest = i;
	  distClosest = dist;
	}
      }
    }
  }
  return( atomClosest );
}

/***********************************/
/* Check compatibility with a mask */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}
