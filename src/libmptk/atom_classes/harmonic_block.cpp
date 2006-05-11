/******************************************************************************/
/*                                                                            */
/*                           harmonic_block.cpp                               */
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

/***************************************************************/
/*                                                             */
/* harmonic_block.cpp: methods for harmonic blocks */
/*                                                             */
/***************************************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Harmonic_Block_c* MP_Harmonic_Block_c::init( MP_Signal_c *setSignal, 
						const unsigned long int setFilterLen,
						const unsigned long int setFilterShift,
						const unsigned long int setFftSize,
						const unsigned char setWindowType,
						const double setWindowOption,
						const MP_Real_t setF0Min,
						const MP_Real_t setF0Max,
						const unsigned int  setMaxNumPartials ) {

  const char* func = "MP_Harmonic_Block_c::init()";
  MP_Harmonic_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Harmonic_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Harmonic block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterLen, setFilterShift, setFftSize,
				  setWindowType, setWindowOption,
				  setF0Min, setF0Max, setMaxNumPartials ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in"
		  " the new Harmonic block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Harmonic block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
}


/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Harmonic_Block_c::init_parameters( const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift,
					  const unsigned long int setFftSize,
					  const unsigned char setWindowType,
					  const double setWindowOption,
					  const MP_Real_t setF0Min,
					  const MP_Real_t setF0Max,
					  const unsigned int  setMaxNumPartials ) {

  const char* func = "MP_Harmonic_Block_c::init_parameters(...)";

  /* Go up the inheritance graph */
  if ( MP_Gabor_Block_c::init_parameters( setFilterLen, setFilterShift, setFftSize,
					  setWindowType, setWindowOption ) ) {
    mp_error_msg( func, "Failed to init the parameters at the Gabor block level"
		  " in the new Harmonic block.\n" );
    return( 1 );
  }

  /* Check the harmonic fields */
  if ( setF0Min < 0.0 ) {
    mp_error_msg( func, "f0Min [%.2f] is negative;"
		  " f0Min must be a positive frequency value"
		  " (in Hz).\n", setF0Min );
    return( 1 );
  }
  if ( setF0Max < setF0Min ) {
    mp_error_msg( func, "f0Max [%.2f] is smaller than f0Min [%.2f]."
		  " f0Max must be a positive frequency value (in Hz)"
		  " bigger than f0Min.\n", setF0Max, setF0Min );
    return( 1 );
  }

  /* Set the harmonic fields */
  f0Min = setF0Min;
  f0Max = setF0Max;
  if ( setMaxNumPartials == 0 ) {
    /* A maxNumPartials set to zero means: explore all the harmonics
       until the Nyquist frequency. */
    maxNumPartials = UINT_MAX;
  }
  else maxNumPartials = setMaxNumPartials;

  /* Allocate the sum array */
  if ( (sum = (double*) calloc( numFreqs , sizeof(double) )) == NULL ) {
    mp_error_msg( func, "Can't allocate an array of [%lu] double elements"
		  "for the sum array.\n", numFreqs );
    return( 1 );
  }
 
  return( 0 );
}


/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Harmonic_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Harmonic_Block_c::plug_signal( signal )";
  unsigned long int maxFundFreqIdx = 0;

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    /* Go up the inheritance graph */
    if ( MP_Gabor_Block_c::plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug a signal at the Gabor block level.\n" );
      nullify_signal();
      return( 1 );
    }

    /* Set and check the signal-related parameters: */
    
    /* - Turn the frequencies (in Hz) into fft bins */
    minFundFreqIdx = (unsigned long int)( floor( f0Min / ((double)(setSignal->sampleRate) / (double)(fftSize)) ) );
    maxFundFreqIdx = (unsigned long int)( floor( f0Max / ((double)(setSignal->sampleRate) / (double)(fftSize)) ) );
    
    /* - Check for going over the Nyquist frequency */
    if ( minFundFreqIdx >= numFreqs ) {
      mp_warning_msg( func, "f0Min [%.2f Hz] is above the signal's Nyquist frequency [%.2f Hz].\n" ,
		      f0Min, ( (double)(setSignal->sampleRate) / 2.0 ) );
      mp_info_msg( func, "For this signal, f0Min will be temporarily reduced to the signal's"
		   " Nyquist frequency.\n" );
      minFundFreqIdx = numFreqs - 1;
    }
    if ( maxFundFreqIdx > numFreqs ) maxFundFreqIdx = (numFreqs - 1); /* For f0Max, rectify silently. */
    /* - Check for going under the DC */
    if ( minFundFreqIdx == 0 ) {
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
void MP_Harmonic_Block_c::nullify_signal( void ) {

  MP_Gabor_Block_c::nullify_signal();

  /* Reset the frequency-related dimensions */
  minFundFreqIdx = 0;
  numFundFreqIdx = 0;
  numFilters = 0;

}


/********************/
/* NULL constructor */
MP_Harmonic_Block_c::MP_Harmonic_Block_c( void )
:MP_Gabor_Block_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Harmonic_Block_c::MP_Harmonic_Block_c()",
		"Constructing a Harmonic block...\n" );

  minFundFreqIdx = numFundFreqIdx = maxNumPartials = 0;

  sum = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Harmonic_Block_c::MP_Harmonic_Block_c()",
		"Done.\n" );

}


/**************/
/* Destructor */
MP_Harmonic_Block_c::~MP_Harmonic_Block_c() {

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
char* MP_Harmonic_Block_c::type_name() {
  return ("harmonic");
}

/**********************/
/* Readable text dump */
int MP_Harmonic_Block_c::info( FILE *fid ) {

  int nChar = 0;

  nChar += mp_info_msg( fid, "HARMONIC BLOCK",
			"%s window (window opt=%g) of length [%lu], shifted by [%lu] samples;\n",
			window_name( fft->windowType ), fft->windowOption,
			filterLen, filterShift );
  nChar += mp_info_msg( fid, "            |-",
			"projected on [%lu] frequencies"
			" and [%lu] fundamental frequencies for a total of [%lu] filters;\n",
			fft->numFreqs, numFundFreqIdx, numFilters );
  nChar += mp_info_msg( fid, "            |-",
			"fundamental frequency in the index range [%lu %lu]\n",
			minFundFreqIdx, minFundFreqIdx+numFundFreqIdx-1 );
  nChar += mp_info_msg( fid, "            |-",
			"                       (normalized range [%lg %lg]);\n",
			((double)minFundFreqIdx)/((double)fft->fftSize),
			((double)(minFundFreqIdx+numFundFreqIdx-1))/((double)fft->fftSize) );
  if ( s != NULL ) {
    nChar += mp_info_msg( fid, "            |-",
			  "                       (Hertz range      [%lg %lg]);\n",
			  (double)minFundFreqIdx * (double)s->sampleRate / (double)fft->fftSize,
			  (double)(minFundFreqIdx+numFundFreqIdx-1) * (double)s->sampleRate
			  / (double)fft->fftSize );
  }
  nChar += mp_info_msg( fid, "            |-",
			"                       (Original range   [%lg %lg]);\n",
			f0Min, f0Max );
  nChar += mp_info_msg( fid, "            |-",
			"maximum number of partials %u;\n",
			maxNumPartials );
  nChar += mp_info_msg( fid, "            O-",
			"The number of frames for this block is [%lu],"
			" the search tree has [%lu] levels.\n",
			numFrames, numLevels );
  return( nChar );
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Harmonic_Block_c::update_frame( unsigned long int frameIdx, 
				        MP_Real_t *maxCorr, 
				        unsigned long int *maxFilterIdx ) {

  unsigned long int inShift;

  MP_Sample_t *in;
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

  inShift = frameIdx*filterShift;
  
  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0, magPtr = mag;    /* <- for each channel */
	chanIdx < numChans;
	chanIdx++,   magPtr += numFreqs ) {
    
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
  max = local_sum; maxIdx = 0;
  /* -- Following GABOR atoms: */
  for ( freqIdx = 1; freqIdx < numFreqs; freqIdx++) {
    /* - make the sum */
    local_sum = (double)(mag[freqIdx]);               /* <- channel 0      */
    for ( chanIdx = 1, magPtr = mag+numFreqs+freqIdx; /* <- other channels */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += numFreqs ) local_sum += (double)(*magPtr);
    sum[freqIdx] = local_sum;
    /* - update the max */
    if ( local_sum > max ) { max = local_sum; maxIdx = freqIdx; }
  }
  /* -- Following HARMONIC elements: */
  for ( /* freqIdx same,*/ fundFreqIdx = minFundFreqIdx;
	freqIdx < numFilters;
	freqIdx++,         fundFreqIdx++) {
    /* Re-check the number of partials */
    numPartials = (numFreqs-1) / fundFreqIdx;
    if ( numPartials > maxNumPartials ) numPartials = maxNumPartials;
    /* - make the sum */
    local_sum = 0.0;
    for ( kPartial = 0, kFundFreqIdx = fundFreqIdx; 
	  kPartial < numPartials;
	  kPartial++,   kFundFreqIdx += fundFreqIdx ) {
      assert( kFundFreqIdx < numFreqs );
      local_sum += sum[kFundFreqIdx];
    }
    /* - update the max */
    if ( local_sum > max ) { max = local_sum; maxIdx = freqIdx; }
  }
  *maxCorr = (MP_Real_t)(max);
  *maxFilterIdx = maxIdx;
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Harmonic_Block_c::create_atom( MP_Atom_c **atom,
					       const unsigned long int frameIdx,
					       const unsigned long int filterIdx ) {
  
  const char* func = "MP_Harmonic_Block_c::create_atom(...)";

  /* --- Return a Gabor atom when it is what filterIdx indicates */
  if ( filterIdx < numFreqs ) return( MP_Gabor_Block_c::create_atom( atom, frameIdx, filterIdx ) );
  /* --- Otherwise create the Harmonic atom :  */
  else {

    MP_Harmonic_Atom_c *hatom = NULL;
    unsigned int kPartial, numPartials;
    /* Parameters for a new FFT run: */
    MP_Sample_t *in;
    unsigned long int fundFreqIdx, kFundFreqIdx;
    /* Parameters for the atom waveform : */
    double re, im;
    double amp, phase, gaborAmp = 1.0, gaborPhase = 0.0;
    double reCorr, imCorr, sqCorr;
    double real, imag, energy;
    /* Misc: */
    int chanIdx;
    unsigned long int pos = frameIdx*filterShift;


    /* Check the position */
    if ( (pos+filterLen) > s->numSamples ) {
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
    if ( (hatom = MP_Harmonic_Atom_c::init( s->numChans, fft->windowType,
					    fft->windowOption , numPartials )) == NULL ) {
      mp_error_msg( func, "Can't allocate a new Harmonic atom."
		    " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
    
    /* 1) set the fundamental frequency and chirp of the atom */
    hatom->freq  = (MP_Real_t)( (double)(fundFreqIdx) / (double)(fft->fftSize));
    hatom->chirp = (MP_Real_t)( 0.0 );     /* So far there is no chirprate */
    hatom->numSamples = pos + filterLen;

    /* For each channel: */
    for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
      
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
	    kPartial++,   kFundFreqIdx += fundFreqIdx ) {

	assert( kFundFreqIdx < numFreqs );

	re  = (double)( *(fftRe + kFundFreqIdx) ); 
	im  = (double)( *(fftIm + kFundFreqIdx) );
	energy = re*re + im*im;
	reCorr = reCorrel[kFundFreqIdx];
	imCorr = imCorrel[kFundFreqIdx];
	sqCorr = sqCorrel[kFundFreqIdx]; assert( sqCorr <= 1.0 );
	
	/* At the Nyquist frequency: */
	if ( kFundFreqIdx == (numFreqs-1) ) {
	  assert( reCorr == 1.0 );
	  assert( imCorr == 0.0 );
	  assert( im == 0 );
	  amp = sqrt( energy );
	  if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
	  else             phase = M_PI; /* corresponds to the '-' sign exp(i\pi) */
	}
	/* When the atom and its conjugate are aligned, they should be real 
	 * and the phase is simply the sign of the inner product (re,im) = (re,0) */
	else {
	  real = (1.0 - reCorr)*re + imCorr*im;
	  imag = (1.0 + reCorr)*im + imCorr*re;
	  amp   = 2.0 * sqrt( real*real + imag*imag );
	  phase = atan2( imag, real ); /* the result is between -M_PI and M_PI */
	}

	/* case of the first partial */
	if ( kPartial == 0 ) {
	  hatom->amp[chanIdx]   = gaborAmp   = (MP_Real_t)( amp   );
	  hatom->phase[chanIdx] = gaborPhase = (MP_Real_t)( phase );
	  hatom->partialAmp[chanIdx][kPartial]   = (MP_Real_t)(1.0);
	  hatom->partialPhase[chanIdx][kPartial] = (MP_Real_t)(0.0);
	} else {
	  hatom->partialAmp[chanIdx][kPartial]   = (MP_Real_t)( amp / gaborAmp   );
	  hatom->partialPhase[chanIdx][kPartial] = (MP_Real_t)( phase - gaborPhase );
	}

	mp_debug_msg( MP_DEBUG_CREATE_ATOM, func, "freq %g chirp %g partial %lu amp %g phase %g\n"
		      " reCorr %g imCorr %g\n re %g im %g 2*(re^2+im^2) %g\n",
		      hatom->freq, hatom->chirp, kPartial+1, amp, phase,
		      reCorr, imCorr, re, im, 2*(re*re+im*im) );

      } /* <--- end loop on partials */

    } /* <--- end loop on channels */
    *atom = hatom;
    return( 1 );
  } 
}


/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition of one harmonic block to a dictionnary */
int add_harmonic_block( MP_Dict_c *dict,
			const unsigned long int windowSize,
			const unsigned long int filterShift,
			const unsigned long int fftSize,
			const unsigned char windowType,
			const double windowOption,
			const MP_Real_t f0Min,
			const MP_Real_t f0Max,
			const unsigned int maxNumPartials) {

  const char* func = "add_harmonic_block(...)";
  MP_Harmonic_Block_c *newBlock;

  newBlock = MP_Harmonic_Block_c::init( dict->signal, windowSize, filterShift,
					fftSize, windowType, windowOption,
					f0Min, f0Max, maxNumPartials);
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    mp_error_msg( func, "Failed to initialize a new harmonic block to add"
		  " to the dictionnary.\n" );
    return( 0 );
  }

  return( 1 );
}


/*****************************************************/
/* Addition of several harmonic blocks to a dictionnary */
int add_harmonic_blocks( MP_Dict_c *dict,
			 const unsigned long int maxWindowSize,
			 const MP_Real_t timeDensity,
			 const MP_Real_t freqDensity, 
			 const unsigned char windowType,
			 const double windowOption,
			 const MP_Real_t f0Min,
			 const MP_Real_t f0Max,
			 const unsigned int  maxNumPartials) {

  unsigned long int windowSize;
  unsigned long int filterShift;
  unsigned long int fftSize;
  int nAddedBlocks = 0;

  assert(timeDensity >  0.0);
  assert(freqDensity >= 1.0);

  for ( windowSize = 4; windowSize <= maxWindowSize; windowSize <<= 1 ) {

    filterShift = (unsigned long int)round((MP_Real_t)windowSize/timeDensity);
    fftSize = (unsigned long int)round((MP_Real_t)(windowSize)*freqDensity);
    nAddedBlocks += add_harmonic_block( dict,
					windowSize, filterShift, fftSize, 
					windowType, windowOption,
					f0Min, f0Max, maxNumPartials );
  }

  return( nAddedBlocks );
}
