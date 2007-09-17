/******************************************************************************/
/*                                                                            */
/*                                  mask.h                                    */
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

/********************************/
/*                              */
/* DEFINITION OF THE MASK CLASS */
/*                              */
/********************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2005-11-25 17:02:47 +0100 (Fri, 25 Nov 2005) $
 * $Revision: 132 $
 *
 */


#ifndef __mask_h_
#define __mask_h_


/***********************/
/* CONSTANTS           */
/***********************/

/** \brief The minimum increase in the allocation size of the sieve array. */
#define MP_MASK_GRANULARITY 1024


/***************************/
/** \brief A class implementing a masking mechanism to select/filter atoms from books.
 */
/***************************/
class MP_Mask_c {

  /********/
  /* DATA */
  /********/

public:
  
  /** \brief The dimension of the filtered book */
  unsigned long int numAtoms;
  
  /** \brief The actual dimension of the sieve array (may be more than numAtoms) */
  unsigned long int maxNumAtoms;
  
  /** \brief An array of numAtoms booleans storing boolean values;
   *  MP_TRUE means keep the atom, MP_FALSE means throw it away.
   *   
   *  Note: we are not using the bool type because this array
   *  will be stored on disk, and we don't know what happens
   *  across platforms in that case. See mp_types.h to see which
   *  type is abstracted by MP_Bool_t.
   */
  MP_Bool_t* sieve;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
  
public:

  /** \brief A constructor initializing the boolean array.
   *
   * \param setNumAtoms The number of booleans in the sieve array.
   */
  MP_Mask_c( unsigned long int setNumAtoms );


  /** \brief Destructor. */
  virtual ~MP_Mask_c();


  /***************************/
  /* FACTORY METHODS         */
  /***************************/

 public:

  /** \brief Factory method. */
  static MP_Mask_c * init( unsigned long int setNumAtoms );


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /** \brief Set one element to MP_TRUE at index i. */
  void set_true( unsigned long int i );

  /** \brief Set one element to MP_FALSE at index i. */
  void set_false( unsigned long int i );


  /** \brief A method setting the whole array to MP_TRUE. */
  void reset_all_true( void );

  /** \brief A method setting the whole array to MP_FALSE. */
  void reset_all_false( void );


private:
  /** \brief A private method to change the size of the sieve array. */
  unsigned long int grow( unsigned long int nElem );

public:

  /** \brief A method appending nElem MP_TRUE elements to the sieve array.
   * \param nElem The number of MP_TRUE elements to add
   * \return 0 if failed, otherwise the new total number of elements in the sieve array.
   * \remark A realloc may be performed, hence the pointer to the sieve array may change.
   */
  unsigned long int append_true( unsigned long int nElem );

  /** \brief A method appending nElem MP_FALSE elements to the sieve array.
   * \param nElem The number of MP_FALSE elements to add
   * \return 0 if failed, otherwise the new total number of elements in the sieve array.
   * \remark A realloc may be performed, hence the pointer to the sieve array may change.
   */
  unsigned long int append_false( unsigned long int nElem );

  /** \brief A method appending any element to the sieve array.
   * \param MP_Bool_t the element to add
   * \return 0 if failed, otherwise the new total number of elements in the sieve array.
   * \remark A realloc may be performed, hence the pointer to the sieve array may change.
   */
  unsigned long int append( MP_Bool_t val );

  /** \brief Check if numAtoms is the same in both masks. */
  MP_Bool_t is_compatible_with( MP_Mask_c mask );


  /***************************/
  /* FILE I/O                */
  /***************************/

  /** \brief A method to read from a stream. */
  unsigned long int read_from_stream( FILE* fid );

  /** \brief A method to write to a stream. */
  unsigned long int write_to_stream( FILE* fid );

  /** \brief A method to read from a file. */
  unsigned long int read_from_file( const char* fName );

  /** \brief A method to write to a file. */
  unsigned long int write_to_file( const char* fName );


  /***************************/
  /* OPERATORS               */
  /***************************/

  /** \brief Assignment operator */
  MP_Mask_c& operator=(  const MP_Mask_c& from );

  /** \brief Operator AND. */
  MP_Mask_c operator&&( const MP_Mask_c& m1 );

  /** \brief Operator OR. */
  MP_Mask_c operator||( const MP_Mask_c& m1 );

  /** \brief Operator NOT. */
  MP_Mask_c operator!( void );

  /** \brief Operator == */
  MP_Bool_t operator==( const MP_Mask_c& m1 );

  /** \brief Operator != */
  MP_Bool_t operator!=( const MP_Mask_c& m1 );

};


#endif /* __mask_h_ */
