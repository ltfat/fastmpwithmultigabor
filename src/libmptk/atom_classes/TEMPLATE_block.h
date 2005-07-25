/******************************************************************************/
/*                                                                            */
/*                             TEMPLATE_block.h                               */
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

/*****************************************************/
/*                                                   */
/* DEFINITION OF THE TEMPLATE BLOCK CLASS            */
/* RELEVANT TO THE TEMPLATE TIME-FREQUENCY TRANSFORM */
/*                                                   */
/*****************************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __TEMPLATE_block_h_
#define __TEMPLATE_block_h_


/* YOUR includes go here. */


/************************/
/* TEMPLATE BLOCK CLASS    */
/************************/

/** \brief Explain what YOUR block does here. */
class MP_TEMPLATE_Block_c:public MP_Block_c {

  /********/
  /* DATA */
  /********/

public:


  /* YOUR specific data members go here. */


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Describe what YOUR constructor does.
   */
  MP_TEMPLATE_Block_c( MP_Signal_c *s,
		    const unsigned long int filterLen,
		    const unsigned long int filterShift,
		    const unsigned long int numFilters
		       /* YOUR specific parameters go here
			  ( don't forget the preceding comma) */ );

  /* Destructor */
  virtual ~MP_TEMPLATE_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text dump */
  virtual int info( FILE* fid );

  /* Update of part of the inner products */
  MP_Support_t update_ip( const MP_Support_t *touch );
  
  /** \brief Creates a new TEMPLATE atom corresponding to atomIdx in the flat array ip[]
   * 
   *   Describe how the atom is determined here.
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int atomIdx );

};


/*************/
/* FUNCTIONS */
/*************/

/** \brief Add a TEMPLATE block to a dictionary
 *
 * \param dict The dictionnary that will be modified
 * \param filterLen   YOUR interpretation goes here
 * \param filterShift YOUR interpretation goes here
 * \param numFilters  YOUR interpretation goes here
 * \return one upon success, zero otherwise 
 *
 * \sa MP_TEMPLATE_Block_c::MP_TEMPLATE_Block_c()
 */
int add_TEMPLATE_block( MP_Dict_c *dict,
		     const unsigned long int filterLen,
		     const unsigned long int filterShift,
		     const unsigned long int numFilters
		       /* YOUR specific parameters go here
			  ( don't forget the preceding comma) */ );


/** \brief Add a family of TEMPLATE blocks to a dictionary.
 *
 * Describe YOUR concept of a TEMPLATE block family.
 *
 * \param dict The dictionnary that will be modified
 *
 * \return the number of added blocks
 *
 * \sa MP_Dict_c::add_TEMPLATE_block()
 */
int add_TEMPLATE_blocks( MP_Dict_c *dict
		       /* YOUR specific parameters go here
			  ( don't forget the preceding comma) */ );


#endif /* __TEMPLATE_block_h_ */
