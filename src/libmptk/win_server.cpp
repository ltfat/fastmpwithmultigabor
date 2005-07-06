/******************************************************************************/
/*                                                                            */
/*                              win_server.cpp                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 13:38:03 $
 * $Revision: 1.2 $
 *
 */

/*****************************************************/
/*                                                   */
/* win_server.cpp: methods for class MP_Win_Server_c */
/*                                                   */
/*****************************************************/

#include "mptk.h"
#include "system.h"


/**********************************/
/* DECLARATION OF A WINDOW SERVER */
/* WITH GLOBAL SCOPE              */
/**********************************/
/**
 * \brief Declaration of a unique window server with global scope over the
 * whole library
 */
MP_Win_Server_c MP_GLOBAL_WIN_SERVER;


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
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Win_Server_c -- Entering the window server destructor.\n" );
#endif

  release();

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Win_Server_c -- Exiting the window server destructor.\n" );
#endif
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Memory release */
void MP_Win_Server_c::release( void ) {

  unsigned short int i;
  unsigned short int n;

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Win_Server_c::release() -- Entering.\n" );
#endif

  for ( i = 0; i < DSP_NUM_WINDOWS; i++ ) {

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Win_Server_c::release() -- Freeing row number %d.\n", i );
#endif

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

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Win_Server_c::release() -- Exiting.\n" );
#endif

}

/* Window service */
unsigned long int MP_Win_Server_c::get_window( MP_Real_t **out,
					       const unsigned long int length,
					       const unsigned char type,
					       double optional ) {
  unsigned short int n;
  unsigned short int num;
  MP_Win_t* ptrWin;
  Dsp_Win_t buffer[length];
  Dsp_Win_t* ptrWinT;
  MP_Real_t* ptrReal;
  unsigned long int center;

  /* Seek if the window exists */
  num = numberOf[type];
  for (  n = 0,   ptrWin = window[type];
	 n < num;
	 n++,     ptrWin++ ) {
    if ( (ptrWin->len == length) && (ptrWin->optional == optional) ) break;
  }

  /* If the window exists (i.E., if the above loop stopped before the last slot),
     return the address of the existing buffer */
  if (n < num) {
    *out = ptrWin->win;
    return( ptrWin->center );
  }
  /* Else, generate the window */
  else if (n == num) {

    /* If needed, add space for more windows */
    if (num == maxNumberOf[type]) { /* Note: num == numberOf[type] */

      if ( (ptrWin = (MP_Win_t*)realloc( window[type], (num+MP_WIN_BLOCK_SIZE)*sizeof(MP_Win_t) )) == NULL ) {
	fprintf( stderr, "mplib error -- MP_Win_Server_c::get_window() - Can't realloc to add a new window."
		 " Returning a NULL buffer.\n" );
	*out = NULL;
	return( 0 );
      }
      else {
	/* If the realloc succeeded, initialize */
	window[type] = ptrWin;
	for ( n = 0, ptrWin = window[type] + num;
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

    /* Fill the new window: */
    ptrWin = window[type] + numberOf[type];
    /* Allocate the window buffer */
    if ( (ptrWin->win = (MP_Real_t*)malloc( length*sizeof(MP_Real_t) )) == NULL ) {
      fprintf( stderr, "mplib error -- MP_Win_Server_c::get_window() - Can't allocate a new window buffer."
	       " Returning a NULL buffer.\n" );
      *out = NULL;
      return( 0 );
    }
    /* If the allocation went OK, compute and fill the buffer, then return the window: */
    else {
      /* Compute the new window */
      center = make_window( buffer, length, type, optional );
      /* Cast it */
      for ( n = 0,      ptrReal = ptrWin->win, ptrWinT = buffer;
	    n < length;
	    n++,        ptrReal++,             ptrWinT++ ) {
	*ptrReal = (MP_Real_t)(*ptrWinT);
      }
      /* Register the other parameters */
      ptrWin->len      = length;
      ptrWin->center   = center;
      ptrWin->optional = optional;
      /* Count the new window */
      numberOf[type] = numberOf[type] + 1;
      /* Return the new window */
      *out = ptrWin->win;
      return( center );
    }
    
  }
  else {
    fprintf( stderr, "mplib error -- MP_Win_Server_c::make_window() - Oooops, this code is theoretically"
	     " unreachable. Returning a NULL buffer.\n" );
    *out = NULL;
    return( 0 );
  }

}

