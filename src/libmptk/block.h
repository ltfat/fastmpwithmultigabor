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
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-07-24 17:27:43 +0200 (mar., 24 juil. 2007) $
 * $Revision: 1120 $
 *
 */


#ifndef __block_h_
#define __block_h_

#include <map>

using namespace std;

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
  /** \brief global block offset : position of the first frame */
  unsigned long int blockOffset;
  /** \brief number of frames for each channel (determined by
  * the size of the signal, the frame shift and the block offset) */
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
  
  protected:
  /** \brief pointer on map defining the parameter of the block */
  map<string, string, mp_ltstring> *parameterMap;
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief an initializer for the parameters which ARE related to the signal */
  MPTK_LIB_EXPORT virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal */
  MPTK_LIB_EXPORT virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setNumFilters, 
			       const unsigned long int setBlockOffset);

  /** \brief nullification of the signal-related parameters */
  MPTK_LIB_EXPORT virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MPTK_LIB_EXPORT MP_Block_c( void );

public:
  /* Destructor */
  MPTK_LIB_EXPORT virtual ~MP_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /** \brief Get the type of the block as a string
   *
   * \return the type as a string */
  MPTK_LIB_EXPORT virtual const char * type_name( void ) = 0;
  /** \brief Send a brief information about the block to a stream
   * \param fid A writeable block
   * \return The number of written characters.
   */
  MPTK_LIB_EXPORT virtual int info( FILE *fid ) = 0;

  /** \brief Get the number of atoms of the block */
  MPTK_LIB_EXPORT virtual unsigned long int num_atoms( void );

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
  MPTK_LIB_EXPORT virtual MP_Support_t update_ip( const MP_Support_t *touch );

  /** \brief update the inner products of a given frame and return the correlation
   * \a maxCorr and index in the frame \a maxFilterIdx of the maximally correlated
   * atom on the frame
   *
   * \param frameIdx the index of the frame used for the inner products
   *
   * \param maxCorr a MP_Real_t* pointer to return the value of the maximum
   * inner product (or maximum correlation) in this frame
   *
   * \param maxFilterIdx an unsigned long int* pointer to return the index of
   * the maximum inner product
   */
  MPTK_LIB_EXPORT virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ) = 0; 

  /** \brief find the location of the maximum inner product (max IP)
   * \param frameSupport a support indicating which frames have been touched
   *  by a previous update of the inner products
   */
  MPTK_LIB_EXPORT MP_Real_t update_max( const MP_Support_t frameSupport );

  /** \brief create a new atom corresponding to a given (frameIdx,filterIdx)
   *
   * \param atom a pointer to (or an array of) reference to the returned atom(s)
   * \param frameIdx the frame coordinate of the atom
   * \param filterIdx the position of the atom in the frame
   * \return the number of extracted atom */  
  MPTK_LIB_EXPORT virtual unsigned int create_atom( MP_Atom_c** atom,
				    const unsigned long int frameIdx,
				    const unsigned long int filterIdx ) = 0;
				    
 MPTK_LIB_EXPORT map< string, string, mp_ltstring> * get_block_parameters_map();
 
};


#endif /* __atom_h_ */
