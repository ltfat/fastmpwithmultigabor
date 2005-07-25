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
/* Specific constructor */
MP_Harmonic_Block_c::MP_Harmonic_Block_c( MP_Signal_c *setSignal, 
					  const unsigned long int setWindowSize,
					  const unsigned long int setFilterShift,
					  const unsigned long int setFftRealSize,
					  const unsigned char setWindowType,
					  const double setWindowOption,
					  const unsigned long int setMinFundFreqIdx,
					  const unsigned long int setMaxFundFreqIdx,
					  const unsigned int  setMaxNumPartials)
  /* Construct the Gabor part */
:MP_Gabor_Block_c( setSignal, setWindowSize, setFilterShift, setFftRealSize, setWindowType, setWindowOption ) {

  /* The minimum fundamental frequency must be strictly positive */
  assert( setMinFundFreqIdx >= 1 ); 
  /* The maximum fundamental frequency must be above the minimum one */
  assert( setMinFundFreqIdx <= setMaxFundFreqIdx );
  /* The maximum fundamental frequency must be within the Nyquist range */
  assert( setMaxFundFreqIdx < setFftRealSize );
  /* There must be at least two partials, otherwise this is simply a Gabor block */
  assert( setMaxNumPartials >= 2);

  /* Set the harmonic fields */
  minFundFreqIdx = setMinFundFreqIdx;
  numFundFreqIdx = setMaxFundFreqIdx-setMinFundFreqIdx+1;
  maxNumPartials = setMaxNumPartials;
  numFilters    += numFundFreqIdx; /* This does numFilters = fftRealSize + numFundFreqIdx 
				    * which is coherent with the fact that we have fftRealSize
				    * plain Gabor atoms, plus numFundFreqIdx harmonic subspaces
				    */

   /* Allocate the sum array */
  if ( (sum = (MP_Real_t*) malloc( setFftRealSize*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Block_c() - Can't allocate an array of [%lu] MP_Real_t elements"
	     "for the sum array. This pointer will remain NULL.\n", setFftRealSize );
  }
  else { unsigned long int i; for ( i=0; i<setFftRealSize; i++ ) *(sum+i) = 0.0; }
 
}

/**************/
/* Destructor */
MP_Harmonic_Block_c::~MP_Harmonic_Block_c() {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Harmonic_Block_c() - Deleting harmonic_block.\n" );
#endif
  if (sum) free(sum);
#ifndef NDEBUG
  fprintf( stderr, "done.\n" );
#endif
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

  nChar += fprintf( fid, "mplib info -- HARMONIC BLOCK: %s window (window opt=%g)", window_name( fft->windowType ), fft->windowOption );
  nChar += fprintf( fid, " of length [%lu], shifted by [%lu] samples, projected on [%lu] frequencies"
		    " and [%lu] fundamental frequencies for a total of [%lu] filters;\n",
		    filterLen, filterShift, fft->fftRealSize, numFundFreqIdx,numFilters );
  nChar += fprintf( fid, " mplib info -- fundamental frequency in the range [%lg %lg] ([%lu %lu]);\n",
		    ((double)minFundFreqIdx)/((double)fft->fftCplxSize),
		    ((double)(minFundFreqIdx+numFundFreqIdx-1))/((double)fft->fftCplxSize),
		    minFundFreqIdx,minFundFreqIdx+numFundFreqIdx-1);
  nChar += fprintf( fid, " mplib info -- maximum number of partials %u;\n",
		    maxNumPartials);
  nChar += fprintf( fid, "mplib info -- The number of frames for this block is [%lu], the search tree has [%lu] levels.\n",
		    numFrames, numLevels );
  return( nChar );
}

/****************************************/
/* Partial update of the inner products */
MP_Support_t MP_Harmonic_Block_c::update_ip( const MP_Support_t *touch ) {
  /* NOTE: if touch == NULL  update all the inner products. */

  unsigned long int fromFrame; /* first frameIdx to be touched, included */
  unsigned long int toFrame;   /* last  frameIdx to be touched, INCLUDED */
  unsigned long int tmpFromFrame, tmpToFrame;
  unsigned long int fromSample;
  unsigned long int toSample;

  int chanIdx;
  int numChans;
  unsigned long int frameIdx;
  unsigned long int inShift;
  unsigned long int freqIdx,fundFreqIdx,numPartials,kFundFreqIdx,k;

  MP_Sample_t *in;
  MP_Real_t *magPtr;
  unsigned long int fftRealSize = fft->fftRealSize;

  double channel_sum,harmonic_sum;
  double max;
  unsigned long int maxIdx;

  MP_Support_t frameSupport;

  assert( s != NULL );
  numChans = s->numChans;

  assert( mag != NULL );

  /*---------------------------*/
  /* The following computes the interval [fromFrame,toFrame] where
     the frames need a FFT+max update.
     
     WARNING: toFrame is INCLUDED. See the LOOP below.

     THIS IS CRITICAL CODE. MODIFY WITH CARE.
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
    
    inShift = frameIdx*filterShift;

    /*----*/
    /* Fill the mag array: */
    for ( chanIdx = 0, magPtr = mag;    /* <- for each channel */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += fftRealSize ) {
      
      assert( s->channel[chanIdx] != NULL );
      
      /* Hook the signal and the inner products to the fft */
      in  = s->channel[chanIdx] + inShift;
      
      /* Execute the FFT (including windowing, conversion to energy etc.) */
      fft->exec_energy( in, magPtr );
      
    } /* end foreach channel */
    /*----*/

    /*----*/
    /* Fill the sum array and find the max over gabor atoms: */
    /* --Gabor atom at freqIdx =  0: */
    /* - make the sum over channels */
    channel_sum = *mag;                                 /* <- channel 0      */
    for ( chanIdx = 1, magPtr = mag+fftRealSize; /* <- other channels */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += fftRealSize )   channel_sum += *magPtr;
    *sum = channel_sum;
    /* - init the max */
    max = channel_sum; maxIdx = 0;
    /* -- Following GABOR atoms: */
    for ( freqIdx = 1; freqIdx<fftRealSize; freqIdx++) {
      /* - make the sum */
      channel_sum = *(mag+freqIdx);                        /* <- channel 0      */
      for ( chanIdx = 1, magPtr = mag+fftRealSize+freqIdx; /* <- other channels */
	    chanIdx < numChans;
	    chanIdx++,   magPtr += fftRealSize ) channel_sum += *magPtr;
      *(sum+freqIdx) = channel_sum;
      /* - update the max */
      if ( channel_sum > max ) { max = channel_sum; maxIdx = freqIdx; }
    }
    /* -- Following HARMONIC elements: */
    for (fundFreqIdx=minFundFreqIdx; freqIdx<numFilters; freqIdx++,fundFreqIdx++) {
      /* - determine how many partials there are for the current fundFreqIdx */
      if      ( maxNumPartials*fundFreqIdx < fftRealSize)  numPartials  = maxNumPartials;
      else if ( maxNumPartials*fundFreqIdx > fftRealSize)  numPartials  = fftRealSize/fundFreqIdx;
      else	                                           numPartials  = (fftRealSize/fundFreqIdx)-1;
      /* - make the sum */
      harmonic_sum = 0.0;
      for ( k = 0, kFundFreqIdx=fundFreqIdx; 
	    k < numPartials; 
	    k++, kFundFreqIdx+=fundFreqIdx) harmonic_sum += *(sum+kFundFreqIdx);
      /* - update the max */
      if ( harmonic_sum > max ) { max = harmonic_sum; maxIdx = freqIdx; }
    }

    /* Register the max value for the current frame */
    *(maxIPValueInFrame + frameIdx) = (MP_Real_t)max;
    *(maxIPIdxInFrame   + frameIdx) = maxIdx;

#ifndef NDEBUG
    fprintf( stderr, "mplib DEBUG -- update_ip() - in frame %lu, max is %g at position %lu.\n",
	     frameIdx, max, maxIdx );
#endif

  } /* end foreach frame */
  

  /* Return a mono-channel support */
  frameSupport.pos = fromFrame;
  frameSupport.len = toFrame - fromFrame + 1;

  return( frameSupport );
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Harmonic_Block_c::create_atom( MP_Atom_c **atom,
					       const unsigned long int atomIdx ) {
  
  /* Time-frequency location: */
  unsigned long int frameIdx;
  unsigned long int freqIdx;
  unsigned long int fftRealSize = fft->fftRealSize;

  /* Locate the atom in the Gabor spectrogram */
  frameIdx = atomIdx / numFilters;
  freqIdx  = atomIdx % numFilters;

  /* --- Return a Gabor atom when it is what atomIdx indicates */
  if (freqIdx<fftRealSize) return(MP_Gabor_Block_c::create_atom(atom,atomIdx));

  /* --- Otherwise create the Harmonic atom :  */
  else {
    MP_Harmonic_Atom_c *hatom = NULL;
    unsigned int k,numPartials;
    /* Parameters for a new FFT run: */
    MP_Sample_t *in;
    MP_Real_t fftRe[fftRealSize];
    MP_Real_t fftIm[fftRealSize];
    unsigned long int fundFreqIdx, kFundFreqIdx;
    /* Parameters for the atom waveform : */
    double re, im;
    double amp, phase, gaborAmp=1.0, gaborPhase=0.0;
    double reCorrel, imCorrel, sqCorrel;
    double real, imag, energy;
    /* Misc: */
    int chanIdx;
    
    /* Compute the fundamental frequency and the number of partials */
    fundFreqIdx = freqIdx-fftRealSize + minFundFreqIdx;
    if ( maxNumPartials*fundFreqIdx < fftRealSize)        numPartials  = maxNumPartials;
    else  if ( maxNumPartials*fundFreqIdx > fftRealSize)  numPartials  = fftRealSize/fundFreqIdx;
    else                                                  numPartials  = (fftRealSize/fundFreqIdx)-1;
    
    /* Allocate the atom */
    *atom = NULL;
    if ( (hatom = new MP_Harmonic_Atom_c( s->numChans, fft->windowType, fft->windowOption , numPartials )) == NULL ) {
      fprintf( stderr, "mplib error -- MP_Harmonic_Block_c::create_atom() - Can't allocate a new Harmonic atom in create_atom()."
	       " Returning NULL as the atom reference.\n" );
      return( 0 );
    }
    
    /* 1) set the fundamental frequency and chirp of the atom */
    hatom->freq  = (MP_Real_t)( (double)(fundFreqIdx) / (double)(fft->fftCplxSize));
    hatom->chirp = (MP_Real_t)( 0.0 );     /* So far there is no chirprate */
      
    /* For each channel: */
    for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {
      
      /* 2) set the support of the atom */
      hatom->support[chanIdx].pos = frameIdx*filterShift; 
      hatom->support[chanIdx].len = filterLen;
      hatom->totalChanLen += filterLen;
      
      /* 3) seek the right location in the signal */
      in  = s->channel[chanIdx] + hatom->support[chanIdx].pos;
      
      /* 4) recompute the inner product of the complex Gabor atoms 
       * corresponding to the partials, using the FFT */
      fft->exec_complex( in, fftRe, fftIm ); 
      
      /* 5) set the amplitude an phase for each partial */
      for (k = 0, kFundFreqIdx=fundFreqIdx; 
	   k < numPartials; 
	   k++,kFundFreqIdx+=fundFreqIdx) {
	re  = (double)( *(fftRe + kFundFreqIdx) ); 
	im  = (double)( *(fftIm + kFundFreqIdx) );
	energy = re*re + im*im;
	reCorrel = fft->reCorrel[kFundFreqIdx];
	imCorrel = fft->imCorrel[kFundFreqIdx];
	sqCorrel = fft->sqCorrel[kFundFreqIdx];
#ifndef NDEBUG
	assert( sqCorrel <= 1.0 );
#endif
	/* Cf. explanations about complex2amp_and_phase() in general.h */
	if ( (kFundFreqIdx+1) < fftRealSize ) {  
	  real = (1.0 - reCorrel)*re + imCorrel*im;
	  imag = (1.0 + reCorrel)*im + imCorrel*re;
	  amp   = 2.0 * sqrt( real*real + imag*imag );
	  phase = atan2( imag, real ); /* the result is between -M_PI and M_PI */
	}
	/* When the atom and its conjugate are aligned, they should be real 
	 * and the phase is simply the sign of the inner product (re,im) = (re,0) */
	else {
#ifndef NDEBUG
	  assert( reCorrel == 1.0 );
	  assert( imCorrel == 0.0 );
	  assert( im == 0 );
#endif
	  amp = sqrt( energy );
	  if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
	  else             phase = M_PI; /* corresponds to the '-' sign exp(i\pi) */
	}

	/* case of the first partial */
	if ( k==0 ) {
	  gaborAmp   = hatom->amp[chanIdx]   = (MP_Real_t)( amp   );
	  gaborPhase = hatom->phase[chanIdx] = (MP_Real_t)( phase );
	  hatom->partialAmp[chanIdx][k]   = (MP_Real_t)(1.0);
	  hatom->partialPhase[chanIdx][k] = (MP_Real_t)(0.0);
	} else {
	  hatom->partialAmp[chanIdx][k]   = (MP_Real_t)( amp/gaborAmp   );
	  hatom->partialPhase[chanIdx][k] = (MP_Real_t)( phase-gaborPhase );
	}
#ifndef NDEBUG
	fprintf( stderr, "mplib DEBUG -- freq %g chirp %g partial %lu amp %g phase %g\n reCorrel %g imCorrel %g\n re %g im %g 2*(re^2+im^2) %g\n",
		 hatom->freq, hatom->chirp, k+1, amp, phase, reCorrel, imCorrel, re, im, 2*(re*re+im*im) );
#endif
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
			const unsigned long int fftRealSize,
			const unsigned char windowType,
			const double windowOption,
			const MP_Real_t minFundFreq,
			const MP_Real_t maxFundFreq,
			const unsigned int  maxNumPartials) {

  unsigned long int minFundFreqIdx;
  unsigned long int maxFundFreqIdx;
  MP_Harmonic_Block_c *newBlock;

  /* Convert the frequency range into frequency indexes */
  minFundFreqIdx = (unsigned long int) ceil((MP_Real_t)(minFundFreq*2*(fftRealSize-1)));
  maxFundFreqIdx = (unsigned long int)floor((MP_Real_t)(maxFundFreq*2*(fftRealSize-1)));

  /* Simple cases where no block can be added */
  if ( maxNumPartials <= 1 ) {
    fprintf( stderr, "mplib error -- add_harmonic_block() - Can't add a new harmonic block to a dictionnary, maxNumPartials [%u] is too small (should be at least 2).",maxNumPartials );
    return ( 0 );
  }
  if ( windowSize > 2*(fftRealSize-1) ) {
    fprintf( stderr, "mplib error -- add_harmonic_block() - Can't add a new harmonic block to a dictionnary, windowSize [%lu] is too big for FFT complex size [%lu].",windowSize,2*(fftRealSize-1) );
    return ( 0 );
  }
  if ( maxFundFreqIdx < minFundFreqIdx) {
    fprintf( stderr, "mplib error -- add_harmonic_block() - Can't add a new harmonic block to a dictionnary, fundamental frequency range [%lu %lu] is empty.", minFundFreqIdx, maxFundFreqIdx );
    return ( 0 );
  }

  newBlock = new MP_Harmonic_Block_c( dict->signal, windowSize, filterShift, fftRealSize, windowType, windowOption, minFundFreqIdx, maxFundFreqIdx, maxNumPartials);
  if ( newBlock != NULL ) { /* \todo I am afraid this does not work and should be removed! */
    dict->add_block( newBlock );
  }
  else {
    fprintf( stderr, "mplib error -- add_harmonic_block() - Can't add a new harmonic block to a dictionnary." );
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
			 const MP_Real_t minFundFreq,
			 const MP_Real_t maxFundFreq,
			 const unsigned int  maxNumPartials) {

  unsigned long int windowSize;
  unsigned long int filterShift;
  unsigned long int fftRealSize;
  int nAddedBlocks = 0;

#ifndef NDEBUG
  assert(timeDensity >  0.0);
  assert(freqDensity >= 1.0);
#endif

  for ( windowSize = 4; windowSize <= maxWindowSize; windowSize <<= 1 ) {

    filterShift = (unsigned long int)round((MP_Real_t)windowSize/timeDensity);
    fftRealSize = (unsigned long int)round((MP_Real_t)(windowSize/2+1)*freqDensity);
    nAddedBlocks += add_harmonic_block( dict,
					windowSize, filterShift, fftRealSize, 
					windowType, windowOption,
					minFundFreq,maxFundFreq,maxNumPartials);
  }

  return(nAddedBlocks);
}
