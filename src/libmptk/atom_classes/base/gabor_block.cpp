/******************************************************************************/
/*                                                                            */
/*                              gabor_block.cpp                               */
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

/*************************************************/
/*                                               */
/* gabor_block.cpp: methods for gabor blocks     */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Gabor_Block_c* MP_Gabor_Block_c::init( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift,
					  const unsigned long int setFftSize,
					  const unsigned char setWindowType,
					  const double setWindowOption ) {

  const char* func = "MP_Gabor_Block_c::init()";
  MP_Gabor_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Gabor_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Gabor block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterLen, setFilterShift, setFftSize,
				  setWindowType, setWindowOption ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Gabor block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Gabor block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
}


/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Gabor_Block_c::init_parameters( const unsigned long int setFilterLen,
				       const unsigned long int setFilterShift,
				       const unsigned long int setFftSize,
				       const unsigned char setWindowType,
				       const double setWindowOption ) {

  const char* func = "MP_Gabor_Block_c::init_parameters(...)";

  /* Check the validity of setFftSize */
  if ( is_odd(setFftSize) ) { /* If fftSize is odd: fftSize has to be even! */
    mp_error_msg( func, "fftSize [%lu] is odd: fftSize must be even.\n" ,
		  setFftSize );
    return( 1 );
  }
  if ( is_odd(setFilterLen) ) { /* If windowLEn is odd, fftSize must be >= windowLen+1 */
    if ( setFftSize < (setFilterLen+1) ) {
      mp_error_msg( func, "fftSize [%lu] must be bigger than windowLen+1"
		    " when windowLen [%lu] is odd.\n", setFftSize, setFilterLen );
      return( 1 );
    }
  }
  else { /* If windowLen is even, fftSize must be >= windowLen */
    if ( setFftSize < setFilterLen ) {
      mp_error_msg( func, "fftSize [%lu] must be bigger than windowLen"
		    " when windowLen [%lu] is even.\n", setFftSize, setFilterLen  );
      return( 1 );
    }
  }
  if ( !(window_type_is_ok(setWindowType)) ) {
    mp_error_msg( func, "Invalid window type.\n" );
    return( 1 );
  }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( setFilterLen, setFilterShift, (setFftSize >> 1) + 1 ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Gabor block.\n" );
    return( 1 );
  }

  /* Set the parameters */
  fftSize = setFftSize;
  numFreqs = (fftSize >> 1) + 1;

  /* Create the FFT object */
  fft = (MP_FFT_Interface_c*)MP_FFT_Interface_c::init( filterLen, setWindowType, setWindowOption,
						       fftSize );
  if ( fft == NULL ) {
    mp_error_msg( func, "Failed to init the FFT in the new Gabor block.\n" );
    fftSize = numFreqs = 0;
    return( 1 );
  }

  /* Allocate the complex fft buffers */
  if ( (fftRe = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the real part of the fft array.\n", numFreqs );
    fftSize = numFreqs = 0;
    delete( fft );
    return( 1 );
  }
  if ( (fftIm = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the imaginary part of the fft array.\n", numFreqs );
    fftSize = numFreqs = 0;
    free( fftRe );
    delete( fft );
    return( 1 );
  }

  /* Allocate the atom's autocorrelations */
  if ( alloc_correl( &reCorrel, &imCorrel, &sqCorrel, &cstCorrel ) ) {
    mp_error_msg( func, "Failed to allocate the block's autocorrelations.\n" );
    fftSize = numFreqs = 0;
    free( fftRe ); free( fftIm );
    delete( fft );
    return( 1 );
  }

  /* Compute the complex atoms FFT to prepare for correlation filling */
  fft->exec_complex( fft->window, fftRe, fftIm );

  /* Tabulate the atom's autocorrelations */
  if ( fill_correl( reCorrel, imCorrel, sqCorrel, cstCorrel ) ) {
    mp_error_msg( func, "Failed to tabulate the block's autocorrelations.\n" );
    fftSize = numFreqs = 0;
    free( fftRe ); free( fftIm );
    free( reCorrel ); free( imCorrel ); free( sqCorrel ); free( cstCorrel );
    delete( fft );
    return( 1 );
  }

  return( 0 );
}


/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Gabor_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_Gabor_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    /* Go up the inheritance graph */
    if ( MP_Block_c::plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }
    
    /* Allocate the mag array */
    if ( (mag = (MP_Real_t*) calloc( numFreqs*s->numChans , sizeof(MP_Real_t) )) == NULL ) {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
		    " for the mag array. Nullifying all the signal-related parameters.\n",
		    numFreqs*s->numChans );
      nullify_signal();
      return( 1 );
    }

  }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Gabor_Block_c::nullify_signal( void ) {

  MP_Block_c::nullify_signal();
  if ( mag ) { free( mag ); mag = NULL; }

}


/********************/
/* NULL constructor */
MP_Gabor_Block_c::MP_Gabor_Block_c( void )
  :MP_Block_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Gabor_Block_c::MP_Gabor_Block_c()",
		"Constructing a Gabor block...\n" );

  fft = NULL;
  mag = NULL;
  fftRe = fftIm = NULL;

  numFreqs = 0;

  reCorrel = imCorrel = NULL;
  sqCorrel = cstCorrel = NULL;

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_Gabor_Block_c::MP_Gabor_Block_c()",
		"Done.\n" );

}


/**************/
/* Destructor */
MP_Gabor_Block_c::~MP_Gabor_Block_c() {

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Gabor_Block_c::~MP_Gabor_Block_c()", "Deleting Gabor block...\n" );

  if ( fft ) delete( fft );

  if ( mag ) free( mag );

  if ( fftRe ) free( fftRe );
  if ( fftIm ) free( fftIm );

  if ( reCorrel  ) free( reCorrel  );
  if ( imCorrel  ) free( imCorrel  );
  if ( sqCorrel  ) free( sqCorrel  );
  if ( cstCorrel ) free( cstCorrel );

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Gabor_Block_c::~MP_Gabor_Block_c()", "Done.\n" );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char * MP_Gabor_Block_c::type_name() {
  return ("gabor");
}

/**********************/
/* Readable text dump */
int MP_Gabor_Block_c::info( FILE *fid ) {

  int nChar = 0;

  nChar += mp_info_msg( fid, "GABOR BLOCK", "%s window (window opt=%g)"
			" of length [%lu], shifted by [%lu] samples,\n",
			window_name( fft->windowType ), fft->windowOption,
			filterLen, filterShift );
  nChar += mp_info_msg( fid, "         |-", "projected on [%lu] frequencies;\n",
			numFilters );
  nChar += mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu], "
			"the search tree has [%lu] levels.\n", numFrames, numLevels );

  return( nChar );
}


/*****************************************/
/* Allocation of the correlation vectors */
int MP_Gabor_Block_c::alloc_correl( MP_Real_t **reCorr, MP_Real_t **imCorr,
				    MP_Real_t **sqCorr, MP_Real_t **cstCorr ) {

  const char* func = "MP_Gabor_Block_c::alloc_correl(...)";

  /* Allocate the memory for the correlations and init it to zero */
  *reCorr = *imCorr = *sqCorr = NULL;
  /* Reminder: ( fftCplxSize == ((numFreqs-1)<<1) ) <=> ( numFreqs == ((fftCplxSize>>1)+1) ) */
  if ( ( *reCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL) {
    mp_error_msg( func, "Can't allocate storage space for the real part"
		  " of the atom correlations. Correlations are left un-initialized.\n");
    return( 1 );
  }
  else if ( ( *imCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL) {
    mp_error_msg( func, "Can't allocate storage space for the imaginary part"
		  " of the atom correlations. Corrations are left un-initialized.\n");
    free( *reCorr ); *reCorr = NULL;
    return( 1 );
  }
  else if ( ( *sqCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL) {
    mp_error_msg( func, "Can't allocate storage space for the squared"
		  " atom correlations. Correlations are left un-initialized.\n");
    free( *reCorr ); *reCorr = NULL;
    free( *imCorr ); *imCorr = NULL;
    return( 1 );
  }
  else if ( ( *cstCorr = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL) {
    mp_error_msg( func, "Can't allocate storage space for the pre-computed"
		  " constant of the atom correlations. Correlations are left un-initialized.\n");
    free( *reCorr ); *reCorr = NULL;
    free( *imCorr ); *imCorr = NULL;
    free( *sqCorr ); *sqCorr = NULL;
    return( 1 );
  }

  return( 0 );
}


/******************************************************/
/** Fill the correlation arrays with 
 * \f$ (\mbox{reCorrel}[k],\mbox{imCorrel[k]}) =
 * \sum_{n=0}^{fftCplxSize-1} \mbox{window}^2[n] e^{2i\pi \frac{2kn}{fftCplxSize}} \f$ */
int MP_Gabor_Block_c::fill_correl( MP_Real_t *reCorr, MP_Real_t *imCorr,
				   MP_Real_t *sqCorr, MP_Real_t *cstCorr ) {

  const char* func = "MP_Gabor_Block_c::fill_correl(...)";
  double re,im,sq;
  int k, cursor;

  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );

  /* Fill reCorr and imCorr with the adequate FFT values: */
  for ( k = cursor = 0;  cursor < (int)(numFreqs);  k++, cursor += 2 ) {
    /* In this loop, cursor is always equal to 2*k. */
    re = fftRe[cursor];
    im = fftIm[cursor];
    *( reCorr + k ) = (MP_Real_t)(   re );
    *( imCorr + k ) = (MP_Real_t)( - im );
    sq = ( re*re + im*im );
    *( sqCorr + k ) = (MP_Real_t)(   sq );
    *( cstCorr + k ) = (MP_Real_t)( 2.0 / (1.0 - sq) );
    /* Rectify a possible numerical innacuracy at DC frequency: */
    if ( k==0 ) {
      *( reCorr + k )  = 1.0;
      *( imCorr + k )  = 0.0;
      *( sqCorr + k )  = 1.0;
      *( cstCorr + k ) = 1.0;
    }
    else {
      if ( (MP_Real_t)(sq) >= 1.0 ) {
	mp_warning_msg( func, "Atom's autocorrelation has value >= 1.0 [diff= %e ]\n"
			"\t\tfor frequency index %d (numFreqs in this block is %lu).\n",
			((MP_Real_t)(sq) - 1.0), k, numFreqs );
      }
    }
  }
  for ( cursor = ( fft->fftSize - cursor );  cursor >= 0 ;  k++, cursor -= 2 ) {
    /* In this loop, cursor is always equal to (fftSize - 2*k). */
    re = fftRe[cursor];
    im = fftIm[cursor];
    *( reCorr + k ) = (MP_Real_t)( re );
    *( imCorr + k ) = (MP_Real_t)( im );
    sq = ( re*re + im*im );
    *( sqCorr + k ) = (MP_Real_t)( sq );
    *( cstCorr + k ) = (MP_Real_t)( 2.0 / (1.0 - sq) );
    /* Rectify a possible numerical innacuracy at Nyquist frequency: */
    if ( k == ((int)(numFreqs)-1) ) {
      *( reCorr + k )  = 1.0;
      *( imCorr + k )  = 0.0;
      *( sqCorr + k )  = 1.0;
      *( cstCorr + k ) = 1.0;
    }
    else {
      if ( (MP_Real_t)(sq) >= 1.0 ) {
	mp_warning_msg( func, "Atom's autocorrelation has value >= 1.0 [diff= %e ]\n"
			"\t\tfor frequency index %d (numFreqs in this block is %lu).\n",
			((MP_Real_t)(sq) - 1.0), k, numFreqs );
      }
    }
  }

  return( 0 );
}


/*************************************/
/* Compute the accurate atom energy  */
void MP_Gabor_Block_c::compute_energy( MP_Real_t *in,
				       MP_Real_t *reCorr, MP_Real_t *imCorr,
				       MP_Real_t *sqCorr, MP_Real_t *cstCorr,
				       MP_Real_t *outMag ) {

  const char* func = "MP_Gabor_Block_c::compute_energy(...)";
  int i;
  double re, im, reSq, imSq, energy;
  double correlSq;

  /* Simple buffer check */
  assert( in  != NULL );
  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );
  assert( outMag != NULL );

  /* Execute the FFT */
  fft->exec_complex( in , fftRe, fftIm );

  /*****/
  /* Get the resulting magnitudes: */

  /* -- At frequency 0: */
  re = fftRe[0];
  *(outMag) = (MP_Real_t)( re * re );

  /* -- At a frequency between 0 and Nyquist: */
  for ( i = 1;  i < ((int)(numFreqs) - 1);  i++ ) {

    /* Get the complex values */
    re = fftRe[i];
    im = fftIm[i];
    reSq = ( re * re );
    imSq = ( im * im );

    /* Get the atom' autocorrelation: */
    correlSq = (double)(*(sqCorr+i));

    /* If the atom's autocorrelation is neglegible: */
    if ( correlSq < MP_ENERGY_EPSILON ) {
      energy = 2 * ( reSq + imSq );
    }
    /* Else, if the atom's autocorrelation is NOT neglegible: */
    else {
	energy  =   ( reSq + imSq )
	          - (double)(*(reCorr+i)) * ( reSq - imSq )
	          + (double)(*(imCorr+i)) * (  2 * re*im  );
      
	energy = (double)(*(cstCorr+i)) * energy;
	/* The following version appears to be slightly slower,
	   but that's not clear cut with our experiments: */
	/* energy = ( 2.0 / (1.0 - correlSq) ) * energy; */
      }

    /* => Compensate for a possible numerical innacuracy
     *    (this case should never happen in practice) */
    if ( energy < 0 ) {
      mp_warning_msg( func, "A negative energy was met."
		      " (energy = [%g])\nEnergy value is reset to 0.0 .",
		      energy );
      energy = 0.0;
    }

    /* Cast and fill mag */
    *(outMag+i) = (MP_Real_t)(energy);

  }

  /* -- At the Nyquist frequency: */
  re = fftRe[numFreqs-1];
  *(outMag+numFreqs-1) = (MP_Real_t)( re * re );

  /*****/

  return;
}


/********************************************/
/* Frame-based update of the inner products */
void MP_Gabor_Block_c::update_frame(unsigned long int frameIdx, 
				    MP_Real_t *maxCorr, 
				    unsigned long int *maxFilterIdx)
{
  unsigned long int inShift;
  unsigned long int i;

  MP_Sample_t *in;
  MP_Real_t *magPtr;

  double sum;
  double max;
  unsigned long int maxIdx;

  int chanIdx;
  int numChans;

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
  /* Make the sum and find the maxcorr: */
  /* --Element 0: */
  /* - make the sum */
  sum = (double)(*(mag));                     /* <- channel 0      */
  for ( chanIdx = 1, magPtr = mag+numFreqs; /* <- other channels */
	chanIdx < numChans;
	chanIdx++,   magPtr += numFreqs )   sum += (double)(*(magPtr));
  /* - init the max */
  max = sum; maxIdx = 0;
  /* -- Following elements: */
  for ( i = 1; i<numFreqs; i++) {
    /* - make the sum */
    sum = (double)(*(mag+i));                     /* <- channel 0      */
    for ( chanIdx = 1, magPtr = mag+numFreqs+i; /* <- other channels */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += numFreqs ) sum += (double)(*(magPtr));
    /* - update the max */
    if ( sum > max ) { max = sum; maxIdx = i; }
  }
  *maxCorr = (MP_Real_t)max;
  *maxFilterIdx = maxIdx;
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Gabor_Block_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int frameIdx,
					    const unsigned long int freqIdx ) {

  const char* func = "MP_Gabor_Block_c::create_atom(...)";
  MP_Gabor_Atom_c *gatom = NULL;
  /* Time-frequency location: */
  unsigned long int pos = frameIdx*filterShift;
  /* Parameters for a new FFT run: */
  MP_Sample_t *in;
  /* Parameters for the atom waveform : */
  double re, im;
  double amp, phase;
  double reCorr, imCorr, sqCorr;
  double real, imag, energy;
  /* Misc: */
  int chanIdx;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }

  /* Allocate the atom */
  *atom = NULL;
  if ( (gatom = MP_Gabor_Atom_c::init( s->numChans, fft->windowType, fft->windowOption )) == NULL ) {
    mp_error_msg( func, "Can't create a new Gabor atom in create_atom()."
		  " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* 1) set the frequency and chirp of the atom */
  gatom->freq  = (MP_Real_t)( (double)(freqIdx) / (double)(fft->fftSize) );
  gatom->chirp = (MP_Real_t)( 0.0 ); /* Gabor atoms from plain gabor blocks have zero chirprate */
  gatom->numSamples = pos + filterLen;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {

    /* 2) set the support of the atom */
    gatom->support[chanIdx].pos = pos;
    gatom->support[chanIdx].len = filterLen;
    gatom->totalChanLen += filterLen;

    /* 3) seek the right location in the signal */
    in  = s->channel[chanIdx] + pos;

    /* 4) recompute the inner product of the complex atom using the FFT */
    fft->exec_complex( in, fftRe, fftIm ); 
    re  = (double)( *(fftRe + freqIdx) ); 
    im  = (double)( *(fftIm + freqIdx) );

    /* 5) set the amplitude an phase */
    /* This is equivalent to: complex2amp_and_phase( re, im, reCorrel, imCorrel, &amp, &phase ); */
    energy = re*re + im*im;
    reCorr = reCorrel[freqIdx];
    imCorr = imCorrel[freqIdx];
    sqCorr = sqCorrel[freqIdx]; assert( sqCorr <= 1.0 );

    /* Cf. explanations about complex2amp_and_phase() in general.h */
    //if ( (freqIdx != 0) && ( (freqIdx+1) < numFreqs ) ) { /* CHECK WITH REMI */
    if ( (freqIdx != 0) && (freqIdx != (numFreqs-1)) ) {  
      real = (1.0 - reCorr)*re + imCorr*im;
      imag = (1.0 + reCorr)*im + imCorr*re;
      amp   = 2.0 * sqrt( real*real + imag*imag );
      phase = atan2( imag, real ); /* the result is between -M_PI and MP_PI */
    }
    /* When the atom and its conjugate are aligned, they should be real 
     * and the phase is simply the sign of the inner product (re,im) = (re,0) */
    else {
      assert( reCorr == 1.0 );
      assert( imCorr == 0.0 );
      assert( im == 0 );

      amp = sqrt( energy );
      if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
      else             phase = M_PI; /* corresponds to the '-' sign exp(i\pi) */
    }

    /* 5) fill in the atom parameters */
    gatom->amp[chanIdx]   = (MP_Real_t)( amp   );
    gatom->phase[chanIdx] = (MP_Real_t)( phase );

    mp_debug_msg( MP_DEBUG_CREATE_ATOM, func,
		  "freq %g chirp %g amp %g phase %g\n reCorr %g imCorr %g\n"
		  " re %g im %g 2*(re^2+im^2) %g\n",
		  gatom->freq, gatom->chirp, amp, phase, reCorr, imCorr,
		  re, im, 2*(re*re+im*im) );
  }

  *atom = gatom;

  return( 1 );

}


/*************/
/* FUNCTIONS */
/*************/

/************************************************/
/* Addition of one gabor block to a dictionnary */
int add_gabor_block( MP_Dict_c *dict,
		     const unsigned long int filterLen,
		     const unsigned long int filterShift,
		     const unsigned long int fftSize,
		     const unsigned char windowType,
		     const double windowOption ) {

  const char* func = "add_gabor_block(...)";
  MP_Gabor_Block_c *newBlock;

  newBlock = MP_Gabor_Block_c::init( dict->signal, filterLen, filterShift, fftSize,
				     windowType, windowOption );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    mp_error_msg( func, "Failed to initialize a new gabor block to add"
		  " to the dictionary.\n" );
    return( 0 );
  }

  return( 1 );
}


/*****************************************************/
/* Addition of several gabor blocks to a dictionnary */
int add_gabor_blocks( MP_Dict_c *dict,
		      const unsigned long int maxFilterLen,
		      const MP_Real_t timeDensity,
		      const MP_Real_t freqDensity, 
		      const unsigned char setWindowType,
		      const double setWindowOption ) {

  unsigned long int setFilterLen;
  unsigned long int setFilterShift;
  unsigned long int setFftSize;
  int nAddedBlocks = 0;

  assert(timeDensity > 0.0);
  assert(freqDensity > 0.0);

  for ( setFilterLen = 4; setFilterLen <= maxFilterLen; setFilterLen <<= 1 ) {

    setFilterShift = (unsigned long int)round((MP_Real_t)setFilterLen/timeDensity);
    setFftSize = (unsigned long int)round((MP_Real_t)(setFilterLen)*freqDensity);
    nAddedBlocks += add_gabor_block( dict,
				     setFilterLen, setFilterShift, setFftSize,
				     setWindowType, setWindowOption );
  }

  return(nAddedBlocks);
}
