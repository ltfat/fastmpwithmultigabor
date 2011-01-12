/******************************************************************************/
/*                                                                            */
/*                                 double_index_book.cpp                      */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Boris Mailh�                                               Tue Aug 05 2008 */
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
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

/**************************************************************/
/*                                                            */
/* double_index_book.cpp: methods for class GP_Double_Book_c  */
/*                                                            */
/**************************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/******************/
/* Constructor    */
GP_Double_Index_Book_c* GP_Double_Index_Book_c::create() {

  const char* func = "GP_Double_Index_Book_c::create()";
  GP_Double_Index_Book_c *newBook = NULL;

  /* Instantiate and check */
  newBook = new GP_Double_Index_Book_c();
  if ( newBook == NULL ) 
  {
	  mp_error_msg( func, "Failed to create a new double indexed book.\n" );
	  return( NULL );
  }
  
  /* Allocate the atom array */
  if ( (newBook->atom = (MP_Atom_c**) calloc( MP_BOOK_GRANULARITY, sizeof(MP_Atom_c*) )) == NULL ) 
  {
	  mp_warning_msg( func, "Can't allocate storage space for [%lu] atoms in the new book. The atom array is left un-initialized.\n", MP_BOOK_GRANULARITY );
	  newBook->atom = NULL;
	  newBook->maxNumAtoms = 0;
  }
  else 
	  newBook->maxNumAtoms = MP_BOOK_GRANULARITY;

  return( newBook );
}

GP_Double_Index_Book_c* GP_Double_Index_Book_c::create(MP_Chan_t setNumChans, unsigned long int setNumSamples, int setSampleRate, unsigned int numBlocks ) {

  const char* func = "GP_Double_Index_Book_c::create(MP_Chan_t numChans, unsigned long int numSamples, unsigned long int numAtoms )";
  GP_Double_Index_Book_c *newBook = NULL;

  /* Instantiate and check */
  newBook = create();
  if ( newBook == NULL ) {
    mp_error_msg( func, "Failed to create a new book.\n" );
    return( NULL );
  }
  
  newBook->sortBook = new GP_Block_Book_c(numBlocks);
  newBook->numChans = setNumChans;
  newBook->numSamples = setNumSamples;
  newBook->sampleRate = setSampleRate;

  return( newBook );
}

/***********************/
/* NULL constructor    */
GP_Double_Index_Book_c::GP_Double_Index_Book_c() {
}

/**************/
/* Destructor */
GP_Double_Index_Book_c::~GP_Double_Index_Book_c() {
	if ( sortBook )
		delete sortBook;
}

/***************************/
/* I/O METHODS             */
/***************************/

/***************************/
/* MISC METHODS            */
/***************************/

/***********************/
/* Clear all the atoms */
void GP_Double_Index_Book_c::reset( void ) {
    
    sortBook->reset();
    numAtoms = 0;
}

/******************/
/* Append an atom */
int GP_Double_Index_Book_c::append( MP_Atom_c *newAtom ) {
  //const char* func = "GP_Double_Index_Book_c::append(*atom)";
  int appended;

  /* If the passed atom is NULL, silently ignore (but return 0 as the number of added atoms) */
  if( newAtom == NULL )
    return( 0 );
  
  appended = MP_Book_c::append(newAtom);
  if ( appended == 0)
    return( 0 );
    
  appended = sortBook->append(newAtom);
  return appended;
}

GP_Pos_Range_Sub_Book_c* GP_Double_Index_Book_c::get_neighbours (MP_Atom_c* atom, MP_Dict_c* dict){
    unsigned long int begin = atom->get_pos(), end = atom->get_pos();
    if (atom->get_pos() >= dict->maxFilterLen-1)
      begin -= (dict->maxFilterLen - 1);
    else
      begin = 0;
    end += (atom->support->len -1);
    
    return sortBook->get_range_book(begin, end);
}
