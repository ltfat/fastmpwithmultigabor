/******************************************************************************/
/*                                                                            */
/*                              win_server.cpp                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-06-27 16:52:43 +0200 (Wed, 27 Jun 2007) $
 * $Revision: 1082 $
 *
 */

/*****************************************************/
/*                                                   */
/* win_server.cpp: methods for class MP_Win_Server_c */
/*                                                   */
/*****************************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Win_Server_c::MP_Win_Server_c( void ) {

  unsigned short int i;

  for ( i = 0; i < DSP_NUM_WINDOWS; i++ ) {
    numberOf[i] = 0;
    maxNumberOf[i] = 0;
    window[i] = NULL;
  }

}

/**************/
/* destructor */
MP_Win_Server_c::~MP_Win_Server_c() {
  const char *func = "MP_Win_Server_c::~MP_Win_Server_c()";
  mp_debug_msg( MP_DEBUG_DESTRUCTION, func,
		"Destroying the window server...\n" );

  release();

  mp_debug_msg( MP_DEBUG_DESTRUCTION, func,
		"Done.\n" );
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Memory release */
void MP_Win_Server_c::release( void ) {
   const char* func = "MP_Win_Server_c::release()";
   mp_debug_msg( MP_DEBUG_FUNC_ENTER, func, "Entering.\n" );

  unsigned short int i;
  unsigned long int n;

 

  for ( i = 0; i < DSP_NUM_WINDOWS; i++ ) {

    if ( window[i] ) {
      /* Free the buffers */
      for ( n = 0; n < numberOf[i]; n++ ) {
	if ( window[i][n].win ) free( window[i][n].win );
      }
      /* Free the window array */
      free( window[i] );
      window[i] = NULL;
    }
    /* Reset the counters */
    numberOf[i] = 0;
    maxNumberOf[i] = 0;

  }
  mp_debug_msg( MP_DEBUG_FUNC_EXIT, func, "Exiting.\n" );
}

/* Window service */
unsigned long int MP_Win_Server_c::get_window( MP_Real_t **out,
					       const unsigned long int length,
					       const unsigned char type,
					       double optional ) {
  const char* func = "MP_Win_Server_c::get_window(...)";
  //  Will initialize allocated_length and buffer with the first nonzero value with which this function is called
  static unsigned long int allocated_length = 0 ;
  static Dsp_Win_t *buffer=NULL;
  
  // Checking input
  if(NULL==out) {
    mp_error_msg( func, "Oooops, out is NULL. Returning without doing anything.\n" );
    return( 0 );
  }
  if(0==length) {
    mp_error_msg( func, "Oooops, length is zero. Returning NULL.\n" );
    *out = NULL;
    return( 0 );
  }

  // (Re)allocating
  if (NULL==buffer || allocated_length != length) {
    if (NULL!=buffer) {
      free(buffer);
      buffer = NULL;
      allocated_length = 0;
    }
    buffer = (Dsp_Win_t*)malloc(length*sizeof(double)) ;
    if(NULL==buffer) {
      mp_error_msg(func,"Could not allocate buffer. Returning NULL.\n");
      *out = NULL;
      return(0);
    }
    allocated_length = length ; 
  }
  
  unsigned long int n;
  unsigned long int numOfType;
  MP_Win_t  *ptrWin  = NULL;
  Dsp_Win_t *ptrWinT = NULL;
  MP_Real_t *ptrReal = NULL;
  unsigned long int center;
  
  // Seek if the window exists
  numOfType = numberOf[type];
  for (  n = 0,   ptrWin = window[type];
	 n < numOfType;
	 n++,     ptrWin++ ) {
    if ( (ptrWin->len == length) && (ptrWin->optional == optional) ) break;
  }
  
  // If the window exists (i.E., if the above loop stopped before the last slot),
  // return the address of the existing buffer
  if (n < numOfType) {
    *out = ptrWin->win;
    return( ptrWin->center );
  }
  // Else, generate the window 
  else if (n == numOfType) {
    
    // If needed, add space for more windows
    if (numOfType == maxNumberOf[type]) { // Note: numOfType == numberOf[type] 
      
      if ( NULL== (ptrWin = (MP_Win_t*)realloc( window[type], (numOfType+MP_WIN_BLOCK_SIZE)*sizeof(MP_Win_t) ))) {
	mp_error_msg( func, "Can't realloc to add a new window. Returning NULL.\n" );
	*out = NULL;
	return( 0 );
      }
      else {
	// If the realloc succeeded, initialize
	window[type] = ptrWin;
	for ( n = 0, ptrWin = window[type] + numOfType;
	      n < MP_WIN_BLOCK_SIZE;
	      n++,   ptrWin++ ) {
	  ptrWin->win    = NULL;
	  ptrWin->len    = 0;
	  ptrWin->center = 0;
	  ptrWin->optional = 0.0;
	}
	maxNumberOf[type] = maxNumberOf[type] + MP_WIN_BLOCK_SIZE;
      }
    }
    
    // Fill the new window:
    ptrWin = window[type] + numberOf[type];

    // Allocate the window buffer 
    if ( NULL==(ptrWin->win = (MP_Real_t*)malloc( length*sizeof(MP_Real_t) )) ) {
      mp_error_msg( func, "Can't allocate a new window buffer. Returning NULL.\n" );
      *out = NULL;
      return( 0 );
    }
    // If the allocation went OK, compute and fill the buffer, then return the window:
    else {
      // Compute the new window
      center = make_window( buffer, length, type, optional );
      // Cast it
      for ( n = 0,      ptrReal = ptrWin->win, ptrWinT = buffer;
	    n < length;
	    n++,        ptrReal++,             ptrWinT++ ) {
	*ptrReal = (MP_Real_t)(*ptrWinT);
      }
      // Register the other parameters
      ptrWin->len      = length;
      ptrWin->center   = center;
      ptrWin->optional = optional;
      // Count the new window 
      numberOf[type] = numberOf[type] + 1;
      // Return the new window
      *out = ptrWin->win;
      return( center );
    }
  }
  else {
    mp_error_msg( func, "Oooops, this code is theoretically unreachable."
		  " Returning a NULL buffer.\n" );
    *out = NULL;
    return( 0 );
  }
}

