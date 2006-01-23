/******************************************************************************/
/*                                                                            */
/*                                mpd_core.h                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
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

/***************************************/
/*                                     */
/* DEFINITION OF THE MPD_CORE CLASS    */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#include <mptk.h>

#ifndef __mpd_core_h_
#define __mpd_core_h_

/* Returned events */
#define MPD_NULL_CONDITION         0
#define MPD_ITER_CONDITION_REACHED 1
#define MPD_SNR_CONDITION_REACHED  2
#define MPD_SAVE_HIT_REACHED       3
#define MPD_REPORT_HIT_REACHED     4
#define MPD_ITER_EXHAUSTED         5


/***********************/
/* MPD_CORE CLASS      */
/***********************/
/** \brief The MP_Mpd_Core_c class implements a standard run of
 *  the Matching Pursuit Decomposition (mpd) utility.
 */
class MP_Mpd_Core_c {

  /********/
  /* DATA */
  /********/

public:

  /* Stopping criteria: */
  unsigned long int stopAfterIter;
  MP_Bool_t useStopAfterIter;
  double stopAfterSnr; /* In energy units, not dB. */
  MP_Bool_t useStopAfterSnr;

  /* Granularity of the snr recomputation: */
  unsigned long int snrFrequency;
  unsigned long int nextSnrHit;  

  /* Verbose mode: */
  MP_Bool_t verbose;
  unsigned long int reportFrequency;
  unsigned long int nextReportHit;
 
  /* Intermediate saves: */
  unsigned long int saveFrequency;
  unsigned long int nextSaveHit;

  /* Manipulated objects */
  MP_Dict_c *dict;
  MP_Dict_c *sig;
  MP_Book_c *book;

  /* Decay array */
  MP_Var_array_c<double> decay;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  static MP_Mpd_Run_c::init( MP_Dict_c *dict, MP_Signal_c *sig, MP_Book_c *book );
  static MP_Mpd_Run_c::init( MP_Dict_c *dict, MP_Signal_c *sig, MP_Book_c *book,
			     unsigned long int stopAfterIter,
			     double stopAfterSnr );

  /***************************/
  /* I/O METHODS             */
  /***************************/

public:
  /* int save( char *fileName );
     int load( char *fileName );
  */

  /***************************/
  /* OTHER METHODS           */
  /***************************/

  /* Settings */
  MP_Dict_c* set_dict( MP_Dict_c *setDict );
  MP_Signal_c* set_signal( MP_Signal_c *setSig );
  MP_Book_c* set_book( MP_Book_c *setBook );

#define set_iter_condition( A ) { stopAfterIter = A; }
#define reset_iter_condition()  { stopAfterIter = ULONG_MAX; }

#define set_snr_condition( SNR ) { stopAfterSnr = pow( 10.0, SNR/10 ); }
#define reset_snr_condition()    { stopAfterSnr = 0.0; }

#define set_snr_frq( A )  { snrFrequency = A; }
#define reset_snr_freq()  { snrFrequency = ULONG_MAX; }

#define set_save_freq( A ) { saveFrequency = A; }
#define reset_save_freq()  { saveFrequency = ULONG_MAX; }

#define set_report_freq( A ) { reportFrequency = A; }
#define reset_report_freq()  { reportFrequency = ULONG_MAX; }

  /* Runtime */
  int resume();
  int step();

  /* Get results */
#define get_current_book() book
#define get_current_signal() signal
#define get_current_decay_vec() decay.elem

}

#endif /* __mpd_core_h_ */
