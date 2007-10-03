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
 * $Date: 2007-05-24 16:50:42 +0200 (Thu, 24 May 2007) $
 * $Revision: 1053 $
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
class MP_Mpd_Core_c:public MP_Abstract_Core_c {

  /********/
  /* DATA */
  /********/

public:
private:
  /* Manipulated objects */
   MP_Book_c* book;
   MP_Dict_c* dict;
   
   /* Output file names */
  char *bookFileName;
  

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
public:
  static MP_Mpd_Core_c* create( MP_Signal_c *signal, MP_Book_c *setBook );
  static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict );
  static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Signal_c* setApproximant );
  static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict, MP_Signal_c* setApproximant );

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
 
   /* Control object*/
  
  /* Set the dictionary */
  MP_Dict_c* change_dict( MP_Dict_c* setDict );
  /* Set a void dictionary */
  void init_dict();
  /* Plug dictionary to a signal */
  void plug_dict_to_signal();
  /* Plug dictionary to a signal */
  void addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap);
  
  int add_default_block_to_dict( const char* blockName );
  
  bool save_dict( const char* dictName );
  /* Runtime settings */
  virtual void plug_approximant( MP_Signal_c *approximant );

  /* Runtime */
  unsigned short int step();

  /* Misc */
  virtual void save_result( void );
  MP_Bool_t can_step( void );
  virtual void info_result( void );
  virtual void info_conditions( void );
  void set_save_hit( const unsigned long int setSaveHit,
                     const char* setBookFileName,                  
                     const char* setResFileName,
                     const char* setDecayFileName );
};

#endif /* __mpd_core_h_ */
