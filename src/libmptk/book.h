/******************************************************************************/
/*                                                                            */
/*                                 book.h                                     */
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
/* DEFINITION OF THE BOOK CLASS        */
/*                                     */
/***************************************/
/*
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __book_h_
#define __book_h_


/***********************/
/* BOOK CLASS          */
/***********************/
/**
 * \brief Books store and manage sequences of atoms collected
 * while iterating several steps of Matching Pursuit. 
 */
class MP_Book_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief Number of atoms actually stored in the book */
  unsigned long int numAtoms;
  /** \brief Number of atoms that could fit in the storage space */
  unsigned long int maxNumAtoms;
  /** \brief Storage space for atoms */
  MP_Atom_c **atom;

  /** \brief Number of channels of the atoms */
  int numChans;                 
  /** \brief Number of samples in each channel of the signal that will be
   * reconstructed using the stored atoms */
  unsigned long int numSamples; 
  /** \brief Sample rate in Hertz of the analyzed signal */
  int sampleRate;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /* Void constructor */
  MP_Book_c();

  /* Destructor */
  ~MP_Book_c();


  /***************************/
  /* I/O METHODS             */
  /***************************/

  /** \brief Print the book to a stream in text or binary format
   *
   * \param  fid A writable stream
   * \param  mode One of MP_TEXT or MP_BINARY
   * \param  mask array of numAtoms 0/1 flags where the 1 indicate which atoms should be used 
   * \return The number of atoms actually printed to the stream 
   * \remark Passing mask == NULL forces all atoms to be used. 
   */
  unsigned long int print( FILE *fid , const char mode, char *mask);

  /** \brief Same as MP_Book_c::print (FILE *fid, const char mode, char*mask) but with a file name */
  unsigned long int print( const char *fName, const char mode, char* mask);

  /** \brief Same as MP_Book_c::print( FILE *fid, const char mode, char *mask) with mask == NULL */
  unsigned long int print( FILE *fid, const char mode );

  /** \brief Same as MP_Book_c::print (FILE *fid, const char mode) but with a file name */
  unsigned long int print( const char *fName, const char mode );

  /** \brief Load the book from a stream, determining automatically
   * whether it is in text or binary format
   *
   * \param  fid A readable stream
   * \return The number of atoms loaded from the stream */
  unsigned long int load( FILE *fid );

  /** \brief Same as MP_Book_c::load (FILE *fid) but with a file name */
  unsigned long int load( const char *fName );

  /** \brief Print readable information about the book to a stream
   *
   * \param  fid A writable stream */
  void info( FILE *fid );


  /***************************/
  /* MISC METHODS            */
  /***************************/

  /** \brief Clear all the atoms from the book.
   */
  void reset( void );

  /** \brief Add a new atom in the storage space, taking care of the necessary allocations 
   * \param newAtom a reference to an atom
   * \return nonzero upon success, zero otherwise
   * \remark The reference newAtom is not copied, it is stored and will be deleted when the book is destroyed
   * \remark \a numChans is set up if this is the first atom to be appended to the book,
   * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
   */
  int append( MP_Atom_c *newAtom );

  /** \brief Substract / add some atom waveforms from / to a signal 
   *
   * \param sigSub signal from which the sum of atom waveforms is to be removed
   * \param sigAdd signal to which the sum of atom waveforms is to be added
   * \param mask array of numAtoms 0/1 flags where the 1 indicate which atoms should be used 
   * \return the number of atoms used
   * \remark Passing sigSub == NULL or sigAdd == NULL skips the corresponding substraction / addition.
   * \remark Passing mask == NULL forces all atoms to be used.
   * \remark An exception (assert) is throwed if the signals numChans does not match the atoms numChans.
   * \remark An exception (assert) is throwed if the support of the atom exceeds the limits of the signal.
   */
  unsigned long int substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd, char *mask );

  /** \brief Build the waveform corresponding to the sum of some atoms of the book into a signal 
   *
   * \param sig signal to which the atom waveform is to be added
   * \param mask array of numAtoms 0/1 flags where the 1 indicate which atoms should be used 
   * \return the number of atoms used
   * \remark The signal numChans, numSamples and sampleRate are set according to those of the book.
   * \remark Passing mask == NULL forces all atoms to be used.
   */
  unsigned long int build_waveform( MP_Signal_c *sig, char *mask );

  /** \brief Adds the sum of the pseudo Wigner-Ville distributions of some atoms to a time-frequency map 
   * \param tfmap The time-frequency map 
   * \param mask array of numAtoms 0/1 flags where the 1 indicate which atoms should be used 
   * \return the number of atoms used
   * \remark Passing mask == NULL forces all atoms to be used.
   */
  unsigned long int add_to_tfmap( MP_TF_Map_c *tfmap, char *mask );

};


#endif /* __book_h_ */
