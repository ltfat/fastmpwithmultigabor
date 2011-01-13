/******************************************************************************/
/*                                                                            */
/*                                mp_core.h                                   */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
    

    /* Decay array */
    MP_Var_Array_c<double> decay;

    /* Output file names */
    
    /**\brief The name for residual wave file */
    char *resFileName;
    /**\brief The name for file to store decay info */
    char *decayFileName;
	/**\brief Whether we should store the decay info */
	char useDecay;
	
    /* Convenient global variables */
    /**\brief The number of iter */
    unsigned long int numIter;
    /**\brief The residual energy */
    double residualEnergy;
    /**\brief The residual befor iterate energy */
    double residualEnergyBefore;
    /**\brief The initial energy */
    double initialEnergy;
    /**\brief The current SNR */
    double currentSnr;
    /**\brief The state of the core */
    unsigned short int state;

    /* Stopping criteria: */
    /**\brief the number of iter */
    unsigned long int stopAfterIter;
    /**\brief Boolean to define if use stop after iter*/
    MP_Bool_t useStopAfterIter;
    /**\brief SNR value */
    double stopAfterSnr; /* In energy units, not dB. */
    /**\brief Boolean to define if use stop after SNR */
    MP_Bool_t useStopAfterSnr;

    /* Granularity of the snr recomputation: */
    /**\brief SNR hit value */
    unsigned long int snrHit;
    /**\brief Next SNR hit value */
    unsigned long int nextSnrHit;

    /* Verbose mode: */
    /**\brief Boolean to define verbose mode */
    MP_Bool_t verbose;
    /**\brief Report hit value */
    unsigned long int reportHit;
    /*\brief Next report hit value */
    unsigned long int nextReportHit;

    /* Intermediate saves: */
    /**\brief Save hit value */
    unsigned long int saveHit;
    /**\brief The next save hit */
    unsigned long int nextSaveHit;
    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/



  protected:
    /**\brief A protected constructor */
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

      /* File names */
      resFileName   = NULL;
      decayFileName = NULL;
	  useDecay      = MP_FALSE;
      /* Global variables */
      numIter = 0;
      initialEnergy  = 0.0;
      residualEnergy = 0.0;
      residualEnergyBefore = 0.0;
      currentSnr = 0.0;
      state = 0;

    };

  public:
    /**\brief A public destructor */
    virtual ~MP_Abstract_Core_c()
    {
      if (resFileName) 
		  free(resFileName);
      if (decayFileName) 
		  free(decayFileName);

    };
    /***************************/
    /* I/O METHODS             */
    /***************************/

  public:
    /***************************/
    /* OTHER METHODS           */
    /***************************/

    /* Get the objects */
    /**\brief Get the current decay vector
    * \return an array of decay */
    MP_Var_Array_c<double> get_current_decay_vec( void )
    {
      return( decay);
    }
	

    /** \brief Get the current stop after iter
     * \return the stop after iter */
    unsigned long int get_current_stop_after_iter( void )
    {
      return ( stopAfterIter );
    }

    /**\brief Get the state if stop after iter state
     * \return  a boolean */
    MP_Bool_t get_use_stop_after_iter_state( void )
    {
      return ( useStopAfterIter );
    }

    /**\brief Get the current stop after SNR
     * \return the current stop */
    double get_current_stop_after_snr( void )
    {
      return ( stopAfterSnr );
    }

    /**\brief Get the state if stop after snr state
     * \return  a boolean */
    MP_Bool_t get_use_stop_after_snr_state( void )
    {
      return ( useStopAfterSnr );
    }

    /**\brief Get the initial energy
    * \return the initial energy in double */
    double get_initial_energy ( void )
    {
      return ( initialEnergy );
    }

    /**\brief Get the residual energy
     * \return the residual energy in double */

    double get_residual_energy ( void )
    {
      return ( residualEnergy );
    }
    /* Runtime settings */

    /**\brief Set the number of iteration
     * \param setIter unsigned long int the number of iteration */
    void set_iter_condition( const unsigned long int setIter )
    {
      stopAfterIter = setIter;
      useStopAfterIter = MP_TRUE;
    }

    /**\brief set the verbose mode */
    void set_verbose ( void )
    {
      verbose =  MP_TRUE;
    }

	/**\brief set the useDecay mode */
    void set_use_decay ( void )
    {
      useDecay =  MP_TRUE;
    }

    /**\brief Reset the number of iter used for iteration */
    void reset_iter_condition()
    {
      stopAfterIter = ULONG_MAX;
      useStopAfterIter = MP_FALSE;
    }

    /**\brief Set the  SNR condition
         * \param setSnr double the snr */
    void set_snr_condition( const double setSnr )
    {
      stopAfterSnr = pow( 10.0, setSnr/10 );
      useStopAfterSnr = MP_TRUE;
      if ( snrHit == ULONG_MAX ) set_snr_hit( 1 );
    }
    /**\brief Reset the SNR condition */
    void reset_snr_condition( void )
    {
      stopAfterSnr = 0.0;
      useStopAfterSnr = MP_FALSE;
    }
    /**\brief Set the setting for the SNR hit
         * \param setReportHit unsigned long int to set the the number of iteration when the report is done    */
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
    /**\brief Reset the verbose mode */
    void reset_verbose ( void )
    {
      verbose =  MP_FALSE;
    }
    /**\brief Reset the setting for SNR hit */
    void reset_snr_hit()
    {
      snrHit = ULONG_MAX;
      nextSnrHit = ULONG_MAX;
    }

    /**\brief Reset the setting for save hit */
    void reset_save_hit( void )
    {
      saveHit = ULONG_MAX;
      nextSaveHit = ULONG_MAX;
    }

    /**\brief Set the setting for the report hit
     * \param setReportHit unsigned long int to set the the number of iteration when the report is done    */
    void set_report_hit( const unsigned long int setReportHit )
    {
      reportHit = setReportHit;
      nextReportHit = numIter + setReportHit;
    }

    /**\brief Reset the setting for report hit */
    void reset_report_hit()
    {
      reportHit = ULONG_MAX;
      nextReportHit = ULONG_MAX;
    }


    /* Runtime */
    virtual unsigned short int step( void ) = 0;
    /*************************/
    /**\brief Make one MP iteration */
    unsigned long int run()
    {

      /* Reset the state info */
      state = 0;

      /* Loop while the return state is NULL */
      while ( step() == 0 );

      /* Return the last state */
      return( numIter );
    }

    /**\brief info on the state of the core */
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

    /* Misc */
    /** \brief Save the result
    *  */
    virtual void save_result( void ) = 0;
    /** \brief test if can step
    *  */
    virtual MP_Bool_t can_step( void ) =0 ;

    /** \brief print informations on the result of decomposition
    *  */
    virtual void info_result( void ) = 0;

    /** \brief print informations on the setting of decomposition
     *  */
    virtual void info_conditions( void ) =0;

    /** \brief Save the decay file
     * \param decayFileName the name of the deacay file
     * \return unsigned long int the number of iter saved
     *  */
    unsigned long int save_decay(const char * fileName )
    {
      return decay.save( fileName );
    }
    /** \brief Get the num of iter
     * \return unsigned long int the number of iter
     *  */
    unsigned long int get_num_iter()
    {
      return numIter;
    }
    /********************************/
    /*\brief Set the state to forced stop */
    void force_stop()
    {
      state = MP_FORCED_STOP;
    }

  };

#endif /*MP_CORE_H_*/
