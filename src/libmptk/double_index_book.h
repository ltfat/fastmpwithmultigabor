/******************************************************************************/
/*                                                                            */
/*                                 double_index_book.h                        */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Boris Mailhé                                               Tue Aug 05 2008 */
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

/*********************************************/
/*                                           */
/* DEFINITION OF THE DOUBLE INDEX BOOK CLASS */
/*                                           */
/*********************************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-03-28 15:20:39 +0200 (Wed, 28 Mar 2007) $
 * $Revision: 1017 $
 *
 */


#ifndef __double_index_book_h_
#define __double_index_book_h_

/***********************/
/* BOOK CLASS          */
/***********************/
/**
 * \brief Books store and manage sequences of atoms collected
 * while iterating several steps of Matching Pursuit. 
 */
class GP_Double_Index_Book_c:public MP_Book_c{
  
public:

  /********/
  /* DATA */
  /********/
    
  GP_Block_Book_c* sortBook;   
  
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  MPTK_LIB_EXPORT static GP_Double_Index_Book_c* create();
  MPTK_LIB_EXPORT static GP_Double_Index_Book_c* create(MP_Chan_t numChans, 
							unsigned long int numSamples, 
							int sampleRate,
                            unsigned int numBlocks );
private:
  /* NULL constructor */
  MPTK_LIB_EXPORT GP_Double_Index_Book_c();

public:
  /* Destructor */
  MPTK_LIB_EXPORT ~GP_Double_Index_Book_c();


  /***************************/
  /* I/O METHODS             */
  /***************************/

  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Clear all the atoms from the book.
   */
  MPTK_LIB_EXPORT void reset( void );

  /** \brief Add a new atom in the storage space, taking care of the necessary allocations 
   * \param newAtom a reference to an atom
   * \return the number of appended atoms (1 upon success, zero otherwise)
   * \remark The reference newAtom is not copied, it is stored and will be deleted when the book is destroyed
   * \remark \a numChans is set up if this is the first atom to be appended to the book,
   * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
   */
   
   MPTK_LIB_EXPORT int append( MP_Atom_c *newAtom );
   
   /** \brief append two books together 
   * \param newBook a reference to a book
   * \return the number of appended atoms (1 upon success, zero otherwise)
   * \remark Use int append( MP_Atom_c *newAtom )
   */
  
  MPTK_LIB_EXPORT unsigned long int append( GP_Double_Index_Book_c *newBook );

  /** \brief get the sub-book containing the neighbours of a given atom in the time domain
   * \param the central atom
   * \return the neighbourhood sub-book
   * */
   
   MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* get_neighbours (MP_Atom_c*, MP_Dict_c*);
};


#endif /* __book_h_ */
