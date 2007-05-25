/******************************************************************************/
/*                                                                            */
/*                        anywave_hilbert_block.cpp                           */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* R�mi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Mar 07 2006 */
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
 * $Date$
 * $Revision$
 *
 */

/***************************************************/
/*                                                 */
/* anywave_block.cpp: methods for anywave blocks */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Anywave_Hilbert_Block_c* MP_Anywave_Hilbert_Block_c::init( MP_Signal_c *setSignal,
							      const unsigned long int setFilterShift,
							      char* anywaveTableFilename,
                                          const unsigned long int setBlockOffset ) {

  const char* func = "MP_Anywave_Hilbert_Block_c::init()";
  MP_Anywave_Hilbert_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Anywave_Hilbert_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Anywave hilbert block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterShift, anywaveTableFilename, setBlockOffset ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Anywave hilbert block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Anywave hilbert block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
  
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Anywave_Hilbert_Block_c::init_parameters( const unsigned long int setFilterShift,
						 char* anywaveTableFilename,
                                       const unsigned long int setBlockOffset ) {

  const char* func = "MP_Anywave_Hilbert_Block_c::init_parameters(...)";

  /* Go up the inheritance graph : copied because the convolution
     object cannot be initialized as in MP_Anywave_Block_c */ 
  extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;  

  /* Load the table */
  tableIdx = MP_GLOBAL_ANYWAVE_SERVER.add(anywaveTableFilename);
  if ( tableIdx >= MP_GLOBAL_ANYWAVE_SERVER.maxNumTables ) {
    /* if the addition of a anywave table in the anywave server failed */ 
    mp_error_msg( func,"The anywave table can't be added to the anywave server. The anywave table remain NULL" );
    tableIdx = 0;
    return(1);
  } else {
    anywaveTable = MP_GLOBAL_ANYWAVE_SERVER.tables[tableIdx];
  }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( anywaveTable->filterLen, setFilterShift, anywaveTable->numFilters,setBlockOffset ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Anywave block.\n" );
    return( 1 );
  }

  init_tables();
 
  /* Create the convolution and hilbert convolution objects */ 
  if (convolution != NULL) { delete(convolution); }
  if ( ( convolution = new MP_Convolution_Fastest_c( anywaveTable, anywaveRealTable, anywaveHilbertTable, setFilterShift ) ) == NULL ) {
    return(1);
  }

  return( 0 );
}

void MP_Anywave_Hilbert_Block_c::init_tables( void ) {

  extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;  
  char* str;

  if ( ( str = (char*) malloc( MP_MAX_STR_LEN * sizeof(char) ) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::init_tables()","The string str cannot be allocated.\n" );    
  }

  /* create the real table if needed */  
  strcpy(str, MP_GLOBAL_ANYWAVE_SERVER.get_filename( tableIdx ));
  str = strcat(str,"_real");
  realTableIdx = MP_GLOBAL_ANYWAVE_SERVER.get_index( str );
  if (realTableIdx == MP_GLOBAL_ANYWAVE_SERVER.numTables) {
    anywaveRealTable = anywaveTable->copy();
    anywaveRealTable->center_and_denyquist();
    anywaveRealTable->normalize();
    anywaveRealTable->set_table_file_name(str);
    realTableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( anywaveRealTable );
  } else {
    anywaveRealTable = MP_GLOBAL_ANYWAVE_SERVER.tables[realTableIdx];
  }

  /* create the hilbert table if needed */
  strcpy(str, MP_GLOBAL_ANYWAVE_SERVER.get_filename( tableIdx ));
  str = strcat(str,"_hilbert");
  hilbertTableIdx = MP_GLOBAL_ANYWAVE_SERVER.get_index( str );
  if (hilbertTableIdx == MP_GLOBAL_ANYWAVE_SERVER.numTables) {
    /* need to create a new table */
    anywaveHilbertTable = anywaveTable->create_hilbert_dual(str);
    anywaveHilbertTable->normalize();    
    hilbertTableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( anywaveHilbertTable );
  } else {
    anywaveHilbertTable = MP_GLOBAL_ANYWAVE_SERVER.tables[hilbertTableIdx];
  }

}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Anywave_Hilbert_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Anywave_Hilbert_Block_c::plug_signal( signal )";

  if ( MP_Anywave_Block_c::plug_signal( setSignal ) ) {
    /* Go up the inheritance graph */
    mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
    nullify_signal();
    return( 1 );
  }    
  
  return( 0 );

}

/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Anywave_Hilbert_Block_c::nullify_signal( void ) {

  MP_Anywave_Block_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Anywave_Hilbert_Block_c::MP_Anywave_Hilbert_Block_c( void )
  :MP_Anywave_Block_c() {

    mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Hilbert_Block_c::MP_Anywave_Hilbert_Block_c()",
		  "Constructing an Anywave hilbert block...\n" );
    
    anywaveRealTable = NULL;
    realTableIdx = 0;
    anywaveHilbertTable = NULL;
    hilbertTableIdx = 0;
    
    mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Hilbert_Block_c::MP_Anywave_Hilbert_Block_c()",
		  "Done.\n" );
  }

/**************/
/* Destructor */
MP_Anywave_Hilbert_Block_c::~MP_Anywave_Hilbert_Block_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Hilbert_Block_c::~MP_Anywave_Hilbert_Block_c()",
		"Deleting an Anywave hilbert block...\n" );

  anywaveRealTable = NULL;
  anywaveHilbertTable = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Hilbert_Block_c::~MP_Anywave_Hilbert_Block_c()",
		"Done.\n" );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_Anywave_Hilbert_Block_c::type_name() {
  return("anywavehilbert");
}


/********/
/* Readable text dump */
int MP_Anywave_Hilbert_Block_c::info( FILE* fid ) {

  int nChar = 0;

  nChar += fprintf( fid, "mplib info -- ANYWAVE HILBERT BLOCK" );
  nChar += fprintf( fid, " of length [%lu], shifted by [%lu] samples, with [%lu] different waveforms;\n",
		    filterLen, filterShift, numFilters );
  nChar += fprintf( fid, "mplib info -- The number of frames for this block is [%lu], the search tree has [%lu] levels.\n",
		    numFrames, numLevels );
  nChar += fprintf( fid, "mplib info -- The number of channels is [%i] in the signal and [%i] in the waveforms.\n",
		    s->numChans, anywaveTable->numChans );

  return ( nChar );
}

/****************************************/
/* Partial update of the inner products */
MP_Support_t MP_Anywave_Hilbert_Block_c::update_ip( const MP_Support_t *touch ) {

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;

  unsigned long int signalLen;
  unsigned long int numTouchedFrames;

  unsigned short int chanIdx;
  unsigned long int tmp;
  

  MP_Support_t frameSupport;

  const char* func = "MP_Anywave_Hilbert_Block_c::update_ip"; 
  if ( s == NULL ) {
    mp_error_msg( func, "The signal s shall have been allocated before calling this function. Now, it is NULL. Exiting from this function.\n");
    /* Return a null mono-channel support */
    frameSupport.pos = 0;
    frameSupport.len = 0;
    return( frameSupport );
  }

  /*---------------------------*/
  /* Computes the interval [fromFrame,toFrame] where
     the frames need an IP+maxCorr update
     
     WARNING: toFrame is INCLUDED. See the LOOP below.

     THIS IS CRITICAL CODE. MODIFY WITH CARE.

     Here, the inner products are recomputed for the same length of
     signal on all the channels
  */
  
  /* -- If touch is NULL, we ask for a full update: */
  if ( touch == NULL ) {
    fromFrame = 0;
    toFrame   = numFrames - 1;
    fromSample = 0;
    toSample = s->numSamples - 1;
    signalLen = s->numSamples;
    numTouchedFrames = numFrames;
  }
  /* -- If touch is not NULL, we specify a touched support: */
  else {
      /* Initialize fromFrame and toFrame using the support on channel 0 */
    if (blockOffset>touch[0].pos) {
      tmp = 0;
    } else {
      tmp = touch[0].pos-blockOffset;
    }
    
    fromFrame = len2numFrames( tmp, filterLen, filterShift );

    toFrame = ( touch[0].pos + touch[0].len - 1 ) / filterShift;

    if ( toFrame >= numFrames )  toFrame = numFrames - 1;

    /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
    for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ ) {
      if (blockOffset>touch[chanIdx].pos) {
        tmp = 0;
      } else {
        tmp = touch[chanIdx].pos-blockOffset;
      }
      
      tmpFromFrame = len2numFrames( tmp, filterLen, filterShift );
      
      if ( tmpFromFrame < fromFrame ) fromFrame = tmpFromFrame;
      
      tmpToFrame  = ( touch[chanIdx].pos + touch[chanIdx].len - 1 ) / filterShift ;
      if ( tmpToFrame >= numFrames )  tmpToFrame = numFrames - 1;
      if ( tmpToFrame > toFrame ) toFrame = tmpToFrame;
    }
    
    fromSample = fromFrame * filterShift + blockOffset;    
    toSample = toFrame * filterShift + filterLen - 1 + blockOffset;
   
  }
  signalLen = toSample - fromSample + 1;
  numTouchedFrames = toFrame - fromFrame + 1;

  /*---------------------------*/

  convolution->compute_max_hilbert_IP(s, signalLen, fromSample, maxIPValueInFrame + fromFrame, maxIPIdxInFrame + fromFrame);

  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = numTouchedFrames;

  return( frameSupport );
}



/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Anywave_Hilbert_Block_c::create_atom( MP_Atom_c **atom,
						      const unsigned long int frameIdx,
						      const unsigned long int filterIdx ) {

  const char* func = "MP_Anywave_Hilbert_Block_c::create_atom";

  MP_Anywave_Hilbert_Atom_c *aatom = NULL;

  /* Misc: */
  unsigned short int chanIdx;
  unsigned long int pos = frameIdx*filterShift + blockOffset;
/*
  MP_Sample_t* pSample;
*/
  MP_Sample_t* pSampleStart;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }
  
  /* Allocate the atom */
  *atom = NULL;
  if ( (aatom = MP_Anywave_Hilbert_Atom_c::init( s->numChans )) == NULL ) {
    mp_error_msg( func, "Can't create a new Anywave Hilbert atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* Set the parameters */
  aatom->anywaveIdx = filterIdx;
  aatom->tableIdx = tableIdx;
  aatom->anywaveTable = anywaveTable;
  aatom->realTableIdx = realTableIdx;
  aatom->anywaveRealTable = anywaveRealTable;
  aatom->hilbertTableIdx = hilbertTableIdx;
  aatom->anywaveHilbertTable = anywaveHilbertTable;
  aatom->numSamples = pos + filterLen;

  if ((double)MP_MAX_UNSIGNED_LONG_INT / (double)s->numChans / (double)filterLen <= 1.0) {
    mp_error_msg( func,
		  "numChans [%lu] . filterLen [%lu] is greater than the max"
		  " for an unsigned long int [%lu]. The field totalChanLen of the atom"
		  " will overflow. Returning a NULL atom.\n",
		  s->numChans, filterLen, MP_MAX_UNSIGNED_LONG_INT);
    delete( aatom );
    *atom = NULL;
    return( 0 );
  }

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {

    /* 2) set the support of the atom */
    aatom->support[chanIdx].pos = pos;
    aatom->support[chanIdx].len = filterLen;
    aatom->totalChanLen += filterLen;

  }

  /* Recompute the inner product of the atom */


  for (chanIdx = 0;
       chanIdx < s->numChans;
       chanIdx ++) {
    pSampleStart = s->channel[chanIdx]+aatom->support[chanIdx].pos;

    if (anywaveTable->numChans == s->numChans) {
      aatom->realPart[chanIdx] = convolution->compute_real_IP( pSampleStart, filterIdx, chanIdx );
      aatom->hilbertPart[chanIdx] = convolution->compute_hilbert_IP( pSampleStart, filterIdx, chanIdx );
    } else {
      aatom->realPart[chanIdx] = convolution->compute_real_IP( pSampleStart, filterIdx, 0 );
      aatom->hilbertPart[chanIdx] = convolution->compute_hilbert_IP( pSampleStart, filterIdx, 0 );
    }

    aatom->amp[chanIdx] = (MP_Real_t) sqrt( (double) (aatom->realPart[chanIdx]*aatom->realPart[chanIdx] + aatom->hilbertPart[chanIdx]*aatom->hilbertPart[chanIdx] ) );

    if (aatom->amp[chanIdx] != 0) {
      aatom->realPart[chanIdx] /= (MP_Real_t) aatom->amp[chanIdx];
      aatom->hilbertPart[chanIdx] /= (MP_Real_t) aatom->amp[chanIdx];
    }
  }

#ifndef NDEBUG
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
    mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "Channel [%d]: filterIdx [%lu] amp [%g] (detail: real [%g] hilbert [%g])\n",
		  chanIdx, aatom->anywaveIdx, aatom->amp[chanIdx], aatom->realPart[chanIdx], aatom->hilbertPart[chanIdx]);
  }
#endif

  *atom = aatom;

  return( 1 );

}

/********************************************/
/* Frame-based update of the inner products */
void MP_Anywave_Hilbert_Block_c::update_frame(unsigned long int frameIdx, 
				      MP_Real_t *maxCorr, 
				      unsigned long int *maxFilterIdx)
{
  /* nothing is done. Indeed this method will never be called because
     update_ip is inherited in this class */
  mp_error_msg( "MP_Anywave_Hilbert_Block_c::update_frame",
		"this method shall not be used, it is present only for compatibility"
		" with inheritance-related classes. Use instead MP_Anywave_Block_c::update_ip()."
		" Parameters were frameIdx=%lu, maxCorr=%p, maxFilterIdx=%p\n",
		frameIdx, maxCorr, maxFilterIdx );
  
}
/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition of one anywave block to a dictionnary */
int add_anywave_hilbert_block( MP_Dict_c *dict,
		       const unsigned long int filterShift,
		       char* anywaveTableFileName ) {

  MP_Anywave_Hilbert_Block_c *newBlock;

  newBlock = MP_Anywave_Hilbert_Block_c::init( dict->signal, filterShift, anywaveTableFileName,0 );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    mp_error_msg( "MP_Anywave_Hilbert_Block_c::add_anywave_block","Can't add a new anywave block to a dictionnary." );
    return( 0 );
  }

  return( 1 );

}


/*****************************************************/
/* Addition of several anywave blocks to a dictionnary */
int add_anywave_hilbert_blocks( MP_Dict_c *dict,
			int numFilterShift,
			unsigned long int* filterShiftPtr,
			int numTables, 
			char** anywaveTableFileNamePtr ) {

  int nAddedBlocks = 0;
  int i,j;
  for ( i = 0; 
	i < numTables; 
	i++ ) {
    for ( j = 0;
	  j < numFilterShift;
	  j++ ) {
      nAddedBlocks += add_anywave_hilbert_block( dict, filterShiftPtr[j], anywaveTableFileNamePtr[i] );
    }
  }
  return(nAddedBlocks);

}
