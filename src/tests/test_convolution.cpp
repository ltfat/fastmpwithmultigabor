/******************************************************************************/
/*                                                                            */
/*                           test_convolution.cpp                             */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Mar 14 2006 */
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
 * $Author: remi $
 * $Date$
 * $Revision$
 *
 */

/** \file convolution_fft.cpp
 * A file with some code that serves both as an example of how to use the 
 *  convolution class and as a test that it is properly working.
 *
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

int is_the_same( double a, double b) {
  double c;
  double d;
  double seuil = 0.00001;
  c = a-b;
  if (c < 0) c = -c;
  
  if (a < 0) d = -a;
  else if (a > 0) d = a;
  else {
    if (b < 0) d = -b;
    else if (b > 0) d = b;
    else return(1);
  }

  if ( c / d  > seuil ) return(0);
  else return(1);
}

int main(void) {

  fprintf(stdout,"\n---TEST CONVOLUTION---\n");
  fflush(stdout);

  char* anywavePath = "/udd/slesage/MPTK/trunk/src/tests/signals/";
  char* anywaveTableFileName = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_table.bin";
  MP_Anywave_Table_c* anywaveTable = new MP_Anywave_Table_c(anywaveTableFileName);
  
  unsigned long int filterShift = 64;

  MP_Convolution_Direct_c * dirConv = new MP_Convolution_Direct_c(anywaveTable,filterShift );
  MP_Convolution_FFT_c * fftConv = new MP_Convolution_FFT_c(anywaveTable,filterShift );
  MP_Convolution_Fastest_c * fasConv = new MP_Convolution_Fastest_c(anywaveTable,filterShift );
    
  MP_Signal_c* signal = MP_Signal_c::init("/udd/slesage/MPTK/trunk/src/tests/signals/anywave_signal.wav");

  MP_Sample_t* input = signal->channel[0];

  unsigned short int chanIdx = 0;
  unsigned short int filterIdx = 0;
  unsigned short int frameIdx = 0;
  
  int verbose = 0;

  FILE* fid;


  /* 
   * 1/ Inner product between the frames of signal and every filter, with a filterShift of 3
   */
  {   
    fprintf(stdout,"\nExpe 1 : computing inner products (compute_IP() method)\n\n");

    unsigned long int inputLen = signal->numSamples;
    unsigned long int numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    unsigned long int numFramesSamples = anywaveTable->numFilters * numFrames;

    /* load the true results */
    const char* resFile1 = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_results_1.bin";
    double* outputTrue1;
    if ((outputTrue1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the outputTrue1 array.\n", numFramesSamples );
      return(1);
    } 
    fid = fopen( resFile1, "rb");    
    if ( fread ( outputTrue1, sizeof(double), numFramesSamples, fid) != numFramesSamples ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] samples from the file [%s].\n", numFramesSamples, resFile1 );
      return(1);
    }
    fclose(fid);

    /* experimental results */
    double* outputFas1;
    double* outputDir1;
    double* outputFft1;
    if ((outputFas1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the outputFas1 array.\n", numFramesSamples );
      return(1);
    } 
    if ((outputDir1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the outputDir1 array.\n", numFramesSamples );
      return(1);
    } 
    if ((outputFft1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the outputFft1 array.\n", numFramesSamples );
      return(1);
    } 


    dirConv->compute_IP( input, inputLen, chanIdx, &outputDir1 );
    fftConv->compute_IP( input, inputLen, chanIdx, &outputFft1 );
    fasConv->compute_IP( input, inputLen, chanIdx, &outputFas1 );
    
    /* Comparison to the true result */
    int fasOK = 1;
    int dirOK = 1;
    int fftOK = 1;
    unsigned long int sampleIdx;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) {
      for (filterIdx = 0; filterIdx < anywaveTable->numFilters; filterIdx++ ) {
    	sampleIdx = frameIdx + filterIdx*numFrames;
	if (verbose) {
	  fprintf(stdout, "Frame[%lu]Filter[%lu] - true [%0.5lg] - dir [%0.5lg] - fft [%0.5lg] - fas [%0.5lg]\n",frameIdx,filterIdx,outputTrue1[sampleIdx],outputDir1[sampleIdx],outputFft1[sampleIdx],outputFas1[sampleIdx]);
	}
	if (is_the_same(outputTrue1[sampleIdx],outputDir1[sampleIdx]) == 0) dirOK = 0;
	if (is_the_same(outputTrue1[sampleIdx],outputFft1[sampleIdx]) == 0) fftOK = 0;
	if (is_the_same(outputTrue1[sampleIdx],outputFas1[sampleIdx]) == 0) fasOK = 0;
      }
    }
    
    /* Printing the verdict */

    fprintf(stdout," using direct convolution  : ");
    if (dirOK == 1) fprintf(stdout,"[OK]\n");
    else fprintf(stdout,"[ERROR]\n");

    fprintf(stdout," using fft convolution     : ");
    if (fftOK == 1) fprintf(stdout,"[OK]\n");
    else fprintf(stdout,"[ERROR]\n");

    fprintf(stdout," using fastest convolution : ");
    if (fasOK == 1) fprintf(stdout,"[OK]\n");
    else fprintf(stdout,"[ERROR]\n");

    fflush(stdout);
  }

  return(0);
}

