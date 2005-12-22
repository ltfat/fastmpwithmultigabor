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
/* Specific constructor */
MP_Gabor_Block_c::MP_Gabor_Block_c( MP_Signal_c *setSignal,
				    const unsigned long int setFilterLen,
				    const unsigned long int setFilterShift,
				    const unsigned long int setFftRealSize,
				    const unsigned char setWindowType,
				    const double setWindowOption )
  /* Create the block structure with numFilters = fftRealSize */
:MP_Block_c( setSignal, setFilterLen, setFilterShift, setFftRealSize ) {
  
  /* Create the FFT object */
  fft = (MP_FFT_Interface_c*)MP_FFT_Interface_c::init( filterLen, setWindowType, setWindowOption,
						       setFftRealSize );

  /* Allocate the mag array */
  if ( (mag = (MP_Real_t*) malloc( setFftRealSize*s->numChans*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Block_c() - Can't allocate an array of [%lu] MP_Real_t elements"
	     " for the mag array. This pointer will remain NULL.\n", setFftRealSize*s->numChans );
  }
  else { unsigned long int i; for ( i=0; i<(setFftRealSize*s->numChans); i++ ) *(mag+i) = 0.0; }

  /* Allocate the complex fft buffers */
  if ( (fftRe = (MP_Real_t*) malloc( setFftRealSize*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Block_c() - Can't allocate an array of [%lu] MP_Real_t elements"
	     " for the real part of the fft array. This pointer will remain NULL.\n", setFftRealSize );
  }
  if ( (fftIm = (MP_Real_t*) malloc( setFftRealSize*sizeof(MP_Real_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Block_c() - Can't allocate an array of [%lu] MP_Real_t elements"
	     " for the imaginary part of the fft array. This pointer will remain NULL.\n", setFftRealSize );
  }

  /* Set the fftRealSize */
  fftRealSize = setFftRealSize;

  /* Allocate the atom's autocorrelations */
  if ( alloc_correl( &reCorrel, &imCorrel, &sqCorrel, &cstCorrel ) ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Block() - "
	     "The allocation of the atom's autocorrelations returned an error.\n");
  }

  /* Compute the complex atoms FFT to prepare for correlation filling */
  fft->exec_complex( fft->window, fftRe, fftIm );

  /* Tabulate the atom's autocorrelations */
  if ( fill_correl( reCorrel, imCorrel, sqCorrel, cstCorrel ) ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Block() - "
	     "The tabulation of the atom's autocorrelations returned an error.\n" );
  }

}


/**************/
/* Destructor */
MP_Gabor_Block_c::~MP_Gabor_Block_c() {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Gabor_Block_c() - Deleting gabor_block..." );
#endif

  delete fft;

  if ( mag ) free( mag );

  if ( fftRe ) free( fftRe );
  if ( fftIm ) free( fftIm );

  if ( reCorrel  ) free( reCorrel  );
  if ( imCorrel  ) free( imCorrel  );
  if ( sqCorrel  ) free( sqCorrel  );
  if ( cstCorrel ) free( cstCorrel );

#ifndef NDEBUG
  fprintf( stderr, "Done.\n" );
#endif

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

  nChar += fprintf( fid, "mplib info -- GABOR BLOCK: %s window (window opt=%g)",
		    window_name( fft->windowType ), fft->windowOption );
  nChar += fprintf( fid, " of length [%lu], shifted by [%lu] samples, projected on [%lu] frequencies;\n",
		    filterLen, filterShift, numFilters );
  nChar += fprintf( fid, "mplib info -- The number of frames for this block is [%lu], "
		    "the search tree has [%lu] levels.\n", numFrames, numLevels );
  return( nChar );
}


/*****************************************/
/* Allocation of the correlation vectors */
int MP_Gabor_Block_c::alloc_correl( MP_Real_t **reCorr, MP_Real_t **imCorr,
				    MP_Real_t **sqCorr, MP_Real_t **cstCorr ) {
 
  /* Allocate the memory for the correlations and init it to zero */
  *reCorr = *imCorr = *sqCorr = NULL;
  /* Reminder: ( fftCplxSize == ((fftRealSize-1)<<1) ) <=> ( fftRealSize == ((fftCplxSize>>1)+1) ) */
  if ( ( *reCorr = (MP_Real_t *) calloc( fftRealSize , sizeof(MP_Real_t)) ) == NULL) {
    fprintf( stderr, "mplib warning -- alloc_correl() - Can't allocate storage space for the real part"
	     " of the atom correlations. Correlations are left un-initialized.\n");
    return( 1 );
  }
  else if ( ( *imCorr = (MP_Real_t *) calloc( fftRealSize , sizeof(MP_Real_t)) ) == NULL) {
    fprintf( stderr, "mplib warning -- alloc_correl() - Can't allocate storage space for the imaginary part"
	     " of the atom correlations. Corrations are left un-initialized.\n");
    free( *reCorr ); *reCorr = NULL;
    return( 1 );
  }
  else if ( ( *sqCorr = (MP_Real_t *) calloc( fftRealSize , sizeof(MP_Real_t)) ) == NULL) {
    fprintf( stderr, "mplib warning -- alloc_correl() - Can't allocate storage space for the squared"
	     " atom correlations. Correlations are left un-initialized.\n");
    free( *reCorr ); *reCorr = NULL;
    free( *imCorr ); *imCorr = NULL;
    return( 1 );
  }
  else if ( ( *cstCorr = (MP_Real_t *) calloc( fftRealSize , sizeof(MP_Real_t)) ) == NULL) {
    fprintf( stderr, "mplib warning -- alloc_correl() - Can't allocate storage space for the pre-computed"
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

  double re,im,sq;
  int k, cursor;

  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );

  /* Fill reCorr and imCorr with the adequate FFT values: */
  for ( k = cursor = 0;  cursor < (int)(fftRealSize);  k++, cursor += 2 ) {
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
	fprintf( stderr, "mplib warning -- fill_correl() - atom's autocorrelation has value >= 1.0 [diff= %e ]\n"
		 "\t\tfor frequency index %d (fftRealSize in this block is %lu).\n",
		 ((MP_Real_t)(sq) - 1.0), k, fftRealSize );
      }
    }
  }
  for ( cursor = ( fft->fftCplxSize - cursor );  cursor >= 0 ;  k++, cursor -= 2 ) {
    /* In this loop, cursor is always equal to (fftCplxSize - 2*k). */
    re = fftRe[cursor];
    im = fftIm[cursor];
    *( reCorr + k ) = (MP_Real_t)( re );
    *( imCorr + k ) = (MP_Real_t)( im );
    sq = ( re*re + im*im );
    *( sqCorr + k ) = (MP_Real_t)( sq );
    *( cstCorr + k ) = (MP_Real_t)( 2.0 / (1.0 - sq) );
    /* Rectify a possible numerical innacuracy at Nyquist frequency: */
    if ( k == ((int)(fftRealSize)-1) ) {
      *( reCorr + k )  = 1.0;
      *( imCorr + k )  = 0.0;
      *( sqCorr + k )  = 1.0;
      *( cstCorr + k ) = 1.0;
    }
    else {
      if ( (MP_Real_t)(sq) >= 1.0 ) {
	fprintf( stderr, "mplib warning -- fill_correl() - atom's autocorrelation has value >= 1.0 [diff= %e ]\n"
		 "\t\tfor frequency index %d (fftRealSize in this block is %lu).\n",
		 ((MP_Real_t)(sq) - 1.0), k, fftRealSize );
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
  for ( i = 1;  i < ((int)(fftRealSize) - 1);  i++ ) {

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
      fprintf( stderr, "mplib warning -- exec_energy() - A negative energy was met."
	       " (energy = [%g])\nEnergy value is reset to 0.0 .", energy );
      energy = 0.0;
    }

    /* Cast and fill mag */
    *(outMag+i) = (MP_Real_t)(energy);

  }

  /* -- At the Nyquist frequency: */
  re = fftRe[fftRealSize-1];
  *(outMag+fftRealSize-1) = (MP_Real_t)( re * re );

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
	chanIdx++,   magPtr += fftRealSize ) {
    
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
  for ( chanIdx = 1, magPtr = mag+fftRealSize; /* <- other channels */
	chanIdx < numChans;
	chanIdx++,   magPtr += fftRealSize )   sum += (double)(*(magPtr));
  /* - init the max */
  max = sum; maxIdx = 0;
  /* -- Following elements: */
  for ( i = 1; i<fftRealSize; i++) {
    /* - make the sum */
    sum = (double)(*(mag+i));                     /* <- channel 0      */
    for ( chanIdx = 1, magPtr = mag+fftRealSize+i; /* <- other channels */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += fftRealSize ) sum += (double)(*(magPtr));
    /* - update the max */
    if ( sum > max ) { max = sum; maxIdx = i; }
  }
  *maxCorr = (MP_Real_t)max;
  *maxFilterIdx = maxIdx;
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Gabor_Block_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int atomIdx ) {

  MP_Gabor_Atom_c *gatom = NULL;
  /* Time-frequency location: */
  unsigned long int pos;
  unsigned long int frameIdx;
  unsigned long int freqIdx;
  /* Parameters for a new FFT run: */
  MP_Sample_t *in;
  /* Parameters for the atom waveform : */
  double re, im;
  double amp, phase;
  double reCorr, imCorr, sqCorr;
  double real, imag, energy;
  /* Misc: */
  int chanIdx;

  /* Allocate the atom */
  *atom = NULL;
  if ( (gatom = new MP_Gabor_Atom_c( s->numChans, fft->windowType, fft->windowOption )) == NULL ) {
    fprintf( stderr, "mplib error -- MP_Gabor_Block_c::create_atom() - "
	     "Can't create a new Gabor atom in create_atom()."
	     " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* Locate the atom in the spectrogram */
  frameIdx = atomIdx / numFilters;
  freqIdx  = atomIdx % numFilters;
  pos = frameIdx*filterShift;

  /* 1) set the frequency and chirp of the atom */
  gatom->freq  = (MP_Real_t)( (double)(freqIdx) / (double)(fft->fftCplxSize) ); /* CHECK WITH REMI */
  gatom->chirp = (MP_Real_t)( 0.0 ); /* Gabor atoms from plain gabor blocks have zero chirprate */

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
    //if ( (freqIdx != 0) && ( (freqIdx+1) < fftRealSize ) ) { /* CHECK WITH REMI */
    if ( (freqIdx != 0) && (freqIdx != (fftRealSize-1)) ) {  
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
#ifndef NDEBUG
    fprintf( stderr, "mplib DEBUG -- freq %g chirp %g amp %g phase %g\n reCorr %g imCorr %g\n re %g im %g 2*(re^2+im^2) %g\n",
	     gatom->freq, gatom->chirp, amp, phase, reCorr, imCorr, re, im, 2*(re*re+im*im) );
#endif
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
		     const unsigned long int fftRealSize,
		     const unsigned char windowType,
		     const double windowOption ) {

  MP_Gabor_Block_c *newBlock;

  
  if( 2*(fftRealSize-1) < filterLen) {
    fprintf( stderr, "mplib error -- add_gabor_block() - fftRealSize %lu is too small"
	     " since window size is %lu.\n", fftRealSize, filterLen);
    return( 0 );
  }

 
  newBlock = new MP_Gabor_Block_c( dict->signal, filterLen, filterShift, fftRealSize,
				   windowType, windowOption );
  if ( newBlock != NULL ) {
    dict->add_block( newBlock );
  }
  else {
    fprintf( stderr, "mplib error -- add_gabor_block() - Can't add a new gabor block to a dictionnary.\n" );
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
  unsigned long int setFftRealSize;
  int nAddedBlocks = 0;

#ifndef NDEBUG
  assert(timeDensity > 0.0);
  assert(freqDensity > 0.0);
#endif

  for ( setFilterLen = 4; setFilterLen <= maxFilterLen; setFilterLen <<= 1 ) {

    setFilterShift = (unsigned long int)round((MP_Real_t)setFilterLen/timeDensity);
    setFftRealSize = (unsigned long int)round((MP_Real_t)(setFilterLen/2+1)*freqDensity);
    nAddedBlocks += add_gabor_block( dict,
				     setFilterLen, setFilterShift, setFftRealSize, setWindowType, setWindowOption );
  }

  return(nAddedBlocks);
}
