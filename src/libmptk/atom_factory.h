/******************************************************************************/
/*                                                                            */
/*                            atom_factory.h                                  */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Roy Benjamin                                               Mon Feb 21 2007 */
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

/****************************************/
/*                                      */
/* DEFINITION OF THE ATOM FACTORY CLASS */
/*                                      */
/****************************************/

#ifndef ATOM_FACTORY_H_
#define ATOM_FACTORY_H_

#include "mptk.h"

/*******************************/
/* ATOM FACTORY CLASS          */
/*******************************/
/**
 * \brief Atom Factory store the create function information for all type of atom.
 * Atom DLL registered this information in hash table when dynamically loaded.
 *
 * An atom is basically a storage space generated by a MP_Block_c with the method
 * MP_Block_c::create_atom() and stored in a book MP_Book_c with MP_Book_c::append().
 */

class MP_Atom_Factory_c
  {

    /********/
    /* DATA */
    /********/

  protected:
    /** \brief Protected pointer on MP_Atom_Factory_c*/
    static MP_Atom_Factory_c * myAtomFactory;

  private:
    /** \brief Boolean set to true when an instance is created */
    static bool instanceFlag;
 
	  /** \brief Hash map to store the atom name and the file creation atom method pointer */
	STL_EXT_NM::hash_map<const char*, MP_Atom_c*(*)(FILE *fid, const char mode),CSTRING_HASHER> atom;
    /** \brief Hash map to store the atom name and the empty atom method pointer */
	STL_EXT_NM::hash_map<const char*, MP_Atom_c*(*)(void),CSTRING_HASHER> atom_empty;



    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief Public destructor  */
    MPTK_LIB_EXPORT virtual ~MP_Atom_Factory_c();

    /** \brief Method to get the MP_Atom_Factory_c singleton*/
    MPTK_LIB_EXPORT static MP_Atom_Factory_c * get_atom_factory();

  private:
    /** \brief Private constructor*/
    MP_Atom_Factory_c();

    /***************************/
    /* MISC METHODS            */
    /***************************/

  public:

    /** \brief Method to register a new method to create atom from file
     *  \param atomName: name of the atom to register
     *  \param createAtomFunctionPointer: a pointer on the function used to create an atom from a file
     */
   MPTK_LIB_EXPORT void register_new_atom(const char* atomName, MP_Atom_c*(*createAtomFunctionPointer)(FILE *fid, const char mode));

    /** \brief Method to register a new method to create empty atom
     *  \param atomName: name of the atom to register
     *  \param createEmptyAtomFunctionPointer: a pointer on the function used to create an empty atom
     *  
     */
   MPTK_LIB_EXPORT void register_new_atom_empty(const char* atomName, MP_Atom_c*(*createEmptyAtomFunctionPointer)(void));

    /** \brief Accesor method to obtain the adress of a function to create empty atom
    *   \param atomName: name of the atom to create
    * \return a pointer on a method able to create an empty atom 
    */
    MPTK_LIB_EXPORT MP_Atom_c*(*get_empty_atom_creator( const char* atomName ))(void);

    /** \brief Accesor method to obtain the adress of a function to create atom initialised from a file
     *   \param atomName: name of the atom to create
     *   \return a pointer on a method able to create an atom from a file
    */
    MPTK_LIB_EXPORT MP_Atom_c*(*get_atom_creator( const char* atomName ))(FILE *fid, const char mode);

    /** \brief Method to fill a vector with the name of all the atoms registred in the atom factory
    *   \param nameVector : pointer on the vector which has to be fill with the name of blocks 
    */
    MPTK_LIB_EXPORT void get_registered_atom_name(vector< string >* nameVector);
	MPTK_LIB_EXPORT	void get_registered_atom_names( char **atomNames );
	MPTK_LIB_EXPORT	int get_atom_size( void );
  };
#endif /*ATOM_FACTORY_H_*/

