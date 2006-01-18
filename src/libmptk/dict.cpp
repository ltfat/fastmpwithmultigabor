/******************************************************************************/
/*                                                                            */
/*                                 dict.cpp                                   */
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

/*****************************************/
/*                                       */
/* dict.cpp: methods for class MP_Dict_c */
/*                                       */
/*****************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/*******************************************/
/* Initialization including signal loading */
MP_Dict_c* MP_Dict_c::init(  const char *dictFileName, const char *sigFileName ) {

  const char* func = "MP_Dict_c::init(dictFileName)";
  MP_Dict_c *newDict = NULL;

  /* Instantiate and check */
  newDict = new MP_Dict_c();
  if ( newDict == NULL ) {
    mp_error_msg( func, "Failed to create a new dictionary.\n" );
    return( NULL );
  }

  /* Load the signal directly into the dictionnary */
  newDict->signal = MP_Signal_c::init( sigFileName );
  if ( newDict->signal == NULL ) {
    mp_error_msg( func, "Failed to create a new signal within the dictionary.\n" );
    delete( newDict );
    return( NULL );
  }
  /* If the siganl is OK: */
  newDict->sigMode = MP_DICT_INTERNAL_SIGNAL;
  /* Allocate the touch array */
  if ( newDict->alloc_touch() ) {
    mp_error_msg( func, "Failed to allocate and initialize the touch array"
		  " in the dictionary constructor. Returning a NULL dictionary.\n" );
    delete( newDict );
    return( NULL );
  }
 

  /* Add some blocks read from the dict file */
  newDict->add_blocks( dictFileName );

  return( newDict );
}


/**********************************************/
/* Initialization from a dictionary file name */
MP_Dict_c* MP_Dict_c::init(  const char *dictFileName ) {

  const char* func = "MP_Dict_c::init(dictFileName)";
  MP_Dict_c *newDict = NULL;

  /* Instantiate and check */
  newDict = new MP_Dict_c();
  if ( newDict == NULL ) {
    mp_error_msg( func, "Failed to create a new dictionary.\n" );
    return( NULL );
  }

  /* Add some blocks read from the dict file */
  newDict->add_blocks( dictFileName );
  /* Note: with a NULL signal, add_blocks will build all the signal-independent
     parts of the blocks. It is then necessary to run a dict.copy_signal(sig)
     or a dict.plug_signal(sig) to actually use the dictionary. */

  return( newDict );
}


/*************************************************/
/* Plain initialization, with no data whatsoever */
MP_Dict_c* MP_Dict_c::init( void ) {

  const char* func = "MP_Dict_c::init(void)";
  MP_Dict_c *newDict = NULL;

  /* Instantiate and check */
  newDict = new MP_Dict_c();
  if ( newDict == NULL ) {
    mp_error_msg( func, "Failed to create a new dictionary.\n" );
    return( NULL );
  }

  return( newDict );
}


/**************/
/* NULL constructor */
MP_Dict_c::MP_Dict_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Dict_c::MP_Dict_c()",
		"Constructing dict...\n" );

  signal = NULL;
  sigMode = MP_DICT_NULL_SIGNAL;
  numBlocks = 0;
  block = NULL;
  blockWithMaxIP = UINT_MAX;
  touch = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Dict_c::MP_Dict_c()",
		"Done.\n" );
}


/**************/
/* Destructor */
MP_Dict_c::~MP_Dict_c() {

  unsigned int i;

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Dict_c::~MP_Dict_c()",
		"Deleting dict...\n" );

  if ( (sigMode == MP_DICT_INTERNAL_SIGNAL) && ( signal != NULL ) ) delete signal;
  if ( block ) {
    for ( i=0; i<numBlocks; i++ ) { if ( block[i] ) delete( block[i] ); }
    free( block );
  }
  if ( touch ) free( touch );

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Dict_c::~MP_Dict_c()",
		"Done.\n" );
}



/***************************/
/* I/O METHODS             */
/***************************/

/******************************/
/* Addition of a single block */
int MP_Dict_c::add_block( MP_Block_c *newBlock ) {

  const char* func = "MP_Dict_c::add_block( newBlock )";
  MP_Block_c **tmp;

  if( newBlock != NULL ) {

    /* Increase the size of the array of blocks... */
    if ( (tmp = (MP_Block_c**) realloc( block, (numBlocks+1)*sizeof(MP_Block_c*) )) == NULL ) {
      mp_error_msg( func, "Can't reallocate memory to add a new block to dictionary."
		    " No new block will be added, number of blocks stays [%d].\n",
		    numBlocks );
      return( 0 );
    }
    /* ... and store the reference on the newly created object. */
    else {
      block = tmp;
      block[numBlocks] = newBlock;
      numBlocks++;
    }
    return( 1 );

  }
  /* else, if the block is NULL, silently ignore it,
     just say that no block has been added. */
  else return( 0 );

}


/**************************/
/* Scanning from a stream */
extern int dict_scanner( FILE *fid, MP_Scan_Info_c *scanInfo );  
int MP_Dict_c::add_blocks( FILE *fid ) {
  
  const char* func = "MP_Dict_c::add_blocks(fid)";
  MP_Block_c *newBlock;
  MP_Scan_Info_c scanInfo;
  int event = NULL_EVENT;
  int count = 0;

  /* Read blocks */
  while ( (event != DICT_CLOSE) && (event != REACHED_END_OF_FILE) ) {

    /* Launch the scanner */
    event = dict_scanner( fid, &scanInfo );
    fflush( stderr ); /* Flush everything the scanner couldn't match */

    switch( event ) {

      /* If the scanner met a block, create it */
    case COMPLETED_BLOCK:
      newBlock = scanInfo.pop_block( signal );
      /* Check and append the block */
      if ( newBlock != NULL ) {
#ifndef NDEBUG
	//newBlock->info(stderr);
	write_block( stderr, newBlock ); fprintf( stderr, "\n" ); fflush(stderr);
#endif
	if ( add_block( newBlock ) != 1 ) {
	  mp_warning_msg( func, "Failed to add the %u-th block."
			  " Proceeding with the remaining blocks.\n",
			  scanInfo.blockCount );
	}
	else count++;
      }
      else {
	mp_warning_msg( func, "Failed to instantiate and add the %u-th block to the dictionary."
			" Proceeding with the remaining blocks.\n", scanInfo.blockCount );
      }
      break;

      /* Else, if the scanner crashed, interrupt and return */
    case ERROR_EVENT:
      mp_warning_msg( func, "The parser crashed somewhere after the %u-th block."
		      " Parsing interrupted, returning a dictionary with [%u] valid block(s) only.\n",
		      scanInfo.blockCount, count );
      return( count );
      break;

      /* If the end of the dictionary or file are met, return */
    case DICT_CLOSE:
    case REACHED_END_OF_FILE:
      mp_debug_msg( MP_DEBUG_FILE_IO, func, "Added [%u] blocks to the dictionary.\n", count );
      return ( count );
      break;

    default:
      mp_warning_msg( func, "The parser returned an unknown event somewhere after the %u-th block."
		      " Parsing interrupted, returning a dictionary with [%u] valid block(s) only.\n",
		      scanInfo.blockCount, count );
      return( count );
      break;

    }

  }

  /* These two last lines should never be reached */
  mp_debug_msg( MP_DEBUG_FILE_IO, func, "NEVER REACHED: Added [%u] blocks to the dictionary.\n",
		count );
  return ( count );
}


/************************/
/* Scanning from a file */
int MP_Dict_c::add_blocks( const char *fName ) {

  FILE *fid;
  int nAddedBlocks = 0;

  fid = fopen(fName,"r");
  if ( fid == NULL ) {
    mp_error_msg( "MP_Dict_c::add_blocks(fileName)",
		  "Could not open file %s to read a dictionary\n",
		  fName );
    return( -1 );
  }
  nAddedBlocks = add_blocks( fid );
  fclose( fid );

  return( nAddedBlocks );
}


/************************/
/* Printing to a stream */
int MP_Dict_c::print( FILE *fid ) {

  unsigned int i;
  int nChar = 0;

  /* Print the xml declaration */
  fprintf( fid, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" );
  /* Print the lib version */
  nChar += fprintf( fid, "<libVersion>%s</libVersion>\n", VERSION );  
  /* Print opening <dict> tag */
  nChar += fprintf( fid, "<dict>\n" );
  for ( i = 0; i < numBlocks; i++ ) {
    nChar += write_block( fid, block[i] );
  }
  /* Print the closing </dict> tag */
  nChar += fprintf( fid, "</dict>\n");

  return( nChar );
}


/**********************/
/* Printing to a file */
int MP_Dict_c::print( const char *fName ) {

  FILE *fid;
  int nChar = 0;

  fid = fopen(fName,"w");
  if ( fid == NULL ) {
    mp_error_msg( "MP_Dict_c::print(fileName)",
		  "Could not open file %s to write a dictionary\n",
		  fName );
    return( -1 );
  }
  nChar = print( fid );
  fclose ( fid );

  return( nChar );
}


/***************************/
/* MISC METHODS            */
/***************************/


/******************************/
/* Return the number of atoms */
unsigned long int MP_Dict_c::size(void) {

  unsigned long int numAtoms = 0;
  unsigned int i;

  for (i = 0; i < numBlocks; i++)
    numAtoms += block[i]->size();

  return(numAtoms);
}


/*****************************************/
/* Copy a new signal into the dictionary */
int MP_Dict_c::copy_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Dict_c::copy_signal( signal )";
  unsigned int i;
  int check = 0;

  if ( setSignal != NULL ) {
    signal = new MP_Signal_c( *setSignal );
    sigMode = MP_DICT_INTERNAL_SIGNAL;
  }
  else {
    signal = NULL;
    sigMode = MP_DICT_NULL_SIGNAL;
  }

  /* Allocate the touch array
     (alloc_touch() will automatically manage the NULL signal case) */
  if ( alloc_touch() ) {
    mp_error_msg( func, "Failed to allocate and initialize the touch array"
		  " in the dictionary constructor. Signal and touch will stay NULL.\n" );
    delete( signal );
    signal = NULL;
    sigMode = MP_DICT_NULL_SIGNAL;
    return( 1 );
  }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ ) {
    if ( block[i]->plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
		    " Proceeding with the remaining blocks.\n", i );
      check = 1;
    }
  }

  return( check );
}


/*****************************************/
/* Load a new signal into the dictionary,
   from a file */
int MP_Dict_c::copy_signal( const char *fName ) {

  const char* func = "MP_Dict_c::copy_signal( fileName )";
  unsigned int i;
  int check = 0;

  assert( fName != NULL );

  signal = MP_Signal_c::init( fName );
  if ( signal == NULL ) {
    mp_error_msg( func, "Failed to instantiate a signal from file [%s]\n",
		  fName );
    sigMode = MP_DICT_NULL_SIGNAL;
    alloc_touch(); /* -> Nullifies the touch array when signal is NULL. */
    return( 1 );
  }
  /* else  */
  sigMode = MP_DICT_INTERNAL_SIGNAL;

  /* Allocate the touch array */
  if ( alloc_touch() ) {
    mp_error_msg( func, "Failed to allocate and initialize the touch array"
		  " in the dictionary constructor. Signal and touch will stay NULL.\n" );
    delete( signal );
    signal = NULL;
    sigMode = MP_DICT_NULL_SIGNAL;
    return( 1 );
  }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ ) {
    if ( block[i]->plug_signal( signal ) ) {
      mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
		    " Proceeding with the remaining blocks.\n", i );
      check = 1;
    }
  }

  return( check );
}


/*****************************************/
/* Copy a new signal into the dictionary */
int MP_Dict_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Dict_c::plug_signal( signal )";
  int check = 0;
  unsigned int i;

  if ( setSignal != NULL ) {
    signal = setSignal;
    sigMode = MP_DICT_EXTERNAL_SIGNAL;
  }
  else {
    signal = NULL;
    sigMode = MP_DICT_NULL_SIGNAL;
  }

  /* Allocate the touch array
     (alloc_touch() will automatically manage the NULL signal case) */
  if ( alloc_touch() ) {
    mp_error_msg( func, "Failed to allocate and initialize the touch array"
		  " in the dictionary constructor. Signal and touch will stay NULL.\n" );
    signal = NULL;
    sigMode = MP_DICT_NULL_SIGNAL;
    return( 1 );
  }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ ) {
    if ( block[i]->plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
		    " Proceeding with the remaining blocks.\n", i );
      check = 1;
    }
  }

  return( check );
}


/*********************************/
/* Allocation of the touch array */
int MP_Dict_c::alloc_touch( void ) {

  const char* func = "MP_Dict_c::alloc_touch()";

  /* If touch already exists, free it; guarantee that it is NULL. */
  if ( touch ) { free( touch ); touch = NULL; }

  /* Check if a signal is present: */
  if ( signal != NULL ) {

    /* Allocate the touch array */
    if ( (touch = (MP_Support_t*) malloc( signal->numChans*sizeof(MP_Support_t) )) == NULL ) {
      mp_error_msg( func, "Failed to allocate the array of touched supports"
		    " in the dictionary. The touch array is left NULL.\n" );
      return( 1 );
    }
    /* If the allocation went OK, initialize the array:*/
    else {
      int i;
      /* Initially we have to consider the whole signal as touched */
      for ( i = 0; i < signal->numChans; i++ ) {
	touch[i].pos = 0;
	touch[i].len = signal->numSamples;
      }
    }

  }
  /* Else, if the signal is NULL, don't allocate touch. */

  return( 0 );
}


/*************************/
/* Delete all the blocks */
int MP_Dict_c::delete_all_blocks( void ) {

  unsigned int i;
  unsigned int oldNumBlocks = numBlocks;

  for( i=0; i<numBlocks; i++) {
    if ( block[i] ) delete( block[i] );
  }
  if ( block ) free( block );
  block = NULL;
  numBlocks = 0;
  blockWithMaxIP = 0;

  return( oldNumBlocks );
}


/************************************/
/* Update of all the inner products which need to be updated, according to the 'touch' field */
MP_Real_t MP_Dict_c::update( void ) {

  unsigned int i;
  MP_Real_t tempMax = -1.0;
  MP_Real_t val;
  MP_Support_t frameSupport;

  for( i=0; i<numBlocks; i++) {
    /* Recompute the inner products */
    frameSupport = block[i]->update_ip( touch );
    /* Recompute and locate the max inner product within a block */
    val = block[i]->update_max( frameSupport );
    /* Locate the max inner product across the blocks */
    if ( val > tempMax ) { tempMax = val; blockWithMaxIP = i; }
  }

  return ( tempMax );
}

/**********************************************/
/* Forces an update of all the inner products */
MP_Real_t MP_Dict_c::update_all( void ) {

  unsigned int i;
  MP_Real_t tempMax = -1.0;
  MP_Real_t val;
  MP_Support_t frameSupport;

  for( i=0; i<numBlocks; i++) {
    /* (Re)compute all the inner products */
    frameSupport = block[i]->update_ip( NULL );
    /* Refresh and locate the max inner product within a block */
    val = block[i]->update_max( frameSupport );
    /* Locate the max inner product across the blocks */
    if ( val > tempMax ) { tempMax = val; blockWithMaxIP = i; }
  }
  return( tempMax );
}

/************************************/
/* Create a new atom corresponding to the best atom of the best block. */
unsigned int MP_Dict_c::create_max_atom( MP_Atom_c** atom ) {

  const char* func = "MP_Dict_c::create_max_atom(**atom)";
  unsigned long int frameIdx;
  unsigned long int filterIdx;
  unsigned int numAtoms;

  /* 1) select the best atom of the best block */
  frameIdx =  block[blockWithMaxIP]->maxIPFrameIdx;
  filterIdx = block[blockWithMaxIP]->maxIPIdxInFrame[frameIdx];

  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,
		"Found max atom in block [%lu], at frame [%lu], with filterIdx [%lu].\n",
		blockWithMaxIP, frameIdx, filterIdx );

  /* 2) create it using the method of the block */
  numAtoms = block[blockWithMaxIP]->create_atom( atom, frameIdx, filterIdx );
  if ( numAtoms == 0 ) {
    mp_error_msg( func, "Failed to create the max atom from block[%ul].\n",
		  blockWithMaxIP );
    return( 0 );
  }

  return( numAtoms );
}

/******************************************/
/* Perform one matching pursuit iteration */
int MP_Dict_c::iterate_mp( MP_Book_c *book , MP_Signal_c *sigRecons ) {

  const char* func = "MP_Dict_c::iterate_mp(...)";
  int chanIdx;
  MP_Atom_c *atom;
  unsigned int numAtoms;

  /* Check if a signal is present */
  if ( signal == NULL ) {
    mp_error_msg( func, "There is no signal in the dictionary. You must"
		  " plug or copy a signal before you can iterate.\n" );
    return( 1 );
  }

  /* 1/ refresh the inner products
   * 2/ create the max atom and store it
   * 3/ substract it from the signal and add it to the approximant
   */

  /** 1/ (Re)compute the inner products according to the current 'touch' indicating where the signal 
   * may have been changed after the last update of the inner products */
  update();

  /** 2/ Create the max atom and store it in the book */
  numAtoms = create_max_atom( &atom );
  if ( numAtoms == 0 ) {
    mp_error_msg( func, "The Matching Pursuit iteration failed. Dictionary, book"
		  " and signal are left unchanged.\n" );
    return( 1 );
  }
  
  if ( book->append( atom ) != 1 ) {
    mp_error_msg( func, "Failed to append the max atom to the book.\n" );
    return( 1 );
  }

  /* 3/ Substract the atom's waveform from the analyzed signal */
  atom->substract_add( signal , sigRecons );

  /* 4/ Keep track of the support where the signal has been modified */
  for ( chanIdx=0; chanIdx < atom->numChans; chanIdx++ ) {
    touch[chanIdx].pos = atom->support[chanIdx].pos;
    touch[chanIdx].len = atom->support[chanIdx].len;
  } 

  return( 0 );
}
