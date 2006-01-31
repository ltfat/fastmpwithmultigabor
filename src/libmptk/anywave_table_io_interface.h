/******************************************************************************/
/*                                                                            */
/*                      anywave_table_io_interface.h                          */
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

/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

#ifndef __anywave_table_io_interface_h_
#define __anywave_table_io_interface_h_

/***********************/
/* CONSTANTS           */
/***********************/

/* Scanner return values (for flow control) */
const unsigned short int ANYWAVE_TABLE_NULL_EVENT = 0;
const unsigned short int ANYWAVE_TABLE_ERROR_EVENT = 1;
const unsigned short int ANYWAVE_TABLE_OPEN = 2;
const unsigned short int ANYWAVE_TABLE_CLOSE = 3;
const unsigned short int ANYWAVE_TABLE_REACHED_END_OF_FILE = 4;

/*****************************************/
/*                                       */
/* class MP_Anywave_Table_Scan_Info_c    */
/*                                       */
/*****************************************/

/** \brief Interface between a MP_Anywave_Table_c object, and the
 * anywave_table_scanner() function
 *
 * anywave_table_scanner() is a function of the file
 * anywave_table_scanner.lpp, that has no documentation. It is a FLEX
 * program used to parse a anywave table import file
 * (e.g. "PATH/anywave.xml"). The function anywave_table_scanner()
 * takes two arguments :
 * -# a FILE* stream : the stream of the file "PATH/anywave.xml"
 * -# a MP_Anywave_Table_Scan_Info_c* : a pointer to this interface
 *
 * Calling anywave_table_scanner() fills in the members of this interface
 * after the parsing. 
 *
 * After calling anywave_table_scanner(), to fill in the members of the
 * MP_Anywave_Table_c object with the members of this interface, call the
 * function pop_table()
 */
class MP_Anywave_Table_Scan_Info_c {

  /********/
  /* DATA */
  /********/

public:
  
  char libVersion[MP_MAX_STR_LEN];
  
  unsigned short int numChans;
  bool numChansIsSet;
  unsigned short int globNumChans;
  bool globNumChansIsSet;
  
  unsigned long int filterLen;
  bool filterLenIsSet;
  unsigned long int globFilterLen;
  bool globFilterLenIsSet;

  unsigned long int numFilters;
  bool numFiltersIsSet;
  unsigned long int globNumFilters;
  bool globNumFiltersIsSet;

  char dataFileName[MP_MAX_STR_LEN];
  bool dataFileNameIsSet;
  char globDataFileName[MP_MAX_STR_LEN];
  bool globDataFileNameIsSet;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief Constructor 
   *
   * calls reset_all to initialize the members
   */
  MP_Anywave_Table_Scan_Info_c( );
  /* \brief Destructor */
  ~MP_Anywave_Table_Scan_Info_c();

  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Reset all the members of the class, including the global
   * members
   */
  void reset_all( void );

  /** \brief Reset the local members of the class */
  void reset( void );
  
  /** \brief Pop a anywave_table respecting the members of the class, and reset the local fields 
   *
   * fills in the members of the MP_Anywave_Table_c pointed by \a table
   * (\a filterLen, \a numChans, \a numFilters, \a dataFileName) and
   * loads the data from the file \a dataFileName.
   *
   * \param table the MP_Anywave_Table_c instance to fill in. It must
   * have been created before calling pop_table().
   *
   * \return true if succeed, false if fails
   *
   * \remark the MP_Anywave_Table_Scan_Info_c is intended to interface between
   * the anywave_table_scanner() function of the anywave_table_scanner.lpp file and
   * the MP_Anywave_Table_c class. Thus, the regular use is first to call
   * anywave_table_scanner() with a pointer to MP_Anywave_Table_Scan_Info_c interface
   * to fill it in, and then to call pop_table() with the
   * MP_Anywave_Table_c instance you want to fill in. Therefore, verify
   * that anywave_table_scanner() has been called before calling pop_table(),
   * otherwise the MP_Anywave_Table_c members will be filled in with the
   * default values of this interface.
   **/
  bool pop_table( MP_Anywave_Table_c* table );
  
};

#endif /* __anywave_table_io_interface_h_ */
