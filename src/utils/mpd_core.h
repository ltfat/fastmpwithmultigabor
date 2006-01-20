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
  double stopAfterSnr;
  MP_Bool_t useStopAfterSnr;

  /* Verbose mode: */
  MP_Bool_t verbose;
  unsigned long int reportHit;
  unsigned long int nextReportHit;
 
  /* Granularity of the snr recomputation: */
  unsigned long int snrHit;  
  unsigned long int nextSnrHit;  

  /* Intermediate saves: */
  unsigned long int saveHit;
  unsigned long int nextSaveHit;

  /* Decay array */
  MP_Var_array_c<double> decay;

  /* Manipulated objects */
  MP_Dict_c *dict;
  MP_Book_c *book;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  static MP_Mpd_Run_c::init();
  static MP_Mpd_Run_c::init( unsigned long int stopAfterIter,
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

  int set_dict( MP_Dict_c *dict );
  int set_signal( MP_Signal_c *sig );
  int set_book( MP_Book_c *book );

  int run( unsigned long int nIter );

  MP_Book_c* get_current_book();
  MP_Signal_c* get_current_signal();
  MP_Var_Array_c* get_current_decay();

}

#endif /* __mpd_core_h_ */
