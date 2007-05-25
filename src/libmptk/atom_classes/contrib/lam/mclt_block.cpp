/******************************************************************************/
/*                                                                            */
/*                         mclt_block.cpp      		                      */
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

/****************************************************************/
/*                                               		*/
/* mclt_block.cpp: methods for mclt blocks			*/
/*                                               		*/
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***********************************************/
/* Factory function  for a strict mclt block   */
MP_Mclt_Block_c* MP_Mclt_Block_c::init( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned char setWindowType,
					  const double setWindowOption,
                                          const unsigned long int setBlockOffset ) {
  const char* func = "MP_Mclt_Block_c::init()";

  /* Parameters for a strict MCLT */
  const unsigned long int setFilterShift = setFilterLen / 2;
  const unsigned long int setFftSize = setFilterLen;
  char* name = window_name(setWindowType);
  if ( strcmp(name,"rectangle") & strcmp(name,"cosine") & strcmp(name,"kbd") ) {
  	mp_error_msg( func, "Wrong window type. It has to be: rectangle, cosine or kbd.\n" );
  }

  /* Call the factory function of a generalized mclt */
  MP_Mclt_Block_c *newBlock = init(setSignal,setFilterLen,setFilterShift,setFftSize,setWindowType,setWindowOption,setBlockOffset);

  return( newBlock );
}

/************************/
/* Factory function     */
MP_Mclt_Block_c* MP_Mclt_Block_c::init( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift,
					  const unsigned long int setFftSize,
					  const unsigned char setWindowType,
					  const double setWindowOption,
                                          const unsigned long int setBlockOffset ) {

  const char* func = "MP_Mclt_Block_c::init()";
  MP_Mclt_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Mclt_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new Mclt block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterLen, setFilterShift, setFftSize,
				  setWindowType, setWindowOption, setBlockOffset ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new Mclt block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new Mclt block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Mclt_Block_c::init_parameters( const unsigned long int setFilterLen,
				       const unsigned long int setFilterShift,
				       const unsigned long int setFftSize,
				       const unsigned char setWindowType,
				       const double setWindowOption,
                                       const unsigned long int setBlockOffset ) {

const char* func = "MP_Mclt_Block_c::init_parameters()";

  MP_Mclt_Abstract_Block_c::init_parameters( setFilterLen, setFilterShift,setFftSize, setWindowType,setWindowOption, setBlockOffset);

  /* Allocate the atom's autocorrelations */
  if ( alloc_correl( &reCorrel, &imCorrel, &sqCorrel, &cstCorrel ) ) {
    mp_error_msg( func, "Failed to allocate the block's autocorrelations.\n" );
    return( 1 );
  }

  /* Tabulate the atom's autocorrelations */
  if ( fill_correl( reCorrel, imCorrel, sqCorrel, cstCorrel ) ) {
    mp_error_msg( func, "Failed to tabulate the block's autocorrelations.\n" );
    return( 1 );
  }

  return( 0 );
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Mclt_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  MP_Mclt_Abstract_Block_c::plug_signal( setSignal );

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Mclt_Block_c::nullify_signal( void ) {

  MP_Mclt_Abstract_Block_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Mclt_Block_c::MP_Mclt_Block_c( void )
  :MP_Mclt_Abstract_Block_c() {

  reCorrel = imCorrel = NULL;
  sqCorrel = cstCorrel = NULL;

}


/**************/
/* Destructor */
MP_Mclt_Block_c::~MP_Mclt_Block_c() {

  if ( reCorrel  ) free( reCorrel  );
  if ( imCorrel  ) free( imCorrel  );
  if ( sqCorrel  ) free( sqCorrel  );
  if ( cstCorrel ) free( cstCorrel );

}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char * MP_Mclt_Block_c::type_name() {
  return ("mclt");
}

/**********************/
/* Readable text dump */
int MP_Mclt_Block_c::info( FILE *fid ) {

  int nChar = 0;

  nChar += mp_info_msg( fid, "MCLT BLOCK", "%s window (window opt=%g)"
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
int MP_Mclt_Block_c::alloc_correl( MP_Real_t **reCorr, MP_Real_t **imCorr,
				    MP_Real_t **sqCorr, MP_Real_t **cstCorr ) {

  const char* func = "MP_Mclt_Block_c::alloc_correl(...)";

  /* Allocate the memory for the correlations and init it to zero */
  *reCorr = *imCorr = *sqCorr = NULL;
  
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
/** Fill the correlation arrays with */
int MP_Mclt_Block_c::fill_correl( MP_Real_t *reCorr, MP_Real_t *imCorr,
				   MP_Real_t *sqCorr, MP_Real_t *cstCorr ) {

  const char* func = "MP_Mclt_Block_c::fill_correl(...)";
  double re,im,sq;
  int k, l;

  assert( reCorr != NULL );
  assert( imCorr != NULL );
  assert( sqCorr != NULL );
  assert( cstCorr != NULL );

  for ( k = 0;  k < (int)(numFreqs);  k++ ) {
    re = 0.0;
    im = 0.0;
    for ( l = 0; l < (int)(filterLen);  l++ ) {
	if ( filterLen == fftSize ) { 
		re += pow(*(fft->window+l),2) * cos( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k + 0.5 ) );
		im += pow(*(fft->window+l),2) * sin( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k + 0.5 ) );
	} else {
		re += pow(*(fft->window+l),2) * cos( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k ) );
		im += pow(*(fft->window+l),2) * sin( 2.0 * MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k ) );
	}
    } 
    *( reCorr + k ) = (MP_Real_t)( re );
    *( imCorr + k ) = (MP_Real_t)( im );
    sq = ( re*re + im*im );
    *( sqCorr + k ) = (MP_Real_t)(   sq );
    *( cstCorr + k ) = (MP_Real_t)( 2.0 / (1.0 - sq) );
  }

  return( 0 );
}

/*************************************/
/* Compute the accurate atom energy  */
void MP_Mclt_Block_c::compute_energy( MP_Real_t *in,
				       MP_Real_t *reCorr, MP_Real_t *imCorr,
				       MP_Real_t *sqCorr, MP_Real_t *cstCorr,
				       MP_Real_t *outMag ) {

  const char* func = "MP_Mclt_Block_c::compute_energy(...)";
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

  /* Execute the mclt transform */
  MP_Mclt_Abstract_Block_c::compute_transform( in );

  /*****/
  /* Get the resulting magnitudes: */

  for ( i = 0;  i < ((int)(numFreqs));  i++ ) {

    /* Get the complex values */
    re = mcltOutRe[i];
    im = mcltOutIm[i];
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

  /*****/

  return;
}

/********************************************/
/* Frame-based update of the inner products */
void MP_Mclt_Block_c::update_frame(unsigned long int frameIdx, 
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

  inShift = frameIdx*filterShift + blockOffset;

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
unsigned int MP_Mclt_Block_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int frameIdx,
					    const unsigned long int freqIdx ) {

  const char* func = "MP_Mclt_Block_c::create_atom(...)";
  MP_Mclt_Atom_c *matom = NULL;
  /* Time-frequency location: */
  unsigned long int pos = frameIdx*filterShift + blockOffset;
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
  if ( (matom = MP_Mclt_Atom_c::init( s->numChans, fft->windowType, fft->windowOption )) == NULL ) {
    mp_error_msg( func, "Can't create a new Mclt atom in create_atom()."
		  " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* 1) set the frequency of the atom */
   if ( filterLen == fftSize ) { 
  	matom->freq  = (MP_Real_t)( (double)(freqIdx + 0.5 ) / (double)(fft->fftSize) );
  } else {
	matom->freq  = (MP_Real_t)( (double)(freqIdx) / (double)(fft->fftSize) );
  }

  matom->numSamples = pos + filterLen;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {

    /* 2) set the support of the atom */
    matom->support[chanIdx].pos = pos;
    matom->support[chanIdx].len = filterLen;
    matom->totalChanLen += filterLen;

    /* 3) seek the right location in the signal */
    in  = s->channel[chanIdx] + pos;

    /* 4) recompute the inner product of the complex atom */
    MP_Mclt_Abstract_Block_c::compute_transform( in );
    re  = (double)( *(mcltOutRe + freqIdx) ); 
    im  = (double)( *(mcltOutIm + freqIdx) );

    /* 5) set the amplitude an phase */
    /* This is equivalent to: complex2amp_and_phase( re, im, reCorrel, imCorrel, &amp, &phase ); */
    energy = re*re + im*im;
    reCorr = reCorrel[freqIdx];
    imCorr = imCorrel[freqIdx];
    sqCorr = sqCorrel[freqIdx]; assert( sqCorr <= 1.0 );
    if ( filterLen == fftSize ) { 
      real = (1.0 - reCorr)*re + imCorr*im;
      imag = (1.0 + reCorr)*im + imCorr*re;
      amp   = 2.0 * sqrt( real*real + imag*imag );
      phase = atan2( imag, real );
    } else {
      if ( (freqIdx != 0) ) {  
        real = (1.0 - reCorr)*re + imCorr*im;
        imag = (1.0 + reCorr)*im + imCorr*re;
        amp   = 2.0 * sqrt( real*real + imag*imag );
        phase = atan2( imag, real ); /* the result is between -M_PI and MP_PI */
      } else {
        amp = sqrt( energy );
        if   ( re >= 0 ) phase = 0.0;  /* corresponds to the '+' sign */
        else             phase = M_PI; /* corresponds to the '-' sign exp(i\pi) */
      }
    }

    /* 6) fill in the atom parameters */
    matom->amp[chanIdx]   = (MP_Real_t)( amp   );
    matom->phase[chanIdx] = (MP_Real_t)( phase );
    
  }

  *atom = matom;

  return( 1 );

}
