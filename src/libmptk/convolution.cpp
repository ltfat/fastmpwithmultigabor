/******************************************************************************/
/*                                                                            */
/*                             convolution.cpp                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Wed Dec 07 2005 */
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
 * $Date$
 * $Revision$
 *
 */

/******************************************************************/
/*                                                                */
/* convolution.cpp: computation of the inner products for anywave */
/* atoms                                                          */
/*                                                                */
/******************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include <time.h>

/*********************************/
/*                               */
/* GENERIC CLASS                 */
/*                               */
/*********************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***************/
/* Constructor */
MP_Convolution_c::  MP_Convolution_c( MP_Anywave_Table_c* setAnywaveTable,
				    const unsigned long int setFilterShift ) {
  anywaveTable = setAnywaveTable;
  filterShift = setFilterShift;
}


/**************/
/* Destructor */
MP_Convolution_c::~MP_Convolution_c( ) {

  anywaveTable = NULL;
}

/*********************************/
/*                               */
/* FASTEST METHOD IMPLEMENTATION */
/*                               */
/*********************************/

/***************************/
/* CONSTRUCTOR/DESTRUCTOR */
/***************************/

MP_Convolution_Fastest_c::MP_Convolution_Fastest_c( MP_Anywave_Table_c* anywaveTable,
						  const unsigned long int filterShift )
  : MP_Convolution_c( anywaveTable, filterShift ) {
    
    /* if filterShift is greater or equal to anywaveTable->filterLen,
       then the fastest method is the direct one. Else the methods are
       compared depending on the length of the signal */
    if (filterShift >= anywaveTable->filterLen) {
      initialize( MP_ANYWAVE_COMPUTE_DIRECT );
    } else {
      initialize();
    }
  }

MP_Convolution_Fastest_c::MP_Convolution_Fastest_c( MP_Anywave_Table_c* anywaveTable,
						  const unsigned long int filterShift,
						  const unsigned short int computationMethod )
  : MP_Convolution_c( anywaveTable, filterShift ) {
    
    initialize( computationMethod );
  }

MP_Convolution_Fastest_c::~MP_Convolution_Fastest_c() {

  release();
}


void MP_Convolution_Fastest_c::initialize(void) {

  /*
  */
  unsigned short int count;
  unsigned long int currSignalLen;
  unsigned long int precSignalLen;
  unsigned long int maxSignalLen;

  unsigned long int i;

  unsigned long int currFactor;
  unsigned long int precFactor;
  unsigned long int maxFactor;

  MP_Sample_t* output;
  MP_Sample_t* signal;
  MP_Sample_t* pSignal;

  clock_t directTime_0;
  clock_t directTime_1;

  clock_t fftTime_0;
  clock_t fftTime_1;
  
  unsigned long int num;
  clock_t precDiff;

  bool goOn;
  
  methods[0] = (MP_Convolution_c *) new MP_Convolution_Direct_c( anywaveTable, filterShift );
  methods[1] = (MP_Convolution_c *) new MP_Convolution_FFT_c( anywaveTable, filterShift );

  methodSwitchLimit = 0;
  
  currFactor = 1;
  precFactor = 1;
  currSignalLen = anywaveTable->filterLen + currFactor * filterShift;
  precSignalLen = 0;
  
  maxFactor = (unsigned long int) ((double)MP_MAX_SIZE_T/(double)anywaveTable->numFilters / (double)sizeof(double)) - 1;
  maxSignalLen = (unsigned long int) ((double)MP_MAX_SIZE_T/ (double)sizeof(MP_Sample_t));

  directTime_1 = 0;
  fftTime_1 = 0;

  count = 0;
  precDiff = 0;

  if (currSignalLen > maxSignalLen) {
    goOn = false;
  } else {
    goOn = true;
  }

  output = NULL;
  signal = NULL;

  
  num = 30; 

#ifndef NDEBUG
  fprintf(stderr,"\nmplib DEBUG -- ~MP_Anywave_Table_c -- Comparing speed of the direct method and of the FFT method:");
  fflush(stderr);
#endif

  while ( goOn ) {
    
    /* Reallocation */
    if ( (signal = (MP_Sample_t *)realloc ( signal, currSignalLen * sizeof(MP_Sample_t) ) ) == NULL ) {
      mp_error_msg( "MP_Convolution_Fastest_c::initialize", "Can't allocate an array of [%lu] MP_Sample_t elements"
		    " for the signal array using realloc. This pointer will remain NULL.\n", currSignalLen *sizeof(MP_Sample_t));
    }
    for (pSignal = signal + precSignalLen;
	 pSignal < signal + currSignalLen;
	 pSignal ++) {
      *pSignal = (MP_Sample_t)0.0;
    }

    if ( (output = (double *)realloc( output, anywaveTable->numFilters * (currFactor+1) * sizeof(double) ) ) == NULL ) {
	mp_error_msg( "MP_Convolution_Fastest_c::initialize", "Can't allocate an array of [%lu] double elements"
		      " for the output array using realloc. This pointer will remain NULL.\n", anywaveTable->numFilters * (currFactor+1) );
    }

    if ( (signal == NULL) || (output == NULL) ) {
      goOn = false;
    } else {
      /* estimating the number of runs */
      if (precSignalLen == 0) {
	i = 0;
	fftTime_0 = clock(); 
	while (clock()-fftTime_0 < CLOCKS_PER_SEC/10) {
	  i++;
	  methods[MP_ANYWAVE_COMPUTE_FFT]->compute_IP( signal, currSignalLen, 0, &output );
	}
#ifndef NDEBUG
	fprintf(stderr,"\nmplib DEBUG -- ~MP_Anywave_Table_c --   Estimating how many runs to perform : TIME:%lu (Clocks Per Second=%lu) - number of runs=%lu",clock()-fftTime_0,CLOCKS_PER_SEC,i);
	fflush(stderr);
#endif
	if (i > 30) {
	  i = 30;
#ifndef NDEBUG
	fprintf(stderr," -> reducing the number of runs to %lu",30);
	fflush(stderr);
#endif
	}
	num = i;      
      }

      /* Measure of the FFT method */
#ifndef NDEBUG
      fprintf(stderr,"\nmplib DEBUG -- ~MP_Anywave_Table_c --   sigLen=%lu - Measure of the FFT method=",currSignalLen);
      fflush(stderr);
#endif
      
      fftTime_0 = clock(); 
      for (i=0;i<num;i++) {
	methods[MP_ANYWAVE_COMPUTE_FFT]->compute_IP( signal, currSignalLen, 0, &output );
      }
      fftTime_1 = clock() - fftTime_0;
#ifndef NDEBUG
      fprintf(stderr,"%li - Estimation of the direct method=",fftTime_1);
      fflush(stderr);
#endif
      /* Estimation of the direct method */
      directTime_1 = (clock_t)(((double) currFactor + 1.0)/((double) precFactor + 1.0) * (double)directTime_1);

#ifndef NDEBUG
      fprintf(stderr,"%li",directTime_1);
      fflush(stderr);
#endif
	
      /* Comparison */
      if (fftTime_1 < directTime_1) {
	/* that's the end */
	goOn = false;
	methodSwitchLimit = currSignalLen;

      } else {
	/* Measure of the direct method */
#ifndef NDEBUG
	fprintf(stderr," - Measure of the direct method=");
	fflush(stderr);
#endif
	directTime_0 = clock(); 
	for (i=0;i<num;i++) {
	  methods[MP_ANYWAVE_COMPUTE_DIRECT]->compute_IP( signal, currSignalLen, 0, &output );
	}
	directTime_1 = clock() - directTime_0;

#ifndef NDEBUG
	fprintf(stderr,"%li",directTime_1);
	fflush(stderr);
#endif

	if (fftTime_1 < directTime_1) {
	  /* that's the end */
	  goOn = false;
	  methodSwitchLimit = currSignalLen;
	} else {
	  if (fftTime_1 - directTime_1 > precDiff) {
	    count += 1;
	  }
	  precDiff = fftTime_1 - directTime_1;

	  /* we go on */
	  precFactor = currFactor;
	  currFactor = precFactor << 1;
	  
	  precSignalLen = currSignalLen;
	  currSignalLen = anywaveTable->filterLen + currFactor * filterShift;
	  
	  if ( (currFactor >= maxFactor) || (currSignalLen >= maxSignalLen) || (count == 3) ){
	    goOn = false;
	  }
	}
      }
    }
  }    
#ifndef NDEBUG
  fprintf(stderr,"\n\n");
  fflush(stderr);
#endif
#ifndef NDEBUG
  if (methodSwitchLimit == 0) {
    fprintf(stderr,"    msg -- Convolution - computed directly for any signal length\n");
    fflush(stderr);
  } else {
    fprintf(stderr,"    msg -- Convolution - computed directly for signal length between [0] and [%lu] and using FFT for larger signal lengths.\n",methodSwitchLimit);
    fflush(stderr);
  }
#endif

}

void MP_Convolution_Fastest_c::initialize( const unsigned short int computationMethod) {

  /* Check that the choosen method is in the range of available methods */
  if ( computationMethod >= MP_ANYWAVE_COMPUTE_NUM_METHODS ) {
    mp_error_msg( "MP_Convolution_Fastest_c::initialize", "Computation method [%hu] does not exists. There are only [%hu] methods. Exiting.\n", computationMethod, MP_ANYWAVE_COMPUTE_NUM_METHODS );
    return;
  }

  methods[0] = (MP_Convolution_c *) new MP_Convolution_Direct_c( anywaveTable, filterShift );
  methods[1] = (MP_Convolution_c *) new MP_Convolution_FFT_c( anywaveTable, filterShift );
  
  if (computationMethod == MP_ANYWAVE_COMPUTE_FFT) {
    methodSwitchLimit = anywaveTable->filterLen + 1;
  } else {
    methodSwitchLimit = 0;
  }
  
}

void MP_Convolution_Fastest_c::release(void) {
  
  unsigned short int methodIdx;

  for ( methodIdx = 0; methodIdx < MP_ANYWAVE_COMPUTE_NUM_METHODS; methodIdx ++ ) {
    if ( methods[methodIdx] != NULL ) {
      delete methods[methodIdx];
    }
  }

}

/***************************/
/* OTHER METHODS           */
/***************************/

unsigned short int MP_Convolution_Fastest_c::find_fastest_method( unsigned long int testInputLen ) {
  
  unsigned short int fastestMethod;
  
  if ( (testInputLen >= methodSwitchLimit) && (methodSwitchLimit > 0) ) {
    fastestMethod = MP_ANYWAVE_COMPUTE_FFT;
  } else {
    fastestMethod = MP_ANYWAVE_COMPUTE_DIRECT;
  }
  
  /* check that the selected method is within the range of available methods */
  if ( fastestMethod >= MP_ANYWAVE_COMPUTE_NUM_METHODS ) {
    mp_error_msg( "MP_Convolution_Fastest_c::find_fastest_method", "The method selected in the fastestMethod array [%hu] does not exist. There are only [%hu] methods available.\n", fastestMethod, MP_ANYWAVE_COMPUTE_NUM_METHODS );
  }
  
  return( fastestMethod );
}

void MP_Convolution_Fastest_c::compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output ) {

  methods[ find_fastest_method( inputLen ) ]->compute_IP(input, inputLen, chanIdx, output);

}

double MP_Convolution_Fastest_c::compute_IP( MP_Sample_t* input, unsigned long int filterIdx, unsigned short int chanIdx ) {
  
  return( ((MP_Convolution_Direct_c*)methods[ MP_ANYWAVE_COMPUTE_DIRECT ])->compute_IP(input,filterIdx,chanIdx) );
  
}

/*************************************/
/*                                   */
/* DIRECT COMPUTATION IMPLEMENTATION */
/*                                   */
/*************************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


MP_Convolution_Direct_c::MP_Convolution_Direct_c(  MP_Anywave_Table_c* anywaveTable,
						 const unsigned long int filterShift )
  : MP_Convolution_c( anywaveTable, filterShift ) {
    
}

/**************/
/* Destructor */
MP_Convolution_Direct_c::~MP_Convolution_Direct_c() {

}


/***************************/
/* OTHER METHODS           */
/***************************/

void MP_Convolution_Direct_c::compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output ) {

  MP_Sample_t* pFrame;
  MP_Sample_t* pFrameStart;
  MP_Sample_t* pFrameEnd;

  MP_Sample_t* pFilter;
  MP_Sample_t* pFilterStart;
  MP_Sample_t* pFilterEnd;

  double* pOutput;
  
  unsigned long int numFrames;
  unsigned long int filterIdx;

  if( inputLen < anywaveTable->filterLen ) {
     mp_error_msg( "MP_Convolution_Direct_c::compute_IP", "Can't compute inner products because the input signal is smaller than the filter\n inputLen=%lu - filterLen=%lu... aborting\n", inputLen, anywaveTable->filterLen);
    exit(1);
  }

  if ( ( inputLen == 0 ) || ( anywaveTable->filterLen == 0 ) ) {
     mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
    exit(1);
  }

  numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;

  pOutput = *output;
  pFrameEnd = input + numFrames*filterShift;

  for (filterIdx = 0; 
       filterIdx < anywaveTable->numFilters; 
       filterIdx ++) {

    pFilterStart = anywaveTable->wave[filterIdx][chanIdx];
    pFilterEnd = pFilterStart + anywaveTable->filterLen;
    
  
    for ( pFrameStart = input;
	  pFrameStart < pFrameEnd;
	  pFrameStart += filterShift, pOutput ++ ) {
      (*pOutput) = 0.0;
      for ( pFrame = pFrameStart, pFilter = pFilterStart;
	    pFilter < pFilterEnd;
	    pFrame++, pFilter++ ) {
	(*pOutput) += ((double)*pFilter) * ((double)*pFrame);
      }
    }
  }
}

inline double MP_Convolution_Direct_c::compute_IP( MP_Sample_t* input, unsigned long int filterIdx, unsigned short int chanIdx ) {

  MP_Sample_t* pFilter;
  MP_Sample_t* pFilterEnd;

  MP_Sample_t* pInput;
  
  double temp;
  
  temp = 0.0;
  pFilterEnd = anywaveTable->wave[filterIdx][chanIdx] + anywaveTable->filterLen;
  for ( pInput = input, pFilter = anywaveTable->wave[filterIdx][chanIdx];
	pFilter < pFilterEnd;
	pInput++, pFilter++ ) {
    temp += ((double)*pFilter) * ((double)*pInput);

  }
  return(temp);
}

/*************************************/
/*                                   */
/* FAST CONVOLUTION IMPLEMENTATION   */
/*                                   */
/*************************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


MP_Convolution_FFT_c::MP_Convolution_FFT_c( MP_Anywave_Table_c* anywaveTable,
								    const unsigned long int filterShift )
  : MP_Convolution_c( anywaveTable, filterShift ) {

    initialize();

  }

/**************/
/* Destructor */
MP_Convolution_FFT_c::~MP_Convolution_FFT_c() {

  release();

}

void MP_Convolution_FFT_c::initialize(void) {

  unsigned long int filterIdx;
  unsigned short int chanIdx;
  
  double* pBuffer;
  double* pBufferEnd;

  MP_Sample_t* pFilter;
  MP_Sample_t* pFilterStart;

  fftw_complex* pFftBuffer;
  fftw_complex* pFftBufferEnd;

  fftw_complex* pStorage;

  /* Initialize fftRealSize and fftCplxSize */
  if ( (double) MP_MAX_UNSIGNED_LONG_INT / (double) anywaveTable->filterLen <= 2.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "fftCplxSize cannot be initialized because 2 . anywaveTable->filterLen, [2] . [%lu], is greater than the max for an unsigned long int [%lu]. Exiting from initialize().\n", anywaveTable->filterLen, MP_MAX_UNSIGNED_LONG_INT);
    return;
  }
  fftCplxSize = 2 * anywaveTable->filterLen;  
  fftRealSize = anywaveTable->filterLen + 1;

  if ( (double) MP_MAX_SIZE_T / (double) fftCplxSize / (double)sizeof(double) <= 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "fftCplxSize [%lu] . sizeof(double) [%lu] is greater than the max for a size_t [%lu]. Cannot allocate arrays. Exiting from initialize().\n", fftCplxSize, sizeof(double), MP_MAX_SIZE_T);
    return;
  }
  if ( (double) MP_MAX_SIZE_T / (double) fftRealSize / (double)sizeof(fftw_complex) <= 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "fftRealSize [%lu] . sizeof(fftw_complex) [%lu] is greater than the max for a size_t [%lu]. Cannot allocate arrays. Exiting from initialize().\n", fftRealSize, sizeof(fftw_complex), MP_MAX_SIZE_T);
    return;
  }
  if ( (double) MP_MAX_SIZE_T / (double) anywaveTable->numFilters / (double)sizeof(fftw_complex**) <= 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "anywaveTable->numFilters [%lu] . sizeof(fftw_complex**) [%lu] is greater than the max for a size_t [%lu]. Cannot allocate arrays. Exiting from initialize().\n", anywaveTable->numFilters, sizeof(fftw_complex**), MP_MAX_SIZE_T);
    return;
  }
  if ( (double) MP_MAX_SIZE_T / (double) anywaveTable->numChans / (double)sizeof(fftw_complex*) <= 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "anywaveTable->numChans [%lu] . sizeof(fftw_complex*) [%lu] is greater than the max for a size_t [%lu]. Cannot allocate arrays. Exiting from initialize().\n", anywaveTable->numChans, sizeof(fftw_complex*), MP_MAX_SIZE_T);
    return;
  }
  if ( (double) MP_MAX_SIZE_T / (double)fftRealSize / (double)anywaveTable->numFilters / (double)anywaveTable->numChans / (double)sizeof(fftw_complex) <= 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "fftRealSize [%lu] . anywaveTable->numFilters [%lu] . anywaveTable->numChans [%lu] . sizeof(fftw_complex) [%lu] is greater than the max for a size_t [%lu]. Cannot allocate arrays. Exiting from initialize().\n", fftRealSize, anywaveTable->numFilters, anywaveTable->numChans, sizeof(fftw_complex), MP_MAX_SIZE_T);
    return;
  }
  
  /* Allocates the buffer signalBuffer (fftCplxSize double) and applies the zero-padding on the second half of the buffer */
  if ((signalBuffer  = (double*) fftw_malloc( sizeof(double) * fftCplxSize )) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] double elements"
		  " for the signalIn array using fftw_malloc. This pointer will remain NULL.\n", fftCplxSize );
  } else {
    /* fills the second part in (the zero-padding) */
    pBufferEnd = signalBuffer + 2*anywaveTable->filterLen;

    for (pBuffer = signalBuffer + anywaveTable->filterLen;
	 pBuffer < pBufferEnd;
	 pBuffer++) {
      *pBuffer = 0.0;
    }
  }

  /* Allocates the buffer signalFftBuffer (fftRealSize fftw_complex) */
  if ((signalFftBuffer = (fftw_complex*) fftw_malloc( sizeof(fftw_complex) * fftRealSize )) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] fftw_complex elements"
		  " for the signalOut array using fftw_malloc. This pointer will remain NULL.\n", fftRealSize );
  }

  /* Creates the local plan for performing FFT */
  fftPlan = fftw_plan_dft_r2c_1d( (int)(fftCplxSize), signalBuffer, signalFftBuffer, FFTW_MEASURE );
  
  /* Allocates the buffer outputBuffer (fftRealSize fftw_complex) */
  if ((outputBuffer  = (double*) fftw_malloc( sizeof(double) * fftCplxSize )) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] double elements"
		  " for the signalIn array using fftw_malloc. This pointer will remain NULL.\n", fftCplxSize );
  }

  /* Allocates the buffer outputFftBuffer (fftCplxSize double) */
  if ((outputFftBuffer = (fftw_complex*) fftw_malloc( sizeof(fftw_complex) * fftRealSize )) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] fftw_complex elements"
		  " for the signalOut array using fftw_malloc. This pointer will remain NULL.\n", fftRealSize );
  }

  /* Creates the local plan for performing IFFT */
  ifftPlan = fftw_plan_dft_c2r_1d( (int)(fftCplxSize), outputFftBuffer, outputBuffer, FFTW_MEASURE );
  
  /* Allocates the tab for accessing the FFT of the filters in filterFftStorage */
  if ( (filterFftBuffer = (fftw_complex***) malloc( sizeof(fftw_complex **) * anywaveTable->numFilters ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] fftw_complex** elements"
		  " for the filterIn array using malloc. This pointer will remain NULL.\n", anywaveTable->numFilters );
  } else {
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters;
	 filterIdx ++) {
      if ( (filterFftBuffer[filterIdx] = (fftw_complex**) malloc( sizeof(fftw_complex *) * anywaveTable->numChans ) ) == NULL ) {
	mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] fftw_complex* elements"
		      " for the filterIn array using malloc. This pointer will remain NULL.\n", anywaveTable->numChans );
      }
    }
  }
	
  /* Allocates the storage for all the fft of the filters and fill it in */
  if ((filterFftStorage = (fftw_complex*) fftw_malloc( sizeof(fftw_complex) * anywaveTable->numFilters * anywaveTable->numChans * fftRealSize )) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] fftw_complex* elements"
		  " for the filterIn array using fftw_malloc. This pointer will remain NULL.\n", anywaveTable->numFilters * anywaveTable->numChans * fftRealSize );
  } else {
    /* fftPlan is used for performing the FFT of the filters */
    pStorage = filterFftStorage;
    
    for (chanIdx = 0;
	 chanIdx < anywaveTable->numChans;
	 chanIdx ++) {
      for (filterIdx = 0;
	   filterIdx < anywaveTable->numFilters;
	   filterIdx ++) {
	
	/* copies the pointer to the storage of the FFT of the channel chanIdx of the filter filterIdx to filterFftBuffer */
	filterFftBuffer[filterIdx][chanIdx] = pStorage;

	/* copies the filter, BACKWARDS, to the first half of signalBuffer */
	pFilterStart = anywaveTable->wave[filterIdx][chanIdx] - 1;
	
	for (pBuffer = signalBuffer, pFilter = pFilterStart + anywaveTable->filterLen;
	     pFilter > pFilterStart;
	     pBuffer++, pFilter-- ) {
	  *pBuffer = (double)*pFilter;
	}

	/* performs FFT */
	fftw_execute( fftPlan );
	
	pFftBufferEnd = signalFftBuffer + fftRealSize;
	/* copies the FFT to filterFftStorage */
	for (pFftBuffer = signalFftBuffer;
	     pFftBuffer < pFftBufferEnd;
	     pFftBuffer++, pStorage++ ) {
	  (*pStorage)[0] = (*pFftBuffer)[0];
	  (*pStorage)[1] = (*pFftBuffer)[1];
	}
      }
    }
  }

}

void MP_Convolution_FFT_c::release( void ) {
  
  unsigned long int filterIdx;

  if (signalBuffer) { fftw_free( signalBuffer );}
  if (signalFftBuffer) { fftw_free( signalFftBuffer );}

  if (outputBuffer) { fftw_free( outputBuffer );}
  if (outputFftBuffer) { fftw_free( outputFftBuffer );}

  if (filterFftStorage) { fftw_free( filterFftStorage );}

  if (filterFftBuffer) {
    for ( filterIdx = 0; 
	  filterIdx < anywaveTable->numFilters; 
	  filterIdx ++) {
      if ( filterFftBuffer[filterIdx] ) {
	free( filterFftBuffer[filterIdx] );
      }
    }
    free( filterFftBuffer );
  }
  
  if (fftPlan) {fftw_destroy_plan( fftPlan );}
  if (ifftPlan) {fftw_destroy_plan( ifftPlan );}

}

/***************************/
/* OTHER METHODS           */
/***************************/

void MP_Convolution_FFT_c::compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output ) {

  unsigned long int slideIdx;
  unsigned long int lowerFrameIdx;
  unsigned long int upperFrameIdx;

  unsigned long int numFrames;
  unsigned long int numSlides;

  MP_Sample_t* pSlide;
  MP_Sample_t* pSlideEnd;
  MP_Sample_t* pInputEnd;

  double* pBuffer;
  double* pOutputBuffer;
  double* pOutputBufferStart;
  double* pOutput;
  double* pOutputStart;
  double* pOutputEnd;

  fftw_complex* pFftSignal;
  fftw_complex* pFftSignalEnd;
  fftw_complex* pFftFilter;
  fftw_complex* pFftOutput;

  unsigned long int filterIdx;


  if( inputLen < anywaveTable->filterLen ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input signal is smaller than the filter\n inputLen=%lu - filterLen=%lu... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( ( inputLen == 0 ) || ( anywaveTable->filterLen == 0 ) ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( inputLen == MP_MAX_UNSIGNED_LONG_INT ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "inputLen [%lu] is equal to the max for an unsigned long int [%lu]. Cannot initialize the number of slides. Exiting from compute_IP()\n", inputLen, MP_MAX_UNSIGNED_LONG_INT );
    return;
  }
  numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
  numSlides = ( inputLen / anywaveTable->filterLen ) + 1;

  /* sets all the elements of output to zero */
  pOutputStart = *output;
  if ( (double)MP_MAX_UNSIGNED_LONG_INT / (double)anywaveTable->numFilters / (double)numFrames <= 1.0) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "anywaveTable->numFilters [%lu] . numFrames [%lu] is greater than the max for an unsigned long int [%lu]. Cannot initialize local variable. Exiting from compute_IP().\n", anywaveTable->numFilters, numFrames, MP_MAX_UNSIGNED_LONG_INT);
    return;
  }
  pOutputEnd = pOutputStart + anywaveTable->numFilters * numFrames;
  for (pOutput = pOutputStart; pOutput < pOutputEnd; pOutput ++) {
    *pOutput = 0.0;
  }

  /* inits pSlide to the first sample of input */
  pSlide = input;
  /* first MP_Sample_t* after input */
  pInputEnd = input + inputLen;
  /* first fftw_complex* after signalFftBuffer */
  pFftSignalEnd = signalFftBuffer + fftRealSize;
  
  /* loop on the slides of size anywaveTable->filterLen */
  for (slideIdx = 0;
       slideIdx < numSlides;
       slideIdx ++) {

    /* puts the slide slideIdx of the input signal in signalBuffer (first half of the buffer) */
    pSlideEnd = pSlide + anywaveTable->filterLen;
    if (pSlideEnd < pInputEnd) {
      for (pBuffer = signalBuffer;
	   pSlide < pSlideEnd;
	   pBuffer++, pSlide++ ) {
	*pBuffer = (double)*pSlide;
      }
    } else {
      for (pBuffer = signalBuffer;
	   pSlide < pInputEnd;
	   pBuffer++, pSlide++ ) {
	*pBuffer = (double)*pSlide;
      }
      for (;
	   pSlide < pSlideEnd;
	   pBuffer++, pSlide++ ) {
	*pBuffer = 0.0;
      }
    }      

    /* computes the FFT of the slide slideIdx of the input signal */
    fftw_execute( fftPlan );

    /* init pFfftFilter to the first filter in the channel chanIdx,
       since for each channel, all the FFTs of the filters are put one
       after the other */
    pFftFilter = filterFftBuffer[0][chanIdx];
    
    /* find the inner products corresponding to a frame of the input signal */
    /* lower bound */
    if (slideIdx == 0) {
      lowerFrameIdx = 0;
    } else {
      lowerFrameIdx = (((slideIdx - 1) * anywaveTable->filterLen + 1) / filterShift);
      while (lowerFrameIdx * filterShift < (slideIdx - 1) * anywaveTable->filterLen + 1) {
	lowerFrameIdx ++;
      }
    }

    /* upper bound */
    upperFrameIdx = (( (slideIdx+1) * anywaveTable->filterLen  )/ filterShift);
    /* greater or EQUAL->in order not to take the last sample in outputBuffer (theoritically, it is always zero)*/
    while ((upperFrameIdx * filterShift >= (slideIdx + 1) * anywaveTable->filterLen ) && (upperFrameIdx > 0)) {
      upperFrameIdx --;
    }
    if (upperFrameIdx >= numFrames) {
      upperFrameIdx = numFrames - 1;
    }

    /* points to the element of outputBuffer to add to the inner
       products in output corresponding to the first involved frame in
       the slide slideIdx */ 
    pOutputBufferStart = outputBuffer + lowerFrameIdx*filterShift - (slideIdx - 1) *anywaveTable->filterLen - 1;

    /* points to inner product in output corresponding to the first
       involved frame in the slide slideIdx */
    pOutputStart = *output + lowerFrameIdx;
    /* points to inner product in output corresponding to the (last+1)
       involved frame in the slide slideIdx */
    pOutputEnd = *output + upperFrameIdx + 1;

    /* loop on the filters */
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters;
	 filterIdx ++) {
      
      /* multiplies the FFT of the signal by the FFT of the inverted filter filterIdx */
      for ( pFftSignal = signalFftBuffer, pFftOutput = outputFftBuffer, pFftFilter = filterFftBuffer[filterIdx][chanIdx];
	    pFftSignal < pFftSignalEnd;
	    pFftSignal += 1, pFftFilter += 1, pFftOutput += 1 ) {
	(*pFftOutput)[0] = ((*pFftSignal)[0]) * ((*pFftFilter)[0]) - ((*pFftSignal)[1]) * ((*pFftFilter)[1]);
	(*pFftOutput)[1] = ((*pFftSignal)[0]) * ((*pFftFilter)[1]) + ((*pFftSignal)[1]) * ((*pFftFilter)[0]);	
      }

      /* computes the IFFT of the multiplication between the FFT of the slide of signal and the filter */
      fftw_execute( ifftPlan );

      /* update the inner products in the output array */
      long int frameIdx;
      for (pOutput = pOutputStart, pOutputBuffer = pOutputBufferStart,frameIdx = lowerFrameIdx;
	   pOutput < pOutputEnd;
	   pOutput++, pOutputBuffer += filterShift, frameIdx++) {

	*pOutput += *pOutputBuffer / fftCplxSize;
	
      }
      pOutputStart += numFrames;
      pOutputEnd += numFrames;

    }
  }
}


