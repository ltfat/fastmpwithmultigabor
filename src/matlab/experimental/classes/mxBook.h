/******************************************************************************/
/*                                                                            */
/*                  	    mxBook.h                                          */
/*          		    mptk4matlab toolbox	      	                      */
/*          Class for interfacing MP_Book with matlab strcture                */
/*                                                                            */
/* Gilles Gonon                                               	  Feb 20 2008 */
/* Remi Gribonval                                              	  July   2008 */
/* -------------------------------------------------------------------------- */
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
 * $Version 0.5.4$
 * $Date 05/22/2007$
 */

#include "mex.h"
#include "mptk.h"
#include "time.h"
#include "matrix.h"
#include "anywave_atom_plugin.h"
#include "anywave_hilbert_atom_plugin.h"
#include "constant_atom_plugin.h"
#include "dirac_atom_plugin.h"
#include "gabor_atom_plugin.h"
#include "harmonic_atom_plugin.h"
#include "nyquist_atom_plugin.h"
#include "mclt_atom_plugin.h"
#include "mdct_atom_plugin.h"
#include "mdst_atom_plugin.h"

#include <map>
#include <vector>
#include <iostream>
#include <sstream>

/**
 *
 *  Class atomGroup :
 *    class for parsing MP_Atom and filling mex structure of atoms per type
 *    (used for converting between MP_Book and MEX book structure )
 *
 *  The matlab 'mxAtomGroup' structure contains the following fields:
 *    - type : string
 *    - params: structure of atoms parameters arrays, whose field are type dependent
 *
 */

class atomGroup {

 private:
  unsigned long int curIdx; //! Current index for filling parameters vectors

 public:
  /** Public members */
  
  string type;              //! String with the type of the atom ("gabor")
  string typeLen;           //! String with the type of the atom and the length ("gabor_32")
  unsigned int typeIdx;     //! Index of type in book structure
  unsigned long int nAtom;  //! Number of atoms of this type
  unsigned int nChannel;    //! Number of channels


  mxArray * atom;           //! Atom mex structure
  map <string, mxArray *> params; //! param name, param value

  /** Class methods */

  /** CONSTRUCTORS */
	//! Constructor with no allocation of params mxArray
  atomGroup(string t, unsigned int nC);
  
  /** DESCTRUCTOR */
  virtual ~atomGroup();

  /** OTHER METHODS */
  //! Allocate Atom matlab memory for each parameters
  void allocParams(unsigned long int nA,MP_Atom_c*);

  //! Read an atom and store values in params mxArrays
  void append(MP_Atom_c *atom);

  //! Fill a given 'atom' structure at index 'a' with parameters
  mxArray * outputMxStruct(mxArray * atom, unsigned int a);

};

// In preparation ...
// Collection = map of groups + index (e.g. sorted book)
// Group = type (string) + map <param name, param vector<A>>
/* class atomCollection {
	private:
		map<string, atomGroup *> atomGroups;
		map <string, atomGroup *>::iterator miter;
	public:
		void atomCollection();
		void append(MP_Atom_c *atom);
		unsigned int get_typeIdx(typeLen);
} */

/**
 *
 *    As the atoms in MPTK have parameters dependent to their type
 *    the matlab 'book' structure store the atom parameters per type of atom.
 *   
 *  The matlab 'book' structure contains the following fields:
 *    - format   : indicating which data format
 *    - numAtoms : number of atoms in book
 *    - numChans : number of channel in book 
 *    - numSamples : number of samples covered by the reconstructed book
 *    - sampleRate : signal samplerate
 *    - index: [ (4+numChans) x numAtoms matrix] : Index for reading/querying/sorting atoms with their occurrence in book
 *         1: Atom number in book (1 to numAtoms)
 *         2: Atom type
 *         3: Atom number in atom(type) structure
 *         4: Atom selected or not (used for saving part of a book)
 *         4+channel: Atom position for channel 'channel' (1 to numChans)
 *    - atom:  [1 x Ntype struct]
 *         +- type : string
 *         +- params: structure of atoms parameters arrays, whose field are type dependent
 *
 */




/** \brief Converts a MP_Book_c object to a Matlab structure 
 * \param book the MPTK object
 * \return the created Matlab structure, NULL in case of problem
 */
MPTK_LIB_EXPORT extern mxArray *mp_create_mxBook_from_book(MP_Book_c *book);

/** \brief Converts a Matlab structure to a MP_Book_c object
 * \param mxBook the Matlab structure
 * \return the created MTPK object, NULL in case of problem
 */
MPTK_LIB_EXPORT extern MP_Book_c *mp_create_book_from_mxBook(const mxArray *mxBook);


