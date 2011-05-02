/******************************************************************************/
/*                                                                            */
/*                            anywave_server.h                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Mon Feb 21 2005 */
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

/*******************************************/
/*                                         */
/* DEFINITION OF THE WAVEFORM SERVER CLASS */
/*                                         */
/*******************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2006-01-31 15:24:23 +0100 (Tue, 31 Jan 2006) $
 * $Revision: 306 $
 *
 */


#ifndef __anywave_server_h_
#define __anywave_server_h_

/***********************/
/* CONSTANTS           */
/***********************/
/** \brief A constant that defines the granularity of the allocation
 * of waves by MP_Anywave_Server_c objects
 */
const unsigned long int MP_ANYWAVE_BLOCK_SIZE = 10;

/************************/
/* ANYWAVE SERVER CLASS */
/************************/
/**
 * \brief A server managing tables of waveforms (MP_Anywave_Table_c),
 * corresponding to anywave atoms
 */
class MP_Anywave_Server_c {


  /********/
  /* DATA */
  /********/

public:
  /** \brief number of waveform tables currently stored in the storage
   * space \a tables
   */
  unsigned long int numTables;
  /** \brief size available in the storage space \a tables for
   * waveform tables
   */
  unsigned long int maxNumTables;
  /** \brief storage space for waveform tables (array of pointers to
   * the anywave tables)
   */
  MP_Anywave_Table_c** tables;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief Default constructor
   */
  MPTK_LIB_EXPORT MP_Anywave_Server_c( void );
  /** \brief Default destructor
   */
  MPTK_LIB_EXPORT virtual ~MP_Anywave_Server_c( void );

  /***************************/
  /* OTHER METHODS           */
  /***************************/

  /** \brief Test function, called by the test executable test_anywave
   **/
  MPTK_LIB_EXPORT static bool test( void );

  /** \brief Add a waveform table to the server
   *
   * \return the number of the added table
   **/
  MPTK_LIB_EXPORT unsigned long int add( MP_Anywave_Table_c* table );

  /** \brief Add the waveform \a table in filename to the server
   *
   * \return the number of the added table
   **/
  MPTK_LIB_EXPORT unsigned long int add( char* filename );

  MPTK_LIB_EXPORT unsigned long int add( map<string, string, mp_ltstring> *paramMap );

	  MPTK_LIB_EXPORT bool reallocate(void);

  /** \brief Get the filename associated to the table number \a index
   *
   *  \return the filename corresponding to the table number \a index
   *  or NULL if not found
   **/
  MPTK_LIB_EXPORT char* get_keyname( unsigned long int index );

  MPTK_LIB_EXPORT int get_keyname_size(void);
  /** \brief Get the table number associated to \a filename 
   *
   * \return the table number, or \a numTables if not found
   */
  MPTK_LIB_EXPORT unsigned long int get_index( char* filename );

	MPTK_LIB_EXPORT void encodeMd5(char *szInputEncode, int iInputSize, char *OutputEncode);

};

#endif /* __anywave_server_h_ */
