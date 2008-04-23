/******************************************************************************/
/*                                                                            */
/*                             chirp_block.cpp                                */
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
 * $Author: sacha $
 * $Date: 2005-07-25 20:37:06 +0200 (Mon, 25 Jul 2005) $
 * $Revision: 22 $
 *
 */

/***************************************************/
/*                                                 */
/* chirp_block.cpp: methods for chirp blocks       */
/*                                                 */
/***************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "gabor_atom_plugin.h"
#include "gabor_block_plugin.h"
#include "chirp_block_plugin.h"
#include <sstream>

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Block_c* MP_Chirp_Block_Plugin_c::create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap )
{
	const char* func = "MP_Gabor_Block_Plugin_c::create( MP_Signal_c *setSignal, map<const char*, const char*, ltstr> *paramMap )";
  MP_Chirp_Block_Plugin_c *newBlock = NULL;
  char*  convertEnd;
  unsigned long int filterLen = 0;
  unsigned long int filterShift =0 ;
  unsigned long int fftSize  = 0;
  unsigned char windowType;
  double windowOption = 0.0;
  unsigned int numFitPoints = 0;
  unsigned int numIter = 0;
  double windowRate =0.0;
  unsigned long int blockOffset = 0;
  
  /* Instantiate and check */
  newBlock = new MP_Chirp_Block_Plugin_c();
  if ( newBlock == NULL )
    {
      mp_error_msg( func, "Failed to create a new Chirp block.\n" );
      return( NULL );
    }
  /* Analyse the parameter map */
  if (strcmp((*paramMap)["type"].c_str(),"chirp"))
    {
      mp_error_msg( func, "Parameter map does not define a Chirp block.\n" );
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
  else  { mp_error_msg( func, "No parameter windowtype in the parameter map.\n" ); 
  return( NULL );
  }
   if ( window_needs_option(windowType) && (!(*paramMap)["windowopt"].size()>0) ) {
      mp_error_msg( func, "Chirp"
		    " requires a window option (the opt=\"\" attribute is probably missing"
		    " in the relevant <window> tag). Returning a NULL block.\n");
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
	  if ((*paramMap)["numFitPoints"].size()>0)
    {
      /*Convert windowopt*/
      numFitPoints=strtol((*paramMap)["numFitPoints"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter numFitPoints in unsigned int.\n" );
          return( NULL );
        }
    }
  else
    {
    numFitPoints = 1;
    }
	
		  if ((*paramMap)["numIter"].size()>0)
    {
      /*Convert windowopt*/
      numIter=strtol((*paramMap)["numIter"].c_str(), &convertEnd, 10);
      if (*convertEnd != '\0')
        {
          mp_error_msg( func, "cannot convert parameter numIter in unsigned int.\n" );
          return( NULL );
        }
    }
  else
    {
      numIter = 1;
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
                                   windowType, windowOption ,
                                  numFitPoints, numIter, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in the new Chirp block.\n" );
      delete( newBlock );
      return( NULL );
    }
    
  /* Set the block parameter map (that are independent from the signal) */
   if ( newBlock->init_parameter_map( filterLen, filterShift, fftSize,
                                   windowType, windowOption ,
                                  numFitPoints, numIter, blockOffset ) )
    {
      mp_error_msg( func, "Failed to initialize some block parameters in the new Chirp block map.\n" );
      delete( newBlock );
      return( NULL );
    }
  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) )
    {
      mp_error_msg( func, "Failed to plug a signal in the new Chirp block.\n" );
      delete( newBlock );
      return( NULL );
    }
	
	
	return( (MP_Block_c*)newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
/************************/
/* Specific constructor */
int MP_Chirp_Block_Plugin_c::init_parameters( const unsigned long int setFilterLen,
                                       const unsigned long int setFilterShift,
                                       const unsigned long int setFftSize,
                                       const unsigned char setWindowType,
                                       const double setWindowOption,
                                       const unsigned int setNumFitPoints,
                                       const unsigned int setNumIter,
                                       const unsigned long int setBlockOffset )
{

  const char* func = "MP_Chirp_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Gabor_Block_Plugin_c::init_parameters( setFilterLen, setFilterShift, setFftSize,
                                          setWindowType, setWindowOption, setBlockOffset  ) )
    {
      mp_error_msg( func, "Failed to init the parameters at the Gabor block level"
                    " in the new Chirp block.\n" );
      return( 1 );
    }

  /* TODO: check the chirp-specific fields ? */

  /* Set the chirp-specific fields */
  numFitPoints = setNumFitPoints;
  totNumFitPoints = 2*numFitPoints+1;
  numIter = setNumIter;

  /* Allocate the chirp-specific buffers */

  /* - Demodulation chirp signal: */
  if ( (chirpRe = (MP_Real_t*) calloc( filterLen , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                    " for the chirpRe array. This pointer will remain NULL.\n", filterLen );
      return( 1 );
    }
  if ( (chirpIm = (MP_Real_t*) calloc( filterLen , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                    " for the chirpIm array. This pointer will remain NULL.\n", filterLen );
      return( 1 );
    }

  /* - Input signal x demodulation chirp: */
  if ( (sigChirpRe = (MP_Real_t*) calloc( filterLen , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                    " for the sigChirpRe array. This pointer will remain NULL.\n", filterLen );
      return( 1 );
    }
  if ( (sigChirpIm = (MP_Real_t*) calloc( filterLen , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                    " for the sigChirpIm array. This pointer will remain NULL.\n", filterLen );
      return( 1 );
    }

  /* - Misc: */
  if ( (fftEnergy = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
                    " for the fftEnergy array. This pointer will remain NULL.\n", numFreqs );
      return( 1 );
    }

  if ( (logAmp = (MP_Real_t*) calloc( totNumFitPoints , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%u] MP_Real_t elements"
                    " for the logAmp array. This pointer will remain NULL.\n", totNumFitPoints );
      return( 1 );
    }
  if ( (phase = (MP_Real_t*) calloc( totNumFitPoints , sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( func, "Can't allocate an array of [%u] MP_Real_t elements"
                    " for the phase array. This pointer will remain NULL.\n", totNumFitPoints );
      return( 1 );
    }

  if ( alloc_correl( &reCorrelChirp, &imCorrelChirp, &sqCorrelChirp, &cstCorrelChirp ) )
    {
      mp_error_msg( func, "Failed to allocate the block's chirp-related autocorrelations.\n" );
      return( 1 );
    }

  return( 0 );
}

/*************************************************************/
/* Initialization of signal-independent block parameters map */
int MP_Chirp_Block_Plugin_c::init_parameter_map( const unsigned long int setFilterLen,
    const unsigned long int setFilterShift,
    const unsigned long int setFftSize,
    const unsigned char setWindowType,
    const double setWindowOption,
    const unsigned int setNumFitPoints,
    const unsigned int setNumIter,
    const unsigned long int setBlockOffset )
{

const char* func = "MP_Chirp_Block_c::init_parameter_map(...)";
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

(*parameterMap)["windowtype"] = window_name(setWindowType);
oss.str("");
if (!(oss << setWindowOption)) { mp_error_msg( func, "Cannot convert windowopt in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["windowopt"] = oss.str();
oss.str("");
if (!(oss << setNumFitPoints)) { mp_error_msg( func, "Cannot convert numFitPoints in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["numFitPoints"] = oss.str();
oss.str("");
if (!(oss << setNumIter)) { mp_error_msg( func, "Cannot convert numIter in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["numIter"]= oss.str();
oss.str("");
if (!(oss << setBlockOffset)) { mp_error_msg( func, "Cannot convert blockOffset in string for parameterMap.\n" 
                     );
      return( 1 );
      }
(*parameterMap)["blockOffset"] = oss.str();
oss.str("");

return( 0 );
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Chirp_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Chirp_Block_c::plug_signal( signal )";

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

    }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Chirp_Block_Plugin_c::nullify_signal( void )
{

  MP_Gabor_Block_Plugin_c::nullify_signal();

}


/********************/
/* NULL constructor */
MP_Chirp_Block_Plugin_c::MP_Chirp_Block_Plugin_c()
    :MP_Gabor_Block_Plugin_c()
{

  numFitPoints = 0;
  totNumFitPoints = 0;
  numIter = 0;

  chirpRe = NULL;
  chirpIm = NULL;

  sigChirpRe = NULL;
  sigChirpIm = NULL;
  fftEnergy = NULL;

  logAmp = NULL;
  phase = NULL;

  reCorrelChirp = NULL;
  imCorrelChirp = NULL;
  sqCorrelChirp = NULL;
  cstCorrelChirp = NULL;

}


/**************/
/* Destructor */
MP_Chirp_Block_Plugin_c::~MP_Chirp_Block_Plugin_c()
{

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Chirp_Block_c::~MP_Chirp_Block_c()", "Deleting chirp_block...\n" );

  if ( chirpRe ) free(chirpRe);
  if ( chirpIm ) free(chirpIm);

  if ( sigChirpRe ) free(sigChirpRe);
  if ( sigChirpIm ) free(sigChirpIm);
  if ( fftEnergy ) free(fftEnergy);

  if ( logAmp ) free(logAmp);
  if ( phase  ) free(phase);

  if ( reCorrelChirp  ) free( reCorrelChirp  );
  if ( imCorrelChirp  ) free( imCorrelChirp  );
  if ( sqCorrelChirp  ) free( sqCorrelChirp  );
  if ( cstCorrelChirp ) free( cstCorrelChirp );

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Chirp_Block_c::~MP_Chirp_Block_c()", "Done.\n" );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char* MP_Chirp_Block_Plugin_c::type_name()
{
  return ("chirp");
}


/********/
/* Readable text dump */
int MP_Chirp_Block_Plugin_c::info( FILE* fid )
{

  int nChar = 0;

  nChar += mp_info_msg( fid, "CHIRP BLOCK", "%s window (window opt=%g) of length [%lu],"
                        " shifted by [%lu] samples,\n",
                        window_name( fft->windowType ), fft->windowOption,
                        filterLen, filterShift );
  nChar += mp_info_msg( fid, "         |-", "projected on [%lu] frequencies;\n",
                        numFilters );
  nChar += mp_info_msg( fid, "         |-", "numFitPoints is [%u], numIter is [%u];\n",
                        numFitPoints, numIter );
  nChar += mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu],"
                        " the search tree has [%lu] levels.\n",
                        numFrames, numLevels );
  return ( nChar );
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Chirp_Block_Plugin_c::create_atom( MP_Atom_c **atom,
    const unsigned long int frameIdx,
    const unsigned long int filterIdx )
{
  const char* func = "MP_Chirp_Block_c::create_atom(...)";
  MP_Gabor_Atom_Plugin_c *gatom = NULL;
  unsigned long int freqIdx, fIdxMin, fIdxMax, k;
  MP_Real_t chirprate, chirprateBefore = 0.0;
  unsigned int l;
  unsigned int iter;

  int chanIdx, numChans;
  MP_Real_t *in;

  MP_Real_t reCenter,imCenter,sqCenter;
  MP_Real_t re,im,reSq,imSq,energy,real,imag,amp,atomphase;
  MP_Real_t a,b,d,e,fftSize;
  MP_Real_t lambda,mu,deltaChirp;
  //MP_Real_t alpha,beta;

  mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "Entering CHIRP::create_atom.\n" );

  /* Check that the treated Gabor atom is the max one
     (otherwise the chirp detection model is not valid) */
  if ( (frameIdx != maxIPFrameIdx) || (filterIdx != maxIPIdxInFrame[maxIPFrameIdx]) )
    {
      mp_error_msg( func, "It is forbidden to re-evaluate the chirp rate of a non-max Gabor atom."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }

  /* Useful dereferences */
  numChans = s->numChans;
  fftSize = (MP_Real_t)(fft->fftSize);

  /* Create the best Gabor atom with chirprate zero */
  if ( ( MP_Gabor_Block_Plugin_c::create_atom( atom, frameIdx, filterIdx ) ) == 0 )
    {
      mp_error_msg( func, "Can't create a new Gabor atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
  gatom = (MP_Gabor_Atom_Plugin_c*)(*atom);
  chirprate = gatom->chirp;
#ifndef NDEBUG
  gatom->info( stderr );
  fflush( stderr );
#endif

  /******************/
  /* I) ITERATION 0 */
  /******************/
  /* Note: during this iteration, the chirprate is equal to 0. */

  /*****************************/
  /* I.1) FIT A NEW CHIRP RATE */

  /* Find the index closest to the frequency of the current atom */
  freqIdx = (unsigned long int) floor( (double)(gatom->freq) * (double)(fftSize) + 0.5 );

  mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                "freqIdx was = %lu , freq = %f (cplxSize = %g).\n",
                freqIdx, gatom->freq, fftSize );

  /* If there is not enough fit points on both sides of the frequency,
   * keep the current chirprate ( =0 ) and return the unchanged gabor atom. */
  if ( (freqIdx <= (unsigned long int)numFitPoints) ||
       ( (freqIdx+(unsigned long int)numFitPoints) >= numFreqs ) )
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "freqIdx = %lu , RETURNING.\n", freqIdx );
      return( 1 );
    }

  /* Reset the logamp and phase accumulators */
  for ( l = 0; l < totNumFitPoints; l++ ) logAmp[l] = phase[l] = 0.0;

  /* Compute an FFT per channel and fill the buffers of points to fit */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {

      assert ( s->channel[chanIdx] != NULL );

      /* Re-compute the complex FFT */
      in = s->channel[chanIdx] + gatom->support[chanIdx].pos;
      fft->exec_complex( in , fftRe, fftIm );

      /* Normalize the FFT at the center point */
      reCenter = fftRe[freqIdx];
      imCenter = fftIm[freqIdx];
      sqCenter = reCenter*reCenter + imCenter*imCenter;
      reCenter = reCenter/sqCenter;
      imCenter = - imCenter/sqCenter;

      /* Convert to 'logpolar' coordinates after division by the complex value at the center point,
         and accumulate the logAmp and phase value for the cross-channel average */
      for ( l = 0; l < totNumFitPoints; l++ )
        {
          re = fftRe[freqIdx-numFitPoints+l]*reCenter - fftIm[freqIdx-numFitPoints+l]*imCenter;
          im = fftRe[freqIdx-numFitPoints+l]*imCenter + fftIm[freqIdx-numFitPoints+l]*reCenter;
          logAmp[l] += (MP_Real_t)( log( fabs(re*re+im*im) ) ); /* Division by two (corresponding to a square root)
                    							       is done when averaging over channels */
          phase[l]  += (MP_Real_t)( atan2(im,re) );
        }  /* <-- end loop on fit points */

    }   /* <-- end loop on channels */

  /* Finalize the average logAmp / phase over channels */
  for ( l = 0; l < totNumFitPoints; l++ )
    {
      logAmp[l] = logAmp[l] / (2*numChans);
      phase[l]  = phase[l]  / numChans;
    }   /* <-- end loop on fit points */

  /* Perform the regression on the fit points */
  parabolic_regression( logAmp, phase, numFitPoints,
                        &a, &b, &d, &e );

  /* Convert the result into a new chirprate */
  lambda = - ( fftSize * fftSize ) * a * MP_INV_PI_SQ;
  mu     = - ( fftSize * fftSize ) * d * MP_INV_PI_SQ;
  //alpha  = ( freqIdx - b/(2*a) ) / fftSize;
  //beta   = ( freqIdx - e/(2*d) ) / fftSize;

  deltaChirp = mu/(MP_PI*(lambda*lambda+mu*mu));
  chirprate += deltaChirp;
  /* BORK */
  //#define MP_CHIRP_THRESH 1e-5
#define MP_CHIRP_THRESH 0.5e-5
  //#define MP_CHIRP_THRESH 0.2e-5
  if ( fabs(chirprate) > MP_CHIRP_THRESH )
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "Chirp rate threshold hit ! chirp = %g , abs = %g."
                    " Returning the original (un-chirped) Gabor atom.\n",
                    chirprate, fabs(chirprate) );
      return( 1 );
    }

  mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                "iter  0 : delta = %g , new chirp = %g.\n",
                deltaChirp, chirprate );


  /**********************************************/
  /* I.2) RE-LOCATE THE ATOM'S CENTER FREQUENCY */

  /* Update the chirp demodulation signal and the related correlations */
  set_chirp_demodulator( chirprate );

  /* Compute the new inner products with the complex chirp on each channel
     and update the energy over all channels at each frequency */

  for ( k = 0; k < numFreqs; k++ ) fftEnergy[k] = 0.0;

  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {

      /* Compute FFT(sig*demodulator) */
      in = s->channel[chanIdx] + gatom->support[chanIdx].pos;
      fft->exec_complex_demod( in, chirpRe, chirpIm, fftRe, fftIm );

      /* Compute the magnitude of the best real chirp atom for each frequency */
      for ( k = 0;  k < numFreqs;  k++ )
        {

          /* Get the complex values */
          re = fftRe[k];
          im = fftIm[k];
          reSq = ( re * re );
          imSq = ( im * im );
          /* If the atom's autocorrelation is neglegible: */
          if ( sqCorrelChirp[k] < MP_ENERGY_EPSILON )
            {
              energy = 2 * ( reSq + imSq );
            }
          /* Else, if the atom's autocorrelation is NOT neglegible: */
          else
            {
              energy  =   ( reSq + imSq )
                          - reCorrelChirp[k] * ( reSq - imSq )
                          + imCorrelChirp[k] * (  2 * re*im  );
              energy = cstCorrelChirp[k] * energy;
            }
          /* => Compensate for a possible numerical innacuracy
           *    (this case should never happen in practice) */
          if ( energy < 0 )
            {
              mp_warning_msg( func, "A negative energy was met."
                              " (energy = [%g])\nEnergy value is reset to 0.0 .", energy );
              energy = 0.0;
            }

          /* Cast and fill mag */
          fftEnergy[k] += energy;
        }
    } /* <-- end loop on channels */


  /* Find the best frequency */
  energy = 0.0;
#define MP_FREQ_RELOC_RANGE 10
  if ( freqIdx > MP_FREQ_RELOC_RANGE ) fIdxMin = freqIdx - MP_FREQ_RELOC_RANGE;
  else                                 fIdxMin = 0;
  fIdxMax = freqIdx + MP_FREQ_RELOC_RANGE;
  if ( fIdxMax > numFreqs ) fIdxMax = numFreqs;
  for ( k = fIdxMin; k < fIdxMax; k++ )
    {
      /* for ( k = 0; k < numFreqs; k++ ) { */
      if ( fftEnergy[k] > energy)
        {
          energy = fftEnergy[k];
          freqIdx = k;
        }
    }


  /* TEST: if the correlation of the chirped atom is less than or equal to
     the correlation of the original unchirped one, it's a case where the chirp
     detection model is invalid => keep the unchirped one and exit. */
  if (energy <= maxIPValue )
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "At iteration 0 in the chirp estimation: the chirping model does not apply."
                    " (Original IP: %g; Chirped IP: %g)"
                    " Returning the original (un-chirped) Gabor atom.\n",
                    maxIPValue, energy );
      return( 1 );
    }
  else if (0==freqIdx || freqIdx==numFreqs-1)
    {
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "At iteration 0 in the chirp estimation: the chirping model does selects a zero frequency."
                    " Returning the original (un-chirped) Gabor atom.\n");
      return( 1 );
    }

  mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                "iter  0 : New freqIdx = %lu.\n", freqIdx );

  /****************************/
  /* II) FOLLOWING ITERATIONS */
  /****************************/

  for ( iter = 1; iter < numIter; iter++ )
    {

      /******************************/
      /* II.1) FIT A NEW CHIRP RATE */

      /* If there is not enough fit points on both sides of the frequency,
       * keep the current chirprate (=0) and stop */
      if ( (freqIdx <= (unsigned long int)numFitPoints) ||
           ( (freqIdx+(unsigned long int)numFitPoints) >= numFreqs ) ) break;

      /* Reset the logamp and phase accumulators */
      for ( l = 0; l < totNumFitPoints; l++ ) logAmp[l] = phase[l] = 0.0;

      /* Compute an FFT per channel and fill the buffers of points to fit */
      for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
        {

          /* Compute the FFT of the demodulated signal */
          in = s->channel[chanIdx] + gatom->support[chanIdx].pos;
          fft->exec_complex_demod( in, chirpRe, chirpIm, fftRe, fftIm );

          /* Normalize the FFT at the center point */
          reCenter = fftRe[freqIdx];
          imCenter = fftIm[freqIdx];
          sqCenter = reCenter*reCenter + imCenter*imCenter;
          reCenter = reCenter/sqCenter;
          imCenter = - imCenter/sqCenter;

          /* Convert to 'logpolar' coordinates after division by the value at the center point,
             and accumulate the logAmp and phase value for the cross-channel average */
          for ( l = 0; l < totNumFitPoints; l++ )
            {
              re = fftRe[freqIdx-numFitPoints+l]*reCenter - fftIm[freqIdx-numFitPoints+l]*imCenter;
              im = fftRe[freqIdx-numFitPoints+l]*imCenter + fftIm[freqIdx-numFitPoints+l]*reCenter;
              logAmp[l] += (MP_Real_t)( log( fabs(re*re+im*im) ) ); /* Division by two (corresponding to a square root)
                            								   is done when averaging over channels */
              phase[l]  += (MP_Real_t)( atan2(im,re) );
            }  /* <-- end loop on fit points */

        } /* <-- end loop on channels */

      /* Finalize the average logAmp / phase over channels */
      for ( l = 0; l < totNumFitPoints; l++ )
        {
          logAmp[l] = logAmp[l] / (2*numChans);
          phase[l]  = phase[l]  / numChans;
        }   /* <-- end loop on fit points */

      /* Perform the regression on the fit points */
      parabolic_regression( logAmp, phase, numFitPoints,
                            &a, &b, &d, &e );


      /* Convert the result into a new chirprate */
      lambda = - ( fftSize * fftSize ) * a * MP_INV_PI_SQ;
      mu     = - ( fftSize * fftSize ) * d * MP_INV_PI_SQ;
      //alpha  = ( freqIdx - b/(2*a) ) / fftSize;
      //beta   = ( freqIdx - e/(2*d) ) / fftSize;

      deltaChirp = mu/(MP_PI*(lambda*lambda+mu*mu));
      chirprateBefore = chirprate;
      chirprate += deltaChirp;

      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "iter %2d : delta = %g , new chirp = %g.\n",
                    iter, deltaChirp, chirprate );

      /***********************************************/
      /* II.2) RE-LOCATE THE ATOM'S CENTER FREQUENCY */

      /* Update (chirpRe,chirpIm), the real and imaginary parts of exp(-i*pi*chirprate*t^2)
       * as well as the correlations reCorrelChirp, imCorrelChirp, sqCorrelChirp, cstCorrelChirp between complex chirp 
       * atoms and their conjugates */
      set_chirp_demodulator( chirprate );

      /* Compute the new inner products with the complex chirp on each channel
      and update the energy over all channels at each frequency */
      for ( k = 0; k < numFreqs; k++ ) fftEnergy[k] = 0.0;

      for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
        {

          /* Compute the FFT of the demodulated signal */
          in = s->channel[chanIdx] + gatom->support[chanIdx].pos;
          fft->exec_complex_demod( in, chirpRe, chirpIm, fftRe, fftIm );

          /* Compute the magnitude of the best real chirp atom for each frequency */
          for ( k = 0;  k < numFreqs;  k++ )
            {

              /* Get the complex values */
              re = fftRe[k];
              im = fftIm[k];
              reSq = ( re * re );
              imSq = ( im * im );
              /* If the atom's autocorrelation is neglegible: */
              if ( sqCorrelChirp[k] < MP_ENERGY_EPSILON )
                {
                  energy = 2 * ( reSq + imSq );
                }
              /* Else, if the atom's autocorrelation is NOT neglegible: */
              else
                {
                  energy  =   ( reSq + imSq )
                              - reCorrelChirp[k] * ( reSq - imSq )
                              + imCorrelChirp[k] * (  2 * re*im  );
                  energy = cstCorrelChirp[k] * energy;
                }
              /* => Compensate for a possible numerical innacuracy
               *    (this case should never happen in practice) */
              if ( energy < 0 )
                {
                  mp_warning_msg( func, " A negative energy was met."
                                  " (energy = [%g])\nEnergy value is reset to 0.0 .", energy );
                  energy = 0.0;
                }

              /* Cast and fill mag */
              fftEnergy[k] += energy;
            }
        } /* <-- end loop on channels */


      /* Find the best frequency */
      energy = 0.0;
      if ( freqIdx > MP_FREQ_RELOC_RANGE ) fIdxMin = freqIdx - MP_FREQ_RELOC_RANGE;
      else                                 fIdxMin = 0;
      fIdxMax = freqIdx + MP_FREQ_RELOC_RANGE;
      if ( fIdxMax > numFreqs ) fIdxMax = numFreqs;
      for ( k = fIdxMin; k < fIdxMax; k++ )
        {
          /* for ( k = 0; k < numFreqs; k++ ) { */
          if ( fftEnergy[k] > energy)
            {
              energy = fftEnergy[k];
              freqIdx = k;
            }
        }

      /* TEST: if the correlation of the chirped atom is less than or equal to
         the correlation of the original unchirped one, it's a case where the chirp
         detection model is invalid => keep the unchirped one and exit. */
      if (energy <= maxIPValue )
        {
          mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                        "At iteration %2d in the chirp estimation: the chirping model ceases to apply."
                        " (Original IP: %g; Chirped IP: %g)"
                        " Returning an atom with the last valid chirprate.\n",
                        iter, maxIPValue, energy );
          chirprate = chirprateBefore;
          break;
        }

      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "iter %2d : New freqIdx = %lu.\n", iter, freqIdx );

    } /* end loop on iterations */


  /******************************************************/
  /* III) ESTIMATE ALL THE PARAMETERS OF THE FINAL ATOM */
  /******************************************************/

  gatom->chirp = chirprate;
  gatom->freq  = (double)(freqIdx) / (double)(fftSize);

  mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                "freqIdx is now = %lu , freq = %f (cplxSize = %g).\n",
                freqIdx, gatom->freq, fftSize );

  /* Compute the magnitude of the best real chirp atom for each frequency and each channel */
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ )
    {

      /* Compute the FFT of the demodulated signal */
      in = s->channel[chanIdx] + gatom->support[chanIdx].pos;
      fft->exec_complex_demod( in, chirpRe, chirpIm, fftRe, fftIm );

      re  = (double)( fftRe[freqIdx] );
      im  = (double)( fftIm[freqIdx] );
      energy = re*re + im*im;
      assert( sqCorrelChirp[freqIdx] <= 1.0 );
      /* Cf. explanations about complex2amp_and_phase() in general.h */
      if ( (freqIdx != 0) && ( (freqIdx+1) < numFreqs ) )
        {
          real = (1.0 - reCorrelChirp[freqIdx])*re + imCorrelChirp[freqIdx]*im;
          imag = (1.0 + reCorrelChirp[freqIdx])*im + imCorrelChirp[freqIdx]*re;
          amp   = 2.0 * sqrt( real*real + imag*imag );
          atomphase = atan2( imag, real ); /* the result is between -M_PI and MP_PI */
        }
      /* When the atom and its conjugate are aligned, they should be real
       * and the phase is simply the sign of the inner product (re,im) = (re,0) */
      else
        {
          assert( reCorrelChirp[freqIdx] == 1.0 );
          assert( imCorrelChirp[freqIdx] == 0.0 );
          assert( im == 0 );
          amp = sqrt( energy );
          if   ( re >= 0 ) atomphase = 0.0;  /* corresponds to the '+' sign */
          else             atomphase = MP_PI; /* corresponds to the '-' sign exp(i\pi) */
        }

      /* 5) fill in the atom parameters */
      gatom->amp[chanIdx]   = (MP_Real_t)( amp   );
      gatom->phase[chanIdx] = (MP_Real_t)( atomphase );
      mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
                    "freq %g chirp %g amp %g phase %g\n reCorrelChirp %g"
                    " imCorrelChirp %g\n re %g im %g 2*(re^2+im^2) %g\n",
                    gatom->freq, gatom->chirp, gatom->amp[chanIdx], gatom->phase[chanIdx],
                    reCorrelChirp[freqIdx], imCorrelChirp[freqIdx], re, im, 2*(re*re+im*im) );
    }

  /* Shall we also adjust the scale ? */

  return( 1 );
}

/******************************************************/
int MP_Chirp_Block_Plugin_c::set_chirp_demodulator( MP_Real_t chirprate )
{

  unsigned long int t;
  MP_Real_t argument;
  MP_Real_t C;

  /* Update the real and imaginary parts of exp(-i*pi*chirprate*t^2) */
  C = MP_PI*chirprate;
  for ( t = 0; t < filterLen; t++ )
    {
      argument = C*t*t;
      chirpRe[t] = cos( argument );
      chirpIm[t] = -sin( argument );
    }

  /* Compute the FFT of  window[t]*window[t]*exp(+2*i*pi*chirprate*t^2)
     (one multiplication by window is done within exec_complex) */
  for (t = 0; t < filterLen; t++)
    {
      sigChirpRe[t] =  (chirpRe[t]*chirpRe[t]-chirpIm[t]*chirpIm[t]);
      sigChirpIm[t] = -2*chirpRe[t]*chirpIm[t];
    }
  fft->exec_complex_demod( fft->window, sigChirpRe, sigChirpIm, fftRe, fftIm );

  /* 3/ Fill reCorrelChirp and imCorrelChirp with the adequate FFT values: */
  if ( fill_correl( reCorrelChirp, imCorrelChirp, sqCorrelChirp, cstCorrelChirp ) )
    {
      mp_warning_msg( "MP_Chirp_Block_c::set_chirp_demodulator(rate)",
                      "The tabulation of the atom's autocorrelations returned an error.\n" );
    }

  return( 0 );
}

/*********************************************/
/* get Paramater type map defining the block */
void MP_Chirp_Block_Plugin_c::get_parameters_type_map(map< string, string, mp_ltstring> * parameterMapType){
const char * func = "void MP_Gabor_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault )";
if ((*parameterMapType).empty()) {
(*parameterMapType)["type"] = "string";
(*parameterMapType)["windowLen"] = "ulong";
(*parameterMapType)["windowShift"] = "ulong";
(*parameterMapType)["fftSize"] = "ulong";
(*parameterMapType)["windowtype"] = "string";
(*parameterMapType)["windowopt"] = "real";
(*parameterMapType)["numFitPoints"] = "uint";
(*parameterMapType)["numIter"] = "uint";
(*parameterMapType)["blockOffset"] = "ulong";

} else  mp_error_msg( func, "Map for parameters type wasn't empty.\n" );

}

/***********************************/
/* get Info map defining the block */
void MP_Chirp_Block_Plugin_c::get_parameters_info_map(map< string, string, mp_ltstring> * parameterMapInfo ){

const char * func = "void MP_Gabor_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault )";
if ((*parameterMapInfo).empty()) {
(*parameterMapInfo)["type"] = "type: the type of blocks";
(*parameterMapInfo)["windowLen"] = "windowLen: the length of the signal window, in number of sample";
(*parameterMapInfo)["windowShift"] = "windowShift: the window shift, in number of samples";
(*parameterMapInfo)["fftSize"] = "fftSize: the size of the FFT, including zero padding";
(*parameterMapInfo)["windowtype"] = "windowType: the window type (see the doc of libdsp_windows.h)";
(*parameterMapInfo)["windowopt"] = "windowOption: the optional window parameter";
(*parameterMapInfo)["numFitPoints"] = "numFitPoints number of frequency points used on both sides of a local maximum to fit a chirp";
(*parameterMapInfo)["numIter"] = "numIter number of iteration done to fit a chirp";
(*parameterMapInfo)["blockOffset"] = "blockOffset: the block offset";

} else  mp_error_msg( func, "Map for parameters info wasn't empty.\n" );

}

/***********************************/
/* get default map defining the block */
void MP_Chirp_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault ){

const char * func = "void MP_Gabor_Block_Plugin_c::get_parameters_default_map( map< string, string, mp_ltstring>* parameterMapDefault )";

if ((*parameterMapDefault).empty()) {
(*parameterMapDefault)["type"] = "chirp";
(*parameterMapDefault)["windowLen"] = "1024";
(*parameterMapDefault)["windowShift"] = "512";
(*parameterMapDefault)["fftSize"] = "1024";
(*parameterMapDefault)["windowtype"] = "gauss";
(*parameterMapDefault)["windowopt"] = "0";
(*parameterMapDefault)["blockOffset"] = "0"; }

 else  mp_error_msg( func, "Map for parameter default wasn't empty.\n" );

}

