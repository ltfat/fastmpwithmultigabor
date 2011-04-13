/******************************************************************************/
/*                                                                            */
/*                           harmonic_block.cpp                               */
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

/***************************************************************/
/*                                                             */
/* harmonic_block.cpp: methods for harmonic blocks */
/*                                                             */
/***************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "gabor_block_plugin.h"
#include "harmonic_block_plugin.h"
#include "gabor_atom_plugin.h"
#include "harmonic_atom_plugin.h"
#include "block_factory.h"
#include <sstream>

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Block_c* MP_Harmonic_Block_Plugin_c::create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap )
{

  const char* func = "MP_Harmonic_Block_Plugin_c::create( MP_Signal_c *setSignal, map<const char*, const char*, ltstr> *paramMap )";
  char*  convertEnd;
  unsigned long int filterLen = 0;
  unsigned long int filterShift =0 ;
  unsigned long int fftSize  = 0;
  unsigned char windowType;
  double windowOption =0.0;
  MP_Real_t f0Min =0.0;
  MP_Real_t f0Max =0.0;
  unsigned int  maxNumPartials =0;
  double windowRate =0.0;
  unsigned long int blockOffset = 0;

  MP_Harmonic_Block_Plugin_c* newBlock = new MP_Harmonic_Block_Plugin_c();
  if ( newBlock == NULL )
    {
      mp_error_msg( func, "Failed to create a new Harmonic block.\n" );
      return( NULL );
    }
  /* Analyse the parameter map */
  if (strcmp((*paramMap)["type"].c_str(),"harmonic"))
    {
      mp_error_msg( func, "Parameter map does not define a Gabor block.\n" );
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
      mp_error_msg( func, "No parameter windowLen in the parameter map.\n" );
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
  if ((*paramMap)["fftSize"].size()>0)
    {
      /*Convert fftSize*/
      fftSize=strtol((*paramMap)["fftSize"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter fftSize in unsigned long int.\n" );
          return( NULL );
        }

    }
  else
    {
     if ( is_odd(filterLen) ) fftSize = filterLen + 1;
	    else                     fftSize = filterLen;
    }

  if ((*paramMap)["windowtype"].size()>0) windowType = window_type((*paramMap)["windowtype"].c_str());
  else
    {
      mp_error_msg( func, "No parameter windowtype in the parameter map.\n" );
      return( NULL );
    } 
     if ( window_needs_option(windowType) && ((*paramMap)["windowopt"].size() == 0) ) {
      mp_error_msg( func, "Gabor or harmonic block"
		    " requires a window option (the opt=\"\" attribute is probably missing"
		    " in the relevant <window> tag). Returning a NULL block.\n" );
      return( NULL );
    } else {
      /*Convert windowopt*/
      windowOption=strtod((*paramMap)["windowopt"].c_str(), &convertEnd);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter window option in double.\n" );
          return( NULL );
        }
    }

  if ((*paramMap)["f0Min"].size()>0)
    {
      /*Convert window length*/
      f0Min=strtod((*paramMap)["f0Min"].c_str(), &convertEnd);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter f0Min in MP_Real_t.\n" );
          return( NULL );
        }
    }
  else
    {
     f0Min = 0.0;
    }

  if ((*paramMap)["f0Max"].size()>0)
    {
      /*Convert window length*/
      f0Max=strtod((*paramMap)["f0Max"].c_str(), &convertEnd);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter f0Max in MP_Real_t.\n" );
          return( NULL );
        }
    }
  else
    {
       f0Max = 1e6;
    }

  if ((*paramMap)["numPartials"].size()>0)
    {
      /*Convert window length*/
      maxNumPartials=strtol((*paramMap)["numPartials"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter numPartials in unsigned long int.\n" );
          return( NULL );
        }
    }
  else
    {
      mp_error_msg( func, "No parameter numPartials in the parameter map.\n" );
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
  if ( newBlock->init_parameters( filterLen, filterShift, fftSize,
                                  windowType, windowOption,
                                  f0Min, f0Max, maxNumPartials, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in"
                    " the new Harmonic block.\n" );
      delete( newBlock );
      return( NULL );
    }
  /* Set the block parameter map (that are independent from the signal) */
  if ( newBlock->init_parameter_map( filterLen, filterShift, fftSize,
                                  windowType, windowOption,
                                  f0Min, f0Max, maxNumPartials, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in"
                    " the new Harmonic block.\n" );
      delete( newBlock );
      return( NULL );
    }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) )
    {
      mp_error_msg( func, "Failed to plug a signal in the new Harmonic block.\n" );
      delete( newBlock );
      return( NULL );
    }

  return( (MP_Block_c*) newBlock );


}
/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Harmonic_Block_Plugin_c::init_parameters( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setFftSize,
    const unsigned char setWindowType,
    const double setWindowOption,
    const MP_Real_t setF0Min,
    const MP_Real_t setF0Max,
    const unsigned int  setMaxNumPartials,
    const unsigned long int setBlockOffset )
{

  const char* func = "MP_Harmonic_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Gabor_Block_Plugin_c::init_parameters( setFilterLen, setFilterShift, setFftSize,
       setWindowType, setWindowOption, setBlockOffset ) )
    {
      mp_error_msg( func, "Failed to init the parameters at the Gabor block level"
                    " in the new Harmonic block.\n" );
      return( 1 );
    }

  /* Check the harmonic fields */
  if ( setF0Min < 0.0 )
    {
      mp_error_msg( func, "f0Min [%.2f] is negative;"
                    " f0Min must be a positive frequency value"
                    " (in Hz).\n", setF0Min );
      return( 1 );
    }
  if ( setF0Max < setF0Min )
    {
      mp_error_msg( func, "f0Max [%.2f] is smaller than f0Min [%.2f]."
                    " f0Max must be a positive frequency value (in Hz)"
                    " bigger than f0Min.\n", setF0Max, setF0Min );
      return( 1 );
    }

  /* Set the harmonic fields */
  f0Min = setF0Min;
  f0Max = setF0Max;
  if ( setMaxNumPartials == 0 )
    {
      /* A maxNumPartials set to zero means: explore all the harmonics
         until the Nyquist frequency. */
      maxNumPartials = UINT_MAX;
    }
  else maxNumPartials = setMaxNumPartials;

  /* Allocate the sum array */
  if ( (sum = (double*) calloc( numFreqs , sizeof(double) )) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] double elements"
                    "for the sum array.\n", numFreqs );
      return( 1 );
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Harmonic_Block_Plugin_c::init_parameter_map( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setFftSize,
    const unsigned char setWindowType,
    const double setWindowOption,
    const MP_Real_t setF0Min,
    const MP_Real_t setF0Max,
    const unsigned int  setMaxNumPartials,
    const unsigned long int setBlockOffset )
{
	
const char* func = "MP_Harmonic_Block_c::init_parameter_map(...)";

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
if (!(oss << setFftSize)) { mp_error_msg( func, "Cannot convert fftSize in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["fftSize"] = oss.str();
oss.str("");
(*parameterMap)["windowtype"] = window_name(setWindowType);

if (!(oss << setWindowOption)) { mp_error_msg( func, "Cannot convert windowopt in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["windowopt"] = oss.str();

oss.str("");

if (!(oss << setF0Min)) { mp_error_msg( func, "Cannot convert f0Min in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["f0Min"] = oss.str();
oss.str("");
if (!(oss << setF0Max)) { mp_error_msg( func, "Cannot convert f0Min in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["f0Max"] = oss.str();
oss.str("");
if (!(oss << setMaxNumPartials)) { mp_error_msg( func, "Cannot convert f0Min in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["numPartials"] = oss.str();
oss.str("");
if (!(oss << setBlockOffset)) { mp_error_msg( func, "Cannot convert blockOffset in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["blockOffset"] = oss.str();
oss.str("");

return (0);
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Harmonic_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Harmonic_Block_c::plug_signal( signal )";
  unsigned long int maxFundFreqIdx = 0;

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL )
    {

      /* Go up the inheritance graph */
      if ( MP_Gabor_Block_Plugin_c::plug_signal( setSignal ) )
        {
          mp_error_msg( func, "Failed to plug a signal at the Gabor block level.\n" );
          nullify_signal();
          return( 1 );
        }

      /* Set and check the signal-related parameters: */

      /* - Turn the frequencies (in Hz) into fft bins */
      minFundFreqIdx = (unsigned long int)( floor( f0Min / ((double)(setSignal->sampleRate) / (double)(fftSize)) ) );
      maxFundFreqIdx = (unsigned long int)( floor( f0Max / ((double)(setSignal->sampleRate) / (double)(fftSize)) ) );

      /* - Check for going over the Nyquist frequency */
      if ( minFundFreqIdx >= numFreqs )
        {
          mp_warning_msg( func, "f0Min [%.2f Hz] is above the signal's Nyquist frequency [%.2f Hz].\n" ,
                          f0Min, ( (double)(setSignal->sampleRate) / 2.0 ) );
          mp_info_msg( func, "For this signal, f0Min will be temporarily reduced to the signal's"
                       " Nyquist frequency.\n" );
          minFundFreqIdx = numFreqs - 1;
        }
      if ( maxFundFreqIdx > numFreqs ) maxFundFreqIdx = (numFreqs - 1); /* For f0Max, rectify silently. */
      /* - Check for going under the DC */
      if ( minFundFreqIdx == 0 )
        {
          mp_warning_msg( func, "f0Min [%.2f Hz] is into the signal's DC frequency range [<%.2f Hz].\n" ,
                          f0Min, ( (double)(setSignal->sampleRate) / (double)(fftSize) ) );
          mp_info_msg( func, "For this signal, f0Min will be temporarily increased to the lower bound"
                       " of the signal's DC frequency band.\n" );
          minFundFreqIdx = 1;
        }

      /* Set the other harmonic fields */
      numFundFreqIdx = maxFundFreqIdx - minFundFreqIdx + 1;

      /* Correct numFilters at the block level */
      numFilters     = numFreqs + numFundFreqIdx; /* We have numFreqs plain Gabor atoms,
            						   plus numFundFreqIdx harmonic subspaces */
    }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Harmonic_Block_Plugin_c::nullify_signal( void )
{

  MP_Gabor_Block_Plugin_c::nullify_signal();

  /* Reset the frequency-related dimensions */
  minFundFreqIdx = 0;
  numFundFreqIdx = 0;
  numFilters = 0;

}


/********************/
/* NULL constructor */
MP_Harmonic_Block_Plugin_c::MP_Harmonic_Block_Plugin_c( void )
    :MP_Gabor_Block_Plugin_c()
{

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Harmonic_Block_c::MP_Harmonic_Block_c()",
                "Constructing a Harmonic block...\n" );

  minFundFreqIdx = numFundFreqIdx = maxNumPartials = 0;

  sum = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Harmonic_Block_c::MP_Harmonic_Block_c()",
                "Done.\n" );

}


/**************/
/* Destructor */
MP_Harmonic_Block_Plugin_c::~MP_Harmonic_Block_Plugin_c()
{

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Harmonic_Block_c::~MP_Harmonic_Block_c()",
                "Deleting harmonic_block...\n" );

  if (sum) free(sum);

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Harmonic_Block_c::~MP_Harmonic_Block_c()",
                "Done.\n" );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
const char* MP_Harmonic_Block_Plugin_c::type_name()
{
  return ("harmonic");
}

/**********************/
/* Readable text dump */
int MP_Harmonic_Block_Plugin_c::info( FILE *fid )
{
	int nChar = 0;

	nChar += (int)mp_info_msg( fid, "HARMONIC BLOCK", "%s window (window opt=%g) of length [%lu], shifted by [%lu] samples;\n", window_name( fft->windowType ), fft->windowOption, filterLen, filterShift );
	nChar += (int)mp_info_msg( fid, "            |-", "projected on [%lu] frequencies and [%lu] fundamental frequencies for a total of [%lu] filters;\n", fft->numFreqs, numFundFreqIdx, numFilters );
	nChar += (int)mp_info_msg( fid, "            |-", "fundamental frequency in the index range [%lu %lu]\n", minFundFreqIdx, minFundFreqIdx+numFundFreqIdx-1 );
	nChar += (int)mp_info_msg( fid, "            |-", "                       (normalized range [%lg %lg]);\n", ((double)minFundFreqIdx)/((double)fft->fftSize), ((double)(minFundFreqIdx+numFundFreqIdx-1))/((double)fft->fftSize) );
	if ( s != NULL )
    {
		nChar += (int)mp_info_msg( fid, "            |-", "                       (Hertz range      [%lg %lg]);\n", (double)minFundFreqIdx * (double)s->sampleRate / (double)fft->fftSize, (double)(minFundFreqIdx+numFundFreqIdx-1) * (double)s->sampleRate / (double)fft->fftSize );
    }
	nChar += (int)mp_info_msg( fid, "            |-", "                       (Original range   [%lg %lg]);\n", f0Min, f0Max );
	nChar += (int)mp_info_msg( fid, "            |-", "maximum number of partials %u;\n", maxNumPartials );
	nChar += (int)mp_info_msg( fid, "            O-", "The number of frames for this block is [%lu], the search tree has [%lu] levels.\n", numFrames, numLevels );
	
	return( nChar );
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Harmonic_Block_Plugin_c::update_frame( unsigned long int frameIdx,
    MP_Real_t *maxCorr,
    unsigned long int *maxFilterIdx )
{

  unsigned long int inShift;

  MP_Real_t *in;
  MP_Real_t *magPtr;

  int chanIdx;
  int numChans;

  unsigned long int freqIdx, fundFreqIdx, kFundFreqIdx;
  unsigned int numPartials, kPartial;
  double local_sum;
  double max;
  unsigned long int maxIdx;

  assert( s != NULL );
  numChans = s->numChans;
  assert( mag != NULL );

  inShift = frameIdx*filterShift + blockOffset;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0, magPtr = mag;    /* <- for each channel */
        chanIdx < numChans;
        chanIdx++,   magPtr += numFreqs )
    {

      assert( s->channel[chanIdx] != NULL );

      /* Hook the signal and the inner products to the fft */
      in  = s->channel[chanIdx] + inShift;

      /* Execute the FFT (including windowing, conversion to energy etc.) */
      compute_energy( in,
                      reCorrel, imCorrel, sqCorrel, cstCorrel,
                      magPtr );

    } /* end foreach channel */
  /*----*/

  /*----*/
  /* Fill the sum array and find the max over gabor atoms: */
  /* --Gabor atom at freqIdx =  0: */
  /* - make the sum over channels */
  local_sum = (double)(*mag);                  /* <- channel 0      */
  for ( chanIdx = 1, magPtr = mag+numFreqs; /* <- other channels */
        chanIdx < numChans;
        chanIdx++,   magPtr += numFreqs )   local_sum += (double)(*magPtr);
  sum[0] = local_sum;
  /* - init the max */
  max = local_sum;
  maxIdx = 0;
  /* -- Following GABOR atoms: */
  for ( freqIdx = 1; freqIdx < numFreqs; freqIdx++)
    {
      /* - make the sum */
      local_sum = (double)(mag[freqIdx]);               /* <- channel 0      */
      for ( chanIdx = 1, magPtr = mag+numFreqs+freqIdx; /* <- other channels */
            chanIdx < numChans;
            chanIdx++,   magPtr += numFreqs ) local_sum += (double)(*magPtr);
      sum[freqIdx] = local_sum;
      /* - update the max */
      if ( local_sum > max )
        {
          max = local_sum;
          maxIdx = freqIdx;
        }
    }
  /* -- Following HARMONIC elements: */
  for ( /* freqIdx same,*/ fundFreqIdx = minFundFreqIdx;
                           freqIdx < numFilters;
                           freqIdx++,         fundFreqIdx++)
    {
      /* Re-check the number of partials */
      numPartials = (numFreqs-1) / fundFreqIdx;
      if ( numPartials > maxNumPartials ) numPartials = maxNumPartials;
      /* - make the sum */
      local_sum = 0.0;
      for ( kPartial = 0, kFundFreqIdx = fundFreqIdx;
            kPartial < numPartials;
            kPartial++,   kFundFreqIdx += fundFreqIdx )
        {
          assert( kFundFreqIdx < numFreqs );
          local_sum += sum[kFundFreqIdx];
        }
      /* - update the max */
      if ( local_sum > max )
        {
          max = local_sum;
          maxIdx = freqIdx;
        }
    }
  *maxCorr = (MP_Real_t)(max);
  *maxFilterIdx = maxIdx;
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Harmonic_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int filterIdx )
{

  const char* func = "MP_Harmonic_Block_c::create_atom(...)";

  /* --- Return a Gabor atom when it is what filterIdx indicates */
  if ( filterIdx < numFreqs ) return( MP_Gabor_Block_Plugin_c::create_atom( atom, frameIdx, filterIdx ) );
  /* --- Otherwise create the Harmonic atom :  */
  else
    {

      //  MP_Harmonic_Atom_c *hatom = NULL;
      MP_Harmonic_Atom_Plugin_c *hatom = NULL;
      unsigned int kPartial, numPartials;
      /* Parameters for a new FFT run: */
      MP_Real_t *in;
      unsigned long int fundFreqIdx, kFundFreqIdx;
      /* Parameters for the atom waveform : */
      double re, im;
      double amp, phase, gaborAmp = 1.0, gaborPhase = 0.0;
      double reCorr, imCorr, sqCorr;
      double real, imag, energy;
      /* Misc: */
      int chanIdx;
      unsigned long int pos = frameIdx*filterShift + blockOffset;


      /* Check the position */
      if ( (pos+filterLen) > s->numSamples )
        {
          mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
                        " Returning a NULL atom.\n" );
          *atom = NULL;
          return( 0 );
        }

      /* Compute the fundamental frequency and the number of partials */
      fundFreqIdx = filterIdx - numFreqs + minFundFreqIdx;
      numPartials = (numFreqs-1) / fundFreqIdx;
      if ( numPartials > maxNumPartials ) numPartials = maxNumPartials;

      /* Allocate the atom */
      *atom = NULL;
      MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("harmonic");
      if (NULL == emptyAtomCreator)
        {
          mp_error_msg( func, "Harmonic atom is not registred in the atom factory" );
          return( 0 );
        }

      if ( (hatom =  (MP_Harmonic_Atom_Plugin_c*)(*emptyAtomCreator)())  == NULL )
        {
          mp_error_msg( func, "Can't allocate a new Harmonic atom."
                        " Returning NULL as the atom reference.\n" );
          return( 0 );
        }
      if ( hatom->alloc_atom_param( s->numChans) )
        {
          mp_error_msg( func, "Failed to allocate some vectors in the new atom. Returning a NULL atom.\n" );
          return( 0 );

        }
      if ( hatom->alloc_gabor_atom_param( s->numChans) )
        {
          mp_error_msg( func, "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
          return( 0 );

        }
      /* Set the window-related values */
      if ( window_type_is_ok( fft->windowType ) )
        {
          hatom->windowType   = fft->windowType;
          hatom->windowOption = fft->windowOption;
        }
      else
        {
          mp_error_msg( func, "The window type is unknown. Returning a NULL atom.\n" );
          return( 0);
        }

      /* Set numPartials */
      if ( numPartials < 2 )
        {
          mp_error_msg( func, "When constructing a Harmonic atom, the number of partials [%u]"
                        " must be greater than 2. Returning a NULL atom.\n", numPartials );
          return(0 );
        }
      else hatom->numPartials  = numPartials;

      if ( hatom->alloc_harmonic_atom_param( s->numChans, numPartials ) )
        {
          mp_error_msg( func, "Failed to allocate some vectors in the new Harmonic atom. Returning a NULL atom.\n" );
          return( 0 );

        }

      /* 1) set the fundamental frequency and chirp of the atom */

      hatom->freq  = (MP_Real_t)( (double)(fundFreqIdx) / (double)(fft->fftSize));
      hatom->chirp = (MP_Real_t)( 0.0 );     /* So far there is no chirprate */
      hatom->numSamples = pos + filterLen;

      /* For each channel: */
      for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ )
        {

          /* 2) set the support of the atom */
          hatom->support[chanIdx].pos = pos;
          hatom->support[chanIdx].len = filterLen;
          hatom->totalChanLen += filterLen;

          /* 3) seek the right location in the signal */
          in  = s->channel[chanIdx] + pos;

          /* 4) recompute the inner product of the complex Gabor atoms
           * corresponding to the partials, using the FFT */
          fft->exec_complex( in, fftRe, fftIm );

          /* 5) set the amplitude an phase for each partial */
          for ( kPartial = 0, kFundFreqIdx = fundFreqIdx;
                kPartial < numPartials;
                kPartial++,   kFundFreqIdx += fundFreqIdx )
            {

              assert( kFundFreqIdx < numFreqs );

              re  = (double)( *(fftRe + kFundFreqIdx) );
              im  = (double)( *(fftIm + kFundFreqIdx) );
              energy = re*re + im*im;
              reCorr = reCorrel[kFundFreqIdx];
              imCorr = imCorrel[kFundFreqIdx];
              sqCorr = sqCorrel[kFundFreqIdx];
              assert( sqCorr <= 1.0 );

              /* At the Nyquist frequency: */
              if ( kFundFreqIdx == (numFreqs-1) )
                {
                  assert( reCorr == 1.0 );
                  assert( imCorr == 0.0 );
                  assert( im == 0 );
                  amp = sqrt( energy );
                  if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
                  else             phase = MP_PI; /* corresponds to the '-' sign exp(i\pi) */
                }
              /* When the atom and its conjugate are aligned, they should be real
               * and the phase is simply the sign of the inner product (re,im) = (re,0) */
              else
                {
                  real = (1.0 - reCorr)*re + imCorr*im;
                  imag = (1.0 + reCorr)*im + imCorr*re;
                  amp   = 2.0 * sqrt( real*real + imag*imag );
                  phase = atan2( imag, real ); /* the result is between -M_PI and M_PI */
                }

              /* case of the first partial */
              if ( kPartial == 0 )
                {
                  hatom->amp[chanIdx]   = gaborAmp   = (MP_Real_t)( amp   );
                  hatom->phase[chanIdx] = gaborPhase = (MP_Real_t)( phase );
                  hatom->partialAmp[chanIdx][kPartial]   = (MP_Real_t)(1.0);
                  hatom->partialPhase[chanIdx][kPartial] = (MP_Real_t)(0.0);
                }
              else
                {
                  hatom->partialAmp[chanIdx][kPartial]   = (MP_Real_t)( amp / gaborAmp   );
                  hatom->partialPhase[chanIdx][kPartial] = (MP_Real_t)( phase - gaborPhase );
                }
              /*
              	mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "freq %g chirp %g partial %lu amp %g phase %g\n"
              		      " reCorr %g imCorr %g\n re %g im %g 2*(re^2+im^2) %g\n",
              		      hatom->freq, hatom->chirp, kPartial+1, amp, phase,
              		      reCorr, imCorr, re, im, 2*(re*re+im*im) );
              */
            } /* <--- end loop on partials */

        } /* <--- end loop on channels */
      *atom = hatom;
      return( 1 );
    }
}


/*********************************************/
/* get Paramater type map defining the block */
void MP_Harmonic_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){
const char * func = "void MP_Harmonic_Block_Plugin_c::get_parameters_type_map()";
if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
(*parameterMapType)["windowLen"] = "ulong";
(*parameterMapType)["windowShift"] = "ulong";
(*parameterMapType)["fftSize"] = "ulong";
(*parameterMapType)["windowtype"] = "string";
(*parameterMapType)["windowopt"] = "real";
(*parameterMapType)["f0Min"] = "real";
(*parameterMapType)["f0Max"] = "real";
(*parameterMapType)["numPartials"] = "uint";
(*parameterMapType)["blockOffset"] = "ulong";
(*parameterMapType)["windowRate"] = "real";
} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );



}

/***********************************/
/* get Info map defining the block */
void MP_Harmonic_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){
const char * func = "void MP_Harmonic_Block_Plugin_c::get_parameters_info_map()";

	if ((*parameterMapInfo).empty()) {
		(*parameterMapInfo)["type"] = "'harmonic' block generate harmonic atoms or Gabor atoms. If a harmonic atom with fundamental frequency within the given range has more energy than all Gabor atoms at the specified scale, then a harmonic atom is generated. If some Gabor atom below the minimum fundamental frequency or above the highest partial has more energy, it is the one generated.";
		(*parameterMapInfo)["windowLen"] = "The common length of the atoms (which is the length of the signal window), in number of samples.";
		(*parameterMapInfo)["windowShift"] = "The shift between atoms on adjacent time frames, in number of samples. It MUST be at least one.";
		(*parameterMapInfo)["fftSize"] = "The size of the FFT, including the effect of zero padding. It MUST be and EVEN integer, at least as large as <windowLen>. It determines the number of discrete frequencies of the collection of Gabor atoms associated with a Gabor block, which is (fftSize/2)+1.";
		(*parameterMapInfo)["windowtype"] = "The window type, which determines its shape. Examples include 'gauss', 'rect', 'hamming' (see the developper documentation of libdsp_windows.h). A related parameter is <windowopt>.";
		(*parameterMapInfo)["windowopt"] = "The optional window shape parameter (see the developper documentation oflibdsp_windows.h).";
		(*parameterMapInfo)["f0Min"] = "Minimum allowed fundamental frequency of the harmonic subspaces, expressed in frequency bins between 0 (DC) and fftSize/2 (Nyquist).";
		(*parameterMapInfo)["f0Max"] = "Maximum allowed fundamental frequency of the harmonic subspaces, expressed in frequency bins between 0 (DC) and fftSize/2 (Nyquist).";
		(*parameterMapInfo)["numPartials"] = "Maximum number of partials to be considered in each harmonic subspace.";
		(*parameterMapInfo)["blockOffset"] = "Offset between beginning of signal and beginning of first atom, in number of samples.";
		(*parameterMapInfo)["windowRate"] = "The shift between atoms on adjacent time frames, in proportion of the <windowLen>. For example, windowRate = 0.5 corresponds to half-overlapping signal windows.";
		
		
	} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );
	
}

/***********************************/
/* get default map defining the block */
void MP_Harmonic_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Harmonic_Block_Plugin_c::get_parameters_default_map()";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "harmonic";
(*parameterMapDefault)["windowLen"] = "1024";
(*parameterMapDefault)["windowShift"] = "512";
(*parameterMapDefault)["fftSize"] = "1024";
(*parameterMapDefault)["windowtype"] = "gauss";
(*parameterMapDefault)["windowopt"] = "0";
(*parameterMapDefault)["f0Min"] = "100";
(*parameterMapDefault)["f0Max"] = "1000";
(*parameterMapDefault)["numPartials"] = "1000";
(*parameterMapDefault)["blockOffset"] = "0";
(*parameterMapDefault)["windowRate"] = "0.5";
 }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}
