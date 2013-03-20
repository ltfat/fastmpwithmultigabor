/******************************************************************************/
/*                                                                            */
/*                             anywave_block.cpp                              */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
 * $Date: 2007-04-24 19:30:55 +0200 (mar., 24 avr. 2007) $
 * $Revision: 1021 $
 *
 */

/***************************************************/
/*                                                 */
/* anywave_block.cpp: methods for anywave blocks */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "anywave_atom_plugin.h"
#include "anywave_block_plugin.h"
#include <sstream>

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */

MP_Block_c* MP_Anywave_Block_Plugin_c::create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap )
{
	const char					*func = "MP_Gabor_Block_Plugin_c::create( MP_Signal_c *setSignal, map<const char*, const char*, mp_ltstring> *paramMap )";
	MP_Anywave_Block_Plugin_c	*newBlock = NULL;
	char						*anywaveTableFilename = NULL;
  
	// Analyse the parameter map
	if (strcmp((*paramMap)["type"].c_str(),"anywave"))
    {
		mp_error_msg( func, "Parameter map does not define a Anywave block.\n" );
		return( NULL );
    }

	// Instantiate and check
	newBlock = new MP_Anywave_Block_Plugin_c();
	if ( newBlock == NULL )
    {
		mp_error_msg( func, "Failed to create a new Anywave block.\n" );
		return( NULL );
    }

	if ((*paramMap)["tableFileName"].size() > 1)
    {
		anywaveTableFilename = (char*) malloc ((strlen((*paramMap)["tableFileName"].c_str())+1 ) * sizeof(char));
		strcpy (anywaveTableFilename,(*paramMap)["tableFileName"].c_str());

		// Set the block parameters (that are independent from the signal)
		if (newBlock->init_parameters(paramMap, anywaveTableFilename))
		{
			mp_error_msg( func, "Failed to initialize some block parameters in the new Anywave block.\n" );
			delete( newBlock );
			return( NULL );
		}
	}
	else
    {
		// Set the block parameters (that are independent from the signal)
		if ( newBlock->init_parameters( paramMap, NULL ) )
		{
			mp_error_msg( func, "Failed to initialize some block parameters in the new Anywave block.\n" );
			delete( newBlock );
			return( NULL );
		}
	}
	// Set the block parameter map (that are independent from the signal) */
	if ( newBlock->init_parameter_map( paramMap ) )
	{
		mp_error_msg( func, "Failed to initialize parameters map in the new Anywave block.\n" );
		delete( newBlock );
		return( NULL );
	} 
	// Set the signal-related parameters */
	if ( newBlock->plug_signal( setSignal ) )
	{
		mp_error_msg( func, "Failed to plug a signal in the new Anywave block.\n" );
		delete( newBlock );
		return( NULL );
	}

	if(anywaveTableFilename) 
		free (anywaveTableFilename);
  
	return( (MP_Block_c*)newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Anywave_Block_Plugin_c::init_parameters( map<string, string, mp_ltstring> *paramMap, char* anywaveTableFilename)
{
	const char			*func = "MP_Anywave_Block_c::init_parameters(...)";
	char				*convertEnd;
	unsigned long int	filterShift =0 ;
	unsigned long int	blockOffset = 0;

	if ((*paramMap)["windowShift"].size()>0)
    {
		// Convert windowShift
		filterShift=strtol((*paramMap)["windowShift"].c_str(), &convertEnd, 10);
		if (*convertEnd != '\0')
        {
			mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
			return 1;
        }
    }
	else
    {
		mp_error_msg( func, "No parameter windowShift in the parameter map.\n" );
		return 1;
    }

    if ((*paramMap)["blockOffset"].size()>0)
    {
		// Convert windowShift
		blockOffset=strtol((*paramMap)["blockOffset"].c_str(), &convertEnd, 10);
		if (*convertEnd != '\0')
        {
			mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
			return 1;
        }
    }

	// Load the table using "anywaveTableFilename" (old xml file) or the "paramMap" (new xml file)
	if(anywaveTableFilename != NULL)
		tableIdx = MPTK_Server_c::get_anywave_server()->add(anywaveTableFilename);
	else
		tableIdx = MPTK_Server_c::get_anywave_server()->add(paramMap);

	if ( tableIdx >= MPTK_Server_c::get_anywave_server()->maxNumTables )
    {
		// if the addition of a anywave table in the anywave server failed */
		mp_error_msg( func,"The anywave table can't be added to the anywave server. The anywave table remain NULL" );
		tableIdx = 0;
		return 1;
    }
	else
    {
		anywaveTable = MPTK_Server_c::get_anywave_server()->tables[tableIdx];
    }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( anywaveTable->filterLen, filterShift, anywaveTable->numFilters, blockOffset ) ) 
    {
      mp_error_msg( func, "Failed to init the block-level parameters in the new Anywave block.\n" );
      return 1;
    }

  /* Create the convolution object */
  if (convolution != NULL)
    {
      delete(convolution);
    }
  if ( ( convolution = new MP_Convolution_Fastest_c( anywaveTable, filterShift ) ) == NULL )
    {
      return 1;
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Anywave_Block_Plugin_c::init_parameter_map( map<string, string, mp_ltstring> *paramMap)
{
	parameterMap = new map< string, string, mp_ltstring>();
   
	(*parameterMap)["type"] = (*paramMap)["type"];
	(*parameterMap)["windowShift"] = (*paramMap)["windowShift"];
	(*parameterMap)["blockOffset"] = (*paramMap)["blockOffset"];
	if((*paramMap)["tableFileName"].size() > 0)
		(*parameterMap)["tableFileName"] = (*paramMap)["tableFileName"];
	if((*paramMap)["data"].size() > 0)
		(*parameterMap)["data"] = (*paramMap)["data"];
	if((*paramMap)["numChans"].size() > 0)
		(*parameterMap)["numChans"] = (*paramMap)["numChans"];
	if((*paramMap)["numFilters"].size() > 0)
		(*parameterMap)["numFilters"] = (*paramMap)["numFilters"];
	if((*paramMap)["filterLen"].size() > 0)
		(*parameterMap)["filterLen"] = (*paramMap)["filterLen"];
	return (0);
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Anywave_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Anywave_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL )
    {

      if ( anywaveTable == NULL )
        {
          /* check that anywaveTable has been set */
          mp_error_msg( func, "no anywave table was loaded. Can't plug a signal. The signal is set to NULL." );
          return( 1 );
        }
      else if ( (anywaveTable->numChans > 1) && (anywaveTable->numChans != setSignal->numChans) )
        {
          /* verify whether the signal and the waveforms have the same
          number of channels or whether the waveforms are monochannel. In
          the other cases, that's not compatible. */
          mp_error_msg( func, "the waveforms and the signal don't have the same number of channels. The signal is set to NULL." );
          return( 1 );
        }
      else if ( MP_Block_c::plug_signal( setSignal ) )
        {
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
void MP_Anywave_Block_Plugin_c::nullify_signal( void )
{
  MP_Block_c::nullify_signal();
}

/********************/
/* NULL constructor */
MP_Anywave_Block_Plugin_c::MP_Anywave_Block_Plugin_c( void )
    :MP_Block_c()
{

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
MP_Anywave_Block_Plugin_c::~MP_Anywave_Block_Plugin_c()
{

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::~MP_Anywave_Block_c()",
                "Deleting an Anywave block...\n" );

  anywaveTable = NULL;
  if ( convolution )
    {
      delete( convolution );
    }
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Anywave_Block_c::~MP_Anywave_Block_c()",
                "Done.\n" );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
const char* MP_Anywave_Block_Plugin_c::type_name()
{
  return ("anywave");
}


/********/
/* Readable text dump */
int MP_Anywave_Block_Plugin_c::info( FILE* fid )
{

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
MP_Support_t MP_Anywave_Block_Plugin_c::update_ip( const MP_Support_t *touch ) {

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;
  unsigned long int tmp;

  unsigned long int signalLen;
  unsigned long int numTouchedFrames;

  unsigned short int chanIdx;

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

  convolution->compute_max_IP(s, signalLen, fromSample, maxIPValueInFrame + fromFrame, maxIPIdxInFrame + fromFrame);

  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = numTouchedFrames;


  return( frameSupport );
}

MP_Support_t MP_Anywave_Block_Plugin_c::update_ip( const MP_Support_t *touch, GP_Pos_Book_c* book ) {
	
	unsigned long int fromFrame; /* first frameIdx to be touched, included */
	unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
	unsigned long int tmpFromFrame, tmpToFrame;
	unsigned long int fromSample;
	unsigned long int toSample;
	unsigned long int tmp;
	
	unsigned long int signalLen;
	unsigned long int numTouchedFrames;
	
	unsigned short int chanIdx;
	
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
	
	convolution->compute_max_IP(s, signalLen, fromSample, maxIPValueInFrame + fromFrame, maxIPIdxInFrame + fromFrame, book);
	cerr << "Max IP = " << *(maxIPValueInFrame + fromFrame) << endl;
	
	/* Return a mono-channel support */
	frameSupport.pos = fromFrame;
	frameSupport.len = numTouchedFrames;
	
	
	return( frameSupport );
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Anywave_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int filterIdx,
    MP_Dict_c* dict )
{

  const char* func = "MP_Anywave_Block_c::create_atom";

  MP_Anywave_Atom_Plugin_c *aatom = NULL;

  /* Misc: */
  unsigned short int chanIdx;
  unsigned long int pos = frameIdx*filterShift + blockOffset;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples )
    {
      mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
                    " Returning a NULL atom.\n" );
      *atom = NULL;
      return( 0 );
    }

  /* Allocate the atom */
  *atom = NULL;
  MP_Atom_c* (*emptyAtomCreator)( MP_Dict_c* dict ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("anywave");
  if (NULL == emptyAtomCreator)
    {
      mp_error_msg( func, "Anywave atom is not registred in the atom factory" );
      return( 0 );
    }

  if ( (aatom =  (MP_Anywave_Atom_Plugin_c *)(*emptyAtomCreator)(dict))  == NULL )
    {
      mp_error_msg( func, "Can't create a new Anywave atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
  if ( aatom->alloc_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
      return( 0 );

    }
  /* Set the parameters */
  aatom->anywaveIdx = filterIdx;
  aatom->tableIdx = tableIdx;
  aatom->anywaveTable = anywaveTable;
  aatom->numSamples = pos + filterLen;

  /* For each channel: */
  if ((double)MP_MAX_UNSIGNED_LONG_INT / (double)s->numChans / (double)filterLen <= 1.0)
    {
      mp_error_msg( func,
                    "numChans [%lu] . filterLen [%lu] is greater than the max"
                    " for an unsigned long int [%lu]. The field totalChanLen of the atom"
                    " will overflow. Returning a NULL atom.\n",
                    s->numChans, filterLen, MP_MAX_UNSIGNED_LONG_INT);
      delete( aatom );
      *atom = NULL;
      return( 0 );
    }

  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {

      /* 2) set the support of the atom */
      aatom->support[chanIdx].pos = pos;
      aatom->support[chanIdx].len = filterLen;
      aatom->totalChanLen += filterLen;

    }

  /* Recompute the inner product of the atom */

  if (anywaveTable->numChans == s->numChans)
    {
      /* multichannel filters */
      for (chanIdx = 0;
           chanIdx < s->numChans;
           chanIdx ++)
        {

          aatom->amp[0] += (MP_Real_t) convolution->compute_IP( s->channel[chanIdx]+aatom->support[chanIdx].pos, filterIdx, chanIdx );
        }
      for (chanIdx = 1;
           chanIdx < s->numChans;
           chanIdx ++)
        {
          aatom->amp[chanIdx] += aatom->amp[0];
        }

    }
  else
    {
      /* monochannel filters */
      for (chanIdx = 0;
           chanIdx < s->numChans;
           chanIdx ++)
        {

          aatom->amp[chanIdx] = (MP_Real_t) convolution->compute_IP( s->channel[chanIdx]+aatom->support[chanIdx].pos, filterIdx, 0 );

        }
    }

#ifndef NDEBUG
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "Channel [%d]: filterIdx %lu amp %g\n",
                    chanIdx, aatom->anywaveIdx, aatom->amp[chanIdx] );
    }
#endif

  *atom = aatom;

  return( 1 );

}

/********************************************/
/* Frame-based update of the inner products */
void MP_Anywave_Block_Plugin_c::update_frame(unsigned long int frameIdx,
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

/*********************************************/
/* get Paramater type map defining the block */
void MP_Anywave_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){

const char * func = "void MP_Anywave_Block_Plugin_c::get_parameters_type_map()";

if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
(*parameterMapType)["tableFileName"] = "string";
(*parameterMapType)["windowShift"] = "ulong";
(*parameterMapType)["blockOffset"] = "ulong";

} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Anywave_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){

const char * func = "void MP_Anywave_Block_Plugin_c::get_parameters_info_map()";

if ((*parameterMapInfo).empty()) {
(*parameterMapInfo)["type"] = "A block corresponding to 'anywave' atoms which are specified by a unit norm waveform selected in a wavetable, an amplitude by which the waveform is multiplied, and a time-shift parameter.";
(*parameterMapInfo)["tableFileName"] =  "Filename of a wavetable where the waveforms of the desired anywave atoms are stored.";
(*parameterMapInfo)["windowShift"] = "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
(*parameterMapInfo)["blockOffset"] = "Offset between beginning of signal and beginning of first atom, in number of samples.";

} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Anywave_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Anywave_Block_Plugin_c::get_parameters_default_map()";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "anywave";
const char *defaultAnyWaveTable = MPTK_Env_c::get_env()->get_config_path("defaultAnyWaveTable");
if(NULL!=defaultAnyWaveTable)
(*parameterMapDefault)["tableFileName"] = string(defaultAnyWaveTable);
else
(*parameterMapDefault)["tableFileName"] = "N/A";
(*parameterMapDefault)["windowShift"] = "512";
(*parameterMapDefault)["blockOffset"] = "0"; }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}
