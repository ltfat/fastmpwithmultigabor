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
 * $Date: 2006-03-23 14:57:33 +0100 (Thu, 23 Mar 2006) $
 * $Revision: 545 $
 *
 */

/** \file test_convolution.cpp
 * A file with some code that serves both as an example of how to use the 
 *  convolution class and as a test that it is properly working.
 *
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

#define ANYWAVE_PATH "/udd/slesage/MPTK/trunk/src/tests/signals/"

int is_the_same( unsigned long int a, unsigned long int b);
int is_the_same( double a, double b);
double relative_error( unsigned long int a, unsigned long int b);
double relative_error( double a, double b);

int main( int argc, char **argv ) {

  fprintf(stdout,"\n---TEST CONVOLUTION---\n");
  fflush(stdout);

  char* anywaveTableFileName = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_table.bin";
  MP_Anywave_Table_c* anywaveTable = new MP_Anywave_Table_c(anywaveTableFileName);
  
  unsigned long int filterShift = 0;

  MP_Convolution_Direct_c * dirConv;
  MP_Convolution_FFT_c * fftConv;
  MP_Convolution_Fastest_c * fasConv;
    
  MP_Signal_c* signal = MP_Signal_c::init("/udd/slesage/MPTK/trunk/src/tests/signals/anywave_signal.wav");

  MP_Sample_t* input = signal->channel[0];

  /* print it to check the precision of the data
     fprintf(stdout,"%0.64lf\n",input[0]);
     fprintf(stdout,"%0.64lf\n",anywaveTable->wave[0][0][0]);
  */
 
  unsigned short int chanIdx = 0;
  unsigned short int filterIdx = 0;
  unsigned short int frameIdx = 0;
  
  int verbose = 0;

  /* test of the fft and inverse fft */
/*  MP_FFT_Interface_c* ff = MP_FFT_Interface_c::init( anywaveTable->filterLen,DSP_RECTANGLE_WIN,0.0,anywaveTable->filterLen ) ;
  MP_Real_t* re = (MP_Real_t*)malloc(sizeof(MP_Real_t) * anywaveTable->filterLen);
  MP_Real_t* im = (MP_Real_t*)malloc(sizeof(MP_Real_t) * anywaveTable->filterLen);
  MP_Real_t* out = (MP_Real_t*)malloc(sizeof(MP_Real_t) * anywaveTable->filterLen);
  ff->exec_complex( anywaveTable->wave[0][0], re, im );

  fprintf(stdout,"Filtre\n");
  for (int n=0;n<5;n++)
    fprintf(stdout," element %i - %lg\n",n,anywaveTable->wave[0][0][n]);

  fprintf(stdout,"FFT Filtre\n");
  for (int n=0;n<5;n++)
    fprintf(stdout,"element %i - re=%lg im=%lg\n",n,re[n],im[n]);

  ff->exec_complex_inverse( re, im, out );
  fprintf(stdout,"Filtre reconstruit\n");
  for (int n=0;n<5;n++)
    fprintf(stdout,"element %i - %lg\n",n,out[n]);

  return(1);
*/

  FILE* fid;

  /* parse the arguments */
  if ( (argc > 1) && ( strcmp(argv[1],"-v") == 0 ) ) {
    verbose = 1;
  } else {
    verbose = 0;
  }

  /* 
   * 1/ Inner product between the frames of signal and every filter, with a filterShift of 3
   */
  {   
    fprintf(stdout,"\nExpe 1 : computing inner products (compute_IP() method)\n");

    unsigned long int inputLen = signal->numSamples;

    /* load the true results */
    const char* resFile1 = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_results_1.bin";
    double* outputTrue1;
    fid = fopen( resFile1, "rb");    
    if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) {
      mp_error_msg( "test_convolution", "Cannot read the filterShift from the file [%s].\n", resFile1 );
      fclose(fid);
      return(1);
    }
    unsigned long int numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    unsigned long int numFramesSamples = anywaveTable->numFilters * numFrames;
    if ((outputTrue1 = (double*) malloc(sizeof(double) * numFramesSamples)) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the outputTrue1 array.\n", numFramesSamples );
      fclose(fid);
      return(1);
    } 
    if ( fread ( outputTrue1, sizeof(double), numFramesSamples, fid) != numFramesSamples ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] samples from the file [%s].\n", numFramesSamples, resFile1 );
      fclose(fid);
      return(1);
    }
    fclose(fid);

    
    dirConv = new MP_Convolution_Direct_c(anywaveTable,filterShift );
    fftConv = new MP_Convolution_FFT_c(anywaveTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveTable,filterShift );

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

/*
    fprintf(stdout,"%0.64lf\n",outputTrue1[0]);
    fprintf(stdout,"%0.64lf\n",outputDir1[0]);
    unsigned long int sampleIdx = 0;
    double temp=0.0;
    for (sampleIdx= 0;sampleIdx< 2;sampleIdx++){
      temp = temp + ((double)input[sampleIdx]) * ((double)anywaveTable->wave[0][0][sampleIdx]);
    }
    fprintf(stdout,"   %0.64lf\n",((double)input[0]) * ((double)anywaveTable->wave[0][0][0]));
    fprintf(stdout,"+  %0.64lf\n",((double)input[1]) * ((double)anywaveTable->wave[0][0][1]));
    fprintf(stdout,"=  %0.64lf\n",temp);
*/  
  
    /* Comparison to the true result */
    double dirMaxRelErr = 0.0;
    double fftMaxRelErr = 0.0;
    double fasMaxRelErr = 0.0;
    int dirOK = 1;
    int fftOK = 1;
    int fasOK = 1;
    unsigned long int sampleIdx;
    double seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) {
      for (filterIdx = 0; filterIdx < anywaveTable->numFilters; filterIdx++ ) {
    	sampleIdx = frameIdx*anywaveTable->numFilters + filterIdx;
	if (verbose) {
	  fprintf(stdout, "Frame[%lu]Filter[%lu] - true [%2.8lf] - dir [%2.8lf] - fft [%2.8lf] - fas [%2.8lf]\n",frameIdx,filterIdx,outputTrue1[sampleIdx],outputDir1[sampleIdx],outputFft1[sampleIdx],outputFas1[sampleIdx]);
	}
	if ( relative_error(outputTrue1[sampleIdx],outputDir1[sampleIdx]) > dirMaxRelErr ) dirMaxRelErr = relative_error(outputTrue1[sampleIdx],outputDir1[sampleIdx]);
	if ( relative_error(outputTrue1[sampleIdx],outputFft1[sampleIdx]) > fftMaxRelErr ) fftMaxRelErr = relative_error(outputTrue1[sampleIdx],outputFft1[sampleIdx]);
	if ( relative_error(outputTrue1[sampleIdx],outputFas1[sampleIdx]) > fasMaxRelErr ) fasMaxRelErr = relative_error(outputTrue1[sampleIdx],outputFas1[sampleIdx]);

      }
    }
    if (dirMaxRelErr > seuil) dirOK = 0;
    if (fftMaxRelErr > seuil) fftOK = 0;
    if (fasMaxRelErr > seuil) fasOK = 0;

    if (verbose) {
      fprintf(stdout, "\n");
    }
    
    /* Printing the verdict */

    fprintf(stdout,"\n using direct convolution  : ");
    if (dirOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error [%e])",dirMaxRelErr);

    fprintf(stdout,"\n using fft convolution     : ");
    if (fftOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error [%e])",fftMaxRelErr);

    fprintf(stdout,"\n using fastest convolution : ");
    if (fasOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error [%e])",fasMaxRelErr);


    fflush(stdout);

    delete(dirConv);
    delete(fftConv);
    delete(fasConv);

    free(outputTrue1);
    free(outputDir1);
    free(outputFft1);
    free(outputFas1);

  }

  /* 
   * 2/ Find the filterIdx, and the value corresponding to the max (in
   * energy) inner product between each frame of signal and
   * all the filters, with a filterShift of 3
   */
  {   
    fprintf(stdout,"\n\nExpe 2 : finding the max inner product between each frame and all the filters (compute_max_IP() method)\n");

    unsigned long int inputLen = signal->numSamples;

    /* load the true results */
    const char* resFile2 = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_results_2.bin";
    double* trueAmp2;
    unsigned long int* trueIdx2;
    fid = fopen( resFile2, "rb");    
    if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) {
      mp_error_msg( "test_convolution", "Cannot read the filterShift from the file [%s].\n", resFile2 );
      fclose(fid);
      return(1);
    }
    unsigned long int numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    if ((trueAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the trueAmp2 array.\n", numFrames );
      fclose(fid);
      return(1);
    } 
    if ((trueIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the trueIdx2 array.\n", numFrames );
      fclose(fid);
      return(1);
    } 
    if ( fread ( trueAmp2, sizeof(double), numFrames, fid) != numFrames ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] amplitudes from the file [%s].\n", numFrames, resFile2 );
      fclose(fid);
      return(1);
    }
    if ( fread ( trueIdx2, sizeof(unsigned long int), numFrames, fid) != numFrames ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] indices from the file [%s].\n", numFrames, resFile2 );
      fclose(fid);
      return(1);
    }
    fclose(fid);
    dirConv = new MP_Convolution_Direct_c(anywaveTable,filterShift );
    fftConv = new MP_Convolution_FFT_c(anywaveTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveTable,filterShift );

    /* experimental results */
    double* dirAmp2;
    double* fftAmp2;
    double* fasAmp2;
    unsigned long int* dirIdx2;
    unsigned long int* fftIdx2;
    unsigned long int* fasIdx2;

    if ((dirAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the dirAmp2 array.\n", numFrames );
      return(1);
    } 
    if ((fftAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the fftAmp2 array.\n", numFrames );
      return(1);
    } 
    if ((fasAmp2 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the fasAmp2 array.\n", numFrames );
      return(1);
    } 
    if ((dirIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] unsigned long int blocks for the dirIdx2 array.\n", numFrames );
      return(1);
    } 
    if ((fftIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] unsigned long int blocks for the fftIdx2 array.\n", numFrames );
      return(1);
    } 
    if ((fasIdx2 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] unsigned long int blocks for the fasIdx2 array.\n", numFrames );
      return(1);
    } 

    unsigned long int fromSample = 0;

    dirConv->compute_max_IP( signal, inputLen, fromSample, dirAmp2, dirIdx2 );
    fftConv->compute_max_IP( signal, inputLen, fromSample, fftAmp2, fftIdx2 );
    fasConv->compute_max_IP( signal, inputLen, fromSample, fasAmp2, fasIdx2 );
    
    /* Comparison to the true result */
    int fasOK = 1;
    int dirOK = 1;
    int fftOK = 1;
    double dirAmpMaxRelErr = 0.0;
    double fftAmpMaxRelErr = 0.0;
    double fasAmpMaxRelErr = 0.0;
    double dirIdxMaxRelErr = 0.0;
    double fftIdxMaxRelErr = 0.0;
    double fasIdxMaxRelErr = 0.0;
    double seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) {
      if (verbose) {
	fprintf(stdout, "Frame[%lu] [max nrg|max idx] - true [%2.8lf|%lu] - dir [%2.8lf|%lu] - fft [%2.8lf|%lu] - fas [%2.8lf|%lu]\n",frameIdx,trueAmp2[frameIdx],trueIdx2[frameIdx],dirAmp2[frameIdx],dirIdx2[frameIdx],fftAmp2[frameIdx],fftIdx2[frameIdx],fasAmp2[frameIdx],fasIdx2[frameIdx]);
      }

      if ( relative_error(trueAmp2[frameIdx],dirAmp2[frameIdx]) > dirAmpMaxRelErr ) dirAmpMaxRelErr = relative_error(trueAmp2[frameIdx],dirAmp2[frameIdx]);
      if ( relative_error(trueAmp2[frameIdx],fftAmp2[frameIdx]) > fftAmpMaxRelErr ) fftAmpMaxRelErr = relative_error(trueAmp2[frameIdx],fftAmp2[frameIdx]);
      if ( relative_error(trueAmp2[frameIdx],fasAmp2[frameIdx]) > fasAmpMaxRelErr ) fasAmpMaxRelErr = relative_error(trueAmp2[frameIdx],fasAmp2[frameIdx]);

      if ( relative_error(trueIdx2[frameIdx],dirIdx2[frameIdx]) > dirIdxMaxRelErr ) dirIdxMaxRelErr = relative_error(trueIdx2[frameIdx],dirIdx2[frameIdx]);
      if ( relative_error(trueIdx2[frameIdx],fftIdx2[frameIdx]) > fftIdxMaxRelErr ) fftIdxMaxRelErr = relative_error(trueIdx2[frameIdx],fftIdx2[frameIdx]);
      if ( relative_error(trueIdx2[frameIdx],fasIdx2[frameIdx]) > fasIdxMaxRelErr ) fasIdxMaxRelErr = relative_error(trueIdx2[frameIdx],fasIdx2[frameIdx]);

    }

    if ((dirAmpMaxRelErr > seuil) && (dirIdxMaxRelErr > seuil)) dirOK = 0;
    if ((fftAmpMaxRelErr > seuil) && (fftIdxMaxRelErr > seuil)) fftOK = 0;
    if ((fasAmpMaxRelErr > seuil) && (fasIdxMaxRelErr > seuil)) fasOK = 0;

    if (verbose) {
      fprintf(stdout, "\n");
    }
    
    /* Printing the verdict */

    fprintf(stdout,"\n using direct convolution  : ");
    if (dirOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error: amp [%e] idx [%e])",dirAmpMaxRelErr,dirIdxMaxRelErr);

    fprintf(stdout,"\n using fft convolution     : ");
    if (fftOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error: amp [%e] idx [%e])",fftAmpMaxRelErr,fftIdxMaxRelErr);

    fprintf(stdout,"\n using fastest convolution : ");
    if (fasOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error: amp [%e] idx [%e])",fasAmpMaxRelErr,fasIdxMaxRelErr);

    fflush(stdout);
    
    delete(dirConv);
    delete(fftConv);
    delete(fasConv);

    free(trueAmp2);
    free(dirAmp2);
    free(fftAmp2);
    free(fasAmp2);
    free(trueIdx2);
    free(dirIdx2);
    free(fftIdx2);
    free(fasIdx2);

  }

  /* 
   * 3/ Find the filterIdx, and the value corresponding to the max (in
   * energy) inner product between each frame of signal and
   * all the filters, in the sense of Hilbert, with a filterShift of 3
   */
  {   
    fprintf(stdout,"\n\nExpe 3 : finding the max inner product (in the Hilbert sense) between each frame and all the filters (compute_max_hilbert_IP() method)\n");

    MP_Anywave_Table_c* anywaveRealTable = anywaveTable->copy();
    anywaveRealTable->center_and_denyquist();
    anywaveRealTable->normalize();

    unsigned long int inputLen = signal->numSamples;

    /* load the true results */
    const char* resFile3 = "/udd/slesage/MPTK/trunk/src/tests/signals/anywave_results_3.bin";
    double* trueAmp3;
    unsigned long int* trueIdx3;
    fid = fopen( resFile3, "rb");    
    if ( fread ( &filterShift, sizeof(unsigned long int), 1, fid) != 1 ) {
      mp_error_msg( "test_convolution", "Cannot read the filterShift from the file [%s].\n", resFile3 );
      fclose(fid);
      return(1);
    }
    unsigned long int numFrames = ((inputLen - anywaveTable->filterLen)/filterShift) + 1;
    if ((trueAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the trueAmp3 array.\n", numFrames );
      fclose(fid);
      return(1);
    } 
    if ((trueIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the trueIdx3 array.\n", numFrames );
      fclose(fid);
      return(1);
    } 
    if ( fread ( trueAmp3, sizeof(double), numFrames, fid) != numFrames ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] amplitudes from the file [%s].\n", numFrames, resFile3 );
      fclose(fid);
      return(1);
    }
    if ( fread ( trueIdx3, sizeof(unsigned long int), numFrames, fid) != numFrames ) {
      mp_error_msg( "test_convolution", "Cannot read the [%lu] indices from the file [%s].\n", numFrames, resFile3 );
      fclose(fid);
      return(1);
    }
    fclose(fid);

    fftConv = new MP_Convolution_FFT_c(anywaveRealTable,filterShift );
    fasConv = new MP_Convolution_Fastest_c(anywaveRealTable,filterShift );

    /* experimental results */
    double* fftAmp3;
    double* fasAmp3;
    unsigned long int* fftIdx3;
    unsigned long int* fasIdx3;

    if ((fftAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the fftAmp3 array.\n", numFrames );
      return(1);
    } 
    if ((fasAmp3 = (double*) calloc(numFrames, sizeof(double))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] double blocks for the fasAmp3 array.\n", numFrames );
      return(1);
    } 
    if ((fftIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] unsigned long int blocks for the fftIdx3 array.\n", numFrames );
      return(1);
    } 
    if ((fasIdx3 = (unsigned long int*) calloc(numFrames, sizeof(unsigned long int))) == NULL) {
      mp_error_msg( "test_convolution", "Cannot alloc [%lu] unsigned long int blocks for the fasIdx3 array.\n", numFrames );
      return(1);
    } 

    unsigned long int fromSample = 0;

    fftConv->compute_max_hilbert_IP( signal, inputLen, fromSample, fftAmp3, fftIdx3 );
    fasConv->compute_max_hilbert_IP( signal, inputLen, fromSample, fasAmp3, fasIdx3 );
    
    /* Comparison to the true result */
    int fftOK = 1;
    int fasOK = 1;
    double fftAmpMaxRelErr = 0.0;
    double fasAmpMaxRelErr = 0.0;
    double fftIdxMaxRelErr = 0.0;
    double fasIdxMaxRelErr = 0.0;
    double seuil = 0.00001;

    for (frameIdx = 0; frameIdx < numFrames; frameIdx ++) {
      if (verbose) {
	fprintf(stdout, "Frame[%lu] [max nrg|max idx] - true [%2.8lf|%lu] - fft [%2.8lf|%lu] - fas [%2.8lf|%lu]\n",frameIdx,trueAmp3[frameIdx],trueIdx3[frameIdx],fftAmp3[frameIdx],fftIdx3[frameIdx],fasAmp3[frameIdx],fasIdx3[frameIdx]);
      }

      if ( relative_error(trueAmp3[frameIdx],fftAmp3[frameIdx]) > fftAmpMaxRelErr ) fftAmpMaxRelErr = relative_error(trueAmp3[frameIdx],fftAmp3[frameIdx]);
      if ( relative_error(trueAmp3[frameIdx],fasAmp3[frameIdx]) > fasAmpMaxRelErr ) fasAmpMaxRelErr = relative_error(trueAmp3[frameIdx],fasAmp3[frameIdx]);

      if ( relative_error(trueIdx3[frameIdx],fftIdx3[frameIdx]) > fftIdxMaxRelErr ) fftIdxMaxRelErr = relative_error(trueIdx3[frameIdx],fftIdx3[frameIdx]);
      if ( relative_error(trueIdx3[frameIdx],fasIdx3[frameIdx]) > fasIdxMaxRelErr ) fasIdxMaxRelErr = relative_error(trueIdx3[frameIdx],fasIdx3[frameIdx]);

    }

    if ((fftAmpMaxRelErr > seuil) && (fftIdxMaxRelErr > seuil)) fftOK = 0;
    if ((fasAmpMaxRelErr > seuil) && (fasIdxMaxRelErr > seuil)) fasOK = 0;

    if (verbose) {
      fprintf(stdout, "\n");
    }
    
    /* Printing the verdict */

    fprintf(stdout,"\n using fft convolution     : ");
    if (fftOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error: amp [%e] idx [%e])",fftAmpMaxRelErr,fftIdxMaxRelErr);

    fprintf(stdout,"\n using fastest convolution : ");
    if (fasOK == 1) fprintf(stdout,"[OK]");
    else fprintf(stdout,"[ERROR]");
    fprintf(stdout," (max relative error: amp [%e] idx [%e])",fasAmpMaxRelErr,fasIdxMaxRelErr);

    fflush(stdout);
    
    delete(fftConv);
    delete(fasConv);
    delete(anywaveRealTable);

    free(trueAmp3);
    free(fftAmp3);
    free(fasAmp3);
    free(trueIdx3);
    free(fftIdx3);
    free(fasIdx3);

  }

  delete(anywaveTable);
  fprintf(stdout,"\n");
  return(0);
}


double relative_error( unsigned long int a, unsigned long int b) {
  return( relative_error( (double)a, (double)b ) );
}

double relative_error( double a, double b) {
  double c;
  double d;
  c = a-b;
  if (c < 0) c = -c;
  
  if (a < 0) d = -a;
  else if (a > 0) d = a;
  else {
    if (b < 0) d = -b;
    else if (b > 0) d = b;
    else return( 0.0 );
  }

  return ( c / d  );
}

int is_the_same( unsigned long int a, unsigned long int b) {
  return( is_the_same( (double)a, (double)b ) );
}

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
