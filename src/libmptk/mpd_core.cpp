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
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book ) {

  const char* func = "MP_Mpd_Core_c::init(2 args)";
  MP_Mpd_Core_c* newCore;

  /* Instantiate and check */
  newCore = new MP_Mpd_Core_c();
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );
  }

  /* plug the objects */
  if ( newCore->set_signal( signal ) == signal ) {
    mp_error_msg( func, "Failed to plug the signal.\n" );
    delete( newCore );
    return( NULL );    
  }
  newCore->residualEnergy = newCore->initialEnergy = signal->energy;
  newCore->decay.append( newCore->initialEnergy );
  if ( newCore->set_book( book ) == book ) {
    mp_error_msg( func, "Failed to plug the book.\n" );
    delete( newCore );
    return( NULL );    
  }

  return( newCore );
}


/***********************/
/* - signal+book+dict: */
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book, MP_Dict_c *dict ) {

  const char* func = "MP_Mpd_Core_c::init(3 args)";
  MP_Mpd_Core_c* newCore;

  /* Instantiate and check */
  newCore = MP_Mpd_Core_c::init( signal, book );
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
MP_Mpd_Core_c* MP_Mpd_Core_c::init( MP_Signal_c *signal, MP_Book_c *book, MP_Dict_c *dict,
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
  newCore =  MP_Mpd_Core_c::init( signal, book, dict );
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
  /* Nothing to be freed. */
}


/***************************/
/* SET OBJECTS             */
/***************************/

/************/
/* Set dict */
MP_Dict_c* MP_Mpd_Core_c::set_dict( MP_Dict_c *setDict ) {

  MP_Dict_c* oldDict = dict;

  dict = setDict;
  if ( dict ) {
    dict->copy_signal( signal );
    residual = dict->signal;
  }

  return( oldDict );
}

/***********/
/* Set signal */
MP_Signal_c* MP_Mpd_Core_c::set_signal( MP_Signal_c *setSig ) {

  MP_Signal_c* oldSig = signal;

  /* If there is already a book, check the sample rate compatibility */
  if ( (setSig != NULL) && (book != NULL) && (setSig->sampleRate != book->sampleRate) ) {
    mp_warning_msg( "mpd::set_signal()",
		    "The new signal has a sample rate [%i] different"
		    " from the book's sample rate [%i]."
		    " Returning the new signal, keeping the previous one.\n",
		    setSig->sampleRate, book->sampleRate );
    return( setSig );
  }

  signal = setSig;
  residualEnergy = initialEnergy = signal->energy;
  decay.clear();
  decay.append( initialEnergy );
  if ( dict ) {
    dict->copy_signal( signal );
    residual = dict->signal;
  }

  return( oldSig );
}

/************/
/* Set book */
MP_Book_c* MP_Mpd_Core_c::set_book( MP_Book_c *setBook ) {

  MP_Book_c* oldBook = book;

  /* If there is already a signal, check the sample rate compatibility */
  if ( (signal != NULL) && (setBook != NULL) && (signal->sampleRate != setBook->sampleRate) ) {
    mp_warning_msg( "mpd::set_book()",
		    "The new book has a sample rate [%i] different"
		    " from the signal's sample rate [%i]."
		    " Returning the new book, keeping the previous one.\n",
		    setBook->sampleRate, signal->sampleRate );
    return( setBook );
  }

  book = setBook;

  return( oldBook );
}


/**********************/
/* Set/reset save hit */
void MP_Mpd_Core_c::set_save_hit( const unsigned long int setSaveHit,
				  char* setBookFileName,
				  char* setResFileName,
				  char* setDecayFileName ) {
  saveHit = setSaveHit;
  nextSaveHit = numIter + setSaveHit;

  bookFileName  = setBookFileName;
  resFileName   = setResFileName;
  decayFileName = setDecayFileName;
}

void MP_Mpd_Core_c::reset_save_hit( void ) {
  saveHit = ULONG_MAX;
  nextSaveHit = ULONG_MAX;

  bookFileName  = NULL;
  resFileName   = NULL;
  decayFileName = NULL;
}


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
  MP_Signal_c *s = refresh_approximant();
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
/* (Re-)compute the approximant */
MP_Signal_c* MP_Mpd_Core_c::refresh_approximant( void ) {

  if ( approximant ) approximant->fill_zero();
  else if ( dict->signal ) approximant = MP_Signal_c::init( dict->signal->numChans,
							    dict->signal->numSamples,
							    dict->signal->sampleRate );
  else approximant = NULL;

  if ( book ) book->substract_add( NULL, approximant, NULL );

  return( approximant );
}


/********************************/
/* Save the book/residual/decay */
void MP_Mpd_Core_c::save_result() {

  const char* func = "Save info";

  /* - Save the book: */
  if ( strcmp( bookFileName, "-" ) != 0 ) {
    book->print( bookFileName, MP_BINARY);
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the book.\n", numIter );  
  }
  /* - Save the residual: */
  if ( resFileName ) {
    dict->signal->wavwrite( resFileName );
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the residual.\n", numIter );
  }
  /* - the decay: */
  if ( decayFileName ) {
    unsigned long int nWrite;
    nWrite = decay.save( decayFileName );
    if( nWrite != (numIter+1) ) {
      mp_warning_msg( func, "Wrote less than the expected number of doubles to the energy decay file.\n" );
      mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter, nWrite );
    }
    if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the energy decay.\n", numIter );	  
  }

}

/*************************/
/* Make one MP iteration */
unsigned short int MP_Mpd_Core_c::step() {

#ifndef NDEBUG
  const char* func = "MP_Mpd_Core_c::step()";
#endif

  /* Reset the state info */
  state = 0;

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n", currentSnr, stopAfterSnr );

  /* 1) Iterate: */
  dict->iterate_mp( book , NULL );
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
  if ( residualEnergy >= residualEnergyBefore ) state = ( state | MPD_INCREASING_ENERGY );

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "EXITING iteration [%lu]/[%lu].\n", numIter, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n", currentSnr, stopAfterSnr );

  return( state );
}


/*************************/
/* Make one MP iteration */
unsigned long int MP_Mpd_Core_c::run() {

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
				       stopAfterIter, dict->size() );
  if ( useStopAfterSnr ) mp_info_msg( func, "This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
				      10*log10( stopAfterSnr ), dict->size() );
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
						     "The residual energy is increasing [%g -> %g] after [%lu] iterations.\n",
						     residualEnergyBefore, residualEnergy, numIter );

  if ( state & MPD_ITER_EXHAUSTED ) mp_info_msg( func,
						 "Reached the absolute maximum number of iterations [%lu].\n",
						 numIter );

}

/*********************************/
/* Info about the current values */
void MP_Mpd_Core_c::info_result( void ) {
  const char* func = "Result";
    mp_info_msg( func, "[%lu] iterations have been performed.\n", numIter );
    mp_info_msg( func, "([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
		 numIter, dict->size() );
    mp_info_msg( func, "The initial signal energy was [%g].\n", initialEnergy );
    mp_info_msg( func, "The residual energy is now [%g].\n", residualEnergy );
    mp_info_msg( func, "The SNR is now [%g].\n", 10*log10( currentSnr ) );
}

