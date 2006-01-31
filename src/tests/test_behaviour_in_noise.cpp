/******************************************************************************/
/*                                                                            */
/*                        test_behaviour_in_noise.cpp                         */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* R?mi Gribonval                                                             */
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
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#include <mptk.h>
#include <time.h>


int main( void ) {

  MP_Dict_c *dict = NULL;
  MP_Signal_c *sig = NULL;

  /***********************/
  /* Make the dictionary */
  dict = MP_Dict_c::init();
  
  /* For blocks with windows (all but Dirac):
     window length = 1024
     window shift  = 512
     fftSize, windowtype = see below
   */
#define WINDOW DSP_HAMMING_WIN
#define FFTSIZE 1024
  
  /*** Block 1: GABOR */
  add_gabor_block( dict,
		   1024, 512, FFTSIZE,
		   WINDOW, 0.0 );

  /*** Block 2: HARMONIC */
  add_harmonic_block( dict,
		      1024, 512, FFTSIZE,
		      WINDOW, 0.0,
		      0, 4000, 0 );

  /*** Block 3: DIRAC */
  add_dirac_block( dict );

  /*** Block 4: CHIRP */
  add_chirp_block( dict,
		   1024, 512, FFTSIZE,
		   WINDOW, 0.0,
		   1, 1 );


  /*******************/
  /* Make the signal */
  /* 1 channel,
     3072 samples (5 frames of length 1024 shifted by 512),
     sample rate 8kHz */
  sig = MP_Signal_c::init( 1, 3072, 8000 );
  /* Fill the signal with noise of energy 1.0 */
  sig->fill_noise( 1.0 );
  /* Check */
  sig->dump_to_double_file( "sig_noise_3072samp.dbl" );
  /* Plug this dummy signal into the dict */
  dict->copy_signal( sig );


  /**************************************/
  /* Make the individual atom waveforms */
  


  /* Clean the house */
  delete( dict );
  delete( sig );

  return( 0 );
}
