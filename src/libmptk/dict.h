/******************************************************************************/
/*                                                                            */
/*                                 dict.h                                     */
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

/***************************************/
/*                                     */
/* DEFINITION OF THE DICTIONNARY CLASS */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __dict_h_
#define __dict_h_


/***********************/
/* DICT CLASS          */
/***********************/
/** \brief Dictionaries are unions of several blocks used to perform Matching Pursuit.
 */
class MP_Dict_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief Signal manipulated by the dictionnary */
  MP_Signal_c *signal;         
  /** \brief The "mode" of the signal: either the signal was copied into the dictionary,
   * or it was just referenced. */
  int sigMode;

  /** \brief Number of blocks stored in the dictionary */
  unsigned int numBlocks;      
  /** \brief Storage space : an array of [numBlocks] block objects */
  MP_Block_c** block;    
  /** \brief Index in the storage space of the block which holds the max inner product */
  unsigned int blockWithMaxIP; 

  /** \brief Multi-channel support that identifies which part of the signal is different from
   * what it was when the blocks were last updated 
   */
  MP_Support_t *touch;         


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief Signal-passing constructor.
   *
   * \param setSignal the signal to store into the dictionary. If NULL,
   * just create the dictionary without storing a signal.
   * \param mode can be MP_DICT_SIG_HOOK if you want the dictionary to act
   * directly on the passed signal (equivalent to a pass by reference),
   * or MP_DICT_SIG_COPY if you want the dictionary to act on a local copy
   * of the passed signal (equivalent to a pass by value). */
  MP_Dict_c( MP_Signal_c *setSignal, int mode );
#define MP_DICT_SIG_HOOK 1
#define MP_DICT_SIG_COPY 2

  /** \brief Constructor which reads the signal that will be
      manipulated by the dictionary from a file */
  MP_Dict_c( const char* sigFileName );

  /* Destructor */
  ~MP_Dict_c();


  /***************************/
  /* I/O METHODS             */
  /***************************/

  /** \brief Print the dictionary structure to a stream,
   * in an ascii form which can be used by add_blocks()
   *
   * \param fid A writeable stream
   * \return The number of printed characters
   */
  int print ( FILE *fid );

  /** \brief Print the dictionary structure to a file, in an ascii form which can be used by add_blocks()
   *
   * \param fName A string containing the file name
   * \return The number of printed characters
   */
  int print( const char *fName );

  /** \brief Add a few blocks which structure is determined by the content of a stream
   * \param fid a readable stream where the structure is read from
   * \return the number of added blocks
   */
  int add_blocks( FILE *fid );

  /** \brief Same as MP_Dict_c::add_blocks(FILE *) but with a file instead of a stream
   */
  int add_blocks( const char *fName );


  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Get the number of atoms of the dictionary */
  unsigned long int size( void );


  /** \brief Hook or copy a new signal to the dictionary
   *
   * \param setSignal the signal to store into the dictionary
   * \param mode can be MP_DICT_SIG_HOOK if you want the dictionary to act
   * directly on the passed signal (equivalent to a pass by reference),
   * or MP_DICT_SIG_COPY if you want the dictionary to act on a local copy
   * of the passed signal (equivalent to a pass by value). */
  int reset_signal( MP_Signal_c *setSignal, int mode );

private:
  /** \brief Allocate the touch array according to the signal size */
  int alloc_touch( void );

public:
  /** \brief Add a block to a dictionary
   */
  int add_block( MP_Block_c *newBlock ); 


  /** \brief Delete all the blocks from a dictionary
   *
   * \return The number of deleted blocks
   */
  int delete_all_blocks( void ); 


  /** \brief Compute all the inner products which need to be updated and finds the max. 
   *
   * Side-effect : blockWithMaxIP is updated.
   * \return The value of the maximum inner product
   */
  MP_Real_t update( void ); 

  /** \brief Compute all the inner products and finds the max 
   *
   * Side-effect : blockWithMaxIP is updated.
   * \return The value of the maximum inner product
   */
  MP_Real_t update_all( void ); 

  /** \brief create a new atom corresponding to the best atom of the best block.
   *
   * \param atom a pointer to (or an array of) reference to the returned atom(s)
   * \return the number of extracted atom */  
  unsigned int create_max_atom( MP_Atom_c** atom );


  /** \brief Perform one matching pursuit iteration (update the blocks, find the max, 
   * create the atom and append it to the book, and substract it from the analyzed signal).
   *
   * \param book The book where to append the selected atom
   * \param sigRecons A signal where to add the selected atom for online reconstruction
   * \return one upon success, zero otherwise
   * \remark Pass sigRecons == NULL to skip the reconstruction step
   * \remark Ideally, the number of arithmetic operations is kept to a minimum.
   */
  int iterate_mp( MP_Book_c* book , MP_Signal_c* sigRecons );   


};


#endif /* __dict_h_ */
