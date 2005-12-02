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


/********************************************/
/* Frame-based update of the inner products */
void MP_Dirac_Block_c::update_frame(unsigned long int frameIdx, 
				    MP_Real_t *maxCorr, 
				    unsigned long int *maxFilterIdx)
{
  double sum = 0.0;
  double amp;

  int chanIdx;
  int numChans;

  assert( s != NULL );
  numChans = s->numChans;
  assert( maxCorr != NULL );
  assert( maxFilterIdx != NULL );

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ ) {
    assert( s->channel[chanIdx] != NULL );
    amp  = s->channel[chanIdx][frameIdx];
    sum += amp*amp;
  }
  *maxCorr = sum; *maxFilterIdx = 0;
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


