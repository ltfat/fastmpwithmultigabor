/******************************************************************************/
/*                                                                            */
/*                            block_factory.cpp                               */
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

/************************************************************/
/*                                                          */
/*  block_factory.cpp: methods for class MP_Block_Factory_c */
/*                                                          */
/************************************************************/

#include "mptk.h"

using namespace std;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
/*boolean flag if BlockFactory instance has been created */
bool MP_Block_Factory_c::instanceFlag = false;
/*Initialise pointer to BlockFactory instance */
MP_Block_Factory_c * MP_Block_Factory_c::myBlockFactory = NULL;

/********************/
/* Void constructor */

/* Block Factory constructor */
MP_Block_Factory_c::MP_Block_Factory_c(){ }

/**************/
/* Destructor */

/* Block Factory destructor */
MP_Block_Factory_c::~MP_Block_Factory_c()
{
  instanceFlag=false;
}

/**************/
/* Singleton */

/* Create a singleton of BlockFactory class */

MP_Block_Factory_c * MP_Block_Factory_c::get_block_factory()
{
  if (!MP_Block_Factory_c::myBlockFactory)
    {
      MP_Block_Factory_c::myBlockFactory = new MP_Block_Factory_c();
      MP_Block_Factory_c::instanceFlag = true;
    }
  return  MP_Block_Factory_c::myBlockFactory;
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Register new block in the hash map */
void MP_Block_Factory_c::register_new_block(const char* blockName, MP_Block_c*(*createBlockFunctionPointer)(MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap),
void (*fillBlockMapTypeFunctionPointer)(map< string, string, mp_ltstring> * parameterMapType),
void (*fillBlockMapInfoFunctionPointer)(map< string, string, mp_ltstring> * parameterMapInfo),
void (*fillBlockMapDefaultFunctionPointer)(map< string, string, mp_ltstring> * parameterMapDefault))
{
  if ((NULL!=blockName)&&(NULL == MP_Block_Factory_c::get_block_factory()->block[blockName]))
    {
      MP_Block_Factory_c::block[blockName] = createBlockFunctionPointer;
      MP_Block_Factory_c::blockType[blockName] = fillBlockMapTypeFunctionPointer;
      MP_Block_Factory_c::blockInfo[blockName] = fillBlockMapInfoFunctionPointer;
      MP_Block_Factory_c::blockDefault[blockName]= fillBlockMapDefaultFunctionPointer;
    }
}

/* get a block create function registered in the hash map */
MP_Block_c* (*MP_Block_Factory_c::get_block_creator( const char* blockName )) (MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap) 
{
	
return MP_Block_Factory_c::get_block_factory()->block[blockName];

}

/* get a block parameter type function registered in the hash map */
void (*MP_Block_Factory_c::get_block_type_map( const char* blockName )) (map< string, string, mp_ltstring>* parameterMapType ) 
{
	
return MP_Block_Factory_c::get_block_factory()->blockType[blockName];

}

/* get a block parameter info function registered in the hash map */
void (*MP_Block_Factory_c::get_block_info_map( const char* blockName )) (map< string, string, mp_ltstring>* parameterMapInfo ) 
{
	
return MP_Block_Factory_c::get_block_factory()->blockInfo[blockName];

}

/* get a block parameter default function registered in the hash map */
void (*MP_Block_Factory_c::get_block_default_map( const char* blockName )) (map< string, string, mp_ltstring>* parameterMapDefault ) 
{
	
return MP_Block_Factory_c::get_block_factory()->blockDefault[blockName];

}

/* fil a vector with the nam of the block registred in block factory */
void MP_Block_Factory_c::get_registered_block_name( vector< string >* nameVector ){

STL_EXT_NM::hash_map<const char*, MP_Block_c*(*)(MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap),mp_hash_fun, mp_eqstr>::iterator iter;

for( iter = MP_Block_Factory_c::block.begin(); iter != MP_Block_Factory_c::block.end(); iter++ ) nameVector->push_back(iter->first);


}


