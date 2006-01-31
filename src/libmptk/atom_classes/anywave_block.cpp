/******************************************************************************/
/*                                                                            */
/*                             anywave_block.cpp                              */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Fri Nov 04 2005 */
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
MP_Anywave_Block_c* MP_Anywave_Block_c::init( MP_Signal_c *setSignal,
					      const unsigned long int setFilterShift,
					      char* anywaveTableFilename ) {

  const char* func = "MP_Anywave_Block_c::init()";
  MP_Anywave_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Anywave_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Anywave block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterShift, anywaveTableFilename ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Anywave block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Anywave block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
  
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Anywave_Block_c::init_parameters( const unsigned long int setFilterShift,
					 char* anywaveTableFilename ) {

  extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;  

  const char* func = "MP_Anywave_Block_c::init_parameters(...)";

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
  if ( MP_Block_c::init_parameters( anywaveTable->filterLen, setFilterShift, anywaveTable->numFilters ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Anywave block.\n" );
    return( 1 );
  }

  /* Create the convolution object */ 
  if (convolution != NULL) { delete(convolution); }
  if ( ( convolution = new MP_Convolution_Fastest_c( anywaveTable, setFilterShift ) ) == NULL ) {
    return(1);
  }

  return( 0 );
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Anywave_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Anywave_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    if ( anywaveTable == NULL ) {
      /* check that anywaveTable has been set */
      mp_error_msg( func, "no anywave table was loaded. Can't plug a signal. The signal is set to NULL." );
      return( 1 );
    } else if ( (anywaveTable->numChans > 1) && (anywaveTable->numChans != setSignal->numChans) )  {
      /* verify whether the signal and the waveforms have the same
	 number of channels or whether the waveforms are monochannel. In
	 the other cases, that's not compatible. */
      mp_error_msg( func, "the waveforms and the signal don't have the same number of channels. The signal is set to NULL." );
      return( 1 );
    } else if ( MP_Block_c::plug_signal( setSignal ) ) {
      /* Go up the inheritance graph */
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }    
  }

  return( 0 );

}

/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Anywave_Block_c::nullify_signal( void ) {

  MP_Block_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Anywave_Block_c::MP_Anywave_Block_c( void )
  :MP_Block_c() {

    mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::MP_Anywave_Block_c()",
		  "Constructing an Anywave block...\n" );
    
    anywaveTable = NULL;
    tableIdx = 0;
    convolution = NULL;
   
    mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::MP_Anywave_Block_c()",
		  "Done.\n" );
  }

/**************/
/* Destructor */
MP_Anywave_Block_c::~MP_Anywave_Block_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::~MP_Anywave_Block_c()",
		"Deleting an Anywave block...\n" );

  anywaveTable = NULL;
  if ( convolution ) { delete( convolution ); }

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::~MP_Anywave_Block_c()",
		"Done.\n" );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/* Test */
bool MP_Anywave_Block_c::test( char* signalFileName, unsigned long int filterShift, char* tableFileName ) {

  unsigned long int frameIdx = 0;
  unsigned long int maxFilterIdx = 0;

  MP_Atom_c* atom;

  
  fprintf( stdout, "\n-- Entering MP_Anywave_Block_c::test \n" );
  fflush( stdout );
  
  fprintf( stdout, "\n---- Creating MP_Anywave_Block_c \n" );
  fflush( stdout );
  /* create an anywave block */ 
  
  MP_Signal_c* signal = MP_Signal_c::init( signalFileName );
  if (signal == NULL) {
    return(false);
  }
  MP_Anywave_Block_c* block = MP_Anywave_Block_c::init( signal, filterShift, tableFileName );
  if (block == NULL) {
    return(false);
  }

  fprintf( stdout, "\n---- type_name() : Type of the block : %s\n",block->type_name() );
  fflush( stdout );
  
  fprintf( stdout, "\n---- info() : Info\n" );
  fflush( stdout );
  block->info(stdout);
  fflush(stdout);

  block->update_ip( NULL );
  fprintf( stdout, "\n---- update_frame() : update all the inner products " );
  fflush( stdout );
  block->create_atom( &atom, frameIdx, maxFilterIdx );
  fprintf( stdout, "\n---- create_atom() : atom corresponds to the %luth frame, using the %luth waveform\n", frameIdx, maxFilterIdx );
  atom->info( stdout );
  fflush( stdout );
  
  fprintf( stdout, "\n---- Deleting MP_Anywave_Block_c \n" );
  fflush( stdout );
  
  delete(block);
  delete(signal);
  fprintf( stdout, "\n-- Exiting MP_Anywave_Block_c::test \n" );
  fflush( stdout );
  
  return(true);
}




/********/
/* Type */
char* MP_Anywave_Block_c::type_name() {
  return ("anywave");
}


/********/
/* Readable text dump */
int MP_Anywave_Block_c::info( FILE* fid ) {

  int nChar = 0;

  nChar += fprintf( fid, "mplib info -- ANYWAVE BLOCK" );
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
MP_Support_t MP_Anywave_Block_c::update_ip( const MP_Support_t *touch ) {

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;

  unsigned long int signalLen;
  unsigned long int numTouchedFrames;

  double* ampPtr;
  double* currentAmpPtr;

  double amp;
  double corr;

  unsigned short int chanIdx;
  unsigned long int frameIdx;
  unsigned long int filterIdx;

  MP_Support_t frameSupport;

  const char* func = "MP_Anywave_Block_c::update_ip"; 
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

    fromFrame = len2numFrames( touch[0].pos, filterLen, filterShift );

    toFrame = ( touch[0].pos + touch[0].len - 1 ) / filterShift;

    if ( toFrame >= numFrames )  toFrame = numFrames - 1;

    /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
    for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ ) {
      tmpFromFrame = len2numFrames( touch[chanIdx].pos, filterLen, filterShift );
      if ( tmpFromFrame < fromFrame ) fromFrame = tmpFromFrame;
      
      tmpToFrame  = ( touch[chanIdx].pos + touch[chanIdx].len - 1 ) / filterShift ;
      if ( tmpToFrame >= numFrames )  tmpToFrame = numFrames - 1;
      if ( tmpToFrame > toFrame ) toFrame = tmpToFrame;
    }
    fromSample = fromFrame * filterShift;    
    toSample = toFrame * filterShift + filterLen - 1;
  }
  signalLen = toSample - fromSample + 1;
  numTouchedFrames = toFrame - fromFrame + 1;

  /*---------------------------*/

  for ( frameIdx = fromFrame; frameIdx <= toFrame; frameIdx++ ) {
    maxIPValueInFrame[frameIdx] = 0.0;
  }

  /* computing + finding the max */

  /* Needs initialization before pointing to its adress */
  if ((double)MP_MAX_SIZE_T / (double)s->numChans / (double)numTouchedFrames / (double)anywaveTable->numFilters / (double)sizeof(double) <= 1.0) {
    mp_error_msg( func, "numChans [%lu] . numTouchedFrames [%lu] . numFilters [%lu] .sizeof(double) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the amplitudes array. it is set to NULL\n", s->numChans, numTouchedFrames, anywaveTable->numFilters, sizeof(double), MP_MAX_SIZE_T);
    ampPtr = NULL;
  } else if ((ampPtr = (double * ) malloc(s->numChans * numTouchedFrames * anywaveTable->numFilters * sizeof(double))) == NULL ) {
    mp_error_msg( func, "Can't allocate an array of [%lu] double elements"
		  " for the ampPtr array. This pointer will remain NULL.\n", s->numChans * numTouchedFrames * anywaveTable->numFilters );
  }

  if (ampPtr == NULL) {
    /* Return a null mono-channel support */
    frameSupport.pos = fromFrame;
    frameSupport.len = 0;
    return( frameSupport );
  }

  if (anywaveTable->numChans == s->numChans) {

    mp_debug_msg( MP_DEBUG, func, "As many channels in signal as in waveforms");
    /* multichannel atoms */
    for (chanIdx = 0, currentAmpPtr = ampPtr;
	 chanIdx < s->numChans;
	 chanIdx ++, currentAmpPtr += numTouchedFrames * anywaveTable->numFilters) {

      mp_debug_msg( MP_DEBUG, func, "Computing inner products between channel [%hu] of the signal and channel [%hu] of the filters", chanIdx, chanIdx);
      
      convolution->compute_IP( s->channel[chanIdx]+fromSample, signalLen, chanIdx, &currentAmpPtr );
    }
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx++) {
      mp_debug_msg( MP_DEBUG, func, "Meeting IPs of the different channels to give global IPs between filter [%lu] and the frames of signal", chanIdx);
      
      for ( frameIdx = fromFrame; 
	    frameIdx <= toFrame; 
	    frameIdx++ ) {
	amp = 0.0;
	currentAmpPtr = ampPtr + filterIdx * numTouchedFrames + (frameIdx - fromFrame);	
	for ( chanIdx = 0;
	      chanIdx < s->numChans; 
	      chanIdx++, currentAmpPtr += numTouchedFrames * anywaveTable->numFilters ) {
	  amp += (*currentAmpPtr);
	}
	corr = amp * amp;
	if (corr >= maxIPValueInFrame[frameIdx]) { 
	  maxIPValueInFrame[frameIdx] = corr; maxIPIdxInFrame[frameIdx] = filterIdx;
	}
      }
    }      
  } else {
    /* monochannel atoms */
    for (chanIdx = 0, currentAmpPtr = ampPtr;
	 chanIdx < s->numChans;
	 chanIdx ++, currentAmpPtr += numTouchedFrames * anywaveTable->numFilters) {
	convolution->compute_IP( s->channel[chanIdx]+fromSample, signalLen, 0, &currentAmpPtr );
    }
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx++) {
      for ( frameIdx = fromFrame; frameIdx <= toFrame; frameIdx++ ) {
	corr = 0.0;
	currentAmpPtr = ampPtr + filterIdx * numTouchedFrames + (frameIdx - fromFrame);
	for ( chanIdx = 0;
	      chanIdx < s->numChans; 
	      chanIdx++, currentAmpPtr += numTouchedFrames * anywaveTable->numFilters ) {
	  corr += (*currentAmpPtr) * (*currentAmpPtr);
	}
	if (corr >= maxIPValueInFrame[frameIdx]) { maxIPValueInFrame[frameIdx] = corr; maxIPIdxInFrame[frameIdx] = filterIdx; }	     
      }
    }
  }
  /*---------------------------*/

  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = numTouchedFrames;

  /* clean the house */
  if (ampPtr) {free(ampPtr);}

  return( frameSupport );
}



/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Anywave_Block_c::create_atom( MP_Atom_c **atom,
					      const unsigned long int frameIdx,
					      const unsigned long int filterIdx ) {

  const char* func = "MP_Anywave_Block_c::create_atom";

  MP_Anywave_Atom_c *aatom = NULL;

  /* Misc: */
  unsigned short int chanIdx;
  unsigned long int pos = frameIdx*filterShift;
  
  /* Check the position */
  if ( (pos+filterLen) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }
  
  /* Allocate the atom */
  *atom = NULL;
  if ( (aatom = new MP_Anywave_Atom_c( s->numChans )) == NULL ) {
    mp_error_msg( func, "Can't create a new Anywave atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* Set the parameters */
  aatom->anywaveIdx = filterIdx;
  aatom->tableIdx = tableIdx;
  aatom->anywaveTable = anywaveTable;
  aatom->numSamples = pos + filterLen;

  /* For each channel: */
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

  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {

    /* 2) set the support of the atom */
    aatom->support[chanIdx].pos = pos;
    aatom->support[chanIdx].len = filterLen;
    aatom->totalChanLen += filterLen;

  }

  /* Recompute the inner product of the atom */

  if (anywaveTable->numChans == s->numChans) {
    /* multichannel filters */
    for (chanIdx = 0;
	 chanIdx < s->numChans;
	 chanIdx ++) {

      aatom->amp[0] += (MP_Real_t) convolution->compute_IP( s->channel[chanIdx]+aatom->support[chanIdx].pos, filterIdx, chanIdx );
    }
    for (chanIdx = 1;
	 chanIdx < s->numChans;
	 chanIdx ++) {
      aatom->amp[chanIdx] += aatom->amp[0];
    } 
   
  } else {
    /* monochannel filters */
    for (chanIdx = 0;
	 chanIdx < s->numChans;
	 chanIdx ++) {

      aatom->amp[chanIdx] += (MP_Real_t) convolution->compute_IP( s->channel[chanIdx]+aatom->support[chanIdx].pos, filterIdx, 0 );

    }
  }

#ifndef NDEBUG
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
    mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "Channel [%d]: filterIdx %lu amp %g\n",
		  chanIdx, aatom->anywaveIdx, aatom->amp[chanIdx] );
  }
#endif

  *atom = aatom;

  return( 1 );

}

/********************************************/
/* Frame-based update of the inner products */
void MP_Anywave_Block_c::update_frame(unsigned long int frameIdx, 
				      MP_Real_t *maxCorr, 
				      unsigned long int *maxFilterIdx)
{
  /* nothing is done. Indeed this method will never be called because
     update_ip is inherited in this class */
  mp_error_msg( "MP_Anywave_Block_c::update_frame",
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
int add_anywave_block( MP_Dict_c *dict,
		       const unsigned long int filterShift,
		       char* anywaveTableFileName ) {

  MP_Anywave_Block_c *newBlock;

  newBlock = MP_Anywave_Block_c::init( dict->signal, filterShift, anywaveTableFileName );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    mp_error_msg( "MP_Anywave_Block_c::add_anywave_block","Can't add a new anywave block to a dictionnary." );
    return( 0 );
  }

  return( 1 );

}


/*****************************************************/
/* Addition of several anywave blocks to a dictionnary */
int add_anywave_blocks( MP_Dict_c *dict,
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
      nAddedBlocks += add_anywave_block( dict, filterShiftPtr[j], anywaveTableFileNamePtr[i] );
    }
  }
  return(nAddedBlocks);

}
