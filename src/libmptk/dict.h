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
  /** \brief The various possible signal modes: */
#define MP_DICT_NULL_SIGNAL     0
#define MP_DICT_EXTERNAL_SIGNAL 1 /* => don't delete the sig when deleting the dict */
#define MP_DICT_INTERNAL_SIGNAL 2 /* => delete the sig when deleting the dict */

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
  /** \brief Factory function
   *
   * \param dictFileName The name of the XML file parsed for a dictionary.
   * \param sigFileName The file name for the signal read into the dictionary.
   */
  static MP_Dict_c* init( const char* dictFileName, const char* sigFileName );

  /** \brief Factory function which reads the dictionary from a file
   *
   * \param dictFileName The name of the XML file parsed for a dictionary.
   *
   * WARNING: this function does not set a signal in the dictionary.
   * It is mandatory to call dict.copy_signal( signal ) or
   * dict.plug_signal( signal ) before starting to iterate.
   */
  static MP_Dict_c* init( const char* dictFileName );

  /** \brief Factory function which creates an empty dictionary.
   *
   * This function makes it possible to create an empty dictionary
   * and then independently call dict.add_blocks( fileName ) and
   * dict.copy_signal( signal ) or dict.plug_signal( signal ).
   *
   */
  static MP_Dict_c* init( void );

protected:
  /* NULL constructor */
  MP_Dict_c();

public:
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
   * \param fName a file name where the structure is read from
   * \return the number of added blocks
   */
  int add_blocks( const char *fName );


  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Test
   */
  static int test( char* signalFileName, char* dicoFileName );

  /** \brief Get the number of atoms of the dictionary */
  unsigned long int num_atoms( void );


  /** \brief Copy a signal into the dictionary
   *
   * The dictionary will act on a local copy of the passed signal
   * (equivalent to a pass by value). Any previously present signal
   * will be replaced.
   *
   * \param setSignal the signal to store into the dictionary.
   * (NULL resets any signal-related info in the dictionary.)
   *
   * \return nonzero in case of failure, zero otherwise.
   */
  int copy_signal( MP_Signal_c *setSignal );

  /** \brief Copy a new signal from a file to the dictionary
   *
   * The dictionary will act on a local copy of the passed signal
   * (equivalent to a pass by value). Any previously present signal
   * will be replaced.
   *
   * \param fName the name of the file where to read the stored signal
   *
   * \return nonzero in case of failure, zero otherwise.
   */
  int copy_signal( const char *fName );

  /** \brief Plug (or hook) a new signal to the dictionary
   *
   * The dictionary will act directly on the passed signal
   * (equivalent to a pass by reference). Any previously present signal
   * will be replaced.
   *
   * \param setSignal the signal to reference into the dictionary
   * (NULL resets any signal-related info in the dictionary.)
   *
   * \return nonzero in case of failure, zero otherwise.
   */
  int plug_signal( MP_Signal_c *setSignal );

  /** \brief Detach the signal from the dictionary, and reset
   * the dictionary signal to NULL.
   *
   * \return The signal which was formerly plugged/copied
   * into the dictionary.
   */
  MP_Signal_c* detach_signal( void );


private:
  /** \brief Allocate the touch array according to the signal size */
  int alloc_touch( void );

public:
  /** \brief Add a block to a dictionary
   *
   * \param newBlock the reference of the block to add.
   *
   * \return the number of added blocks, i.e. 1 if the block
   * has been successfully added, or 0 in case of failure or
   * if newBlock was initially NULL.
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
