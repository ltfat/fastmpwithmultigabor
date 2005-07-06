/******************************************************************************/
/*                                                                            */
/*                                 block.h                                    */
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

/**************************************/
/*                                    */
/* DEFINITION OF THE BASE BLOCK CLASS */
/*                                    */
/**************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/05/16 14:41:39 $
 * $Revision: 1.1 $
 *
 */


#ifndef __block_h_
#define __block_h_


/***********************/
/* BLOCK CLASS         */
/***********************/
/**
 * \brief Blocks efficiently compute and search through the inner products
 * between a signal and a family of regularly shifted atoms of a given size.
 */
class MP_Block_c {
  
  /********/
  /* DATA */
  /********/

public:
  /** \brief bond to the analyzed signal */
  MP_Signal_c *s;
  /* General characteristics of the atoms */
  /** \brief common length of the atoms (== frame length) */
  unsigned long int filterLen;   
  /** \brief sliding step between two consecutive frames (== frame shift) */
  unsigned long int filterShift; 
  /** \brief number of atoms/filters per frame */
  unsigned long int numFilters;  
  /** \brief number of frames for each channel (determined by
   * the size of the signal and the frame shift) */
  unsigned long int numFrames;   
  
  /* Max of inner products (in fact, max of the squared energy): */
  /** \brief value of the max IP across all frames */
  MP_Real_t maxIPValue;                     
  /** \brief index of the frame where the max value is */
  unsigned long int maxIPFrameIdx;          
  /** \brief max IP value for each frame */
  MP_Real_t *maxIPValueInFrame; 
  /** \brief index of the frequency where the max IP is for each frame */
  unsigned long int *maxIPIdxInFrame; 
  /** \brief index where the max IP is in the flat array ipStorage[] */
  unsigned long int maxAtomIdx;

  /** \brief The following variables support an arborescent search of the max IP: */

  /** \brief Number of levels in the search tree */
  unsigned long int numLevels;
  /** \brief The tree structure that propagates the max value */
  MP_Real_t **elevator;
  /** \brief The storage space for the tree structure that propagates the max value */
  MP_Real_t *elevSpace;
  /** \brief The tree structure that propagates the max frame index */
  unsigned long int **elevatorFrame;
  /** \brief The storage space for the tree structure that propagates the max frame index */
  unsigned long int *elevFrameSpace;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief a constructor which allocates the storage space and initializes it to zero */
  MP_Block_c( MP_Signal_c *s,
	      const unsigned long int filterLen,
	      const unsigned long int filterShift,
	      const unsigned long int numFilters );

  /* Destructor */
  virtual ~MP_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

private:

  /** \brief allocates the storage space and initializes it to zero */
  int alloc_block( MP_Signal_c *s,
		   const unsigned long int filterLen,
		   const unsigned long int filterShift,
		   const unsigned long int numFilters );

public:

  /** \brief Get the type of the block as a string
   *
   * \return the type as a string */
  virtual char * type_name( void ) = 0;
  /** \brief Send a brief information about the block to a stream
   * \param fid A writeable block
   * \return The number of written characters.
   */
  virtual int info( FILE *fid ) = 0;
  /** \brief Get the number of atoms of the block */
  virtual unsigned long int size( void );

  /* Other */
  /** \brief update the inner products with a minimum number of arithmetic operations
   * and indicates which frames have been updated.
   *
   * \param touch Multi-channel support (i.e., array[s->numChans] of MP_Support_t)
   * that identifies which part of each channel of the signal is different from what it was
   * when the block was last updated, so as to update only the IP at places
   * where they might have been touched.
   * \return a support indicating which frames have been touched by the inner products' update 
   * \remark Pass touch == NULL to force a full update. */
  virtual MP_Support_t update_ip( const MP_Support_t *touch ) = 0; 

  /** \brief find the location of the maximum inner product (max IP)
   * \param frameSupport a support indicating which frames have been touched
   *  by a previous update of the inner products
   */
  MP_Real_t update_max( const MP_Support_t frameSupport );

  /** \brief create a new atom corresponding to a given atomIdx in the flat array ip[].
   *
   * \param atom a pointer to (or an array of) reference to the returned atom(s)
   * \param atomIdx  index of the atom to be extracted by the block
   * \return the number of extracted atom */  
  virtual unsigned int create_atom( MP_Atom_c** atom,
				    const unsigned long int atomIdx ) = 0;

};


#endif /* __atom_h_ */
