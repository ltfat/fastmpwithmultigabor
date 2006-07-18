/******************************************************************************/
/*                                                                            */
/*                             nyquist_block.cpp                             */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Mon Apr 03 2006 */
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

/***************************************************/
/*                                                 */
/* nyquist_block.cpp: methods for nyquist blocks */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Nyquist_Block_c* MP_Nyquist_Block_c::init( MP_Signal_c *setSignal,
						const unsigned long int setFilterLen,
						const unsigned long int setFilterShift ) {
  
  const char* func = "MP_Nyquist_Block_c::init()";
  MP_Nyquist_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Nyquist_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Nyquist block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterLen, setFilterShift ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Nyquist block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Nyquist block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
}


/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Nyquist_Block_c::init_parameters( const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift ) {

  const char* func = "MP_Nyquist_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( setFilterLen, setFilterShift, 1 ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Nyquist block.\n" );
    return( 1 );
  }

  return( 0 );
}


/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Nyquist_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Nyquist_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    /* Go up the inheritance graph */
    if ( MP_Block_c::plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }

  }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Nyquist_Block_c::nullify_signal( void ) {

  MP_Block_c::nullify_signal();

}


/********************/
/* NULL constructor */
MP_Nyquist_Block_c::MP_Nyquist_Block_c( void )
:MP_Block_c() {
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Nyquist_Block_c::MP_Nyquist_Block_c()",
		"Constructing a Nyquist_block...\n" );
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Nyquist_Block_c::MP_Nyquist_Block_c()",
		"Done.\n" );
}


/**************/
/* Destructor */
MP_Nyquist_Block_c::~MP_Nyquist_Block_c() {
  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Nyquist_Block_c::~MP_Nyquist_Block_c()",
		"Deleting Nyquist_block...\n" );
  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Nyquist_Block_c::~MP_Nyquist_Block_c()",
		"Done.\n" );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_Nyquist_Block_c::type_name() {
  return ("nyquist");
}


/********/
/* Readable text dump */
int MP_Nyquist_Block_c::info( FILE* fid ) {

  int nChar = 0;

  nChar += mp_info_msg( fid, "NYQUIST BLOCK", "window of length [%lu], shifted by [%lu] samples,\n",
			filterLen, filterShift );
  nChar += mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu], "
			"the search tree has [%lu] levels.\n", numFrames, numLevels );

  return ( nChar );
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Nyquist_Block_c::update_frame(unsigned long int frameIdx, 
				    MP_Real_t *maxCorr, 
				    unsigned long int *maxFilterIdx)
{
  double sum = 0.0;
  double ip;
  MP_Sample_t* pAmp;
  unsigned long int t;
  unsigned long int inShift;

  int chanIdx;
  int numChans;

  assert( s != NULL );
  numChans = s->numChans;
  assert( maxCorr != NULL );
  assert( maxFilterIdx != NULL );

  inShift = frameIdx*filterShift;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ ) {
    
    assert( s->channel[chanIdx] + inShift + filterLen <= s->channel[0] + s->numSamples );

    ip = 0.0;
    for (t = 0, pAmp = s->channel[chanIdx] + inShift;
	 t < filterLen;
	 t+=2, pAmp+=2) {
      ip += *pAmp;
    }
    for (t = 1, pAmp = s->channel[chanIdx] + inShift + 1;
	 t < filterLen;
	 t+=2, pAmp+=2) {
      ip -= *pAmp;
    }
    sum += ip;
  }
  *maxCorr = sum/filterLen; *maxFilterIdx = 0;
}

/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Nyquist_Block_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int frameIdx,
					    const unsigned long int /* filterIdx */ ) {

  const char* func = "MP_Nyquist_Block_c::create_atom(...)";
  MP_Nyquist_Atom_c *datom;
  int chanIdx;
  unsigned long int pos = frameIdx*filterShift;
  unsigned long int t;
  MP_Sample_t* pAmp;
  MP_Sample_t ip;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }

  /* Allocate the atom */
  *atom = NULL;
  if ( (datom = MP_Nyquist_Atom_c::init( s->numChans )) == NULL ) {
    mp_error_msg( "MP_Nyquist_Block_c::create_atom(...)", "Can't create a new Nyquist atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* Set the numSamples */
  datom->numSamples = pos + filterLen;    
  datom->totalChanLen = 0;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
 
    datom->support[chanIdx].pos = pos;
    datom->support[chanIdx].len = filterLen;
    datom->totalChanLen        += filterLen;
    ip = 0.0;
    for (t = 0, pAmp = s->channel[chanIdx] + pos;
	 t < filterLen;
	 t+=2, pAmp+=2) {
      ip += (*pAmp);
    }
    for (t = 1, pAmp = s->channel[chanIdx] + pos + 1;
	 t < filterLen;
	 t+=2, pAmp+=2) {
      ip -= (*pAmp);
    }
    datom->amp[chanIdx]         = ip / (MP_Sample_t)sqrt((double)filterLen);
  }

  *atom = datom;
  return( 1 );
}


/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition the nyquist block to a dictionnary */
int add_nyquist_block( MP_Dict_c *dict,
		       const unsigned long int setFilterLen,
		       const unsigned long int setFilterShift ) {

  MP_Nyquist_Block_c *newBlock;

  newBlock = MP_Nyquist_Block_c::init( dict->signal, setFilterLen, setFilterShift );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    mp_error_msg( "add_nyquist_block( dict )",
		  "Can't instantiate a new nyquist block to add to a dictionary." );
    return( 0 );
  }

  return( 1 );
}


