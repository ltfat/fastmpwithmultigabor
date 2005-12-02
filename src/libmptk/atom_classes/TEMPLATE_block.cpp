/******************************************************************************/
/*                                                                            */
/*                             TEMPLATE_block.cpp                             */
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

/***************************************************/
/*                                                 */
/* TEMPLATE_block.cpp: methods for TEMPLATE blocks */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"

/* YOUR includes go here. */


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Specific constructor */
MP_TEMPLATE_Block_c::MP_TEMPLATE_Block_c( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift,
					  const unsigned long int setNumFilters
					  /* YOUR parameters */ )
:MP_Block_c( setSignal, setFilterLen, setFilterShift, setNumFilters ) {
  
  /* YOUR code */
}


/**************/
/* Destructor */
MP_TEMPLATE_Block_c::~MP_TEMPLATE_Block_c() {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_TEMPLATE_Block_c() - Deleting TEMPLATE_block.\n" );
#endif

  /* YOUR code */
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_TEMPLATE_Block_c::type_name() {
  return ("TEMPLATE");
}


/********/
/* Readable text dump */
int MP_TEMPLATE_Block_c::info( FILE* fid ) {

  int nChar = 0;

  nChar += fprintf( fid, "mplib info -- TEMPLATE BLOCK: your info goes here.\n" );
  return ( nChar );
}


/****************************************/
/* Partial update of the inner products */
MP_Support_t MP_TEMPLATE_Block_c::update_ip( const MP_Support_t *touch ) {
  /* NOTE: if touch == NULL  update all the inner products. */

  unsigned long int fromFrame; /* first frameIdx to be touched, inclusive */
  unsigned long int toFrame;   /* last frameIdx to be touched, NOT included */

  int chanIdx;
  unsigned long int frameIdx;

  MP_Support_t frameSupport;

  assert( s != NULL );

  /*************************************************/
  /* 1) Refresh the inner products of each channel */
  for ( chanIdx = 0; chanIdx < s->numChans; chanIdx++ ) {
    
    assert( s->channel[chanIdx] != NULL );

    /* Default values if touch is NULL, that is to say if we ask a full update */
    if ( touch == NULL ) {
      fromFrame = 0;
      toFrame   = numFrames;
    } 
    /* If we specify a touched support */
    else {
      /* If the channel has been modified: compute the update bounds in terms of frame indexes */
      support2frame(touch[chanIdx],filterLen,filterShift,&fromFrame,&toFrame);
      if ( toFrame > numFrames ) {
	toFrame = numFrames;
      }
    } /* end specify a touch support */
	
    /* Browse the out of date frames: */
    for ( frameIdx = fromFrame; frameIdx < toFrame; frameIdx++ ) {
	  
      /*********************************************/
      /* YOUR inner product computation goes here. */
      /*********************************************/

    } /* end foreach frame */

  } /* end foreach channel */
  
  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = toFrame - fromFrame + 1;

  return( frameSupport );
}

void MP_TEMPLATE_Block_c::update_frame(unsigned long int frameIdx, 
				       MP_Real_t *maxCorr, 
				       unsigned long int *maxFilterIdx)
{
  double max;
  unsigned long int i;
  
  max = 0;
  i = 0;
  
  /*********************************************/
  /* YOUR inner product computation goes here. */
  /*********************************************/
  frameIdx = 0;

  *maxCorr = max;
  *maxFilterIdx = i;
}

/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_TEMPLATE_Block_c::create_atom( MP_Atom_c **atom,
					       const unsigned long int atomIdx ) {
  MP_Atom_c **dummy;
  unsigned long int dummyUl;

  /* YOUR code */
  dummy = atom;
  dummyUl = atomIdx;

  return( 1 );
}


/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition of one TEMPLATE block to a dictionnary */
int add_TEMPLATE_block( MP_Dict_c *dict,
			const unsigned long int filterLen,
			const unsigned long int filterShift,
			const unsigned long int numFilters
			/* YOUR parameters */ ) {

  MP_TEMPLATE_Block_c *newBlock;

  newBlock = new MP_TEMPLATE_Block_c( dict->signal, filterLen, filterShift, numFilters /* + YOUR parameters*/ );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    fprintf( stderr, "mplib error -- add_TEMPLATE_block() - Can't add a new TEMPLATE block to a dictionnary." );
    return( 0 );
  }

  return( 1 );
}


/*****************************************************/
/* Addition of several TEMPLATE blocks to a dictionnary */
int add_TEMPLATE_blocks( MP_Dict_c *dict
			 /* YOUR parameters */ ) {

  int nAddedBlocks = 0;
  MP_Dict_c* dummy;

  /* YOUR code */
  dummy = dict;

  return(nAddedBlocks);
}
