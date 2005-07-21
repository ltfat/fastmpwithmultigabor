/******************************************************************************/
/*                                                                            */
/*                             dirac_block.h                               */
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

/**************************************************/
/*                                                */
/* DEFINITION OF THE DIRAC BLOCK CLASS            */
/*                                                */
/**************************************************/
/*
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __dirac_block_h_
#define __dirac_block_h_


/************************/
/* DIRAC BLOCK CLASS    */
/************************/

/** \brief Dirac blocks are the most basic blocks of the MP_Block_c class.
 *
 * They are used to create (multichannel) dirac atoms */
class MP_Dirac_Block_c:public MP_Block_c {

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Constructor of a dirac block 
   * \param s the signal on which the block will work */
  MP_Dirac_Block_c( MP_Signal_c *s);

  /* Destructor */
  virtual ~MP_Dirac_Block_c();


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
  
  /** \brief Creates a new dirac atom corresponding to atomIdx in the flat array ip[]
   * \todo   Describe how the atom is determined here.
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int atomIdx );

};


/*************/
/* FUNCTIONS */
/*************/

/** \brief Add the dirac block to a dictionary
 *
 * \param dict The dictionnary that will be modified
 * \return one upon success, zero otherwise 
 *
 * \sa MP_Dirac_Block_c::MP_Dirac_Block_c()
 */
int add_dirac_block( MP_Dict_c *dict );


#endif /* __dirac_block_h_ */
