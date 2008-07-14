/******************************************************************************/
/*                                                                            */
/*                        anywave_hilbert_block.cpp                           */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
#include "anywave_hilbert_atom_plugin.h"
#include "anywave_hilbert_block_plugin.h"
#include <sstream>


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
/** \brief Factory function for a Anywave block
   *
   * a static initialization function that construct a new instance of
   * MP_Anywave_Block_c from a file containing the anywave table
   * \param setSignal the signal on which the block will work
   * \param paramMap the map containing the parameter to construct the block:
   * setFilterShift the filter shift between two successive
   * atoms
   * anywaveTableFileName the name of the file containing the anywave table
   *
   * \return A pointer to the new MP_Anywave_Block_c instance
   **/
   

MP_Block_c* MP_Anywave_Hilbert_Block_Plugin_c::create(MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap){

 const char* func = "MP_Anywave_Hilbert_Block_Plugin_c::create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap )";
  MP_Anywave_Hilbert_Block_Plugin_c *newBlock = NULL;
  char*  convertEnd;
  char* anywaveTableFilename;
  unsigned long int filterShift =0 ;
  unsigned long int blockOffset = 0;


  /* Instantiate and check */
  newBlock = new MP_Anywave_Hilbert_Block_Plugin_c();
  if ( newBlock == NULL )
    {
      mp_error_msg( func, "Failed to create a new Anywave block.\n" );
      return( NULL );
    }

  /* Analyse the parameter map */
  if (strcmp((*paramMap)["type"].c_str(),"anywavehilbert"))
    {
      mp_error_msg( func, "Parameter map does not define a Anywave block.\n" );
      return( NULL );
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

  if ((*paramMap)["tableFileName"].size()>0&& strlen((*paramMap)["tableFileName"].c_str()) > 1)
    {
      anywaveTableFilename = (char*) malloc ((strlen((*paramMap)["tableFileName"].c_str())+1 ) * sizeof(char));
      strcpy (anywaveTableFilename,(*paramMap)["tableFileName"].c_str());
    }
  else
    {
      mp_error_msg( func, "No parameter tableFileName in the parameter map.\n" );
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
  if ( newBlock->init_parameters( filterShift, anywaveTableFilename, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in the new Anywave Hilbert block.\n" );
      delete( newBlock );
      return( NULL );
    }

  /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map( filterShift, anywaveTableFilename , blockOffset ) )
                                   {
      mp_error_msg( func, "Failed to initialize parameters map in the new Anywave Hilbert  block.\n" );
      delete( newBlock );
      return( NULL );
    }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) )
    {
      mp_error_msg( func, "Failed to plug a signal in the new Gabor block.\n" );
      delete( newBlock );
      return( NULL );
    }
if(anywaveTableFilename) free (anywaveTableFilename);
  return( (MP_Block_c*)newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Anywave_Hilbert_Block_Plugin_c::init_parameters( const unsigned long int setFilterShift,
    char* anywaveTableFilename, 
    const unsigned long int setBlockOffset)
{

  const char* func = "MP_Anywave_Hilbert_Block_c::init_parameters(...)";


  /* Load the table */
  tableIdx = MPTK_Server_c::get_anywave_server()->add(anywaveTableFilename);
  if ( tableIdx >= MPTK_Server_c::get_anywave_server()->maxNumTables )
    {
      /* if the addition of a anywave table in the anywave server failed */
      mp_error_msg( func,"The anywave table can't be added to the anywave server. The anywave table remain NULL" );
      tableIdx = 0;
      return(1);
    }
  else
    {
      anywaveTable = MPTK_Server_c::get_anywave_server()->tables[tableIdx];
    }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( anywaveTable->filterLen, setFilterShift, anywaveTable->numFilters, blockOffset ) )  
    {
      mp_error_msg( func, "Failed to init the block-level parameters in the new Anywave block.\n" );
      return( 1 );
    }

  init_tables();

  /* Create the convolution and hilbert convolution objects */
  if (convolution != NULL)
    {
      delete(convolution);
    }
  if ( ( convolution = new MP_Convolution_Fastest_c( anywaveTable, anywaveRealTable, anywaveHilbertTable, setFilterShift ) ) == NULL )
    {
      return(1);
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Anywave_Hilbert_Block_Plugin_c::init_parameter_map( const unsigned long int setFilterShift,
    char* anywaveTableFilename,
    const unsigned long int setBlockOffset )
{
const char* func = "MP_Anywave_Hilbert_Block_c::init_parameter_map(...)";

parameterMap = new map< string, string, mp_ltstring>();
   
/*Create a stream for convert number into string */
std::ostringstream oss;

(*parameterMap)["type"] = type_name();

if (!(oss << setFilterShift)) { mp_error_msg( func, "Cannot convert windowShift in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["windowShift"] = oss.str();
oss.str("");
(*parameterMap)["tableFileName"] = anywaveTableFilename;

if (!(oss << setBlockOffset)) { mp_error_msg( func, "Cannot convert blockOffset in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["blockOffset"] = oss.str();
oss.str("");

return (0);
}

void MP_Anywave_Hilbert_Block_Plugin_c::init_tables( void )
{

  char* str;

  if ( ( str = (char*) malloc( MP_MAX_STR_LEN * sizeof(char) ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::init_tables()","The string str cannot be allocated.\n" );
    }

  /* create the real table if needed */
  strcpy(str, MPTK_Server_c::get_anywave_server()->get_filename( tableIdx ));
  str = strcat(str,"_real");
  realTableIdx = MPTK_Server_c::get_anywave_server()->get_index( str );
  if (realTableIdx == MPTK_Server_c::get_anywave_server()->numTables)
    {
      anywaveRealTable = anywaveTable->copy();
      anywaveRealTable->center_and_denyquist();
      anywaveRealTable->normalize();
      anywaveRealTable->set_table_file_name(str);
      realTableIdx = MPTK_Server_c::get_anywave_server()->add( anywaveRealTable );
    }
  else
    {
      anywaveRealTable = MPTK_Server_c::get_anywave_server()->tables[realTableIdx];
    }

  /* create the hilbert table if needed */
  strcpy(str, MPTK_Server_c::get_anywave_server()->get_filename( tableIdx ));
  str = strcat(str,"_hilbert");
  hilbertTableIdx = MPTK_Server_c::get_anywave_server()->get_index( str );
  if (hilbertTableIdx == MPTK_Server_c::get_anywave_server()->numTables)
    {
      /* need to create a new table */
      anywaveHilbertTable = anywaveTable->create_hilbert_dual(str);
      anywaveHilbertTable->normalize();
      hilbertTableIdx = MPTK_Server_c::get_anywave_server()->add( anywaveHilbertTable );
    }
  else
    {
      anywaveHilbertTable = MPTK_Server_c::get_anywave_server()->tables[hilbertTableIdx];
    }

  if (str != NULL) {
    free(str);
  }
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Anywave_Hilbert_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Anywave_Hilbert_Block_c::plug_signal( signal )";

  if ( MP_Anywave_Block_Plugin_c::plug_signal( setSignal ) )
    {
      /* Go up the inheritance graph */
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }

  return( 0 );

}

/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Anywave_Hilbert_Block_Plugin_c::nullify_signal( void )
{

  MP_Anywave_Block_Plugin_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Anywave_Hilbert_Block_Plugin_c::MP_Anywave_Hilbert_Block_Plugin_c( void )
    :MP_Anywave_Block_Plugin_c()
{

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
MP_Anywave_Hilbert_Block_Plugin_c::~MP_Anywave_Hilbert_Block_Plugin_c()
{

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
char* MP_Anywave_Hilbert_Block_Plugin_c::type_name()
{
  return("anywavehilbert");
}


/********/
/* Readable text dump */
int MP_Anywave_Hilbert_Block_Plugin_c::info( FILE* fid )
{

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
MP_Support_t MP_Anywave_Hilbert_Block_Plugin_c::update_ip( const MP_Support_t *touch )
{

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

  const char* func = "MP_Anywave_Hilbert_Block_c::update_ip";
  if ( s == NULL )
    {
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
  if ( touch == NULL )
    {
      fromFrame = 0;
      toFrame   = numFrames - 1;
      fromSample = 0;
      toSample = s->numSamples - 1;
      signalLen = s->numSamples;
      numTouchedFrames = numFrames;
    }
  /* -- If touch is not NULL, we specify a touched support: */
  else
    {
      /* Initialize fromFrame and toFrame using the support on channel 0 */
    if (blockOffset>touch[0].pos) {
      tmp = 0;
     } else {
      tmp = touch[0].pos-blockOffset;
     }
      fromFrame = len2numFrames( touch[0].pos, filterLen, filterShift );

      toFrame = ( touch[0].pos + touch[0].len - 1 ) / filterShift;

      if ( toFrame >= numFrames )  toFrame = numFrames - 1;

      /* Adjust fromFrame and toFrame with respect to supports on the subsequent channels */
      for ( chanIdx = 1; chanIdx < s->numChans; chanIdx++ )
        {  if (blockOffset>touch[chanIdx].pos) {
        tmp = 0;
      } else {
        tmp = touch[chanIdx].pos-blockOffset;
      }
          tmpFromFrame = len2numFrames( touch[chanIdx].pos, filterLen, filterShift );
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
unsigned int MP_Anywave_Hilbert_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int filterIdx )
{

  const char* func = "MP_Anywave_Hilbert_Block_c::create_atom";

  MP_Anywave_Hilbert_Atom_Plugin_c *aatom = NULL;

  /* Misc: */
  unsigned short int chanIdx;
  unsigned long int pos = frameIdx*filterShift + blockOffset;
  /*
    MP_Real_t* pSample;
  */
  MP_Real_t* pSampleStart;

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
  MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("AnywaveHilbertAtom");
  if (NULL == emptyAtomCreator)
    {
      mp_error_msg( func, "Anywave Hilbert atom is not registred in the atom factory" );
      return( 0 );
    }

  if ( (aatom =  (MP_Anywave_Hilbert_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL )
    {
      mp_error_msg( func, "Can't create a Anywave Hilbert atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
  if ( aatom->alloc_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new atom. Returning a NULL atom.\n" );
      return( 0 );

    }
      if ( aatom->alloc_hilbert_atom_param( s->numChans) )
    {
      mp_error_msg( func, "Failed to allocate some vectors in the new Hilbert Atom. Returning a NULL atom.\n" );
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

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {

      /* 2) set the support of the atom */
      aatom->support[chanIdx].pos = pos;
      aatom->support[chanIdx].len = filterLen;
      aatom->totalChanLen += filterLen;

    }

  /* Recompute the inner product of the atom */


  for (chanIdx = 0;
       chanIdx < s->numChans;
       chanIdx ++)
    {
      pSampleStart = s->channel[chanIdx]+aatom->support[chanIdx].pos;

      if (anywaveTable->numChans == s->numChans)
        {
          aatom->realPart[chanIdx] = convolution->compute_real_IP( pSampleStart, filterIdx, chanIdx );
          aatom->hilbertPart[chanIdx] = convolution->compute_hilbert_IP( pSampleStart, filterIdx, chanIdx );
        }
      else
        {
          aatom->realPart[chanIdx] = convolution->compute_real_IP( pSampleStart, filterIdx, 0 );
          aatom->hilbertPart[chanIdx] = convolution->compute_hilbert_IP( pSampleStart, filterIdx, 0 );
        }

      aatom->amp[chanIdx] = (MP_Real_t) sqrt( (double) (aatom->realPart[chanIdx]*aatom->realPart[chanIdx] + aatom->hilbertPart[chanIdx]*aatom->hilbertPart[chanIdx] ) );

      if (aatom->amp[chanIdx] != 0)
        {
          aatom->realPart[chanIdx] /= (MP_Real_t) aatom->amp[chanIdx];
          aatom->hilbertPart[chanIdx] /= (MP_Real_t) aatom->amp[chanIdx];
        }
    }

#ifndef NDEBUG
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "Channel [%d]: filterIdx [%lu] amp [%g] (detail: real [%g] hilbert [%g])\n",
                    chanIdx, aatom->anywaveIdx, aatom->amp[chanIdx], aatom->realPart[chanIdx], aatom->hilbertPart[chanIdx]);
    }
#endif

  *atom = aatom;

  return( 1 );

}

/********************************************/
/* Frame-based update of the inner products */
void MP_Anywave_Hilbert_Block_Plugin_c::update_frame(unsigned long int frameIdx,
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

/*********************************************/
/* get Paramater type map defining the block */
void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){

const char * func = "void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_type_map()";

if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
(*parameterMapType)["tableFileName"] = "string";
(*parameterMapType)["windowShift"] = "ulong";
(*parameterMapType)["blockOffset"] = "ulong";

} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){

const char * func = "void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_info_map()";

if ((*parameterMapInfo).empty()) {
(*parameterMapInfo)["type"] = "the type of blocks";
(*parameterMapInfo)["tableFileName"] =  "Filename of a wavetable where the waveforms of the desired anywave atoms are stored.";
(*parameterMapInfo)["windowShift"] = "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
(*parameterMapInfo)["blockOffset"] = "Offset between beginning of signal and beginning of first atom, in number of samples.";

} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_default_map()";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "anywavehilbert";
(*parameterMapDefault)["tableFileName"] = "none";
(*parameterMapDefault)["windowShift"] = "512";
(*parameterMapDefault)["blockOffset"] = "0"; }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}

/***********/
/*FUNCTIONS*/

/******************************************************/
/* Registration of new block (s) in the block factory */

DLL_EXPORT void registry(void)
{
  MP_Block_Factory_c::get_block_factory()->register_new_block("anywave",&MP_Anywave_Block_Plugin_c::create, &MP_Anywave_Block_Plugin_c::get_parameters_type_map, &MP_Anywave_Block_Plugin_c::get_parameters_info_map, &MP_Anywave_Block_Plugin_c::get_parameters_default_map );
  MP_Block_Factory_c::get_block_factory()->register_new_block("anywavehilbert",&MP_Anywave_Hilbert_Block_Plugin_c::create, &MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_type_map, &MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_info_map, &MP_Anywave_Hilbert_Block_Plugin_c::get_parameters_default_map );
}
