/******************************************************************************/
/*                                                                            */
/*                             dirac_block.cpp                             */
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
 * $Date: 2007-04-24 19:30:55 +0200 (mar., 24 avr. 2007) $
 * $Revision: 1021 $
 *
 */

/*********************************************/
/*                                           */
/* dirac_block.cpp: methods for dirac blocks */
/*                                           */
/*********************************************/

#include "mptk.h"
#include "mp_system.h"
#include "dirac_atom_plugin.h"
#include "dirac_block_plugin.h"
#include "block_factory.h"
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Block_c* MP_Dirac_Block_Plugin_c::create( MP_Signal_c* setSignal , map<string, string, mp_ltstring> *paramMap) {

  const char* func = "MP_Dirac_Block_c::create()";
  MP_Dirac_Block_Plugin_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Dirac_Block_Plugin_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Dirac block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters() ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Dirac block.\n" );
    delete( newBlock );
    return( NULL );
  }
     /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map() )
                                   {
      mp_error_msg( func, "Failed to initialize parameters map in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );
    } 

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Dirac block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( (MP_Block_c*) newBlock );
}
/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Dirac_Block_Plugin_c::init_parameters( void ) {

  const char* func = "MP_Dirac_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( 1, 1, 1, 0 ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Dirac block.\n" );
    return( 1 );
  }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Dirac_Block_Plugin_c::init_parameter_map( void )
{
	const char* func = "MP_Gabor_Block_c::init_parameter_map(...)";

	parameterMap = new map< string, string, mp_ltstring>();
	if(NULL==parameterMap)  {
		mp_error_msg(func,"could not create map");
	} else {
		(*parameterMap)["type"] = type_name();
	}
	return (0);

}
/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Dirac_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Dirac_Block_c::plug_signal( signal )";

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
void MP_Dirac_Block_Plugin_c::nullify_signal( void ) {

  MP_Block_c::nullify_signal();

}


/********************/
/* NULL constructor */
MP_Dirac_Block_Plugin_c::MP_Dirac_Block_Plugin_c( void )
:MP_Block_c() {
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Dirac_Block_c::MP_Dirac_Block_c()",
		"Constructing a Dirac_block...\n" );
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Dirac_Block_c::MP_Dirac_Block_c()",
		"Done.\n" );
}


/**************/
/* Destructor */
MP_Dirac_Block_Plugin_c::~MP_Dirac_Block_Plugin_c() {
  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Dirac_Block_c::~MP_Dirac_Block_c()",
		"Deleting Dirac_block...\n" );
  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Dirac_Block_c::~MP_Dirac_Block_c()",
		"Done.\n" );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_Dirac_Block_Plugin_c::type_name() {
  return ("dirac");
}


/********/
/* Readable text dump */
int MP_Dirac_Block_Plugin_c::info( FILE* fid ) {

  int nChar = 0;
  nChar += mp_info_msg( fid, "DIRAC BLOCK", "The number of frames for this block is [%lu],"
			" the search tree has [%lu] levels.\n",
			numFrames, numLevels );
  return ( nChar );
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Dirac_Block_Plugin_c::update_frame(unsigned long int frameIdx, 
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
unsigned int MP_Dirac_Block_Plugin_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int frameIdx,
					    const unsigned long int /* filterIdx */ ) {

  const char* func = "MP_Dirac_Block_c::create_atom(...)";
  MP_Dirac_Atom_Plugin_c *datom;
  int chanIdx;

  /* Check the position */
  if ( (frameIdx+1) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }

  /* Allocate the atom */
  *atom = NULL;
    MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("dirac");
  if (NULL == emptyAtomCreator)
    {
      mp_error_msg( func, "Dirac atom is not registred in the atom factory" );
      return( 0 );
    }

  if ( (datom =  (MP_Dirac_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL )
  {
    mp_error_msg( "MP_Dirac_Block_c::create_atom(...)", "Can't create a new Dirac atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }
  if ( datom->alloc_atom_param( s->numChans ) ) {
    mp_error_msg( func, "Failed to allocate some vectors in the new Dirac atom.\n" );
    return( 0);
  }
  /* Set the numSamples */
  datom->numSamples = frameIdx + 1;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
 
    datom->support[chanIdx].pos = frameIdx;
    datom->support[chanIdx].len = 1;
    datom->totalChanLen        += 1;
    datom->amp[chanIdx]         = s->channel[chanIdx][frameIdx];
  }

  *atom = datom;
  return( 1 );
}


/*********************************************/
/* get Paramater type map defining the block */
void MP_Dirac_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){

const char * func = "void MP_Dirac_Block_Plugin_c::get_parameters_type_map()";

if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Dirac_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){

const char * func = "void MP_Dirac_Block_Plugin_c::get_parameters_info_map()";

if ((*parameterMapInfo).empty()) {
(*parameterMapInfo)["type"] = "The 'dirac' block generates 'dirac' atoms, with a single nonzero sample. It is useless to include several 'dirac' blocks in a dictionary.";
} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Dirac_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Dirac_Block_Plugin_c::get_parameters_default_map()";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "dirac"; }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}

/******************************************************/
/* Registration of new block (s) in the block factory */

DLL_EXPORT void registry(void)
{
  MP_Block_Factory_c::get_block_factory()->register_new_block("dirac",&MP_Dirac_Block_Plugin_c::create , &MP_Dirac_Block_Plugin_c::get_parameters_type_map, &MP_Dirac_Block_Plugin_c::get_parameters_info_map, &MP_Dirac_Block_Plugin_c::get_parameters_default_map );
}

