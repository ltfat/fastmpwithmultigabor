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
#define MPD_NULL_CONDITION          0
#define MPD_ITER_CONDITION_REACHED  1
#define MPD_SNR_CONDITION_REACHED  (1 << 1)
#define MPD_NEG_ENERGY_REACHED     (1 << 2)
#define MPD_INCREASING_ENERGY      (1 << 3)
#define MPD_SAVE_HIT_REACHED       (1 << 4)
#define MPD_REPORT_HIT_REACHED     (1 << 5)
#define MPD_ITER_EXHAUSTED         (1 << 6)
#define MPD_FORCED_STOP            (1 << 7)

/* Initialization modes */
#define MPD_WITH_APPROXIMANT MP_TRUE
#define MPD_NO_APPROXIMANT   MP_FALSE


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
  unsigned long int snrHit;
  unsigned long int nextSnrHit;  

  /* Verbose mode: */
  MP_Bool_t verbose;
  unsigned long int reportHit;
  unsigned long int nextReportHit;
 
  /* Intermediate saves: */
  unsigned long int saveHit;
  unsigned long int nextSaveHit;

  /* Manipulated objects */
  MP_Dict_c   *dict;
  MP_Signal_c *signal;
  MP_Book_c   *book;
  MP_Signal_c *residual;
  MP_Signal_c *approximant;

  /* Decay array */
  MP_Var_Array_c<double> decay;

  /* Output file names */
  const char *bookFileName;
  const char *approxFileName;
  const char *resFileName;
  const char *decayFileName;

  /* Convenient global variables */
  unsigned long int numIter;
  double residualEnergy;
  double residualEnergyBefore;
  double initialEnergy;
  double currentSnr;
  unsigned short int state;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  static MP_Mpd_Core_c* init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant );
  static MP_Mpd_Core_c* init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant,
			      MP_Dict_c *dict );
  static MP_Mpd_Core_c* init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant,
			      MP_Dict_c *dict,
			      unsigned long int stopAfterIter,
			      double stopAfterSnr );

private:
  MP_Mpd_Core_c();

public:
  ~MP_Mpd_Core_c();

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

  /* Set the objects */
  MP_Dict_c* set_dict( MP_Dict_c *setDict );

  /* Detach/delete the objects */
  MP_Dict_c* detach_dict( void );
  void delete_dict( void );

  MP_Signal_c* detach_signal( void );
  void delete_signal( void );

  MP_Book_c* detach_book( void );
  void delete_book( void );

  MP_Signal_c* detach_residual( void );
  void delete_residual( void );

  MP_Signal_c* detach_approximant( void );
  void delete_approximant( void );


  /* Get the objects */
  double* get_current_decay_vec( void ) { return( decay.elem ); }


  /* Runtime settings */
  void set_iter_condition( const unsigned long int setIter ) {
    stopAfterIter = setIter; useStopAfterIter = MP_TRUE; }
  void reset_iter_condition() { stopAfterIter = ULONG_MAX; useStopAfterIter = MP_FALSE; }

  void set_snr_condition( const double setSnr );
  void reset_snr_condition( void );

  void set_snr_hit( const unsigned long int setSnrHit ) {
    snrHit = setSnrHit; nextSnrHit = numIter + setSnrHit; }
  void reset_snr_hit() { snrHit = ULONG_MAX; nextSnrHit = ULONG_MAX; }

  void set_save_hit( const unsigned long int setSaveHit,
		      const char* bookFileName,
		     const char* approxFileName,
		     const char* resFileName,
		     const char* decayFileName );
  void reset_save_hit( void );

  void set_report_hit( const unsigned long int setReportHit ) {
    reportHit = setReportHit; nextReportHit = numIter + setReportHit; }
  void reset_report_hit() { reportHit = ULONG_MAX;  nextReportHit = ULONG_MAX; }


  /* Runtime */
  unsigned short int step();
  unsigned long int run();

  /* Infos */
  void info_conditions( void );
  void info_state( void );
  void info_result( void );

  /* Misc */
  void save_result( void );
  MP_Bool_t can_step( void );
  void force_stop( void );

};

#endif /* __mpd_core_h_ */
