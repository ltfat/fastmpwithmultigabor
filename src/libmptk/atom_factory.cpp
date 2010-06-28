/******************************************************************************/
/*                                                                            */
/*                            atom_factory.cpp                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2007 */
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

/**********************************************************/
/*                                                        */
/* atom_factory.cpp : methods for class MP_Atom_Factory_c */
/*                                                        */
/**********************************************************/

#include "mptk.h"

using namespace std;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/*boolean flag if MP_Atom_Factory_c instance has been created */
bool MP_Atom_Factory_c::instanceFlag = false;

/*Initialise pointer to MP_Atom_Factory_c instance */
MP_Atom_Factory_c * MP_Atom_Factory_c::myAtomFactory = NULL;

/********************/
/* Void constructor */

MP_Atom_Factory_c::MP_Atom_Factory_c()
{
}

/**************************/
/* Singleton of the class */

/*Create a singleton of MP_Atom_Factory_c class */

MP_Atom_Factory_c * MP_Atom_Factory_c::get_atom_factory()
{
  if (!MP_Atom_Factory_c::myAtomFactory)
    {
      myAtomFactory = new MP_Atom_Factory_c();
      MP_Atom_Factory_c::instanceFlag = true;
    }
  return  MP_Atom_Factory_c::myAtomFactory;
}

/**************/
/* Destructor */
MP_Atom_Factory_c::~MP_Atom_Factory_c()
{
  instanceFlag=false;
}



/***************************/
/* OTHER METHODS           */
/***************************/

/* Method to obtain a create function for empty atom */
MP_Atom_c*(*MP_Atom_Factory_c::get_empty_atom_creator( const char* atomName ))(void)
{

  return  MP_Atom_Factory_c::get_atom_factory()->atom_empty[atomName];

}

/* Method to obtain a create function for atom initialised from a file */
MP_Atom_c*(*MP_Atom_Factory_c::get_atom_creator( const char* atomName ))(FILE *fid, const char mode)
{

  return MP_Atom_Factory_c::get_atom_factory()->atom[atomName];

}

/* Register new empty Atom create function in the hash map */
void MP_Atom_Factory_c::register_new_atom(const char* nameplug, MP_Atom_c*(*createAtomFunctionPointer)(FILE *fid, const char mode))
{
  const char *func = "MP_Atom_Factory_c::register_new_atom()";
  if  (NULL == MP_Atom_Factory_c::get_atom_factory()->atom[nameplug])
    {
        mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Registering atom [%s].\n",nameplug );
      MP_Atom_Factory_c::get_atom_factory()->atom[nameplug] = createAtomFunctionPointer;
    } else {
    mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Warning: trying to register atom [%s] which is already registered.\n",nameplug );
  }
}

/* Register new Atom create function in the hash map */
void MP_Atom_Factory_c::register_new_atom_empty(const char* nameplug, MP_Atom_c*(*createEmptyAtomFunctionPointer)( void ))
{
  const char *func = "MP_Atom_Factory_c::register_new_atom_empty()";
  if  (NULL == MP_Atom_Factory_c::get_atom_factory()->atom_empty[nameplug])
    {
        mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Registering atom [%s].\n",nameplug );
      MP_Atom_Factory_c::get_atom_factory()->atom_empty[nameplug] = createEmptyAtomFunctionPointer;
    }else {
    mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Warning: trying to register atom [%s] which is already registered.\n",nameplug );
  }
}


/* fill a vector with the name of the atoms registred in atom factory */
void MP_Atom_Factory_c::get_registered_atom_name( vector< string >* nameVector ){
	const char *func = "MP_Atom_Factory_c::get_registered_atom_name()";
	STL_EXT_NM::hash_map<const char*, MP_Atom_c*(*)(void),CSTRING_HASHER>::iterator iter;
	for( iter = MP_Atom_Factory_c::atom_empty.begin(); iter != MP_Atom_Factory_c::atom_empty.end(); iter++ ) {
		if(NULL!=iter->first) {
			nameVector->push_back(string(iter->first));
		} else {
			mp_error_msg(func,"Cannot push a NULL string into nameVector\n");
		}
	}
	return;
}	 

/* fill a vector with the name of the atoms registred in atom factory */
void MP_Atom_Factory_c::get_registered_atom_names( char **atomNames ){
	int iIndex = 0;
	const char *func = "MP_Atom_Factory_c::get_registered_atom_names()";
	STL_EXT_NM::hash_map<const char*, MP_Atom_c*(*)(void),CSTRING_HASHER>::iterator iter;

	for(iter = MP_Atom_Factory_c::atom_empty.begin(); iter != MP_Atom_Factory_c::atom_empty.end(); iter++) 
	{
		if(NULL!=iter->first) {
			atomNames[iIndex++]=(char *)iter->first;
		} else {
			mp_error_msg(func,"Cannot push a NULL string into nameVector\n");
		}
	}
	return;
}	 

/* Returns the size of the atom vector */
int MP_Atom_Factory_c::get_atom_size( void ){
	return MP_Atom_Factory_c::atom_empty.size();
}	 
