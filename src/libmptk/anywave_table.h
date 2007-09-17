/******************************************************************************/
/*                                                                            */
/*                             anywave_table.h                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Nov 03 2005 */
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
/* DEFINITION OF THE ANYWAVE TABLE CLASS */
/*                                       */
/*****************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2007-07-04 15:41:07 +0200 (Wed, 04 Jul 2007) $
 * $Revision: 1086 $
 *
 */


#ifndef __anywave_table_h_
#define __anywave_table_h_

/***********************/
/* ANYWAVE TABLE CLASS */
/***********************/
/**
 * \brief A MP_Anywave_Table_c object is used to store waveforms,
 * corresponding to anywave atoms.
 *
 * In one MP_Anywave_Table_c, all the waveforms have the same length. To
 * use atoms with different sizes, you need several MP_Anywave_Table_c
 * instances.
**/
class MP_Anywave_Table_c {


  /********/
  /* DATA */
  /********/

public:
  /** \brief Name of the file containing the waveform table (text
   * file, extension .xml)
   *
   * To import the waveforms of the anywave atoms (e.g. generated with Matlab) in MPTK, you need to save them in two files as described below :
   * - a xml file (text mode), describing all the parameters of the waveforms
   * - a bin file (binary mode), containing the raw data
   *
   * The xml file, e.g. called "PATH/anywave.xml", must have the following syntax :
   * - \<?xml version="1.0" encoding="ISO-8859-1"?\>
   * - \<libVersion\>0.4beta\</libVersion\>
   * - \<table\>
   *   - \<par type = "numChans"\>2\</par\>
   *   - \<par type = "filterLen"\>50\</par\>
   *   - \<par type = "numFilters"\>10\</par\>
   *   - \<par type = "data"\>PATH/anywave_data.bin\</par\>
   * - \</table\>
   *
   * The associated binary file, called "PATH/anywave_data.bin", only
   * contains the waveforms, in "double" format. The data is then \a
   * numChans * \a numFilters * \a filterLen numbers in "double"
   * format, in this example 1000 numbers, in the following order :
   * - the first waveform, channel after channel
   * - the second waveform, channel after channel 
   * - so on ...
   *
   */
  char tableFileName[MP_MAX_STR_LEN];
  /** \brief Name of the file containing the data of the waveform table (binary file, extension .bin)
   *
   * To import the waveforms of the anywave atoms (e.g. generated with Matlab) in MPTK, you need to save them in two files as described below :
   * - a xml file (text mode), describing all the parameters of the waveforms
   * - a bin file (binary mode), containing the raw data
   *
   * The xml file, e.g. called "PATH/anywave.xml", must have the following syntax :
   * - \<?xml version="1.0" encoding="ISO-8859-1"?\>
   * - \<libVersion\>0.4beta\</libVersion\>
   * - \<table\>
   *   - \<par type = "numChans"\>2\</par\>
   *   - \<par type = "filterLen"\>50\</par\>
   *   - \<par type = "numFilters"\>10\</par\>
   *   - \<par type = "data"\>PATH/anywave_data.bin\</par\>
   * - \</table\>
   *
   * The associated binary file, called "PATH/anywave_data.bin", only
   * contains the waveforms, in "double" format. The data is then \a
   * numChans * \a numFilters * \a filterLen numbers in "double"
   * format, in this example 1000 numbers, in the following order :
   * - the first waveform, channel after channel
   * - the second waveform, channel after channel 
   * - so on ...
   *
   */
  char dataFileName[MP_MAX_STR_LEN];

  /** \brief Storage of the data. 
   *
   * The filters are stored one after the other, and for each filter,
   * one channel after the other (as they are stored in the binary
   * data file).
   */
  MP_Real_t* storage;

  /** \brief Number of channels of the waveforms. 
   * 
   * It must be either 1, either the same number as in the signal to
   * analyze.
   */
  MP_Chan_t numChans;

  /** \brief Length of the waveforms
   */
  unsigned long int filterLen;

  /** \brief Number of waveforms
   */
  unsigned long int numFilters;

  /** \brief Flag indicating if the waveforms have been normalized
   *
   * 0 if not normalized
   * 1 if normalized
   * 2 if an error occured during the normalization.
   */
  unsigned long int normalized;
  unsigned long int centeredAndDenyquisted;

  /** \brief Table of pointers to the waveforms
   *
   * The table is indexed by the number of the filter and the number
   * of the channel. By example, wave[2][1] points to the second
   * channel of the third waveform
  */
  MP_Real_t*** wave;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief Default constructor 
   */
  MP_Anywave_Table_c( void );

  /** \brief Constructor using a filename
   *
   * \param fileName A string containing the name of the file that
   * describes the anywave table ("PATH/anywave.xml" in the example)
   */
  MP_Anywave_Table_c( char* fileName );

  /** \brief Default destructor
   */
  virtual ~MP_Anywave_Table_c( void );

  /***************************/
  /* I/O METHODS             */
  /***************************/

 public:
  /** \brief Print the table structure to a stream
   *
   * the output is the same as the file tableFileName
   *
   * \param fid A writeable stream
   * \return The number of printed characters
   * \remark DO NOT WRITE THE DATA TO the file \a dataFileName
   */
  unsigned long int print ( FILE *fid );

  /** \brief Print the table structure to a file
   *
   * the output is the same as the file tableFileName
   *
   * \param fName A string containing the file name
   * \return The number of printed characters
   * \remark DO NOT WRITE THE DATA TO the file \a dataFileName
   */
  unsigned long int print( const char *fName );

  /** \brief load the data contained in dataFileName, store it in
   * storage and update the pointers in wave
   *
   * \return true for success, false for failure
  **/
  bool load_data( void );



  /* Creates a copy of this table (with all allocations needed) */
  MP_Anywave_Table_c* copy( void );

  /* Creates the dual table, named name, containing, for each filter, its hilbert transform */
  MP_Anywave_Table_c* create_hilbert_dual( char* name );

 private:
  /** \brief Parse the xml file fName that describes the table
   *
   * \return true for success, false for failure
   */
  bool parse_xml_file(const char* fName);

  /***************************/
  /* OTHER METHODS           */
  /***************************/
  
 public:
  /** \brief Test function, called by the test executable test_anywave
   **/
  static bool test( char* filename );
  
  /** \brief Initialize all the members
   **/
  void set_null( void );
  
  /** \brief Re-initialize all the members
   **/
  void reset( void );

  /** \brief Normalize the waveforms and update the flag \a normalize
   *
   * \returns 1 if succeed or 2 if an error occured
   */
  unsigned long int normalize( void );

  /** \brief Sets the mean and the nyquist component of the waveforms
   * to 0, and update the flag \a centeredAndDenyquisted
   *
   * \returns 1 if succeed or 2 if an error occured
   */
  unsigned long int center_and_denyquist( void );

  /** \brief set the \a tableFileName member to  \a fileName
   * \return pointer to the string \a tableFileName
   */
  char* set_table_file_name( const char* fileName );

  /** \brief set the \a dataFileName member to \a fileName
   * \return pointer to the string \a dataFileName
   */
  char* set_data_file_name( const char* fileName );

 private:  
  /** \brief Allocate the pointers array \a wave, using the dimensions
   * \a numFilters and \a numChans
   * 
   * \return false if failed, true if succeed
   **/
  bool alloc_wave( void );

  /** \brief Free the pointer array \a wave
   **/
  void free_wave( void );
};

#endif /* __anywave_table_h_ */
