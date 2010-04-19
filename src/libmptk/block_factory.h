/******************************************************************************/
/*                                                                            */
/*                            block_factory.h                                 */
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

/*****************************************/
/*                                       */
/* DEFINITION OF THE BLOCK FACTORY CLASS */
/*                                       */
/*****************************************/

#ifndef BLOCK_FACTORY_H_
#define BLOCK_FACTORY_H_

#include "mptk.h"
#include "block.h"
#include <map>
#include <vector>



/******************************/
/* BLOCK FACTORY CLASS        */
/******************************/
/** \brief Bloc factory is used to create the several blocks used to perform Matching Pursuit.
 *  \brief The blocks are loaded with a DLL system
 */

class MP_Block_Factory_c
  {


    /********/
    /* DATA */
    /********/


  private:
    /** \brief Boolean set to true when an instance is created */
    static bool instanceFlag;

    /** \brief Protected pointer on MP_Atom_Factory_c*/
    static MP_Block_Factory_c * myBlockFactory;
 

	    /** \brief Hash map to store the block name and method to create it*/
	STL_EXT_NM::hash_map<const char*, MP_Block_c*(*)(MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap),CSTRING_HASHER> block;
    /** \brief Hash map to store the block parameter type map */
	STL_EXT_NM::hash_map<const char*, void (*)(map< string, string, mp_ltstring> * parameterMapType),CSTRING_HASHER> blockType;
    /** \brief Hash map to store the block parameter info map */
	STL_EXT_NM::hash_map<const char*, void (*)(map< string, string, mp_ltstring> * parameterMapInfo),CSTRING_HASHER> blockInfo;
    /** \brief Hash map to store the block parameter default map */
	STL_EXT_NM::hash_map<const char*, void (*)(map< string, string, mp_ltstring> * parameterMapDefault),CSTRING_HASHER> blockDefault;


    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  public:

    /** \brief Public destructor  */
    MPTK_LIB_EXPORT virtual ~MP_Block_Factory_c();

    /** \brief Method to get the MP_Atom_Factory_c */
    MPTK_LIB_EXPORT static MP_Block_Factory_c * get_block_factory();

  private:
    /** \brief Private constructor*/
    MP_Block_Factory_c();

    /***************************/
    /* MISC METHODS            */
    /***************************/

  public:
    /** \brief Method to register a new block with blockName
    *   \param blockName : name of the block to create
    *   \param createBlockFunctionPointer : a pointer on the function used to create an block with a parameter map
    *   \param fillBlockMapTypeFunctionPointer : a pointer on the function used to fill a map with the type of the block's parameters
    *   \param fillBlockMapInfoFunctionPointer : a pointer on the function used to fill a map with the info on the block's parameters
    *   \param fillBlockMapDefaultFunctionPointer :a pointer on the function used to fill a map with the default values of the block's parameters
    * 
    */ 
    MPTK_LIB_EXPORT void register_new_block(const char* blockName, MP_Block_c*(*createBlockFunctionPointer)(MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap),
                            void (*fillBlockMapTypeFunctionPointer)(map< string, string, mp_ltstring> * parameterMapType),
                            void (*fillBlockMapInfoFunctionPointer)(map< string, string, mp_ltstring> * parameterMapInfo),
                            void (*fillBlockMapDefaultFunctionPointer)(map< string, string, mp_ltstring> * parameterMapDefault) );

    /** \brief Accesor method to obtain the adress of a factory method to create a block from a parameter map
    *   \param blockName : name of the block to create
    *   \return a pointer on a Factory Method able to create the block identified by blockName with a MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap
    *   , NULL if the block isn't registered
    */
    MPTK_LIB_EXPORT MP_Block_c*(*get_block_creator( const char* blockName ))(MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap);

    /** \brief Method to fill a parameter type map for blockName 
    *   \param blockName : name of the block for which the map has to be fill
    *   \return a pointer on a method able to fill the given map< string, string, mp_ltstring> * parameterMapType
    */
    MPTK_LIB_EXPORT void(*get_block_type_map( const char* blockName )) (map< string, string, mp_ltstring> * parameterMapType);
    
    /** \brief Method to fill a parameter info map for blockName
    *   \param blockName : name of the block for which the map has to be fill
    *   \return a pointer on a method able to fill the given map< string, string, mp_ltstring> * parameterMapInfo
    */
    MPTK_LIB_EXPORT void (*get_block_info_map( const char* blockName )) (map< string, string, mp_ltstring> * parameterMapInfo);
    
    /** \brief Method to fill a parameter default map for blockName
    *   \param blockName : name of the block for which the map has to be fill
    *  \return a pointer on a method able to fill the given map< string, string, mp_ltstring> * parameterMapDefault
    */
    MPTK_LIB_EXPORT void (*get_block_default_map( const char* blockName )) (map< string, string, mp_ltstring> * parameterMapDefault);
   
    /** \brief Method to fill a vector with the name of all the blocks registred in the block factory
    *   \param nameVector : pointer on the vector which has to be fill with the name of blocks 
    */
    MPTK_LIB_EXPORT void get_registered_block_name(vector< string >* nameVector);

  };

#endif /*BLOCK_FACTORY_H_*/
