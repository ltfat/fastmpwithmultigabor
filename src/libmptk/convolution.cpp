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

void MP_Convolution_Fastest_c::compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput ) {

  methods[ find_fastest_method( inputLen) ]->compute_max_IP(s, inputLen, fromSample, ampOutput, idxOutput);

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
     mp_error_msg( "MP_Convolution_Direct_c::compute_IP", "Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
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


void MP_Convolution_Direct_c::compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput ) {

  unsigned short int chanIdx;


  unsigned long int numFrames;
  unsigned long int frameIdx;

  MP_Sample_t** pSignal;

  double tmp;
  double* pAmp;
  unsigned long int* pIdx;

  double doubleTmp;

  unsigned long int filterIdx;

  if ( fromSample > s->numSamples) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP","Inputs ask to process a slice of signal beginning at sample [%lu], whereas the signal contains only [%lu] samples... aborting\n", fromSample, s->numSamples);
    return;
  }
  if ( inputLen > s->numSamples - fromSample ) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP","Inputs ask to process the slice of signal beginning at sample [%lu], of length [%lu], whereas the signal contains only [%lu] samples... aborting\n", fromSample, inputLen, s->numSamples);
    return;
  }

  if( inputLen < anywaveTable->filterLen ) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP","Can't compute inner products because the input signal is smaller than the filter\n inputLen=%lu - filterLen=%lu... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( ( inputLen == 0 ) || ( anywaveTable->filterLen == 0 ) ) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP","Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( inputLen == MP_MAX_UNSIGNED_LONG_INT ) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP", "inputLen [%lu] is equal to the max for an unsigned long int [%lu]. Cannot initialize the number of slices. Exiting from compute_IP()\n", inputLen, MP_MAX_UNSIGNED_LONG_INT );
    return;
  }

  numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;

  if ( (double)MP_MAX_UNSIGNED_LONG_INT / (double)anywaveTable->numFilters / (double)numFrames <= 1.0) {
    mp_error_msg( "MP_Convolution_Direct_c::compute_IP", "anywaveTable->numFilters [%lu] . numFrames [%lu] is greater than the max for an unsigned long int [%lu]. Cannot initialize local variable. Exiting from compute_IP().\n", anywaveTable->numFilters, numFrames, MP_MAX_UNSIGNED_LONG_INT);
    return;
  }

  pAmp = ampOutput;
  pIdx = idxOutput;

  if ( (pSignal = (MP_Sample_t**) malloc( sizeof(MP_Sample_t*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] MP_Sample_t* elements"
		  " for the pSignal array using malloc. This pointer will remain NULL.\n", s->numChans );
  } else {
    for ( chanIdx = 0;
	  chanIdx < s->numChans;
	  chanIdx ++ ) {
      pSignal[chanIdx] = s->channel[chanIdx] + fromSample;
    }
  }

  for (frameIdx = 0;
       frameIdx < numFrames;
       frameIdx ++) {

    *pAmp = 0.0;
    *pIdx = 0;

    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      
      doubleTmp = 0.0;
      
      for ( chanIdx = 0;
	    chanIdx < s->numChans;
	    chanIdx ++ ) {
	if (s->numChans == anywaveTable->numChans){
	  doubleTmp += compute_IP( pSignal[chanIdx], filterIdx, chanIdx );
	} else {
	  tmp = compute_IP( pSignal[chanIdx], filterIdx, 0 );
	  doubleTmp += tmp * tmp;
	}
      }
      if (s->numChans == anywaveTable->numChans){
	doubleTmp *= doubleTmp;
      }	
	
      if (doubleTmp > *pAmp) {
	*pAmp = (MP_Real_t)doubleTmp;
	*pIdx = filterIdx;
      }
    }
    pAmp ++;
    pIdx ++;
    for ( chanIdx = 0;
	  chanIdx < s->numChans;
	  chanIdx ++ ) {
      pSignal[chanIdx] += filterShift;
    }
    
  }
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

  /* Allocates the tabs outputBufferAdd and outputBufferNew */
  if ( (outputBufferAdd = (double**) malloc( sizeof(double*) * anywaveTable->numFilters ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] double* elements"
		  " for the outputBufferAdd array using malloc. This pointer will remain NULL.\n", anywaveTable->numFilters );
  }
  if ( (outputBufferNew = (double**) malloc( sizeof(double*) * anywaveTable->numFilters ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::initialize", "Can't allocate an array of [%lu] double* elements"
		  " for the outputBufferNew array using malloc. This pointer will remain NULL.\n", anywaveTable->numFilters );
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
  
  if (outputBufferAdd) {free( outputBufferAdd );}
  if (outputBufferNew) {free( outputBufferNew );}
}

/***************************/
/* OTHER METHODS           */
/***************************/

MP_Sample_t* MP_Convolution_FFT_c::slice( unsigned long int sliceIdx, MP_Sample_t* inputStart ) {
  
  if ( (double) MP_MAX_UNSIGNED_LONG_INT / (double) sliceIdx / (double) anywaveTable->filterLen < 1.0 ) {
    mp_error_msg( "MP_Convolution_FFT_c::slice","Can't add the sliceIdx [%lu] . anywaveTable->filterLen [%lu] to inputStart, because it is bigger than the max unsigned long int [%lu].\n Returning NULL.", sliceIdx, anywaveTable->filterLen, MP_MAX_UNSIGNED_LONG_INT );
    return( NULL );
  } else {
    return( inputStart + sliceIdx * anywaveTable->filterLen );
  }
}

void MP_Convolution_FFT_c::circular_convolution( MP_Sample_t* pSlice, MP_Sample_t* pNextSlice, unsigned short int chanIdx, double** outputBufferAdd, double** outputBufferNew, unsigned long int firstFrameSample, unsigned long int numFramesAdd, unsigned long int numFramesNew ) {

  MP_Sample_t* pSample;
  double* pBuffer;
  unsigned long int frameIdx;

  MP_Sample_t* pSliceEnd;

  double* pOutputBuffer;
  double* pOutputBufferStart;
  double* pOutput;

  fftw_complex* pFftSignal;
  fftw_complex* pFftSignalEnd;
  fftw_complex* pFftFilter;
  fftw_complex* pFftOutput;

  unsigned long int filterIdx;


  /* puts this slice of the input signal in signalBuffer (first half of the buffer) */
  pSliceEnd = pSlice + anywaveTable->filterLen;
  for (pBuffer = signalBuffer, pSample = pSlice;
       pSample < pNextSlice;
       pBuffer++, pSample++ ) {
    *pBuffer = (double)*pSample;
  }
  for (;
       pSample < pSliceEnd;
       pBuffer++, pSample++ ) {
    *pBuffer = 0.0;
  }


  /* computes the FFT of this slice of the input signal */
  fftw_execute( fftPlan );

  /* init pFftFilter to the first filter in the channel chanIdx,
     since for each channel, all the FFTs of the filters are put one
     after the other */
  pFftFilter = filterFftBuffer[0][chanIdx];
    
  /* points to the element of outputBuffer to add to the inner
     products in output corresponding to the first involved frame in
     the slice sliceIdx */ 
  pOutputBufferStart = outputBuffer + firstFrameSample;
  
  pFftSignalEnd = signalFftBuffer + fftRealSize;

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
    
    /* computes the IFFT of the multiplication between the FFT of the slice of signal and the filter */
    fftw_execute( ifftPlan );
    
    /* update the inner products in the output arrays */    
    for (pOutput = outputBufferAdd[filterIdx], pOutputBuffer = pOutputBufferStart,frameIdx = 0;
	 frameIdx < numFramesAdd;
	 pOutput++, pOutputBuffer += filterShift, frameIdx++) {      
      *pOutput += *pOutputBuffer / fftCplxSize;      
    }

    for (pOutput = outputBufferNew[filterIdx],frameIdx = 0;
	 frameIdx < numFramesNew;
	 pOutput++, pOutputBuffer += filterShift, frameIdx++) {      
      *pOutput = *pOutputBuffer / fftCplxSize;      
    }    
  }
  
}

void MP_Convolution_FFT_c::compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output ) {

  unsigned long int sliceIdx;

  unsigned long int numFrames;
  unsigned long int frameIdx;
  unsigned long int numFramesAdd;
  unsigned long int numFramesNew;
  unsigned long int firstFrameSample;
  unsigned long int nextFirstFrameSample;
  unsigned long int numSlices;

  MP_Sample_t* p;
  MP_Sample_t* pSlice;
  MP_Sample_t* pNextSlice;
  MP_Sample_t* pInputEnd;

  double** tmp;
  
  double* pOutput;
  double* pOutputStart;

  unsigned long int filterIdx;

  if( chanIdx >= anywaveTable->numChans ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","chanIdx [%hu] is larger than the number of channels [%hu]... aborting\n", chanIdx, anywaveTable->numChans);
    return;
  }

  if( inputLen < anywaveTable->filterLen ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input signal is smaller than the filter\n inputLen=%lu - filterLen=%lu... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( ( inputLen == 0 ) || ( anywaveTable->filterLen == 0 ) ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( inputLen == MP_MAX_UNSIGNED_LONG_INT ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "inputLen [%lu] is equal to the max for an unsigned long int [%lu]. Cannot initialize the number of slices. Exiting from compute_IP()\n", inputLen, MP_MAX_UNSIGNED_LONG_INT );
    return;
  }
  numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
  numSlices = ( inputLen / anywaveTable->filterLen ) + 1;

  pOutputStart = *output;
  if ( (double)MP_MAX_UNSIGNED_LONG_INT / (double)anywaveTable->numFilters / (double)numFrames <= 1.0) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "anywaveTable->numFilters [%lu] . numFrames [%lu] is greater than the max for an unsigned long int [%lu]. Cannot initialize local variable. Exiting from compute_IP().\n", anywaveTable->numFilters, numFrames, MP_MAX_UNSIGNED_LONG_INT);
    return;
  }

  /* inits pSlice to the first sample of input */
  pSlice = input;
  pNextSlice = pSlice + anywaveTable->filterLen;
  numFramesAdd = 0;
  numFramesNew = 0;
  nextFirstFrameSample = 0;

  /* first MP_Sample_t* after input */
  pInputEnd = input + inputLen;
  
  p = pSlice;
  while ((p < pNextSlice)&&(p < (pInputEnd - anywaveTable->filterLen + 1))) {
    numFramesNew ++;
    p += filterShift;
  }


  /* sets the elements of the first slice of output to zero */
  for (filterIdx = 0;
       filterIdx < anywaveTable->numFilters; 
       filterIdx ++) {
    outputBufferNew[filterIdx] = pOutputStart + filterIdx*numFrames;

    for (frameIdx = 0, pOutput = outputBufferNew[filterIdx];
	 frameIdx < numFramesNew; 
	 frameIdx ++, pOutput++) {
      *pOutput = 0.0;
    }
  }

  /* loop on the slices of size anywaveTable->filterLen */

  for (sliceIdx = 0, pSlice = input;
       sliceIdx < numSlices;
       sliceIdx ++, pSlice += anywaveTable->filterLen) {
    
    pNextSlice = pSlice + anywaveTable->filterLen;
    if ( pNextSlice > pInputEnd ) {      
      pNextSlice = pInputEnd;
    }
    
    tmp = outputBufferAdd;
    outputBufferAdd = outputBufferNew;
    outputBufferNew = tmp;
    numFramesAdd = numFramesNew;
    numFramesNew = 0;

    firstFrameSample = nextFirstFrameSample;
    nextFirstFrameSample = p - pNextSlice;    

    while ((p < pNextSlice + anywaveTable->filterLen)&&(p < (pInputEnd - anywaveTable->filterLen + 1))) {
      numFramesNew ++;
      p += filterShift;
    }

    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      outputBufferNew[filterIdx] = outputBufferAdd[filterIdx] + numFramesAdd;
    }
    circular_convolution( pSlice, pNextSlice, chanIdx, outputBufferAdd, outputBufferNew, firstFrameSample, numFramesAdd, numFramesNew );

  }
}


void MP_Convolution_FFT_c::compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput ) {

  unsigned short int chanIdx;

  unsigned long int sliceIdx;

  unsigned long int numFrames;
  unsigned long int frameIdx;

  unsigned long int numFramesAdd;
  unsigned long int numFramesNew;
  unsigned long int maxNumFramesPerSlice;
  unsigned long int firstFrameSample;
  unsigned long int nextFirstFrameSample;
  unsigned long int numSlices;

  MP_Sample_t** pSlice;
  MP_Sample_t** pNextSlice;
  MP_Sample_t** pInputEnd;

  unsigned long int tmp;
  unsigned long int tmpMax;
  double* pAmp;
  unsigned long int* pIdx;

  double doubleTmp;

  double* outputAdd;
  double* outputNew;
  double* pOutputAdd;
  double* pOutputNew;

  double*** accessOutputAdd;
  double*** accessOutputNew;
  double*** accessSwitch;

  unsigned long int filterIdx;

  if ( fromSample > s->numSamples) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Inputs ask to process a slice of signal beginning at sample [%lu], whereas the signal contains only [%lu] samples... aborting\n", fromSample, s->numSamples);
    return;
  }
  if ( inputLen > s->numSamples - fromSample ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Inputs ask to process the slice of signal beginning at sample [%lu], of length [%lu], whereas the signal contains only [%lu] samples... aborting\n", fromSample, inputLen, s->numSamples);
    return;
  }

  if( inputLen < anywaveTable->filterLen ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input signal is smaller than the filter\n inputLen=%lu - filterLen=%lu... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( ( inputLen == 0 ) || ( anywaveTable->filterLen == 0 ) ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP","Can't compute inner products because the input or filter length has not been filled in :\n inputLen=%lu - filterLen=%lu  ... aborting\n", inputLen, anywaveTable->filterLen);
    return;
  }

  if ( inputLen == MP_MAX_UNSIGNED_LONG_INT ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_IP", "inputLen [%lu] is equal to the max for an unsigned long int [%lu]. Cannot initialize the number of slices. Exiting from compute_IP()\n", inputLen, MP_MAX_UNSIGNED_LONG_INT );
    return;
  }
  numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
  numSlices = ( inputLen / anywaveTable->filterLen ) + 1;

  pAmp = ampOutput;
  pIdx = idxOutput;

  /* inits pSlice to the first sample of input */
  numFramesAdd = 0;

  numFramesNew = 1;
  nextFirstFrameSample = anywaveTable->filterLen-1;
  tmp = nextFirstFrameSample;
  tmpMax = inputLen;

  tmp += filterShift;
  tmp -= anywaveTable->filterLen;
  tmpMax -= anywaveTable->filterLen;
  maxNumFramesPerSlice = ( (anywaveTable->filterLen - 1) / filterShift) + 1;

  if ( (double)MP_MAX_SIZE_T / (double)anywaveTable->numFilters / (double)maxNumFramesPerSlice / (double)s->numChans / (double)sizeof(double) <= 1.0) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "anywaveTable->numFilters [%lu] . maxNumFramesPerSlice [%lu] . s->numChans [%lu] is greater than the max for a size_t [%lu]. Cannot initialize local variable. Exiting from compute_max_IP().\n", anywaveTable->numFilters, maxNumFramesPerSlice, s->numChans, MP_MAX_SIZE_T);
    return;
  } else {
    if ( (outputAdd = (double*) calloc( maxNumFramesPerSlice * anywaveTable->numFilters * s->numChans, sizeof(double) ) ) == NULL ) {
      mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		    " for the outputAdd array using malloc. This pointer will remain NULL.\n", maxNumFramesPerSlice * anywaveTable->numFilters * s->numChans );
    }
    if ( (outputNew = (double*) calloc( maxNumFramesPerSlice * anywaveTable->numFilters * s->numChans, sizeof(double) ) ) == NULL ) {
      mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		    " for the outputNew array using malloc. This pointer will remain NULL.\n", maxNumFramesPerSlice * anywaveTable->numFilters * s->numChans );
    }    
  }

  if ( (accessOutputAdd = (double***) malloc( sizeof(double**) * anywaveTable->numFilters ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		  " for the accessOutputAdd array using malloc. This pointer will remain NULL.\n", anywaveTable->numFilters );
  } else {
    for (filterIdx = 0, pOutputAdd = outputAdd;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      if ( (accessOutputAdd[filterIdx] = (double**) malloc( sizeof(double*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		  " for the accessOutputAdd[%lu] array using malloc. This pointer will remain NULL.\n", s->numChans, filterIdx );
      } else {
	for ( chanIdx = 0;
	      chanIdx < s->numChans;
	      chanIdx ++, pOutputAdd += maxNumFramesPerSlice ) {
	  accessOutputAdd[filterIdx][chanIdx] = pOutputAdd;
	}
      }
    }
  }
  if ( (accessOutputNew = (double***) malloc( sizeof(double**) * anywaveTable->numFilters ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		  " for the accessOutputNew array using malloc. This pointer will remain NULL.\n", anywaveTable->numFilters );
  } else {
    for (filterIdx = 0, pOutputNew = outputNew;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      if ( (accessOutputNew[filterIdx] = (double**) malloc( sizeof(double*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] double elements"
		  " for the accessOutputNew[%lu] array using malloc. This pointer will remain NULL.\n", s->numChans, filterIdx );
      } else {
	for ( chanIdx = 0;
	      chanIdx < s->numChans;
	      chanIdx ++, pOutputNew += maxNumFramesPerSlice ) {
	  accessOutputNew[filterIdx][chanIdx] = pOutputNew;
	}
      }
    }
  }   

  if ( (pSlice = (MP_Sample_t**) malloc( sizeof(MP_Sample_t*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] MP_Sample_t* elements"
		  " for the pSlice array using malloc. This pointer will remain NULL.\n", s->numChans );
  }
  if ( (pInputEnd = (MP_Sample_t**) malloc( sizeof(MP_Sample_t*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] MP_Sample_t* elements"
		  " for the pInputEnd array using malloc. This pointer will remain NULL.\n", s->numChans );
  }
  if ( (pNextSlice = (MP_Sample_t**) malloc( sizeof(MP_Sample_t*) * s->numChans ) ) == NULL ) {
    mp_error_msg( "MP_Convolution_FFT_c::compute_max_IP", "Can't allocate an array of [%lu] MP_Sample_t* elements"
		  " for the pNextSlice array using malloc. This pointer will remain NULL.\n", s->numChans );
  }

  for (chanIdx = 0;
       chanIdx < s->numChans;
       chanIdx ++) {
    pSlice[chanIdx] = s->channel[chanIdx] + fromSample;
    pInputEnd[chanIdx] = pSlice[chanIdx] + inputLen;
    pNextSlice[chanIdx] = pSlice[chanIdx] + anywaveTable->filterLen;
  }

  /* loop on the slices of size anywaveTable->filterLen */
  for (sliceIdx = 0;
       sliceIdx < numSlices;
       sliceIdx ++) {

    accessSwitch = accessOutputAdd;
    accessOutputAdd = accessOutputNew;    
    accessOutputNew = accessSwitch;
    
    numFramesAdd = numFramesNew;
    numFramesNew = 0;
    
    firstFrameSample = nextFirstFrameSample;
    nextFirstFrameSample = tmp;    
    
    while ((tmp < anywaveTable->filterLen)&&(tmp < tmpMax)) {
      numFramesNew ++;
      tmp += filterShift;
    }
    if (tmp >= (inputLen - anywaveTable->filterLen + 1)) {
      tmp -= inputLen + anywaveTable->filterLen - 1;
    } else {
      tmp -= anywaveTable->filterLen;
      tmpMax -= anywaveTable->filterLen;
    }

    for ( chanIdx = 0;
	  chanIdx < s->numChans;
	  chanIdx ++ ) {

      if ( pNextSlice[chanIdx] > pInputEnd[chanIdx] ) {      
	pNextSlice[chanIdx] = pInputEnd[chanIdx];
      }
    
      for (filterIdx = 0, pOutputNew = outputNew;
	   filterIdx < anywaveTable->numFilters; 
	   filterIdx ++) {
	outputBufferAdd[filterIdx] = accessOutputAdd[filterIdx][chanIdx];
	outputBufferNew[filterIdx] = accessOutputNew[filterIdx][chanIdx];
      }

      if (s->numChans == anywaveTable->numChans){
	circular_convolution( pSlice[chanIdx], pNextSlice[chanIdx], chanIdx, outputBufferAdd, outputBufferNew, firstFrameSample, numFramesAdd, numFramesNew );
      } else {
	circular_convolution( pSlice[chanIdx], pNextSlice[chanIdx], 0, outputBufferAdd, outputBufferNew, firstFrameSample, numFramesAdd, numFramesNew );
      }

      pSlice[chanIdx] += anywaveTable->filterLen;
      pNextSlice[chanIdx] += anywaveTable->filterLen;

    }
    /* computes the inner products and find the max */

    for (frameIdx = 0;
	 frameIdx < numFramesAdd;
	 frameIdx ++ ) {
      *pAmp = 0.0;
      *pIdx = 0;

      for (filterIdx = 0;
	   filterIdx < anywaveTable->numFilters; 
	   filterIdx ++) {

	doubleTmp = 0.0;

	for ( chanIdx = 0;
	      chanIdx < s->numChans;
	      chanIdx ++ ) {

	  if (s->numChans == anywaveTable->numChans){
	    doubleTmp += accessOutputAdd[filterIdx][chanIdx][frameIdx];
	  } else {
	    doubleTmp += accessOutputAdd[filterIdx][chanIdx][frameIdx] * accessOutputAdd[filterIdx][chanIdx][frameIdx];
	  }
	}
	if (s->numChans == anywaveTable->numChans){
	  doubleTmp *= doubleTmp;
	}	
	
	if (doubleTmp > *pAmp) {
	  *pAmp = (MP_Real_t)doubleTmp;
	  *pIdx = filterIdx;
	}
      }

      pAmp ++;
      pIdx ++;
    }
  }

  /* clean the house */

  if ( outputAdd ) { free (outputAdd); }
  if ( outputNew ) { free (outputNew); }

  if ( accessOutputAdd) {
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      if ( accessOutputAdd[filterIdx] ) {
	free( accessOutputAdd[filterIdx] );
      }
    }
    free( accessOutputAdd );
  }
  if ( accessOutputNew) {
    for (filterIdx = 0;
	 filterIdx < anywaveTable->numFilters; 
	 filterIdx ++) {
      if ( accessOutputNew[filterIdx] ) {
	free( accessOutputNew[filterIdx] );
      }
    }
    free( accessOutputNew );
  }

}
