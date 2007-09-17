/******************************************************************************/
/*                                                                            */
/*                             constant_block.h                               */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Mon Apr 03 2006 */
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

/**************************************************/
/*                                                */
/* DEFINITION OF THE CONSTANT BLOCK CLASS         */
/*                                                */
/**************************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2006-07-03 17:27:31 +0200 (lun., 03 juil. 2006) $
 * $Revision: 582 $
 *
 */


#ifndef __constant_block_plugin_h_
#define __constant_block_plugin_h_


/************************/
/* CONSTANT BLOCK CLASS    */
/************************/

/**
 * \brief Constants are rectangular windows, of unit norm
 *
 * They are used to create (multichannel) constant atoms */

class MP_Constant_Block_Plugin_c:public MP_Block_c {

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Factory function of a constant block 
   *  \param s the signal on which the block will work 
   *  \param paramMap the parametrer map defining the block
   */
   
  static MP_Block_c* create( MP_Signal_c *s,
				    map<string, string, mp_ltstring> *paramMap );

  /** \brief an initializer for the parameters which ARE related to the signal */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal
   * \param filterLen the length of the signal window, in number of samples
   * \param filterShift the window shift, in number of samples
   * \param blockOffset the block offset
   */
  virtual int init_parameters( const unsigned long int filterLen,
			       const unsigned long int filterShift,
			       const unsigned long int blockOffset );
			       
   /** \brief an initializer for the parameter map which ARE NOT related to the signal for the map parameter 
   * \param filterLen the length of the signal window, in number of samples
   * \param filterShift the window shift, in number of samples
   * \param blockOffset the block offset
   */
  virtual int init_parameter_map( const unsigned long int filterLen,
			       const unsigned long int filterShift,
			       const unsigned long int blockOffset );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Constant_Block_Plugin_c( void );

public:
  /* Destructor */
  virtual ~MP_Constant_Block_Plugin_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text dump */
  virtual int info( FILE* fid );

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
   * \todo Describe computation of inner products in the Constant case.
   *
   * \sa MP_Block_c::update_frame()
   * \sa MP_Block_c::update_ip()
   */
  virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ); 

  /** \brief Creates a new constant atom corresponding to (frameIdx,filterIdx)
   * \todo   Describe how the atom is determined here.
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

};

#endif /* __constant_block_plugin_h_ */
