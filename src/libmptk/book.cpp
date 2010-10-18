/******************************************************************************/
/*                                                                            */
/*                                 book.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
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
MP_Book_c* MP_Book_c::create() {

  const char* func = "MP_Book_c::create()";
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

MP_Book_c* MP_Book_c::create(MP_Chan_t setNumChans, unsigned long int setNumSamples, int setSampleRate ) {

const char* func = "MP_Book_c::create(MP_Chan_t numChans, unsigned long int numSamples, unsigned long int numAtoms )";
MP_Book_c *newBook = NULL;

/* Instantiate and check */
  newBook = create();
  if ( newBook == NULL ) {
    mp_error_msg( func, "Failed to create a new book.\n" );
    return( NULL );
  }
  newBook->numChans = setNumChans;
  newBook->numSamples = setNumSamples;
  newBook->sampleRate = setSampleRate;
  
 return( newBook );
}
/********************************************************/
/* Load from a stream, either in ascii or binary format */
MP_Book_c* MP_Book_c::create( FILE *fid ) {
  
  const char* func = "MP_Book_c::create(fid)";
  MP_Book_c *newBook = NULL;
  unsigned int fidNumChans;
  int fidSampleRate;
  unsigned long int i, fidNumAtoms, fidNumSamples;
  unsigned long int nRead = 0;
  char mode;
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  MP_Atom_c* newAtom = NULL;

  /* Read the format */
  if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) {
    mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
    return( NULL );
  }

  /* Try to determine the format */
  if      ( !strcmp( line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" ) ) mode = MP_TEXT;
  else if ( !strcmp( line, "bin\n" ) ) mode = MP_BINARY;
  else {
    mp_error_msg( func, "The loaded book has an erroneous file format."
		  " This book will remain un-changed.\n" );
    return( NULL );
  }

  /* Read the header */
  if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) ||
       (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
		" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\">\n",
		&fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str ) != 5 )
       ) {
    mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
    return( NULL );
  }
  newBook = create( fidNumChans, fidNumSamples, fidSampleRate);
  if ( newBook == NULL ) {
    mp_error_msg( func, "Failed to create a new book.\n" );
    return( NULL );
  }
  /* Read the atoms */
  for ( i=0; i<fidNumAtoms; i++ ) {
    /* Try to create a new atom */
    
    
   // newAtom = read_atom( fid, mode );
  /* Try to read the atom header */
  switch ( mode ) {

  case MP_TEXT:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t<atom type=\"%[a-z]\">\n", str ) != 1 ) ) {
      mp_error_msg( func, "Cannot scan the atom type (in text mode).\n");
    }
    break;

  case MP_BINARY:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "%[a-z]\n", str ) != 1 ) ) {
      mp_error_msg( func, "Cannot scan the atom type (in binary mode).\n");
    }
    break;

  default:
    mp_error_msg( func, "Unknown read mode in read_atom().\n");
    break;
  }
  /* Scan the hash map to get the create function of the atom*/
  MP_Atom_c* (*createAtom)( FILE *fid, const char mode ) = MP_Atom_Factory_c::get_atom_factory()->get_atom_creator( str );
  /* Scan the hash map to get the create function of the atom*/
  if ( NULL != createAtom ){ 
  /* Create the the atom*/
  newAtom = (*createAtom)(fid,mode);}
  else mp_error_msg( func, "Cannot read atoms of type '%s'\n",str);
  
  if ( NULL == newAtom )  mp_error_msg( func, "Failed to create an atom of type[%s].\n", str);

  /* In text mode... */
  if ( mode == MP_TEXT ) {
    /* ... try to read the closing atom tag */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( strcmp( line, "\t</atom>\n" ) ) ) {
      mp_error_msg( func, "Failed to read the </atom> tag.\n");
      if ( newAtom ) newAtom = NULL ;
    }
  }
       
    if ( newAtom == NULL ) mp_warning_msg( func, "Failed to read an atom. This atom will be skipped.\n");
    /* If the atom is OK, add it */
    else { newBook->append( newAtom ); nRead++; }
  }
  
  /* Read the terminating </book> tag */
  if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
       ( strcmp( line, "</book>\n" ) ) ) {
    mp_warning_msg( func, "Could not find the </book> tag."
		    " (%lu atoms added, %lu atoms expected.)\n",
		    nRead, fidNumAtoms );
  return( NULL );
  }

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
    for ( nAtom = 0; nAtom < numAtoms; nAtom++ ){
    if ( mode == MP_TEXT ) {
    fprintf( fid, "\t<atom type=\"");
    fprintf( fid, "%s", atom[nAtom]->type_name() );
    fprintf( fid, "\">\n" );
      /* Call the atom's write function */
    atom[nAtom]->write( fid, mode );
   
    }


    else if( mode == MP_BINARY ) {
    fprintf( fid, "%s\n", atom[nAtom]->type_name() );
      /* Call the atom's write function */
    atom[nAtom]->write( fid, mode );

    } else mp_error_msg( func, "Unknown write mode for Atom, Atom is skipped." );

  /* Print the closing tag if needed */
  if ( mode == MP_TEXT ) fprintf( fid, "\t</atom>\n" );
    
    
    }
    
     // write_atom( fid, mode, atom[nAtom] );
      // atom[nAtom]->write( fid, mode );
  }
  else {
   for ( i = 0, nAtom = 0; i < numAtoms; i++ ) {
      if ( mask->sieve[i] ) {
	//write_atom( fid, mode, atom[i] );
	// atom[i]->write( fid, mode );
	 if ( mode == MP_TEXT ) {
    fprintf( fid, "\t<atom type=\"");
    fprintf( fid, "%s", atom[i]->type_name() );
    fprintf( fid, "\">\n" );
      /* Call the atom's write function */
    atom[i]->write( fid, mode );
   
    }


    else if( mode == MP_BINARY ) {
    fprintf( fid, "%s\n", atom[i]->type_name() );
      /* Call the atom's write function */
    atom[i]->write( fid, mode );

    } else mp_error_msg( func, "Unknown write mode for Atom, Atom is skipped." );

  /* Print the closing tag if needed */
  if ( mode == MP_TEXT ) fprintf( fid, "\t</atom>\n" );
  
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

  if ( ( fid = fopen( fName, "wb" ) ) == NULL ) {
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
  MP_Atom_c* newAtom = NULL;

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
    //newAtom = read_atom( fid, mode );
     /* Try to read the atom header */
  switch ( mode ) {

  case MP_TEXT:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t<atom type=\"%[a-z]\">\n", str ) != 1 ) ) {
      mp_error_msg( func, "Cannot scan the atom type (in text mode).\n");
    }
    break;

  case MP_BINARY:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "%[a-z]\n", str ) != 1 ) ) {
      mp_error_msg( func, "Cannot scan the atom type (in binary mode).\n");
    }
    break;

  default:
    mp_error_msg( func, "Unknown read mode in read_atom().\n");
    break;
  }
  /* Scan the hash map to get the create function of the atom*/
  MP_Atom_c* (*createAtom)( FILE *fid, const char mode ) = MP_Atom_Factory_c::get_atom_factory()->get_atom_creator( str );
  /* Scan the hash map to get the create function of the atom*/
  if ( NULL != createAtom ){ 
  /* Create the the atom*/
  newAtom = (*createAtom)(fid,mode);}
  else mp_error_msg( func, "Cannot read atoms of type '%s'\n",str);
  
  if ( NULL == newAtom )  mp_error_msg( func, "Failed to create an atom of type[%s].\n", str);

  /* In text mode... */
  if ( mode == MP_TEXT ) {
    /* ... try to read the closing atom tag */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( strcmp( line, "\t</atom>\n" ) ) ) {
      mp_error_msg( func, "Failed to read the </atom> tag.\n");
      if ( newAtom ) newAtom = NULL ;
    }
  }
    
    
    if ( newAtom == NULL ) mp_warning_msg( func, "Failed to read an atom. This atom will be skipped.\n");
    /* If the atom is OK, add it */
    else { append( newAtom ); nRead++; }
  }

  /* Check the global data fields */

  if ( numChans != fidNumChans ) {
    mp_warning_msg( func, "The book object contains a number of channels [%i] different from"
		    " the one read in the stream [%i].\n",
		    numChans, fidNumChans );
  }
  if ( numSamples != fidNumSamples ) {
    mp_warning_msg( func, "The book object contains a number of samples [%lu] different from"
		    " the one read in the stream [%lu].\n",
		    numSamples, fidNumSamples );
	if(numSamples < fidNumSamples) {
		mp_warning_msg(func, "The book.numSamples has been set to match the stream numSamples\n");
		mp_warning_msg(func, "This is a new behaviour in MPTK 0.5.6 which will become standard\n");
		numSamples = fidNumSamples;		
	} else {
		mp_error_msg(func,"This is very weired, please check your book file\n");
	}
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

  if ( ( fid = fopen(fName,"rb") ) == NULL ) {
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
  bakStream = (FILE*) get_info_stream();
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
  MP_Chan_t numChansAtom;

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
/******************/
/* Append an atom */
unsigned long int MP_Book_c::append( MP_Book_c *newBook ) {
	
const char* func = "MP_Book_c::append(*book)";
unsigned long int nAppend = 0;
if (is_compatible_with(newBook)){
for (unsigned long int i = 0 ; i< newBook->numAtoms; i++){
     if (append( newBook->atom[i] ) ) nAppend++;
     
     
}

return (nAppend);}
else {mp_error_msg( func, "Books have not the same parameters  "
		       );
	
	 return (0);}

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
  MP_Chan_t checkedNumChans = 0;
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
/*unsigned long int MP_Book_c::build_waveform( MP_Signal_c *sig, MP_Mask_c* mask ) {
  const char *func = "MP_Book_c::build_waveform";
  unsigned long int i;
  unsigned long int n = 0;
  
  // check input
  if (NULL==sig) {
	mp_error_msg(func,"The signal is NULL.");
	return 0;
  }
  
  // allocate the signal at the right size
  if (sig->init_parameters( numChans, numSamples, sampleRate ) )
	{
      mp_error_msg( func, "Failed to perform the internal allocations for the reconstructed signal.\n" );
      return 0;
    }


  // add the atom waveforms
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) {
      atom[i]->substract_add( NULL, sig );
      n++;
	// #ifndef NDEBUG
	// display a 'progress bar'
	// fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]",
	//(int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
	//#endif
	// TODO: make a "progress bar" generic + text function
	//in mp_messaging.{h,cpp}
    }
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) {
	atom[i]->substract_add( NULL, sig );
	n++;
	  // #ifndef NDEBUG
	  // display a 'progress bar'
	  // fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]",
	  //   (int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
	  //   #endif
      }
    }
  }

	// #ifndef NDEBUG
	// terminate the display of the 'progress bar'
	// fprintf( stderr, "\r%2d %%\t [%lu]\t [%lu / %lu]\n",
	//    (int)(100*(float)i/(float)numAtoms), n, i, numAtoms );
	//  #endif

  return( n );
}
*/
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
MP_Atom_c* MP_Book_c::get_closest_atom(MP_Real_t time, MP_Real_t freq,
				       MP_Chan_t chanIdx, MP_Mask_c* mask,
				       unsigned long int *nClosest ) {

  unsigned long int i;
  MP_Atom_c *atomClosest = NULL;
  MP_Real_t dist, distClosest;
  
  //distClosest = 1e700;
  distClosest = 1.7e308;
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
MP_Bool_t MP_Book_c::can_append( FILE * fid ){
	
  const char* func = "MP_Book_c::can_append(fid)";
  unsigned int fidNumChans;
  int fidSampleRate;
  unsigned long int fidNumAtoms, fidNumSamples;
  char mode;
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
    /* Read the format */
  if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) {
    mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  }

  /* Try to determine the format */
  if      ( !strcmp( line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" ) ) mode = MP_TEXT;
  else if ( !strcmp( line, "bin\n" ) ) mode = MP_BINARY;
  else {
    mp_error_msg( func, "The loaded book has an erroneous file format."
		  " This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  }

  /* Read the header */
  if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) ||
       (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
		" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\">\n",
		&fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str ) != 5 )
       ) {
    mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  } else
  /* test compatibility */
  if ( ((sampleRate != 0) && (sampleRate == fidSampleRate)) && (( numChans != 0 ) && ( numChans == fidNumChans )) && ((numSamples != 0) && (numSamples == fidNumSamples)) ) {
  fseek ( fid , 0L , SEEK_SET );
  return( true );
  } else return false;
}
/***********************************/
/* Check compatibility with a mask */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}

/*****************************************/
/* Check compatibility betwenn two books */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Book_c *book ){
   return ( (numChans == book->numChans) && (numSamples == book->numSamples) && (sampleRate == book->sampleRate) );
}

/***************************************/
/* Check compatibility with parameters */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Chan_t testedNumChans, int testedSampleRate ){
return ( (numChans ==  testedNumChans) && (sampleRate == testedSampleRate ) );

}

/***********************************/
/* Check compatibility with signal */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Signal_c *sig ){
 return( (numChans == sig->numChans)  && (sampleRate == sig->sampleRate) );
 // && (numSamples == sig->numSamples)
}



