/******************************************************************************/
/*                                                                            */
/*                         anywave_hilbert_block.h                            */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Mar 07 2006 */
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
/* DEFINITION OF THE anywave hilbert BLOCK CLASS     */
/*                                                   */
/*****************************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2006-07-03 17:27:31 +0200 (lun., 03 juil. 2006) $
 * $Revision: 582 $
 *
 */


#ifndef __anywave_hilbert_block_plugin_h_
#define __anywave_hilbert_block_plugin_h_

#include "anywave_block_plugin.h"

/************************/
/* ANYWAVE_HILBERT BLOCK CLASS  */
/************************/

/** \brief a MP_Anywave_Hilbert_Block_c is a block corresponding to anywave_hilbert atoms. 
 *
 * An anywave atom (MP_Anywave_Atom_c) is only specified by :
 * - a "waveform", that is the series of the time samples, and that is normalized,
 * - an amplitude that multiplies the waveform.
 *
 * Technically, the waveforms are not stored in the MP_Anywave_Atom_c
 * or in the MP_Anywave_Block_c, but in a anywave table
 * (MP_Anywave_Table_c). That anywave table is not stored in the
 * MP_Anywave_Block_c object, but in a global object of type
 * MP_Anywave_Server_c, MP_GLOBAL_ANYWAVE_SERVER, that is visible from all
 * the objects in the library. In the MP_Anywave_Block_c, the
 * anywave table is refered to as the member anywaveTable.
 *
 * All the waveforms in a MP_Anywave_Block_c are stored in the same
 * MP_Anywave_Table_c and then have the same length.
 */
class MP_Anywave_Hilbert_Block_Plugin_c:public MP_Anywave_Block_Plugin_c {

  /********/
  /* DATA */
  /********/

public:

  /** \brief Pointer to the anywave hilbert table
   *
   * In order to save room, the anywave hilbert table is stored in the
   * global MP_Anywave_Server_c object, MP_GLOBAL_ANYWAVE_SERVER, and
   * all the instances of MP_Anywave_Hilbert_Block_c point to it. The
   * anywave hilbert table contains all the waveforms that can be used
   * in this block.
   **/
  MP_Anywave_Table_c* anywaveHilbertTable;
  MP_Anywave_Table_c* anywaveRealTable;

  /** \brief Indice of the anywave table in the global MP_Anywave_Server_c
   * object MP_GLOBAL_ANYWAVE_SERVER.
   *
   * In order to save room, the anywave table is stored in the global
   * MP_Anywave_Server_c object, MP_GLOBAL_ANYWAVE_SERVER, and all the
   * instances of MP_Anywave_Block_c point to it. The anywave table
   * contains all the waveforms that can be used in this block.
   */
  unsigned long int hilbertTableIdx;
  unsigned long int realTableIdx;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

 public:
  
  /** \brief Factory function for a Anywave block
   *
   * a static initialization function that construct a new instance of
   * MP_Anywave_Block_c from a file containing the anywave table
   * \param setSignal the signal on which the block will work
   * \param paramMap the map containing the parameter to construct the block:
   * setFilterShift the filter shift between two successive
   * atoms
   * anywaveTableFileName the name of the file containing the anywave table
   *
   * \return A pointer to the new MP_Anywave_Block_c instance
   **/
   
  static MP_Block_c* create(MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap);

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal */
  virtual int init_parameters( map<string, string, mp_ltstring> *paramMap, char* anywaveTableFileName);

  /** \brief an initializer for the parameters which ARE NOT related to the signal in a parameter map */			       
	virtual int init_parameter_map( map<string, string, mp_ltstring> *paramMap );
			       
  void init_tables(void);
  /** \brief an initializer for the parameters which ARE related to the signal 
   *
   * Check that the number of channels of \a setSignal and \a anywaveTable verify :
   * - as many channels in \a anywaveTable as in \a setSignal, and
   * - only one channel in \a anywaveTable
   */
  virtual int plug_signal( MP_Signal_c *setSignal );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
 MP_Anywave_Hilbert_Block_Plugin_c( void );

  /** \brief Destructor
   **/
  virtual ~MP_Anywave_Hilbert_Block_Plugin_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

 public:

  /** \brief Test function, called by the test executable test_anywave
   *
   * \todo more documentation
   **/
  static bool test( char* signalFileName, unsigned long int filterShift, char* tableFileName );
  
  /** \brief Gives the type of block
   *  \return the string "anywave"
   **/
  virtual const char *type_name( void );
  
  /** \brief Readable text dump 
   * \param fid the stream where to print the info
   * \return the number of printed characters
   **/
  virtual int info( FILE* fid );

  /** \brief Update the inner products
   *
   * update the inner products with a minimum number of arithmetic
   * operations and indicates which frames have been updated.
   *
   * To manage these computations, the member convolution of type
   * MP_Convolution_Fastest_c is used. This object automatically
   * selects the fastest method for the given signal length.
   *
   * \param touch Multi-channel support (i.e., array[s-\>numChans] of
   * MP_Support_t) that identifies which part of each channel of the
   * signal is different from what it was when the block was last
   * updated, so as to update only the IP at places where they might
   * have been touched.
   *
   * \return a support indicating which frames have been touched by
   * the inner products' update
   *
   * \remark Pass touch == NULL to force a full update. */
  MP_Support_t update_ip( const MP_Support_t *touch );
  
  /** \brief Creates a new MP_Anywave_Atom_c atom corresponding to atomIdx in the flat array ip[]
   * 
   * an empty anywave atom is created and all its members are
   * specified. The inner products are computed, and put in the amp
   * array.
   *
   * \remark For a multichannel signal \a signal, one make a
   * difference between monochannel and multichannel filters. 
   *
   * For a monochannel filter \a filter, the channel indexed by \a
   * chanIdx of the \a signal is approximated by : \f$ signal[chanIdx]
   * \approx amp[chanIdx] . filter \f$, where \a amp is the
   * multichannel amplitude computed as : \f$ amp[chanIdx] = \langle
   * signal[chanIdx], filter \rangle \f$. 
   *
   * For a multichannel filter \a filter, the approximation of \a
   * signal is propotionnal to filter, and then the amplitude \a amp
   * is the same on each channel : \f$ signal[chanIdx] \approx amp
   * . filter[chanIdx] \f$. \a amp is computed as : \f$ amp =
   * \sum_{chanIdx=0}^{chanIdx-1} \langle signal[chanIdx],
   * filter[chanIdx] \rangle \f$.
   *
   * \param atom a double pointer to an instance of MP_Anywave_Atom_c,
   * casted to MP_Atom_c
   *
   * \param frameIdx the frame index in the "waveOgram" (analog to the
   * spectrogram when replacing the frequencies by as many waveforms).
   * \param filterIdx the atom number, within the framIdx frame,
   * in the "waveOgram"
   *
   * \return 1 for success and 0 for failure
   **/
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int frameIdx,
			    const unsigned long int filterIdx );

  /** \brief Set only for compatibility with inheritance-related
   * classes - Never used
   *
   * It is empty and will never be called because update_ip is
   * inherited in this class
   **/
  virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ); 
			      
  /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   * \param parameterMapType the map to fill .
   */
  static void get_parameters_type_map(map< string, string, mp_ltstring>* parameterMapType);
   /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   *
   * \param parameterMapInfo the map to fill.
   */
  static void get_parameters_info_map(map< string, string, mp_ltstring>* parameterMapInfo);
   /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   *
   * \param parameterMapDefault the map to fill.
   */
  static void  get_parameters_default_map(map< string, string, mp_ltstring>* parameterMapDefault);
};
#endif /* __anywave_block_plugin_h_ */
