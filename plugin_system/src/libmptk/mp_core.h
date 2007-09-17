/******************************************************************************/
/*                                                                            */
/*                                mp_core.h                                   */
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


/***********************************************/
/*                                             */
/* DEFINITION OF THE Abstract MP_CORE CLASS    */
/*                                             */
/***********************************************/

#include <mptk.h>
#include <vector>
#include <math.h>

#ifndef MP_CORE_H_
#define MP_CORE_H_

/* Returned events */
#define MP_NULL_CONDITION          0
#define MP_ITER_CONDITION_REACHED  1
#define MP_SNR_CONDITION_REACHED  (1 << 1)
#define MP_NEG_ENERGY_REACHED     (1 << 2)
#define MP_INCREASING_ENERGY      (1 << 3)
#define MP_SAVE_HIT_REACHED       (1 << 4)
#define MP_REPORT_HIT_REACHED     (1 << 5)
#define MP_ITER_EXHAUSTED         (1 << 6)
#define MP_FORCED_STOP            (1 << 7)

/* Initialization modes */
#define MP_WITH_APPROXIMANT MP_TRUE
#define MP_NO_APPROXIMANT   MP_FALSE


/***********************/
/* MPD_CORE CLASS      */
/***********************/
/** \brief The MP_Mpd_Core_c class implements a standard run of
 *  the Matching Pursuit Decomposition (mpd) utility.
 */
class MP_Abstract_Core_c
  {

    /********/
    /* DATA */
    /********/

protected:

/* Manipulated objects */
    MP_Signal_c *residual;
    MP_Signal_c *approximant;

    /* Decay array */
    MP_Var_Array_c<double> decay;
       
    /* Output file names */
    char *approxFileName;
    char *resFileName;
    char *decayFileName;

 /* Convenient global variables */
    unsigned long int numIter;
    double residualEnergy;
    double residualEnergyBefore;
    double initialEnergy;
    double currentSnr;
    unsigned short int state;
    
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
    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/



  protected:
    MP_Abstract_Core_c ()
    {

      /* Stopping criteria: */
      stopAfterIter = ULONG_MAX;
      useStopAfterIter = MP_FALSE;
      stopAfterSnr = 0.0; /* In energy units, not dB. */
      useStopAfterSnr = MP_FALSE;

      /* Granularity of the snr recomputation: */
      snrHit = ULONG_MAX;
      nextSnrHit = ULONG_MAX;

      /* Verbose mode: */
      verbose = MP_FALSE;
      reportHit = ULONG_MAX;
      nextReportHit = ULONG_MAX;

      /* Intermediate saves: */
      saveHit = ULONG_MAX;
      nextSaveHit = ULONG_MAX;

      /* Manipulated objects */
      residual = NULL;
      approximant = NULL;
      
      
      /* File names */
      approxFileName   = NULL;
      resFileName   = NULL;
      decayFileName = NULL;

      /* Global variables */
      numIter = 0;
      initialEnergy  = 0.0;
      residualEnergy = 0.0;
      residualEnergyBefore = 0.0;
      currentSnr = 0.0;
      state = 0;

    };

  public:
    virtual ~MP_Abstract_Core_c()
    {
    if (approxFileName) free(approxFileName);
    if (resFileName) free(resFileName);
    if (decayFileName) free(decayFileName);
    	
    };
/***************************/
/* I/O METHODS             */
/***************************/

public:
/***************************/
/* OTHER METHODS           */
/***************************/

/* Get the objects */
double* get_current_decay_vec( void )
{
  return( decay.elem );
}

unsigned long int get_current_stop_after_iter( void )
{
return ( stopAfterIter ); 
}
MP_Bool_t get_use_stop_after_iter_state( void )
{
return ( useStopAfterIter );
}
double get_current_stop_after_snr( void )
{
return ( stopAfterSnr );
}
MP_Bool_t get_use_stop_after_snr_state( void )
{
return ( useStopAfterSnr );
}
double get_initial_energy ( void )
{
return ( initialEnergy );
}
double get_residual_energy ( void )
{
return ( residualEnergy );
}
/* Runtime settings */
void set_iter_condition( const unsigned long int setIter )
{
  stopAfterIter = setIter;
  useStopAfterIter = MP_TRUE;
}
void set_verbose ( void )
{
  verbose =  MP_TRUE;
}
void reset_iter_condition()
{
  stopAfterIter = ULONG_MAX;
  useStopAfterIter = MP_FALSE;
}

void set_snr_condition( const double setSnr )
{
  stopAfterSnr = pow( 10.0, setSnr/10 );
  useStopAfterSnr = MP_TRUE;
  if ( snrHit == ULONG_MAX ) set_snr_hit( 1 );
}
void reset_snr_condition( void )
{
  stopAfterSnr = 0.0;
  useStopAfterSnr = MP_FALSE;
}

void set_snr_hit( const unsigned long int setSnrHit )
{

  const char* func = "Set snr hit";

  if ( setSnrHit  < 0.0 )
    {
      mp_error_msg( func, "The target SNR [%g] can't be negative.\n",
                    setSnrHit );
      snrHit = ULONG_MAX;
      nextSnrHit = ULONG_MAX;
    }
  snrHit = setSnrHit;
  nextSnrHit = numIter + setSnrHit;
}
void reset_verbose ( void )
{
  verbose =  MP_FALSE;
}
void reset_snr_hit()
{
  snrHit = ULONG_MAX;
  nextSnrHit = ULONG_MAX;
}

void reset_save_hit( void )
{
  saveHit = ULONG_MAX;
  nextSaveHit = ULONG_MAX;
}

void set_report_hit( const unsigned long int setReportHit )
{
  reportHit = setReportHit;
  nextReportHit = numIter + setReportHit;
}
void reset_report_hit()
{
  reportHit = ULONG_MAX;
  nextReportHit = ULONG_MAX;
}

/* Runtime settings */
virtual void plug_approximant( MP_Signal_c *approximant ) = 0;

/* Runtime */
virtual unsigned short int step( void ) = 0;
/*************************/
/* Make one MP iteration */
unsigned long int run() {

  /* Reset the state info */
  state = 0;

  /* Loop while the return state is NULL */
  while( step() == 0 );

  /* Return the last state */
  return( numIter );
}


void info_state( void )
{

  const char* func = "Current state";

  if ( state & MP_ITER_CONDITION_REACHED )  mp_info_msg( func,
        "Reached the target number of iterations [%lu].\n",
        numIter );

  if ( state & MP_SNR_CONDITION_REACHED )  mp_info_msg( func,
        "The current SNR [%g] reached or passed"
        " the target SNR [%g] in [%lu] iterations.\n",
        10*log10( currentSnr ), 10*log10( stopAfterSnr ), numIter );

  if ( state & MP_NEG_ENERGY_REACHED )  mp_info_msg( func,
        "The current residual energy [%g] has gone negative"
        " after [%lu] iterations.\n",
        residualEnergy, numIter );

  if ( state & MP_INCREASING_ENERGY )  mp_info_msg( func,
        "The residual energy is increasing [%g -> %g]"
        " after [%lu] iterations.\n",
        residualEnergyBefore, residualEnergy, numIter );

  if ( state & MP_ITER_EXHAUSTED ) mp_info_msg( func,
        "Reached the absolute maximum number of iterations [%lu].\n",
        numIter );

  if ( state & MP_FORCED_STOP ) mp_info_msg( func,
        "Forced stop at iteration [%lu].\n",
        numIter );

}

/* Misc */

virtual void save_result( void ) = 0;
virtual MP_Bool_t can_step( void ) =0 ;
virtual void info_result( void ) = 0;
virtual void info_conditions( void ) =0;

unsigned long int save_decay(const char * decayFileName ){
return decay.save( decayFileName );
}

unsigned long int get_num_iter(){
return numIter;
}
/********************************/
/* Set the state to forced stop */
void force_stop() {
  state = MP_FORCED_STOP;
}

};

#endif /*MP_CORE_H_*/
