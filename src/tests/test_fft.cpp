/******************************************************************************/
/*                                                                            */
/*                                test_fft.cpp                                */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
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
 * $Author: slesage $
 * $Date: 2006-03-15 12:09:32 +0100 (Wed, 15 Mar 2006) $
 * $Revision: 533 $
 *
 */

/** \file test_fft.cpp
 * A file with some code that serves both as an example of how to use the
 *  FFT interface and as a test that it is properly working.
 *
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

#define WIN_SIZE 256
#define OUT_SIZE 257
#define IN_SIZE 512
#define MAX_SIZE 8192

int main(void) {

  MP_Sample_t in[MAX_SIZE];
  MP_Real_t   mag_out[OUT_SIZE];
  FILE *fid;
  unsigned long int i;


  /* Creates the signal */
  for (i=0; i < MAX_SIZE; i++) {
    in[i] = (MP_Sample_t) rand();
  }

  MP_FFT_Interface_c::test(2,DSP_RECTANGLE_WIN, 0.0, in);
  MP_FFT_Interface_c::test(4,DSP_RECTANGLE_WIN, 0.0, in);
  MP_FFT_Interface_c::test(8,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(16,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(32,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(64,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(128,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(256,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(512,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(1024,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(2048,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(4096,DSP_RECTANGLE_WIN, 0.0,in);
  MP_FFT_Interface_c::test(8192,DSP_RECTANGLE_WIN, 0.0,in);

  MP_FFT_Interface_c::test(2,DSP_HAMMING_WIN, 0.0, in);
  MP_FFT_Interface_c::test(4,DSP_HAMMING_WIN, 0.0, in);
  MP_FFT_Interface_c::test(8,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(16,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(32,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(64,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(128,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(256,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(512,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(1024,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(2048,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(4096,DSP_HAMMING_WIN, 0.0,in);
  MP_FFT_Interface_c::test(8192,DSP_HAMMING_WIN, 0.0,in);

  MP_FFT_Interface_c::test(2,DSP_EXPONENTIAL_WIN, 0.0, in);
  MP_FFT_Interface_c::test(4,DSP_EXPONENTIAL_WIN, 0.0, in);
  MP_FFT_Interface_c::test(8,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(16,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(32,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(64,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(128,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(256,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(512,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(1024,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(2048,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(4096,DSP_EXPONENTIAL_WIN, 0.0,in);
  MP_FFT_Interface_c::test(8192,DSP_EXPONENTIAL_WIN, 0.0,in);


  /* 
   * 1/ FFT computation with internally computed window 
   */
  {
    MP_FFT_Interface_c * myFFT = MP_FFT_Interface_c::init( WIN_SIZE, DSP_HAMMING_WIN, 0.0, OUT_SIZE );
    
    /* Execute the FFT */
    myFFT->exec_mag( in, mag_out );
    /* Output to files */
    if ( ( fid = fopen("signals/window_out.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/window_out.dbl" );
      exit(-1);
    }
    mp_fwrite( myFFT->window, sizeof(Dsp_Win_t), WIN_SIZE, fid );
    fclose(fid);

    if ( ( fid = fopen("signals/magnitude_out.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/magnitude_out.dbl" );
      exit(-1);
    }
    mp_fwrite( mag_out, sizeof(MP_Real_t), OUT_SIZE, fid );
    fclose(fid);
    delete myFFT;
  }

  /* 
   * 2/ FFT computation with externally tabulated window
   */

  /* OBSOLETE: all the windows are now tabulated in the window server. */

  /* 
   * 3/ FFT computation for the "2 cosines" signal
   */
  {
    MP_Sample_t buffer[256];
    MP_Real_t   magbuf[512];
    MP_FFT_Interface_c *fft = MP_FFT_Interface_c::init( 256, DSP_HAMMING_WIN, 0.0, 512 );
    if ( ( fid = fopen("signals/2_cosines.flt","r") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in read mode.\n",
	       "signals/2_cosines.flt" );
      exit(-1);
    }
    mp_fread( buffer, sizeof(float), 256, fid );
    fclose(fid);
    fft->exec_mag( buffer, magbuf);
    if ( ( fid = fopen("signals/out_two_peaks.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/out_two_peaks.dbl" );
      exit(-1);
    }
    mp_fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
    fclose(fid);
    delete fft;
  }

  /* 
   * 4/ FFT computation for the whole of the "2 cosines" signal
   */
  {
    MP_Sample_t buffer[8000];
    MP_Real_t   magbuf[512];
    printf("Testing something that makes the fft crash ... \n");fflush(stdout);    
    MP_FFT_Interface_c *fft = MP_FFT_Interface_c::init( 8000, DSP_HAMMING_WIN, 0.0, 512 );
    printf("2\n");fflush(stdout);    
    if ( ( fid = fopen("signals/2_cosines.flt","r") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in read mode.\n",
	       "signals/2_cosines.flt" );
      exit(-1);
    }
    printf("3\n");fflush(stdout);    
    mp_fread( buffer, sizeof(float), 8000, fid );
    printf("4\n");fflush(stdout);    
    fclose(fid);
    printf("5\n");fflush(stdout);    
    fft->exec_mag( buffer, magbuf);
    printf("6\n");fflush(stdout);    
    if ( ( fid = fopen("signals/out_two_peaks_whole.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/out_two_peaks_whole.dbl" );
      exit(-1);
    }
    printf("7\n");fflush(stdout);    
    mp_fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
    printf("8\n");fflush(stdout);    
    fclose(fid);
    printf("9\n");fflush(stdout);    
    delete fft;
    printf("10\n");fflush(stdout);    
  }
  printf("1\n");fflush(stdout);    
  printf("A FFT of the first [%d] samples of the signal in file [%s]\n"
	 "was computed with a [%d] points Hamming window and stored in file [%s]\n",
	 256,"signals/2_cosines.flt",256,"signals/out_two_peaks.dbl");
  printf("2\n");fflush(stdout);    
  printf("The first [%d] points of a FFT of the whole signal in file [%s]\n"
	 "was computed with a [%d] points Hamming window and stored in file [%s]\n",
	 512,"signals/2_cosines.flt",8000,"signals/out_two_peaks_whole.dbl");
  printf("3\n");fflush(stdout);    
  return( 0 );
}
