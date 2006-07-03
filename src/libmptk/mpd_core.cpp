/******************************************************************************/
/*                                                                            */
/*                              mpd_core.cpp                                  */
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
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#include <mptk.h>
#include "mpd_core.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************/
/* Factory functions: */
/********************/
/* - signal+book only: */
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant ) {

  const char* func = "MP_Mpd_Core_c::init(2 args)";
  MP_Mpd_Core_c* newCore;

  /* Check for NULL entries */
  if ( signal == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_Mpd_Core_c from a NULL signal.\n" );
    return( NULL );
  }
  if ( book == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_Mpd_Core_c from a NULL book.\n" );
    return( NULL );
  }

  /* Check the sample rate compatibility */
  if ( signal->sampleRate != book->sampleRate ) {
    mp_warning_msg( func,
		    "The new signal has a sample rate [%i] different"
		    " from the book's sample rate [%i]."
		    " Defaulting all to the signal sample rate.\n",
		    signal->sampleRate, book->sampleRate );
    book->sampleRate = signal->sampleRate;
  }

  /* Instantiate and check */
  newCore = new MP_Mpd_Core_c();
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );
  }

  /* Plug the book */
  newCore->book = book;

  /* Plug the signal */
  newCore->signal = signal;
  newCore->residual = new MP_Signal_c( *signal );
  newCore->residualEnergy = newCore->initialEnergy = signal->energy;
  newCore->decay.clear();
  newCore->decay.append( newCore->initialEnergy );

  if ( use_approximant ) {
    /* Initialize the approximant */
    newCore->approximant = MP_Signal_c::init( signal->numChans,
					      signal->numSamples,
					      signal->sampleRate );
    if ( newCore->approximant == NULL ) {
      mp_error_msg( func, "Failed to create an approximant in the new mpd core."
		    " Returning NULL.\n" );
      delete( newCore );
      return( NULL );
    }
    /* Rebuild from the book */
    book->substract_add( NULL, newCore->approximant, NULL );
  }
  else newCore->approximant = NULL;

  return( newCore );
}


/***********************/
/* - signal+book+dict: */
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant,
				    MP_Dict_c *dict ) {

  const char* func = "MP_Mpd_Core_c::init(3 args)";
  MP_Mpd_Core_c* newCore;

  /* Instantiate and check */
  newCore = MP_Mpd_Core_c::init( signal, book, use_approximant );
  if ( newCore == NULL ) {
    return( NULL );
  }

  if ( newCore->set_dict( dict ) ) {
    mp_error_msg( func, "Failed to plug the dictionary.\n" );
    delete( newCore );
    return( NULL );    
  }

  return( newCore );
}


/**********************************/
/* - signal+book+dict+conditions: */
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book, const char use_approximant,
				    MP_Dict_c *dict,
				    unsigned long int stopAfterIter,
				    double stopAfterSnr ) {

  const char* func = "MP_Mpd_Core_c::init(5 args)";
  MP_Mpd_Core_c* newCore;

  /* Check the input parameters */
  if ( stopAfterSnr < 0.0 ) {
    mp_error_msg( func, "The target SNR [%g] can't be negative.\n",
		  stopAfterSnr );
    return( NULL );
  }

  /* Instantiate and check */
  newCore =  MP_Mpd_Core_c::init( signal, book, use_approximant, dict );
  if ( newCore == NULL ) {
    return( NULL );
  }

  /* Set the parameters */
  if ( stopAfterIter != 0 ) newCore->set_iter_condition( stopAfterIter );
  if ( stopAfterSnr != 0.0 ) newCore->set_snr_condition( stopAfterSnr );

  return( newCore );
}


/********************/
/* NULL constructor */
MP_Mpd_Core_c::MP_Mpd_Core_c() {

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
  dict = NULL;
  signal = NULL;
  book = NULL;
  residual = NULL;
  approximant = NULL;

  /* File names */
  bookFileName  = NULL;
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

}

/**************/
/* Destructor */
MP_Mpd_Core_c::~MP_Mpd_Core_c() {
  if ( dict )        delete( dict );
  if ( signal )      delete( signal );
  if ( book )        delete( book );
  if ( residual )    delete( residual );
  if ( approximant ) delete( approximant );
}


/***************************/
/* SET OBJECTS             */
/***************************/

/************/
/* Set dict */
MP_Dict_c* MP_Mpd_Core_c::set_dict( MP_Dict_c *setDict ) {

  MP_Dict_c* oldDict = dict;

  /* If there was a non-NULL dictionary before, detach the residual
     to avoid its destruction: */
  if ( oldDict ) residual = oldDict->detach_signal();

  /* Set the new dictionary: */
  dict = setDict;

  /* If the new dictionary is not NULL, replug the residual: */
  if ( dict ) dict->plug_signal( residual );

  /* Note:
     - if a NULL dictionary is given, the residual is kept alive
     in the residual variable;
     - at the first use of set_dict(dict), the oldDict is NULL and
     the residual is copy-constructed from the signal at
     the mpdCore->init(signal,book) pahse. */

  return( oldDict );
}


/*******************************/
/* Set/reset the snr condition */
/****/
void MP_Mpd_Core_c::set_snr_condition( const double setSnr ) {
  stopAfterSnr = pow( 10.0, setSnr/10 );
  useStopAfterSnr = MP_TRUE;
  if ( snrHit == ULONG_MAX ) set_snr_hit( 1 );
}
/****/
void MP_Mpd_Core_c::reset_snr_condition( void ) {
  stopAfterSnr = 0.0;
  useStopAfterSnr = MP_FALSE;
}
/****/

/**********************/
/* Set/reset save hit */
/****/
void MP_Mpd_Core_c::set_save_hit( const unsigned long int setSaveHit,
				  const char* setBookFileName,
				  const char* setApproxFileName,
				  const char* setResFileName,
				  const char* setDecayFileName ) {
  saveHit = setSaveHit;
  nextSaveHit = numIter + setSaveHit;

  bookFileName  = setBookFileName;
  approxFileName = setApproxFileName;
  resFileName   = setResFileName;
  decayFileName = setDecayFileName;
}
/****/
void MP_Mpd_Core_c::reset_save_hit( void ) {
  saveHit = ULONG_MAX;
  nextSaveHit = ULONG_MAX;
}
/****/


/***************************/
/* DETACH/DELETE OBJECTS   */
/***************************/
MP_Dict_c* MP_Mpd_Core_c::detach_dict( void ) {
  MP_Dict_c *d = dict;
  dict = NULL;
  return( d );
}
void MP_Mpd_Core_c::delete_dict( void ) {
  if ( dict ) {
    delete( dict );
    dict = NULL;
  } 
}

MP_Signal_c* MP_Mpd_Core_c::detach_signal( void ) {
  MP_Signal_c *s = signal;
  signal = NULL;
  return( s );
}
void MP_Mpd_Core_c::delete_signal( void ) {
  if ( signal ) {
    delete( signal );
    signal = NULL;
  }
}

MP_Book_c* MP_Mpd_Core_c::detach_book( void ) {
  MP_Book_c *b = book;
  book = NULL;
  return( b );
}
void MP_Mpd_Core_c::delete_book( void ) {
  if ( book ) {
    delete( book );
    book = NULL;
  }
}

MP_Signal_c* MP_Mpd_Core_c::detach_residual( void ) {
  MP_Signal_c *s = NULL;
  if ( dict ) s = dict->detach_signal();
  residual = NULL;
  return( s );
}
void MP_Mpd_Core_c::delete_residual( void ) {
  if ( dict ) dict->plug_signal( NULL );
  residual = NULL;
}

MP_Signal_c* MP_Mpd_Core_c::detach_approximant( void ) {
  MP_Signal_c *s = approximant;
  approximant = NULL;
  return( s );
}
void MP_Mpd_Core_c::delete_approximant( void ) {
  delete( approximant );
  approximant = NULL;
}


/***************************/
/* OTHER METHODS           */
/***************************/

/********************************/
/* Save the book/residual/decay */
void MP_Mpd_Core_c::save_result() {

  const char* func = "Save info";

  /* - Save the book: */
  if ( strcmp( bookFileName, "-" ) != 0 ) {
    book->print( bookFileName, MP_BINARY);
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the book.\n", numIter );  
  }
  /* - Save the approximant: */
  if ( approxFileName ) {
    approximant->wavwrite( approxFileName );
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the approximant.\n", numIter );
  }
  /* - Save the residual: */
  if ( resFileName ) {
    residual->wavwrite( resFileName );
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the residual.\n", numIter );
  }
  /* - the decay: */
  if ( decayFileName ) {
    unsigned long int nWrite;
    nWrite = decay.save( decayFileName );
    if( nWrite != (numIter+1) ) {
      mp_warning_msg( func, "Wrote less than the expected number of doubles to the energy decay file.\n" );
      mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter+1, nWrite );
    }
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the energy decay.\n", numIter );	  
  }

}

/*************************/
/* Make one MP iteration */
unsigned short int MP_Mpd_Core_c::step() {

  const char* func = "MP_Mpd_Core_c::step()";

  /* Reset the state info */
  state = 0;

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
		10*log10(currentSnr), 10*log10(stopAfterSnr) );

  /* 1) Iterate: */
  dict->iterate_mp( book , approximant ); /* Note: if approximant is NULL, no computation
					     will be performed on it. */
  residualEnergyBefore = residualEnergy;
  residualEnergy = (double)dict->signal->energy;
  if ( decayFileName ) decay.append( residualEnergy );
  numIter++;
  
  /* 2) Check for possible breakpoints: */
  if ( numIter == nextSnrHit ) {
    currentSnr = initialEnergy / residualEnergy;
    nextSnrHit += snrHit;
  }

  if ( numIter == nextReportHit ) {
    mp_progress_msg( "At iteration", "[%lu] : the residual energy is [%g] and the SNR is [%g].\n",
		     numIter, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
    nextReportHit += reportHit;
  }
    
  if ( numIter == nextSaveHit ) { save_result(); nextSaveHit += saveHit; }

  if ( numIter == ULONG_MAX ) state = ( state | MPD_ITER_EXHAUSTED );
  
  /* 3) Check for possible stopping conditions: */
  if ( useStopAfterIter && (numIter >= stopAfterIter) )   state = ( state | MPD_ITER_CONDITION_REACHED );
  if ( useStopAfterSnr  && (currentSnr >= stopAfterSnr) ) state = ( state | MPD_SNR_CONDITION_REACHED );
  if ( residualEnergy < 0.0 ) state = ( state | MPD_NEG_ENERGY_REACHED );
  if ( residualEnergy >= residualEnergyBefore ) {
    mp_warning_msg( func, "Iteration [%lu] increases the energy of the residual ! Before: [%g] Now: [%g]\n",
		    numIter, residualEnergyBefore, residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    book->atom[book->numAtoms-1]->info( stderr );
    //state = ( state | MPD_INCREASING_ENERGY );
  }
  /*if ( (residualEnergyBefore - residualEnergy) < 5e-4 ) {
    mp_warning_msg( func, "At iteration [%lu]: the energy decreases very slowly !"
		    " Before: [%g] Now: [%g] Diff: [%g]\n",
		    numIter, residualEnergyBefore, residualEnergy, residualEnergyBefore - residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    book->atom[book->numAtoms-1]->info( stderr );
    /* BORK BORK BORK */
    /* Export the considered signal portion */
    /* RES */
    /* MP_Signal_c *exportSig = MP_Signal_c::init( dict->signal, book->atom[book->numAtoms-1]->support[0] );
    if ( exportSig != NULL ) exportSig->dump_to_double_file( "res.dbl" );
    fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [res.dbl].\n",
	     exportSig->numChans,
	     book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );*/
    /* ATOM */
    /*MP_Signal_c *atomSig = MP_Signal_c::init( book->atom[book->numAtoms-1], dict->signal->sampleRate );
    if ( atomSig != NULL ) atomSig->dump_to_double_file( "atom.dbl" );
    fprintf( stderr, "Exported [%hu] channels from atom of length [%lu] to file [atom.dbl].\n",
    atomSig->numChans, atomSig->numSamples );*/
    /* SUM */
    /*unsigned long int i;
    for ( i = 0; i < (exportSig->numSamples*exportSig->numChans); i++ ) {
      exportSig->storage[i] = exportSig->storage[i] + atomSig->storage[i];
    }
    exportSig->dump_to_double_file( "bad_signal.dbl" );
    exportSig->wavwrite( "bad_signal.wav" );
    fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [bad_signal.dbl].\n",
	     exportSig->numChans,
	     book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );
    fflush( stderr );
    exit( 0 );*/
    /* \BORK BORK BORK */
  /*}*/

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "EXITING iteration [%lu]/[%lu].\n", numIter, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
		10*log10(currentSnr), 10*log10(stopAfterSnr) );

  return( state );
}


/********************************/
/* Set the state to forced stop */
void MP_Mpd_Core_c::force_stop() {
  state = MPD_FORCED_STOP;
}

/*************************/
/* Make one MP iteration */
unsigned long int MP_Mpd_Core_c::run() {

  /* Reset the state info */
  state = 0;

  /* Loop while the return state is NULL */
  while( step() == 0 );

  /* Return the last state */
  return( numIter );
}


/**************************************/
/* Info about the stopping conditions */
void MP_Mpd_Core_c::info_conditions( void ) {

  const char* func = "Conditions";

  if ( useStopAfterIter ) mp_info_msg( func, "This run will perform [%lu] iterations, using [%lu] atoms.\n",
				       stopAfterIter, dict->num_atoms() );
  if ( useStopAfterSnr ) mp_info_msg( func, "This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
				      10*log10( stopAfterSnr ), dict->num_atoms() );
  if ( bookFileName ) {  
    if ( strcmp( bookFileName, "-" ) == 0 ) mp_info_msg( func, "The resulting book will be written"
							 " to the standard output [%s].\n", bookFileName );
    else mp_info_msg( func, "The resulting book will be written to book file [%s].\n", bookFileName );
  }
  if ( resFileName ) mp_info_msg( func, "The residual will be written to file [%s].\n", resFileName );
  else mp_info_msg( func, "The residual will not be saved.\n" );
  if ( decayFileName ) mp_info_msg( func, "The energy decay will be written to file [%s].\n", decayFileName );
  else mp_info_msg( func, "The energy decay will not be saved.\n" );

}

/********************************/
/* Info about the current state */
void MP_Mpd_Core_c::info_state( void ) {

  const char* func = "Current state";

  if ( state & MPD_ITER_CONDITION_REACHED )  mp_info_msg( func,
							  "Reached the target number of iterations [%lu].\n",
							  numIter );

  if ( state & MPD_SNR_CONDITION_REACHED )  mp_info_msg( func,
							 "The current SNR [%g] reached or passed"
							 " the target SNR [%g] in [%lu] iterations.\n",
							 10*log10( currentSnr ), 10*log10( stopAfterSnr ), numIter );

  if ( state & MPD_NEG_ENERGY_REACHED )  mp_info_msg( func,
						      "The current residual energy [%g] has gone negative"
						      " after [%lu] iterations.\n",
						      residualEnergy, numIter );

  if ( state & MPD_INCREASING_ENERGY )  mp_info_msg( func,
						     "The residual energy is increasing [%g -> %g]"
						     " after [%lu] iterations.\n",
						     residualEnergyBefore, residualEnergy, numIter );

  if ( state & MPD_ITER_EXHAUSTED ) mp_info_msg( func,
						 "Reached the absolute maximum number of iterations [%lu].\n",
						 numIter );

  if ( state & MPD_FORCED_STOP ) mp_info_msg( func,
					      "Forced stop at iteration [%lu].\n",
					      numIter );

}

/*********************************/
/* Info about the current values */
void MP_Mpd_Core_c::info_result( void ) {
  const char* func = "Result";
    mp_info_msg( func, "[%lu] iterations have been performed.\n", numIter );
    mp_info_msg( func, "([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
		 numIter, dict->num_atoms() );
    mp_info_msg( func, "The initial signal energy was [%g].\n", initialEnergy );
    mp_info_msg( func, "The residual energy is now [%g].\n", residualEnergy );
    mp_info_msg( func, "The SNR is now [%g].\n", 10*log10( initialEnergy / residualEnergy ) );
}


/**********************************/
/* Check if some objects are null */
MP_Bool_t MP_Mpd_Core_c::can_step( void ) {
  /* Check that all of dict, book and signal are not NULL */
  return(  dict  &&  book  &&  dict->signal  );
}
