/******************************************************************************/
/*                                                                            */
/*                               win_server.h                                 */
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

/*****************************************/
/*                                       */
/* DEFINITION OF THE WINDOW SERVER CLASS */
/*                                       */
/*****************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/05/16 14:41:41 $
 * $Revision: 1.1 $
 *
 */


#ifndef __win_server_h_
#define __win_server_h_

#include <dsp_windows.h>

/***********************/
/* CONSTANTS           */
/***********************/
/** \brief A constant that defines the granularity of the allocation of windows by MP_Win_Server_c objects */
#define MP_WIN_BLOCK_SIZE 10

/***********************/
/* TYPES               */
/***********************/
/** \brief An object that can store the content of a window */
typedef struct {           

  /** \brief The length of the window */
  unsigned long int len;

  /** \brief The center of the window */
  unsigned long int center;

  /** \brief The optional window parameter */
  double optional;

  /** \brief A pointer on the window buffer */
  MP_Real_t* win;

} MP_Win_t;


/***********************/
/* WINDOW SERVER CLASS */
/***********************/
/**
 * \brief A smart way to avoid re-computing signal windows
 */
class MP_Win_Server_c {


  /********/
  /* DATA */
  /********/

public:
  /** \brief number of windows of each type actually stored in the storage space
   *
   * For example numberOf[DSP_GAUSS_WIN] gives the number of Gaussian windows currently stored.
   */
  unsigned short int numberOf[DSP_NUM_WINDOWS];
  /** \brief size available in the storage space for windows of the various types 
   *
   * For example maxNumberOf[DSP_GAUSS_WIN] gives the number of Gaussian windows that can be stored.
   */
  unsigned short int maxNumberOf[DSP_NUM_WINDOWS];
  /** \brief storage space for windows of various types 
   *
   * For example window[DSP_GAUSS_WIN] is an array of numberOf[DSP_GAUSS_WIN] 
   * Gaussian windows of various sizes.
   */
  MP_Win_t* window[DSP_NUM_WINDOWS];


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  MP_Win_Server_c( void );
  ~MP_Win_Server_c( void );

  /***************************/
  /* OTHER METHODS           */
  /***************************/

  /** \brief Destroy all the windows stored in the server and release all the memory
   **/
  void release( void );

  /** \brief Returns a reference on an array with various classical windows of unit energy.
   * If the window has been computed before, it does not re-compute it.
   *
   * \param out The array pointing on the asked window. 
   * \param length The number of samples in the window. 
   * A one point window of any type is always filled with the value 1.
   * \param type The type of the window. 
   * \param optional An optional parameter to describe the shape of certain types of windows. 
   * \return The offset of the sample at the 'center' of the window compared to the first sample.
   * By convention, for most symmetric windows wich are bump functions, the 'center' is
   * the first sample at which the absolute maximum of the window is reached.
   * \remark  When the window type is not a known one, an error message is printed to stderr and out is not filled.
   * \warning DO NOT FREE OR DELETE the returned array.
   * \sa window_type_is_ok() can be used with the argument \a type before calling get_window() to check if the type is known
   * \sa make_window()
   */
  unsigned long int get_window( MP_Real_t **out,
				const unsigned long int length,
				const unsigned char type,
				double optional );
};

#endif /* __win_server_h_ */
