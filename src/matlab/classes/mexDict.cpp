/******************************************************************************/
/*                                                                            */
/*                  	          mexDict.cpp                       	      */
/*                                                                            */
/*				mptk4matlab toolbox		      	      */
/*                                                                            */
/* Remi Gribonval                                            	  July   2008 */
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

#include "mptk4matlab.h"

mxArray * mp_create_mxDict_from_dict(MP_Dict_c *dict)
{
  char *func = "mp_create_mxDict_from_dict";
  
  // Case of a NULL input
  if(NULL==dict) {
    mp_error_msg(func,"the input is NULL\n");
    return(NULL);
  }
  
  // Create the block cell
  mxArray *mxBlockCell = mxCreateCellMatrix((mwSize)dict->numBlocks,(mwSize)1);
  if(NULL==mxBlockCell) {
    mp_error_msg(func,"could not create block cell\n");
    return(NULL);
  }
  // Loop to create each block
  for (unsigned int i=0; i < dict->numBlocks; i++)  {
    MP_Block_c *block = dict->block[i];
    if(NULL==block) {
      mp_error_msg(func,"block 0<=i=%d<%d is NULL\n",i,dict->numBlocks);
      mxDestroyArray(mxBlockCell);
      return(NULL);
    }
    map<string,string,mp_ltstring> *paramMap = block->get_block_parameters_map();
    if (NULL==paramMap) {
      mp_error_msg(func,"empty paramMap for block 0<=i=%d<%d\n", i,dict->numBlocks);
      mxDestroyArray(mxBlockCell);
      return(NULL);
    }
    // Create mxBlock
    mxArray *mxBlock = mxCreateStructMatrix((mwSize)1,(mwSize)1,0,NULL);
    if(NULL==mxBlock) {
      mp_error_msg(func,"could not create block 0<=i=%d<%d\n",i,dict->numBlocks);
      mxDestroyArray(mxBlockCell);
      return(NULL);
    }
    // Add all fields
    map<string, string, mp_ltstring>::const_iterator iter;
    for ( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ ) {
      // Add the field
      mxAddField(mxBlock,iter->first.c_str());
      // Set the field value
      // TODO: Here we may want to convert according to type of value
      mxArray *mxTmp = mxCreateString(iter->second.c_str());
      if(NULL==mxTmp) {
	mp_error_msg(func,"could not create field %s in block 0<=i=%d<%d\n",iter->second.c_str(),i,dict->numBlocks);
	mxDestroyArray(mxBlockCell);
	return(NULL);
      }
      mxSetField(mxBlock,0,iter->first.c_str(),mxTmp);
    }
    // Put the mxBlock in the mxBlockCell
    mxSetCell(mxBlockCell,i,mxBlock);
    // Delete tmp variables 
    delete paramMap;
  }
  // Create the output information structure 
  // dict.block{numBlocks}
  int numDictFieldNames    = 1;
  const char *dictFieldNames[] = {"block"};
  mxArray *mxDict = mxCreateStructMatrix((mwSize)1,(mwSize)1,numDictFieldNames,dictFieldNames);
  if(NULL==mxDict) {
    mp_error_msg(func,"could not create dict\n");
    mxDestroyArray(mxBlockCell);
    return(NULL);
  }
  mxSetField(mxDict,0,"block",mxBlockCell);
  return(mxDict);
}


MP_Dict_c * mp_create_dict_from_mxDict(const mxArray *mxDict)
{
  char *func = "mp_create_dict_from_mxDict";
  MP_Dict_c *dict = NULL;

  if (NULL==mxDict) {
    mp_error_msg(func,"input is NULL\n");
    return(NULL);
  }
	
  // Check that the input dictionary structure has the right fields
  mxArray *mxBlockCell = mxGetField(mxDict,0,"block");
  if (NULL==mxBlockCell) {
    mp_error_msg(func,"the dict.block field is missing\n");
    return(NULL);
  }
  int nBlocks = (int)mxGetNumberOfElements(mxBlockCell);
  if (0==nBlocks) {
    mp_error_msg(func,"the number of blocks should be at least one\n");
    return(NULL);
  }
  if(!mxIsCell(mxBlockCell)) {
    mp_error_msg(func,"the dict.block is not a cell array\n");
    return(NULL);
  }
  // Reach all blocks 
  for (int i = 0; i < nBlocks; i++ ) {
    mxArray *mxBlock = mxGetCell(mxBlockCell,i);
    if (NULL==mxBlock) { // This should never happen
      mp_error_msg(func,"dict.block{%d} could not be retrieved\n",i+1);
      // Clean the house
      if(NULL!=dict) delete dict; 
      return(NULL);
    }
    size_t numFields = mxGetNumberOfFields(mxBlock);
    if (0==numFields) {
      mp_error_msg(func,"the number of fields %d should be at least one in dict.block{%d}\n",numFields,i+1);
      // Clean the house
      if(NULL!=dict) delete dict; 
      return(NULL);
    }
    
    // Reach all fields of the block and put them in a map
    map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
    if(NULL==paramMap) {
      mp_error_msg(func,"could not allocate paramMap\n");
      // Clean the house
      if(NULL!=dict) delete dict; 
      return(NULL);
    }
    for (int j= 0; j <numFields ; j++) {
      const char * fieldName = mxGetFieldNameByNumber(mxBlock,j);
      if (NULL==fieldName) {
	mp_error_msg(func,"field number %d in dict.block{%d} could not be retrieved\n",j+1,i+1);
	// Clean the house
	if(NULL!=dict) delete dict; 
	return(NULL);
      }
      // Retrieve the field value 
      mxArray *mxTmp = mxGetField(mxBlock,0,fieldName);
      if(NULL==mxTmp) {
	mp_error_msg(func,"value of field number %d in dict.block{%d} could not be retrieved\n",j+1,i+1);
	// Clean the house
	if(NULL!=dict) delete dict; 
	return(NULL);
      }
      char * fieldValue = mxArrayToString(mxTmp);
      if(NULL==fieldValue) {
	mp_error_msg(func,"string value of field number %d in dict.block{%d} could not be retrieved\n",j+1,i+1);
	// Clean the house
	if(NULL!=dict) delete dict; 
	return(NULL);
      }
      // Store it in the map and free 
      (*paramMap)[string(fieldName)]=string(fieldValue);
      mxFree(fieldValue);
      fieldValue = NULL;
    }
    
    // Retrieve the block creator
    MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
    blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*paramMap)["type"].c_str());
    if (NULL == blockCreator) {
      mp_error_msg(func,"the block factory does not contain type %s of dict.block{%d}\n",(*paramMap)["type"].c_str(),i+1);
      // Clean the house
      if(NULL!=dict) delete dict; 
      delete paramMap;
      return(NULL);
    }
    // Create the block 
    MP_Block_c *block =  blockCreator(NULL, paramMap);
    if (NULL == block) {
      mp_error_msg(func,"the dict.block{%d}, of type %s was not successfully created\n",i+1,(*paramMap)["type"].c_str());
      // Clean the house
      if(NULL!=dict) delete dict; 
      delete paramMap;
      return(NULL);
    }
    
    // Create the dictionary if needed (i.e. when adding first block)
    if (NULL==dict) {
      dict = MP_Dict_c::init();
      if (NULL==dict) {
	mp_error_msg(func,"Failed to create an empty dictionary\n");
	delete paramMap;
	delete block;
	return(NULL);
      }
    }
    // Add the block to the dictionary 
    dict->add_block(block);
    delete paramMap;
  }
  return(dict);
}

