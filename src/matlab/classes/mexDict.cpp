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
	const char						*func = "mp_create_mxDict_from_dict";
	map<string,string,mp_ltstring>	*paramMap;
	mxArray							*mxBlockCell;
	mxArray							*mxBlock;
	mxArray							*mxDict;
	mxArray							*mxTable;
	MP_Block_c						*block;
	int								numDictFieldNames;
	int								iFieldNumber;
	const char						*dictFieldNames[] = {"block"};
  
	// Case of a NULL input
	if(NULL==dict) 
	{
	    mp_error_msg(func,"the input is NULL\n");
	    return(NULL);
	}
  
	// Create the block cell
	mxBlockCell = mxCreateCellMatrix((mwSize)dict->numBlocks,(mwSize)1);
	if(mxBlockCell == NULL) 
	{
		mp_error_msg(func,"could not create block cell\n");
		return(NULL);
	}
	// Loop to create each block
	for (unsigned int i=0; i < dict->numBlocks; i++)  
	{
		block = dict->block[i];
		if(block == NULL) 
		{
			mp_error_msg(func,"block 0<=i=%d<%d is NULL\n",i,dict->numBlocks);
			mxDestroyArray(mxBlockCell);
			return(NULL);
		}
		paramMap = block->get_block_parameters_map();
		if (NULL==paramMap) 
		{
			mp_error_msg(func,"empty paramMap for block 0<=i=%d<%d\n", i,dict->numBlocks);
			mxDestroyArray(mxBlockCell);
			return(NULL);
		}
		// Create mxBlock
		mxBlock = mxCreateStructMatrix((mwSize)1,(mwSize)1,0,NULL);
		if(mxBlock == NULL) 
		{
			mp_error_msg(func,"could not create block 0<=i=%d<%d\n",i,dict->numBlocks);
			mxDestroyArray(mxBlockCell);
			return(NULL);
		}
		// Add all fields
		map<string, string, mp_ltstring>::const_iterator iter;
		for ( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ ) 
		{
			if(!strcmp(iter->first.c_str(),"data"))
			{
				// Adding the table Field		
				mxAddField(mxBlock,"data");
				if((mxTable = anywaveTableRead(paramMap, NULL)) == NULL) 
				{
					mp_error_msg(func,"could not load the anywaveTable %s\n",iter->second.c_str());
					mxDestroyArray(mxBlockCell);
					return(NULL);
				}
				iFieldNumber = mxGetFieldNumber(mxBlock, "data");
				mxSetCell(mxBlock,iFieldNumber,mxTable);
			}	
			else
			{
				// Add the field
				mxAddField(mxBlock,iter->first.c_str());
				// Set the field value
				mxArray *mxTmp = mxCreateString(iter->second.c_str());
				if(NULL==mxTmp) 
				{
					mp_error_msg(func,"could not create field %s in block 0<=i=%d<%d\n",iter->second.c_str(),i,dict->numBlocks);
					mxDestroyArray(mxBlockCell);
					return(NULL);
				}
				mxSetField(mxBlock,0,iter->first.c_str(),mxTmp);
				// If the paramMap contains a file link to xml, then go search it !
				if(!strcmp(iter->first.c_str(),"tableFileName"))
				{
					// Adding the table Field		
					mxAddField(mxBlock,"data");
					if((mxTable = anywaveTableRead(paramMap, (char *)iter->second.c_str())) == NULL) 
					{
						mp_error_msg(func,"could not load the anywaveTable %s\n",iter->second.c_str());
						mxDestroyArray(mxBlockCell);
						return(NULL);
					}
					iFieldNumber = mxGetFieldNumber(mxBlock, "data");
					mxSetCell(mxBlock,iFieldNumber,mxTable);
				}	
			}
		}
		// Put the mxBlock in the mxBlockCell
		mxSetCell(mxBlockCell,i,mxBlock);
		// Delete tmp variables 
		delete paramMap;
	}
	// Create the output information structure 
	// dict.block{numBlocks}
	numDictFieldNames = 1;
	mxDict = mxCreateStructMatrix((mwSize)1,(mwSize)1,numDictFieldNames,dictFieldNames);
	if(NULL==mxDict) 
	{
		mp_error_msg(func,"could not create dict\n");
		mxDestroyArray(mxBlockCell);
		return(NULL);
	}
	mxSetField(mxDict,0,"block",mxBlockCell);
	return(mxDict);
}


MP_Dict_c * mp_create_dict_from_mxDict(const mxArray *mxDict)
{
	const char						*func = "mp_create_dict_from_mxDict";
	const char						*fieldName;
	map<string,string,mp_ltstring>	*paramMap;
	MP_Dict_c						*dict = NULL;
	mxArray							*mxBlockCell,*mxBlock,*mxTmp;
	mwSize							mwNumDimension;
	const mwSize					*mwDimension;
	size_t							numFields;
	int								nBlocks;
	int								iSizeOfTable,iDimensions,iIndexDimension,iDimension;
	char							*fieldValue;
	double							*dTable = NULL;
	string							szDataString,szReturnString;
				

	if (NULL==mxDict) 
	{
	    mp_error_msg(func,"input is NULL\n");
	    return(NULL);
	}
	
	// Check that the input dictionary structure has the right fields
	mxBlockCell = mxGetField(mxDict,0,"block");
	if (NULL==mxBlockCell) 
	{
		mp_error_msg(func,"the dict.block field is missing\n");
		return(NULL);
	}
	nBlocks = (int)mxGetNumberOfElements(mxBlockCell);
	if (0==nBlocks) 
	{
		mp_error_msg(func,"the number of blocks should be at least one\n");
		return(NULL);
	}
	if(!mxIsCell(mxBlockCell)) 
	{
		mp_error_msg(func,"the dict.block is not a cell array\n");
		return(NULL);
	}
	// Reach all blocks 
	for (int i = 0; i < nBlocks; i++ ) 
	{
		mxBlock = mxGetCell(mxBlockCell,i);
		if (NULL==mxBlock) 
		{ 
			// This should never happen
			mp_error_msg(func,"dict.block{%d} could not be retrieved\n",i+1);
			// Clean the house
			if(NULL!=dict) delete dict; 
			return(NULL);
		}
		numFields = mxGetNumberOfFields(mxBlock);
		if (0==numFields) 
		{
			mp_error_msg(func,"the number of fields %d should be at least one in dict.block{%d}\n",numFields,i+1);
			// Clean the house
			if(NULL!=dict) delete dict; 
			return(NULL);
		}
    
		// Reach all fields of the block and put them in a map
		paramMap = new map<string, string, mp_ltstring>();
		if(NULL==paramMap) 
		{
			mp_error_msg(func,"could not allocate paramMap\n");
			// Clean the house
			if(NULL!=dict) delete dict; 
			return(NULL);
		}
		for (unsigned int j= 0; j <numFields ; j++) 
		{
			fieldName = mxGetFieldNameByNumber(mxBlock,j);
			if (fieldName == NULL) 
			{
				mp_error_msg(func,"field number %d in dict.block{%d} could not be retrieved\n",j+1,i+1);
				// Clean the house
				if(NULL!=dict) delete dict; 
				return(NULL);
			}

			// Retrieve the field value 
			mxTmp = mxGetField(mxBlock,0,fieldName);
			if(mxTmp == NULL) 
			{
				mp_error_msg(func,"value of field number %d in dict.block{%d} could not be retrieved\n",j+1,i+1);
				// Clean the house
				if(NULL!=dict) delete dict; 
				return(NULL);
			}

			if(mxIsDouble(mxTmp)) 
			{
				// Retrieve the dimension of the double
				mwNumDimension = mxGetNumberOfDimensions(mxTmp);
				mwDimension = mxGetDimensions(mxTmp);
				iDimension = 1;
				for(iIndexDimension = 0; iIndexDimension < mwNumDimension; iIndexDimension++)
					iDimension *= mwDimension[iIndexDimension];
				// Add the dimension of a double
				iDimension = iDimension * sizeof(double);
				// Getting the storage field
				if((dTable = (MP_Real_t*)malloc(iDimension)) == NULL)
				{
					mp_error_msg(func,"The double storage has not been allocaed\n");
					return(NULL);
				}
				// Loading the mxArray
				if((dTable = mp_get_anywave_datas_from_mxAnywaveTable(mxBlock)) == NULL)
				{
					mp_error_msg(func,"A double value has been found but could not be retrieved\n");
					// Clean the house
					if(NULL!=dict) delete dict; 
					return(NULL);
				}
				// Store it in the map and free 
				(*paramMap)["doubledata"] = string((char *)dTable, iDimension);	
			}
			else
			{
				fieldValue = mxArrayToString(mxTmp);
				if(fieldValue == NULL) 
				{
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
		}
    
		// Retrieve the block creator
		MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
		blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*paramMap)["type"].c_str());
		if (NULL == blockCreator) 
		{
			mp_error_msg(func,"the block factory does not contain type %s of dict.block{%d}\n",(*paramMap)["type"].c_str(),i+1);
			// Clean the house
			if(NULL!=dict) delete dict; 
			delete paramMap;
			return(NULL);
		}
		// Create the block 
		MP_Block_c *block =  blockCreator(NULL, paramMap);
		if (NULL == block) 
		{
			mp_error_msg(func,"the dict.block{%d}, of type %s was not successfully created\n",i+1,(*paramMap)["type"].c_str());
			// Clean the house
			if(NULL!=dict) delete dict; 
			delete paramMap;
			return(NULL);
		}
    	// Create the dictionary if needed (i.e. when adding first block)
		if (NULL==dict) 
		{
			dict = MP_Dict_c::init();
			if (NULL==dict) 
			{
				mp_error_msg(func,"Failed to create an empty dictionary\n");
				delete paramMap;
				delete block;
				return(NULL);
			}
		}
		// Add the block to the dictionary 
		dict->add_block(block);

		if(dTable) free(dTable);
		delete paramMap;
	}
	return(dict);
}



mxArray *anywaveTableRead(map<string, string, mp_ltstring> *paramMap, char *szFileName)
{
	MP_Anywave_Table_c	*mpTable;
	mxArray				*mxTable;
	
	//-------------------
	// Loading the table
	//-------------------
	mpTable = new MP_Anywave_Table_c();
	if (mpTable == NULL ) 
	{
		mexPrintf("Failed to create an anywave table.\n");
		mexErrMsgTxt("Aborting");
		return NULL;
	} 
	if (szFileName == NULL) 
	{
		if(!mpTable->AnywaveCreator(paramMap))
		{
			mexPrintf("Failed to create an anywave table from binary file [%s].\n", szFileName);
			mexErrMsgTxt("Aborting");
			mxFree(szFileName);
			return NULL;
		} 
	}
	else
	{
		if(!mpTable->AnywaveCreator(szFileName))
		{
			mexPrintf("Failed to create an anywave table from binary file [%s].\n", szFileName);
			mexErrMsgTxt("Aborting");
			mxFree(szFileName);
			return NULL;
		} 
	}

	//-----------------------------------------------
	// Loading dictionary object in Matlab structure
	//-----------------------------------------------
	mxTable = mp_create_mxAnywaveTable_from_anywave_table(mpTable);
	if(!mxTable) 
	{
		mexPrintf("Failed to convert an anywave table from MPTK to Matlab.\n");
		mexErrMsgTxt("Aborting");
		return NULL;
	}

	free(mpTable);
	return mxTable;
}