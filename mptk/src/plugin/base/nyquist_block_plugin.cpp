/******************************************************************************/
/*                                                                            */
/*                             nyquist_block.cpp                             */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
 * $Date: 2007-04-24 19:30:55 +0200 (mar., 24 avr. 2007) $
 * $Revision: 1021 $
 *
 */

/***************************************************/
/*                                                 */
/* nyquist_block.cpp: methods for nyquist blocks */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "nyquist_atom_plugin.h"
#include "nyquist_block_plugin.h"
#include <sstream>

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */

MP_Block_c* MP_Nyquist_Block_Plugin_c::create( MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap){
 
 const char* func = "MP_Nyquist_Block_c::init()";
  MP_Nyquist_Block_Plugin_c *newBlock = NULL;
  unsigned long int filterLen = 0;
  unsigned long int filterShift =0 ;
  double windowRate =0.0;
  unsigned long int blockOffset = 0;
  char*  convertEnd;
  
  /* Instantiate and check */
  newBlock = new MP_Nyquist_Block_Plugin_c();
  if ( newBlock == NULL )
    {
      mp_error_msg( func, "Failed to create a new Nyquist block.\n" );
      return( NULL );
    }
 /* Analyse the parameter map */
  if (strcmp((*paramMap)["type"].c_str(),"nyquist"))
    {
      mp_error_msg( func, "Parameter map does not define a Nyquist block.\n" );
      return( NULL );
    }
  if ((*paramMap)["windowLen"].size()>0)
    {
      /*Convert window length*/
      filterLen=strtol((*paramMap)["windowLen"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowLen in unsigned long int.\n" );
          return( NULL );
        }
    }
  else
    {
      if ((*paramMap)["windowRate"].size()>0)
        {
          windowRate =strtod((*paramMap)["windowRate"].c_str(), &convertEnd);
          if (*convertEnd != '\0')
            {
              mp_error_msg( func, "cannot convert parameter windowRate in unsigned long int.\n");
              return( NULL );
            }
          filterShift = (unsigned long int)( (double)(filterLen)*windowRate + 0.5 );
          filterShift = ( filterShift > 1 ? filterShift : 1 ); /* windowShift has to be 1 or more */

        }
      else
        {
          mp_error_msg( func, "No parameter windowShift or windowRate in the parameter map.\n" );
          return( NULL );
        }
    }

  if ((*paramMap)["windowShift"].size()>0)
    {
      /*Convert windowShift*/
      filterShift=strtol((*paramMap)["windowShift"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
          return( NULL );
        }
    }
  else
    {
      mp_error_msg( func, "No parameter windowShift in the parameter map.\n" );
      return( NULL );
    }
    
      if ((*paramMap)["blockOffset"].size()>0)
    {
      /*Convert windowShift*/
      blockOffset=strtol((*paramMap)["blockOffset"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter windowShift in unsigned long int.\n");
          return( NULL );
        }
    }
    
  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( filterLen , filterShift, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in the new Nyquist block.\n" );
      delete( newBlock );
      return( NULL );
    }
    
       /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map( filterLen , filterShift, blockOffset ) )
                                   {
      mp_error_msg( func, "Failed to initialize parameters map in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );}

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( s ) )
    {
      mp_error_msg( func, "Failed to plug a signal in the new Nyquist block.\n" );
      delete( newBlock );
      return( NULL );
    }

  return( (MP_Block_c*) newBlock );
 }
/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Nyquist_Block_Plugin_c::init_parameters( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setblockOffset )
{

  const char* func = "MP_Nyquist_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( setFilterLen, setFilterShift, 1, setblockOffset ) )
    {
      mp_error_msg( func, "Failed to init the block-level parameters in the new Nyquist block.\n" );
      return( 1 );
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Nyquist_Block_Plugin_c::init_parameter_map( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setblockOffset )
{
const char* func = "MP_Gabor_Block_c::init_parameter_map(...)";

parameterMap = new map< string, string, mp_ltstring>();
   
/*Create a stream for convert number into string */
std::ostringstream oss;

(*parameterMap)["type"] = type_name();

/* put value in the stream */
if (!(oss << setFilterLen)) { 
	  mp_error_msg( func, "Cannot convert windowLen in string for parameterMap.\n" );
      return( 1 );
      }
/* put stream in string */
(*parameterMap)["windowLen"] = oss.str();
/* clear stream */
oss.str("");
if (!(oss << setFilterShift)) { mp_error_msg( func, "Cannot convert windowShift in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["windowShift"] = oss.str();
oss.str("");

return (0);
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Nyquist_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Nyquist_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL )
    {

      /* Go up the inheritance graph */
      if ( MP_Block_c::plug_signal( setSignal ) )
        {
          mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
          nullify_signal();
          return( 1 );
        }

    }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Nyquist_Block_Plugin_c::nullify_signal( void )
{

  MP_Block_c::nullify_signal();

}


/********************/
/* NULL constructor */
MP_Nyquist_Block_Plugin_c::MP_Nyquist_Block_Plugin_c( void )
    :MP_Block_c()
{
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Nyquist_Block_c::MP_Nyquist_Block_c()",
                "Constructing a Nyquist_block...\n" );
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Nyquist_Block_c::MP_Nyquist_Block_c()",
                "Done.\n" );
}


/**************/
/* Destructor */
MP_Nyquist_Block_Plugin_c::~MP_Nyquist_Block_Plugin_c()
{
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
const char* MP_Nyquist_Block_Plugin_c::type_name()
{
  return ("nyquist");
}


/********/
/* Readable text dump */
int MP_Nyquist_Block_Plugin_c::info( FILE* fid )
{

  int nChar = 0;

  nChar += (int)mp_info_msg( fid, "NYQUIST BLOCK", "window of length [%lu], shifted by [%lu] samples,\n", filterLen, filterShift );
  nChar += (int)mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu], the search tree has [%lu] levels.\n", numFrames, numLevels );

  return ( nChar );
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Nyquist_Block_Plugin_c::update_frame(unsigned long int frameIdx,
                                      MP_Real_t *maxCorr,
                                      unsigned long int *maxFilterIdx)
{
  double sum = 0.0;
  double ip;
  MP_Real_t* pAmp;
  unsigned long int t;
  unsigned long int inShift;

  int chanIdx;
  int numChans;

  assert( s != NULL );
  numChans = s->numChans;
  assert( maxCorr != NULL );
  assert( maxFilterIdx != NULL );

  inShift = frameIdx*filterShift + blockOffset;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {

      assert( s->channel[chanIdx] + inShift + filterLen <= s->channel[0] + s->numSamples );

      ip = 0.0;
      for (t = 0, pAmp = s->channel[chanIdx] + inShift;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip += *pAmp;
        }
      for (t = 1, pAmp = s->channel[chanIdx] + inShift + 1;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip -= *pAmp;
        }
      sum += ip*ip;
    }
  *maxCorr = sum/filterLen;
  *maxFilterIdx = 0;
}

void MP_Nyquist_Block_Plugin_c::update_frame(unsigned long int frameIdx,
                                      MP_Real_t *maxCorr,
                                      unsigned long int *maxFilterIdx,
                                      GP_Param_Book_c* touchBook)
{
  double sum = 0.0;
  double ip;
  MP_Real_t* pAmp;
  unsigned long int t;
  unsigned long int inShift;

  int chanIdx;
  int numChans;
  GP_Param_Book_Iterator_c iter;

  if (!touchBook){
	  update_frame(frameIdx, maxCorr, maxFilterIdx);
	  return;
  }

  assert( s != NULL );
  numChans = s->numChans;
  assert( maxCorr != NULL );
  assert( maxFilterIdx != NULL );

  iter = touchBook->begin();

  inShift = frameIdx*filterShift + blockOffset;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {

      assert( s->channel[chanIdx] + inShift + filterLen <= s->channel[0] + s->numSamples );

      ip = 0.0;
      for (t = 0, pAmp = s->channel[chanIdx] + inShift;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip += *pAmp;
        }
      for (t = 1, pAmp = s->channel[chanIdx] + inShift + 1;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip -= *pAmp;
        }
      if (iter != touchBook->end())
    	  iter->corr[chanIdx] = ip/sqrt((double)filterLen);
      sum += ip*ip;
    }
  *maxCorr = sum/filterLen;
  *maxFilterIdx = 0;
}

/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Nyquist_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int /* filterIdx */, MP_Dict_c* dict )
{

  const char* func = "MP_Nyquist_Block_c::create_atom(...)";
  MP_Nyquist_Atom_Plugin_c *datom;
  int chanIdx;
  unsigned long int pos = frameIdx*filterShift + blockOffset;
  unsigned long int t;
  MP_Real_t* pAmp;
  MP_Real_t ip;

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
  MP_Atom_c* (*emptyAtomCreator)( MP_Dict_c* dict ) = MP_Atom_Factory_c::get_empty_atom_creator("nyquist");
    if (NULL == emptyAtomCreator)
    {
      mp_error_msg( func, "Nyquist atom is not registred in the atom factory" );
      return( 0 );
    }

  if ( (datom =  (MP_Nyquist_Atom_Plugin_c*)(*emptyAtomCreator)(dict))  == NULL )
    {
      mp_error_msg( "MP_Nyquist_Block_c::create_atom(...)", "Can't create a new Nyquist atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
  if ( datom ->alloc_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
      return( 0 );

    }
  /* Set the numSamples */
  datom->numSamples = pos + filterLen;
  datom->totalChanLen = 0;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {

      datom->support[chanIdx].pos = pos;
      datom->support[chanIdx].len = filterLen;
      datom->totalChanLen        += filterLen;
      ip = 0.0;
      for (t = 0, pAmp = s->channel[chanIdx] + pos;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip += (*pAmp);
        }
      for (t = 1, pAmp = s->channel[chanIdx] + pos + 1;
           t < filterLen;
           t+=2, pAmp+=2)
        {
          ip -= (*pAmp);
        }
      datom->amp[chanIdx]         = ip / (MP_Real_t)sqrt((double)filterLen);
    }

  *atom = datom;
  return( 1 );
}

/*********************************************/
/* get Paramater type map defining the block */
void MP_Nyquist_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){

const char * func = "void MP_Nyquist_Block_Plugin_c::get_parameters_type_map()";

if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
(*parameterMapType)["windowLen"] = "ulong";
(*parameterMapType)["windowShift"] = "ulong";
(*parameterMapType)["blockOffset"] = "ulong";
(*parameterMapType)["windowRate"]  = "real";

} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Nyquist_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){

const char * func = "void MP_Nyquist_Block_Plugin_c::get_parameters_info_map()";

if ((*parameterMapInfo).empty()) {
(*parameterMapInfo)["type"] = "the type of blocks";
(*parameterMapInfo)["windowLen"] = "The common length of the atoms (which is the length of the signal window), in number of samples.";
(*parameterMapInfo)["windowShift"] = "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
(*parameterMapInfo)["blockOffset"] = "Offset between beginning of signal and beginning of first atom, in number of samples.";
(*parameterMapInfo)["windowRate"] = "The shift between atoms on adjacent time frames, in proportion of the <windowLen>. For example, windowRate = 0.5 corresponds to half-overlapping signal windows.";

} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Nyquist_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Nyquist_Block_Plugin_c::get_parameters_default_map()";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "nyquist";
(*parameterMapDefault)["windowLen"] = "1024";
(*parameterMapDefault)["windowShift"] = "512";
(*parameterMapDefault)["blockOffset"] = "0";
(*parameterMapDefault)["windowRate"] = "0.5";
 }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}
/*************/
/* FUNCTIONS */
/*************/

/******************************************************/
/* Registration of new block (s) in the block factory */

DLL_EXPORT void registry(void)
{
  MP_Block_Factory_c::register_new_block( "nyquist", &MP_Nyquist_Block_Plugin_c::create, &MP_Nyquist_Block_Plugin_c::get_parameters_type_map, &MP_Nyquist_Block_Plugin_c::get_parameters_info_map, &MP_Nyquist_Block_Plugin_c::get_parameters_default_map );
}
