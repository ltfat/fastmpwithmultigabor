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

  char* func = "test";

  MP_Dict_c *dict = NULL;
  MP_Signal_c *sig = NULL;

  MP_Signal_c *gsig = NULL;
  MP_Signal_c *hsig = NULL;
  MP_Signal_c *dsig = NULL;
  MP_Signal_c *csig = NULL;

  MP_Gabor_Atom_c    *gatom = NULL;
  MP_Harmonic_Atom_c *hatom = NULL;
  MP_Dirac_Atom_c    *datom = NULL;
  MP_Gabor_Atom_c    *catom = NULL; /* <- Chirp atom */

  unsigned long int i;

  //set_debug_mask( 0 );

  /***********************/
  /* Make the dictionary */
  /***********************/
  dict = MP_Dict_c::init();
  
  /* For blocks with windows (all but Dirac):
     window length = 1024
     window shift  = 512
     fftSize, windowtype = see below
   */
#define WINDOW DSP_HANNING_WIN
#define FFTSIZE 1024
  
  /*** Block 1: GABOR */
  add_gabor_block( dict,
		   1024, 512, FFTSIZE,
		   WINDOW, 0.0 );

  /*** Block 2: HARMONIC */
  add_harmonic_block( dict,
		      1024, 512, FFTSIZE,
		      WINDOW, 0.0, 
		      1, 4000, 0 );

  /*** Block 3: DIRAC */
  add_dirac_block( dict );

  /*** Block 4: CHIRP */
  add_chirp_block( dict,
		   1024, 512, FFTSIZE,
		   WINDOW, 0.0,
		   1, 1 );


  /*******************/
  /* Make the signal */
  /*******************/
  func = "noise";
  mp_info_msg( func, "****************************************\n" );
  mp_info_msg( func, "NOISE REPORT:\n" );
  mp_info_msg( func, "****************************************\n" );
  /* 1 channel,
     3072 samples (5 frames of length 1024 shifted by 512),
     sample rate see below */
#define SAMPLE_RATE 8000
  sig = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
  /* Fill the signal with noise of energy 1.0 */
  //set_debug_mask( MP_DEBUG_GENERAL );
  sig->fill_noise( 1.0 );
  /* Check */
  sig->dump_to_double_file( "signals/sig_noise_3072samp_8kHz.dbl" );
  sig->wavwrite( "signals/sig_noise_3072samp_8kHz.wav" );
  sig->info();
  /* Plug this dummy signal into the dict */
  dict->copy_signal( sig );


  /**************************************/
  /* Make the individual atoms          */
  /**************************************/
  func = "atom";
  mp_info_msg( func, "****************************************\n" );
  mp_info_msg( func, "ATOM REPORT:\n" );
  mp_info_msg( func, "****************************************\n" );
  /* Gabor: */
  dict->block[0]->create_atom( (MP_Atom_c**)(&gatom), 2, 128 );
  gatom->amp[0]   = 1.0;
  gatom->phase[0] = 0.0;
  gatom->info( stderr );
  /* Harmo: */
  dict->block[1]->create_atom( (MP_Atom_c**)(&hatom), 2, 576 ); /* 576 = 512 (last gabor) + 64 (Nyquist/16) */
  hatom->amp[0]   = 1.0;
  hatom->phase[0] = 0.0;
  for ( i = 1; i < hatom->numPartials; i++) {
    hatom->partialAmp[0][i] = 1.0;
    //hatom->partialPhase[0][i] = 0.0;
    hatom->partialPhase[0][i] += 0.01;
  }
  hatom->info( stderr );
  /* Dirac: */
  dict->block[2]->create_atom( (MP_Atom_c**)(&datom), 1536, 0 );
  datom->amp[0]   = 1.0;
  datom->info( stderr );
  /* Chirp: */
  dict->block[3]->create_atom( (MP_Atom_c**)(&catom), 2, 128 );
  catom->amp[0]   = 1.0;
  catom->phase[0] = 0.0;
  catom->chirp = 0.000025;
  catom->info( stderr );

  /**************************************/
  /* Make the individual atom waveforms */
  /**************************************/
  mp_info_msg( func, "****************************************\n" );
  mp_info_msg( func, "ENERGIES:\n" );
  mp_info_msg( func, "****************************************\n" );

  gsig = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
  //gatom->build_waveform( gsig->storage + 1024 );
  //gsig->refresh_energy();
  gatom->substract_add( NULL, gsig );
  gsig->dump_to_double_file( "signals/gatom_3072samp_8kHz.dbl" );
  gsig->wavwrite( "signals/gatom_3072samp_8kHz.wav" );
  mp_info_msg( func, "GABOR ATOM: energy before norm [%g]\n", gsig->energy );
  gsig->apply_gain( 1.0 / gsig->l2norm() );
  mp_info_msg( func, "        |-: energy  after norm [%g]\n", gsig->energy );
  

  hsig = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
  //hatom->build_waveform( hsig->storage + 1024 );
  //hsig->refresh_energy();
  hatom->substract_add( NULL, hsig );
  hsig->dump_to_double_file( "signals/hatom_3072samp_8kHz.dbl" );
  hsig->wavwrite( "signals/hatom_3072samp_8kHz.wav" );
  mp_info_msg( func, "HARMO ATOM: energy before norm [%g]\n", hsig->energy );
  hsig->apply_gain( 1.0 / hsig->l2norm() );
  mp_info_msg( func, "        |-: energy  after norm [%g]\n", hsig->energy );

  dsig = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
  //datom->build_waveform( dsig->storage + 1024 );
  //dsig->refresh_energy();
  datom->substract_add( NULL, dsig );
  dsig->dump_to_double_file( "signals/datom_3072samp_8kHz.dbl" );
  dsig->wavwrite( "signals/datom_3072samp_8kHz.wav" );
  mp_info_msg( func, "DIRAC ATOM: energy before norm [%g]\n", dsig->energy );
  mp_info_msg( func, "        |-: energy  after norm [%g]\n", dsig->energy );

  csig = MP_Signal_c::init( 1, 3072, SAMPLE_RATE );
  //catom->build_waveform( csig->storage + 1024 );
  //csig->refresh_energy();
  catom->substract_add( NULL, csig );
  csig->dump_to_double_file( "signals/catom_3072samp_8kHz.dbl" );
  csig->wavwrite( "signals/catom_3072samp_8kHz.wav" );
  mp_info_msg( func, "CHIRP ATOM: energy before norm [%g]\n", csig->energy );
  csig->apply_gain( 1.0 / csig->l2norm() );
  mp_info_msg( func, "        |-: energy  after norm [%g]\n", csig->energy );


  /*******************/
  /* Clean the house */
  /*******************/
  delete( dict );
  delete( sig );

  delete( gsig );
  delete( hsig );
  delete( dsig );
  delete( csig );

  delete( gatom );
  delete( hatom );
  delete( datom );
  delete( catom );

  return( 0 );
}
