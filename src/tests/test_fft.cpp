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
 * $Author$
 * $Date$
 * $Revision$
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

int main(void) {

  MP_Sample_t in[IN_SIZE];
  MP_Real_t   mag_out[OUT_SIZE];
  FILE *fid;
  unsigned long int i;

  MP_FFT_Interface_c myFFT( WIN_SIZE, DSP_HAMMING_WIN, 0.0, OUT_SIZE );

  /* Creates the signal */
  for (i=0; i < IN_SIZE; i++) {
    in[i] = (MP_Sample_t) rand();
  }

  /* 
   * 1/ FFT computation with internally computed window 
   */

  /* Execute the FFT */
  myFFT.exec_mag( in, mag_out );

  /* Output to files */
  if ( ( fid = fopen("signals/window_out.dbl","w") ) == NULL ) {
    fprintf( stderr, "Can't open file [%s] in write mode.\n",
	     "signals/window_out.dbl" );
    exit(-1);
  }
  fwrite( myFFT.window, sizeof(Dsp_Win_t), WIN_SIZE, fid );
  fclose(fid);

  if ( ( fid = fopen("signals/magnitude_out.dbl","w") ) == NULL ) {
    fprintf( stderr, "Can't open file [%s] in write mode.\n",
	     "signals/magnitude_out.dbl" );
    exit(-1);
  }
  fwrite( mag_out, sizeof(MP_Real_t), OUT_SIZE, fid );
  fclose(fid);

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
    MP_FFT_Interface_c fft( 256, DSP_HAMMING_WIN, 0.0, 512 );

    if ( ( fid = fopen("signals/2_cosines.flt","r") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in read mode.\n",
	       "signals/2_cosines.flt" );
      exit(-1);
    }
    fread( buffer, sizeof(float), 256, fid );
    fclose(fid);

    fft.exec_mag( buffer, magbuf);

    if ( ( fid = fopen("signals/out_two_peaks.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/out_two_peaks.dbl" );
      exit(-1);
    }
    fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
    fclose(fid);
  }

  /* 
   * 4/ FFT computation for the whole of the "2 cosines" signal
   */
  {
    MP_Sample_t buffer[8000];
    MP_Real_t   magbuf[512];
    MP_FFT_Interface_c fft( 8000, DSP_HAMMING_WIN, 0.0, 512 );

    if ( ( fid = fopen("signals/2_cosines.flt","r") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in read mode.\n",
	       "signals/2_cosines.flt" );
      exit(-1);
    }
    fread( buffer, sizeof(float), 8000, fid );
    fclose(fid);

    fft.exec_mag( buffer, magbuf);

    if ( ( fid = fopen("signals/out_two_peaks_whole.dbl","w") ) == NULL ) {
      fprintf( stderr, "Can't open file [%s] in write mode.\n",
	       "signals/out_two_peaks_whole.dbl" );
      exit(-1);
    }
    fwrite( magbuf, sizeof(MP_Real_t), 512, fid );
    fclose(fid);
  }
  printf("A FFT of the first [%d] samples of the signal in file [%s]\n"
	 "was computed with a [%d] points Hamming window and stored in file [%s]\n",
	 256,"signals/2_cosines.flt",256,"signals/out_two_peaks.dbl");
  printf("The first [%d] points of a FFT of the whole signal in file [%s]\n"
	 "was computed with a [%d] points Hamming window and stored in file [%s]\n",
	 512,"signals/2_cosines.flt",8000,"signals/out_two_peaks_whole.dbl");
  return( 0 );
}
