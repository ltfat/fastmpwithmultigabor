/******************************************************************************/
/*                                                                            */
/*                          mdct_block.h            	                      */
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

/****************************************************************/
/*                                                		*/
/* DEFINITION OF THE MDCT BLOCK CLASS           	 	*/
/* RELEVANT TO THE MDCT TIME-FREQUENCY TRANSFORM 		*/
/*                                                		*/
/****************************************************************/


#ifndef __mdct_block_plugin_h_
#define __mdct_block_plugin_h_

#include "mclt_abstract_block_plugin.h"

/********************************/
/* MDCT BLOCK CLASS    		*/
/********************************/

/** \brief Blocks corresponding to MDCT frames 
 *
 */
class MP_Mdct_Block_Plugin_c:public MP_Mclt_Abstract_Block_Plugin_c {

  /********/
  /* DATA */
  /********/

  /** \brief Storage space for the atom energy
   * 
   */ 
  MP_Real_t *atomEnergy;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
public:

   /** \brief Factory function for a generalized mdct block
   *
   * \param setSignal the signal on which the block will work
   * \param paramMap the map containing the parameter to construct the block, for a generalized mdct block 
   */
  static MP_Block_c* create( MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap );
 
  /** \brief an initializer for the parameters which ARE related to the signal 
   * \param setSignal the signal to set
   */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal
   *  \param setFilterLen the length of the signal window, in number of samples
   *  \param setFilterShift the window shift, in number of samples
   *  \param setFftSize the size of the FFT, including zero padding
   *  \param setWindowType the window type (see the doc of libdsp_windows.h)
   *  \param setWindowOption the optional window parameter.
   *  \param setBlockOffset the block offset
   * Warning: In this case, the window type must be rectangle, cosine or kbd. 
   * 
   *  */
  virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const unsigned long int setBlockOffset );
			    
/** \brief an initializer for the parameters which ARE NOT related to the signal in a parameter map
   *  \param setFilterLen the length of the signal window, in number of samples
   *  \param setFilterShift the window shift, in number of samples
   *  \param setFftSize the size of the FFT, including zero padding
   *  \param setWindowType the window type (see the doc of libdsp_windows.h)
   *  \param setWindowOption the optional window parameter.
   *  \param setBlockOffset the block offset
   * Warning: In this case, the window type must be rectangle, cosine or kbd. 
   */

  virtual int init_parameter_map( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const unsigned long int setBlockOffset );
			       
  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Mdct_Block_Plugin_c( void );

public:

  /* Destructor */
  virtual ~MP_Mdct_Block_Plugin_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text output */
  virtual int info( FILE *fid );

  /** \brief update the inner products of a given frame and return the
   * correlation \a maxCorr and index in the frame \a maxFilterIdx of the
   * maximally correlated atom on the frame
   *
   * \param frameIdx the index of the frame used for the inner products
   *
   * \param maxCorr a MP_Real_t* pointer to return the value of the maximum
   * inner product (or maximum correlation) in this frame
   *
   * \param maxFilterIdx an unsigned long int* pointer to return the index of
   * the maximum inner product
   *
   * On each frame, this method computes the square of the correlation values
   * normalized by the atom's energy
   *
   * \sa MP_Block_c::update_frame()
   * \sa MP_Block_c::update_ip()
   */
  virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ); 

  /** \brief Creates a new MDCT atom corresponding to (frameIdx,filterIdx)
   * 
   * The waveform of a MDCT atom is
   * \f[
   * \mbox{window}(t) \cdot \mbox{amp} \cdot cos \left[  \frac{\pi}{L} \left( t + \frac{1}{2} + \frac{L}{2} \right) \left( f + \frac{1}{2} \right) \right]
   * \f]
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int frameIdx,
			    const unsigned long int filterIdx );

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

protected:

  /** \brief Allocates the atom's energy.
   */
  int alloc_energy( MP_Real_t **atomEnergy );

  /** \brief Computes and tabulates the atom's energy.
   */
  int fill_energy( MP_Real_t *atomEnergy );


};
 

#endif /* __mdct_block_plugin_h_ */
