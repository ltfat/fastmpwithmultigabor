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
 * CVS log:
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
#include "system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/******************************************/
/* Plain initialization, with signal copy */
MP_Dict_c::MP_Dict_c( const MP_Signal_c setSignal ) {

  /* Copy the signal into the dictionnary */
  signal = new MP_Signal_c( setSignal );

  /* Allocate the touch array */
  if ( (touch = (MP_Support_t*) malloc( signal->numChans*sizeof(MP_Support_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Dict_c() - Can't allocate array of supports in new dictionnary with [%u] channels."
	     " Touch array is left un-initialized.\n", signal->numChans );
  }
  else {
    int i;
    /* Initially we have to consider the whole signal as touched */
    for ( i=0; i < signal->numChans; i++ ) { touch[i].pos = 0; touch[i].len = signal->numSamples; }
  }
  numBlocks = 0;
  block = NULL;
  blockWithMaxIP = 0;

}

/****************************************************/
/* Plain initialization, with signal as a file name */
MP_Dict_c::MP_Dict_c(  const char *sigFileName ) {

  /* Copy the signal into the dictionnary */
  signal = new MP_Signal_c( sigFileName );

  /* Allocate the touch array */
  if ( (touch = (MP_Support_t*) malloc( signal->numChans*sizeof(MP_Support_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Dict_c(file) - Can't allocate array of supports in new dictionnary with [%u] channels."
	     " Touch array is left un-initialized.\n", signal->numChans );
  }
  else {
    int i;
    /* Initially we have to consider the whole signal as touched */
    for ( i=0; i < signal->numChans; i++ ) { touch[i].pos = 0; touch[i].len = signal->numSamples; }
  }
  numBlocks = 0;
  block = NULL;
  blockWithMaxIP = 0;

}

/**************/
/* Destructor */
MP_Dict_c::~MP_Dict_c() {

  unsigned int i;

#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- delete dict\n");
#endif

  if ( signal ) delete signal;
  for ( i=0; i<numBlocks; i++ ) {
    if ( block[i] ) delete( block[i] );
  }
  if ( block ) free( block );
  if ( touch ) free( touch );

#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- delete dict done\n");
#endif

}



/***************************/
/* I/O METHODS             */
/***************************/

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
  if ( fid==NULL) {
    fprintf(stderr,"mplib warning -- MP_Dict_c::print() - Could not open file %s to write a dictionary\n",fName);
    return(0);
  }
  nChar = print(fid);
  fclose (fid);
  return(nChar);

}


/**************************/
/* Scanning from a stream */
extern int dict_scanner( FILE *fid, MP_Scan_Info_c *scanInfo );  
int MP_Dict_c::add_blocks( FILE *fid ) {
  
  MP_Block_c *newBlock;
  MP_Scan_Info_c scanInfo;
  int event = NULL_EVENT;
  int count = 0;

  /* Read blocks */
  while ( (event != DICT_CLOSE) && (event != REACHED_END_OF_FILE) ) {
    /* Launch the scanner */
    event = dict_scanner( fid, &scanInfo );
    fflush( stderr ); /* Flush everything the scanner couldn't match */
    /* If the scanner met a block, create it */
    if (event == COMPLETED_BLOCK) {
      newBlock = scanInfo.pop_block( signal );
      /* Check and append the block */
      if ( newBlock != NULL ) {
#ifndef NDEBUG
	//newBlock->info(stderr);
	write_block( stderr, newBlock );
	fprintf( stderr, "\n" );
	fflush(stderr);
#endif
	add_block( newBlock );
	count++;
      }
      else {
	fprintf(stderr, "mplib warning -- MP_Dict_c::add_blocks(fid,scanInfo) - The %u-th block could not be instanciated."
		" It won't be added to the dictionary. Proceeding with the remaining blocks.\n", scanInfo.blockCount );
      }
    }
    else if (event == ERROR_EVENT) {
      fprintf(stderr, "mplib warning -- MP_Dict_c::add_blocks(fid,scanInfo) - The parser crashed somewhere"
	      " after the %u-th block. Parsing interrupted, returning a dictionary with [%u] valid block(s) only.\n",
	      scanInfo.blockCount, count );
      return( count );
    }
  }

#ifndef NDEBUG
  fprintf( stderr,"mplib DEBUG -- MP_Dict_c::add_blocks(fid,scanInfo) - Added [%u] blocks to the dictionary.\n", count );
  fflush( stderr );
#endif

  return ( count );
}


/************************/
/* Scanning from a file */
int MP_Dict_c::add_blocks(const char *fName) {

  FILE *fid;
  int nAddedBlocks;

  fid = fopen(fName,"r");
  if ( fid==NULL) {
    fprintf(stderr,"mplib error -- MP_Dict_c::add_blocks(fname) - Could not open file %s to read dictionary\n",
      fName);
    return(0);
  }

  nAddedBlocks = add_blocks(fid);
  fclose (fid);

  return(nAddedBlocks);
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


/***********************/
/* Addition of a block */
int MP_Dict_c::add_block( MP_Block_c *newBlock ) {

  MP_Block_c **tmp;

  assert( newBlock != NULL );

  /* Increase the size of the array of blocks... */
  if ( (tmp = (MP_Block_c**) realloc( block, (numBlocks+1)*sizeof(MP_Block_c*) )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Dict_c::add_block(block) - Can't reallocate memory to add a new block to dictionary. "
	     "No new block will be added, number of blocks stays [%d].\n",
	     numBlocks );
    return(0);
  }
  /* ... and store the reference on the newly created object. */
  else {
    block = tmp;
    block[numBlocks] = newBlock;
    numBlocks++;
    /* Next time we iterate, the newly added block will have to be fully updated */
    int i;
    for ( i=0; i<signal->numChans; i++ ) {
      touch[i].pos = 0;
      touch[i].len = signal->numSamples;
    }
  }

  return(1);
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

  unsigned long int atomIdx;
  unsigned int numAtoms;

  /* 1) select the best atom of the best block */
  atomIdx =  block[blockWithMaxIP]->maxAtomIdx;

  /* 2) create it using the method of the block */
  numAtoms = block[blockWithMaxIP]->create_atom( atom, atomIdx );

  return(numAtoms);
}

/******************************************/
/* Perform one matching pursuit iteration */
int MP_Dict_c::iterate_mp( MP_Book_c *book , MP_Signal_c *sigRecons ) {

  int chanIdx;
  MP_Atom_c *atom;
  unsigned long int atomIdx;
  unsigned int numAtoms;

  /* 1/ refresh the inner products
   * 2/ create the max atom and store it
   * 3/ substract it from the signal and add it to the approximant
   */

  /** 1/ (Re)compute the inner products according to the current 'touch' indicating where the signal 
   * may have been changed after the last update of the inner products */
  update();

  /** 2/ Create the max atom and store it in the book */
  atomIdx = block[blockWithMaxIP]->maxAtomIdx;
#ifndef NDEBUG
  fprintf( stderr, "iterate_mp() DEBUG -- Found max atom in block [%lu] with atomIdx [%lu].\n",
	   blockWithMaxIP, atomIdx );
#endif

  numAtoms = block[blockWithMaxIP]->create_atom( &atom, atomIdx );

  if (numAtoms == 0) {
    fprintf( stderr, "mplib error -- MP_Dict_c::iterate_mp() - Iteration failed in dictionary: block[%ul]->create_atom() "
	     "returned no atom. Dictionary, book and signal are left unchanged.\n",
	     blockWithMaxIP );
    return(0);
  } 
  
  book->append( atom );

  /* 3/ Substract the atom's waveform from the analyzed signal */
  atom->substract_add( signal , sigRecons );

  /* 4/ Keep track of the support where the signal has been modified */
  for ( chanIdx=0; chanIdx < atom->numChans; chanIdx++ ) {
    touch[chanIdx].pos = atom->support[chanIdx].pos;
    touch[chanIdx].len = atom->support[chanIdx].len;
  } 

  return(1);
}
