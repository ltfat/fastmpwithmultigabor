/******************************************************************************/
/*                                                                            */
/*                              anywave_atom.h                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Thu Nov 03 2005 */
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

/*****************************************************/
/*                                                   */
/* DEFINITION OF THE anywave ATOM CLASS,            */
/* RELEVANT TO THE anywave TIME-FREQUENCY TRANSFORM */
/*                                                   */
/*****************************************************/
/*
 * CVS log:
 *
 * $Author: broy $
 * $Date: 2007-04-24 19:30:55 +0200 (mar., 24 avr. 2007) $
 * $Revision: 1021 $
 *
 */


#ifndef __anywave_hilbert_atom_plugin_h_
#define __anywave_hilbert_atom_plugin_h_

#include "anywave_atom_plugin.h"

/******************************/
/* anywave hilbert ATOM CLASS */
/******************************/

/**
 * \brief The anywaveTable MUST contain atoms without mean and Nyquist components.
 **/
class MP_Anywave_Hilbert_Atom_Plugin_c: public MP_Anywave_Atom_Plugin_c {

  /********/
  /* DATA */
  /********/

 public:
  /** \brief Index of the anywave hilbert table
   *
   * Index of the anywave hilbert table that contains the hilbert
   * waveforms, in the global anywave server MP_GLOBAL_ANYWAVE_SERVER
   */
  unsigned long int hilbertTableIdx;
  /** \brief Index of the anywave real table
   *
   */
  unsigned long int realTableIdx;
  /** \brief pointer to the anywave hilbert table
   *
   * it is NULL if no anywave hilbert table is associated to the
   * atom. Otherwise, it points to a anywave hilbert table, generally
   * stored in the global anywave server MP_GLOBAL_ANYWAVE_SERVER.
   */
  MP_Anywave_Table_c* anywaveHilbertTable;
  /** \brief pointer to the anywave real table
   *
   */
  MP_Anywave_Table_c* anywaveRealTable;

  /** \brief The four parameters
   * (the sum of the squares is one)
   */
  MP_Real_t* realPart;
  MP_Real_t* hilbertPart;

 public:
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

  /** \brief Default constructor 
   *
   * sets the ampHilbert array, and anywaveHilbertTable to NULL and
   * atomType, and hilbertTableIdx to zero
   *
   **/
  MP_Anywave_Hilbert_Atom_Plugin_c( void );


  /** \brief Specific factory function
   **/
   static MP_Atom_c* anywave_hilbert_atom_create_empty(void);

  /** \brief File constructor
   *
   * construct the atom from a stream, in general when reading a
   * book. The stream shall follow the syntax of books, e.g. for text
   * formatted stream :
   *
   * - \<par type="filename"\>./anywaveTable.bin\</par\>
   * - \<par type="filterIdx"\>13\</par\>
   * - \<anywavePar chan = "1"\>
   *   - \<par type="amp"\>12.7\</par\>
   *   - \<par type="phase"\>0.23\</par\>
   * - \</anywavePar\>
   * - ...
   *
   * \param fid the stream 
   *
   * \param mode MP_TEXT or MP_BINARY
   **/
  static MP_Atom_c* create( FILE *fid, MP_Dict_c *dict, const char mode );

  /** \brief Default destructor 
   **/
  virtual ~MP_Anywave_Hilbert_Atom_Plugin_c( void );


  int init_parts(void);

  int init_tables( void);
  
   /** \brief Internal allocations of all the vectors */
  virtual int alloc_hilbert_atom_param( const MP_Chan_t setNumChans );

  virtual int read( FILE *fid, 
		    const char mode );
  

  /***************************/
  /* OUTPUT METHOD           */
  /***************************/

  /** \brief Write the atom to the stream
   *
   * write the atom to the stream, in general when writing a book. The
   * syntax, for text formatted stream, is like in the following
   * example :
   *
   * - \<par type="filename"\>./anywaveTable.bin\</par\>
   * - \<par type="filterIdx"\>13\</par\>
   * - \<anywavePar chan = "1"\>
   *   - \<par type="amp"\>12.7\</par\>
   *   - \<par type="phase"\>0.62\</par\>
   * - \</anywavePar\>
   * - ...
   *
   * \param fid the stream
   *
   * \param mode MP_TEXT or MP_BINARY
   **/
  virtual int write( FILE *fid, const char mode );

  /***************************/
  /* OTHER METHODS           */
  /***************************/

  /** \brief returns the type of the atom : "anywavehilbert"
   * \return the string "anywavehilbert"
   **/
  virtual const char * type_name(void);

  /** \brief Print the details of the anywave hilbert atom to the stream
   **/
  virtual int info();

  /** \brief Build the waveform of an anywave hilbert atom. 
   *
   * if the waveform has only one channel, then, for each channel,
   * copy to outBuffer the sum of :
   * - this monochannel waveform, multiplied by cos(phase[chanIdx])
   * - the hilbert transform of the monochannel waveform, multiplied
   * by sin(phase[chanIdx])
   *   all p multiplied by the amplitude on the channel amp[chanIdx]
   *
   * if the waveform has as many channels as the signal, then, for
   * each channel \a chanIdx, copy the sum of the channel \a chanIdx
   * of the waveform, multiplied by cos(phase[0]) and of the
   * hilbert transform of the channel \a chanIdx of the waveform,
   * multiplied by sin(phase[0]), all multiplied by amp[0]
   * 
   *
   * \param outBuffer the buffer containing the samples of the output
   * waveform, channel after channel
   **/
  virtual void build_waveform( MP_Real_t *outBuffer );

  /** \brief NOT IMPLEMENTED
   **/
  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

  /** \brief Check if the atom has the field
   *
   * The following fields will return MP_TRUE in this class, in
   * addition to those of the class MP_Anywave_Atom_c :
   * - MP_REAL_PART_PROP (realPart)
   * - MP_HILBERT_PART_PROP (hilbertPart)
   * - MP_REAL_TABLE_IDX_PROP (realTableIdx)
   * - MP_ANYWAVE_REAL_TABLE_PROP (anywaveRealTable)
   * - MP_HILBERT_TABLE_IDX_PROP (hilbertTableIdx)
   * - MP_ANYWAVE_HILBERT_TABLE_PROP (anywaveHilbertTable)
   *
   * \param field the field to check, eg, MP_AMP_HILBERT_PROP for the
   * member ampHilbert
   *
   * \return MP_TRUE if the field exists, MP_FALSE else
   **/
  virtual int       has_field( int field );

  /** \brief Get the value of the field \a field on the channel \a chanIdx
   *
   * If the atom has the field, return its value on the channel
   * chanIdx. For example, get_field( MP_MEAN_PART_PROP, 1 ) returns the
   * amplitude part due to the mean on the second channel.
   * 
   * The available fields for the get_value() function are :
   * - MP_REAL_TABLE_IDX_PROP (realTableIdx)
   * - MP_HILBERT_TABLE_IDX_PROP (hilbertTableIdx)
   * - MP_REAL_PART_PROP (realPart)
   * - MP_HILBERT_PART_PROP (hilbertPart)
   *
   * \param field the field 
   *
   * \param chanIdx the index of the channel
   *
   * \return the value of the field on the specified channel
   **/
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );

};

#endif /* __anywave_hilbert_atom_plugin_h_ */
