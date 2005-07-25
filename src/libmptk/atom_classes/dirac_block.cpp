/******************************************************************************/
/*                                                                            */
/*                             dirac_block.cpp                             */
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

/*********************************************/
/*                                           */
/* dirac_block.cpp: methods for dirac blocks */
/*                                           */
/*********************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Specific constructor */
MP_Dirac_Block_c::MP_Dirac_Block_c( MP_Signal_c *setSignal )
  :MP_Block_c( setSignal, 1, 1, 1 ) {
}


/**************/
/* Destructor */
MP_Dirac_Block_c::~MP_Dirac_Block_c() {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Dirac_Block_c() - Deleting Dirac_block.\n" );
#endif
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_Dirac_Block_c::type_name() {
  return ("dirac");
}


/********/
/* Readable text dump */
int MP_Dirac_Block_c::info( FILE* fid ) {

  int nChar = 0;
  nChar += fprintf( fid, "mplib info -- dirac block.\n" );
  return ( nChar );
}


/****************************************/
/* Partial update of the inner products */
MP_Support_t MP_Dirac_Block_c::update_ip( const MP_Support_t *touch ) {
  /* Since the dirac atoms are exactly the signal samples, 
   * there is not much to do here apart from computing the multichannel 
   * support of touched samples */
  /* NOTE: if touch == NULL  update all the inner products. */

  unsigned long int tmpFromFrame, fromFrame; /* first frameIdx to be touched, inclusive */
  unsigned long int tmpToFrame, toFrame;   /* last frameIdx to be touched, NOT included */
  unsigned long int fromSample; /* first sample to be touched, inclusive */
  unsigned long int toSample;   /* last sample to be touched, NOT included */

  int chanIdx;
  unsigned long int frameIdx;

  MP_Support_t frameSupport;
  MP_Real_t amp,energy;

  assert( s != NULL );

  /* 1/ Computes the interval [fromFrame,toFrame] where
     the frames need an update.
     
     WARNING: toFrame is INCLUDED. See the LOOP below.

  */
  
  /* -- If touch is NULL, we ask for a full update: */
  if ( touch == NULL ) {
    fromFrame = 0;
    toFrame   = numFrames - 1;
  }
  /* -- If touch is not NULL, we specify a touched support: */
  else {
    /* Initialize fromFrame and toFrame using the support on channel 0 */
    fromSample = touch[0].pos;
    fromFrame = len2numFrames( fromSample, filterLen, filterShift );
    
    toSample = ( fromSample + touch[0].len - 1 );
    toFrame  = toSample / filterShift ;
    if ( toFrame >= numFrames )  toFrame = numFrames - 1;
    /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
    for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ ) {
      fromSample = touch[chanIdx].pos;
      tmpFromFrame = len2numFrames( fromSample, filterLen, filterShift );
      if ( tmpFromFrame < fromFrame ) fromFrame = tmpFromFrame;
      
      toSample = ( fromSample + touch[chanIdx].len - 1 );
      tmpToFrame  = toSample / filterShift ;
      if ( tmpToFrame >= numFrames )  tmpToFrame = numFrames - 1;
      if ( tmpToFrame > toFrame ) toFrame = tmpToFrame;
    }
  }
  /*---------------------------*/

 #ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- update_ip() - Updating frames from %lu to %lu / %lu.\n",
	   fromFrame, toFrame, numFrames );
#endif

  /*---------------------------*/
  /* LOOP : Browse the frames which need an update. */
  for ( frameIdx = fromFrame; frameIdx <= toFrame; frameIdx++ ) {

    energy = 0.0;

    for ( chanIdx = 0; chanIdx < s->numChans; chanIdx++ ) {
      assert( s->channel[chanIdx] != NULL );
      amp = s->channel[chanIdx][frameIdx];
      energy += amp*amp;
    } /* end foreach channel */

    *(maxIPValueInFrame + frameIdx) = energy;
    *(maxIPIdxInFrame   + frameIdx) = 0;
  } /* end foreach frame */

  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = toFrame - fromFrame + 1;

  return( frameSupport );
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Dirac_Block_c::create_atom( MP_Atom_c **atom,
					       const unsigned long int atomIdx ) {
  MP_Dirac_Atom_c *datom;
  int chanIdx;

  /* Allocate the atom */
  *atom = NULL;
  if ( (datom = new MP_Dirac_Atom_c( s->numChans )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Dirac_Block_c::create_atom() - Can't create a new Dirac atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
 
    datom->support[chanIdx].pos = atomIdx;
    datom->support[chanIdx].len = 1;
    datom->totalChanLen        += 1;
    datom->amp[chanIdx]         = s->channel[chanIdx][atomIdx];
  }

  *atom = datom;
  return( 1 );
}


/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition the dirac block to a dictionnary */
int add_dirac_block( MP_Dict_c *dict ) {

  MP_Dirac_Block_c *newBlock;

  newBlock = new MP_Dirac_Block_c( dict->signal );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    fprintf( stderr, "mplib error -- add_dirac_block() - Can't add a new dirac block to a dictionnary." );
    return( 0 );
  }

  return( 1 );
}


