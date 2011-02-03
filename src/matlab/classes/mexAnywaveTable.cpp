/******************************************************************************/
/*                                                                            */
/*                                  mptk.h                                    */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
//* -------------------------------------------------------------------------- */
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

#include "mptk4matlab.h"

/********************************************************************************/
/*  Function :		mp_create_mxAnywaveTable_from_anywave_table					*/
/*  Description :	Transforms the Anywave table AnyTable into a Matlab array	*/
/*					table mxTable												*/
/*  Input :																		*/
/*		+ MP_Anywave_Table_c *AnyTable : The input Anywave table				*/
/*  Output :																	*/
/*		+ mxArray *mxTable : The output Matlab table							*/
/*																				*/
/********************************************************************************/
mxArray *mp_create_mxAnywaveTable_from_anywave_table(const MP_Anywave_Table_c *AnyTable) 
{
    int					numBookFieldNames = 0;
	char				*func = "mp_create_mxAnywaveTable_from_anywave_table";
    unsigned long int	filterIdx, chanIdx, sampleIdx;
	mxArray				*mxReturnTable;
	mxArray				*mxTempMatrix;
	mxArray				*mxWaveArray;
	mwSize				*mwDimension;
	
	//--------------------------
	// Checking input arguments
	//--------------------------
	if(!AnyTable) 
	{
		mp_error_msg(func,"the input is NULL\n");
		return(NULL);
	}
  
	//--------------------------
	// Creating output arguments
	//--------------------------
	// Allocate Output book structure
	const char *bookFieldNames[] = {"tableFileName","dataFileName","normalized","centeredAndDenyquisted","wave"};
	numBookFieldNames = 5;
	mxReturnTable = mxCreateStructMatrix((mwSize)1,(mwSize)1,numBookFieldNames,bookFieldNames);
	if(!mxReturnTable)
	{
		mp_error_msg(func,"could not create the table\n");
		return(NULL);
	}
  
	//-----------------------------------------
	// Adding the datas into the output matrix
	//-----------------------------------------
	// Adding the table filename to the return table
	mxTempMatrix = mxCreateString(AnyTable->tableFileName);
	if(!mxTempMatrix)	
	{
		mp_error_msg(func,"could not read the table file name\n");
		mxDestroyArray(mxReturnTable);
		return(NULL);
	}
	mxSetField(mxReturnTable,0,"tableFileName",mxTempMatrix);
	
	// Adding the datas filename to the return table
	mxTempMatrix = mxCreateString(AnyTable->dataFileName);
	if(!mxTempMatrix)
	{
		mp_error_msg(func,"could not read the data file name\n");
		mxDestroyArray(mxReturnTable);
		return(NULL);
	}
	mxSetField(mxReturnTable,0,"dataFileName",mxTempMatrix);
  
	// Adding the normalised datas to the return table
	mxTempMatrix = mxCreateDoubleScalar(AnyTable->normalized);
	if(!mxTempMatrix)
	{
		mp_error_msg(func,"could not read the normalised datas\n");
		mxDestroyArray(mxReturnTable);
		return(NULL);
	}
	mxSetField(mxReturnTable,0,"normalized",mxTempMatrix);
	
	// Adding the centered and deny quisted datas to the return table
	mxTempMatrix = mxCreateDoubleScalar(AnyTable->centeredAndDenyquisted);
	if(!mxTempMatrix)
	{
		mp_error_msg(func,"could not read the normalised datas\n");
		mxDestroyArray(mxReturnTable);
		return(NULL);
	}
	mxSetField(mxReturnTable,0,"centeredAndDenyquisted",mxTempMatrix);
  
	// Create the waveform array
	mwDimension = new mwSize[3];
	mwDimension[0] = AnyTable->filterLen;
	mwDimension[1] = AnyTable->numChans;
	mwDimension[2] = AnyTable->numFilters;
	mxWaveArray = mxCreateNumericArray(3, mwDimension, mxDOUBLE_CLASS, mxREAL);
	if(!mxWaveArray)
	{
		mp_error_msg(func,"could not create waveform array\n");
		mxDestroyArray(mxReturnTable);
		return(NULL);
	}
  
	//Fill in the waveform array
	for (filterIdx = 0 ; filterIdx < AnyTable->numFilters ; filterIdx++)
		for (chanIdx = 0 ; chanIdx < AnyTable->numChans ; chanIdx++)
			for (sampleIdx = 0; sampleIdx < AnyTable->filterLen ; sampleIdx++)
				mxGetPr(mxWaveArray)[(filterIdx*AnyTable->numChans + chanIdx)*AnyTable->filterLen +  sampleIdx] = (double) (AnyTable->wave[filterIdx][chanIdx][sampleIdx]);
  
	// Adding the waveform array
	mxSetField(mxReturnTable, 0, "wave", mxWaveArray);
  
	// Clean the house
	delete[]mwDimension;

	return mxReturnTable;
}

/********************************************************************************/
/*  Function :		mp_create_anywave_table_from_mxAnywaveTable					*/
/*  Description :	Transforms the Matlab array mxTable into an Anywave table	*/
/*					AnyTable													*/
/*  Input :																		*/
/*		+ mxArray *mxTable : The input Maltab table								*/
/*  Output :																	*/
/*		+ MP_Anywave_Table_c *AnyTable : The output Anywave table				*/
/*																				*/
/********************************************************************************/
MP_Anywave_Table_c *mp_create_anywave_table_from_mxAnywaveTable(const mxArray *mxTable)
{
    char					*func = "mp_create_anywave_table_from_mxAnywaveTable";
	char					*szString;
	MP_Anywave_Table_c		*AnyTable;
	mxArray					*mxTempMatrix;
	const mwSize			*mwDimension;
	unsigned long int		iFilterIdx, iSampleIdx;
	MP_Chan_t				iChanIdx;

	//----------------------------
	// Creating the Anywave table
	//----------------------------
	AnyTable = new MP_Anywave_Table_c();
	AnyTable->set_null();
	
	//-------------------------------------
	// Getting fields of the Anywave table
	//-------------------------------------
	// Getting the table filename field
	mxTempMatrix = mxGetField(mxTable,0,"tableFileName");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.tableFileName field is missing\n");
		delete AnyTable;
		return(NULL);
	}
	szString = mxArrayToString(mxTempMatrix);
	strcpy(AnyTable->tableFileName, szString);
	mxFree(szString);
  
	// Getting the datas filename field
	mxTempMatrix = mxGetField(mxTable,0,"dataFileName");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.dataFileName field is missing\n");
		delete AnyTable;
		return(NULL);
	}
	szString = mxArrayToString(mxTempMatrix);
	strcpy(AnyTable->dataFileName, szString);
	mxFree(szString);

	// Getting the normalized field
	mxTempMatrix = mxGetField(mxTable,0,"normalized");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.normalized field is missing\n");
		delete AnyTable;
		return(NULL);
	}
	AnyTable->normalized = (unsigned long int) mxGetScalar(mxTempMatrix);
	
	// Getting the centered and deny quisted field
	mxTempMatrix = mxGetField(mxTable,0,"centeredAndDenyquisted");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.centeredAndDenyquisted field is missing\n");
		delete AnyTable;
		return(NULL);
	}
	AnyTable->centeredAndDenyquisted = (unsigned long int) mxGetScalar(mxTempMatrix);
	
	// Getting the wave field
	mxTempMatrix = mxGetField(mxTable,0,"wave");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.wave field is missing\n");
		delete AnyTable;
		return(NULL);
	}
	mwDimension = mxGetDimensions(mxTempMatrix);
	AnyTable->filterLen = mwDimension[0];
	AnyTable->numChans = mwDimension[1];
	AnyTable->numFilters = mwDimension[2];
  
	// Getting the wave field
	AnyTable->wave = (MP_Real_t***)mxMalloc(AnyTable->numFilters*sizeof(MP_Real_t**));
	if(!AnyTable->wave) 
	{
		mp_error_msg(func,"could not allocate the wave table for the waveforms\n");
		delete AnyTable;
		return(NULL);
	}  
    for (iFilterIdx = 0 ; iFilterIdx < AnyTable->numFilters ; iFilterIdx++)
	{
		AnyTable->wave[iFilterIdx] = (MP_Real_t**)mxMalloc(AnyTable->numChans*sizeof(MP_Real_t*));
		if(!AnyTable->wave[iFilterIdx]) 
		{
			mp_error_msg(func,"could not allocate index space for the waveforms in the wave table\n");
			delete AnyTable;
			return(NULL);
		}
		
		for (iChanIdx = 0 ; iChanIdx < AnyTable->numChans ; iChanIdx++)
		{
			AnyTable->wave[iFilterIdx][iChanIdx] = (MP_Real_t*)mxMalloc(AnyTable->filterLen*sizeof(MP_Real_t));
			for (iSampleIdx = 0; iSampleIdx < AnyTable->filterLen ; iSampleIdx++)
				AnyTable->wave[iFilterIdx][iChanIdx][iSampleIdx] = mxGetPr(mxTempMatrix)[(iFilterIdx * AnyTable->numChans + iChanIdx) * AnyTable->filterLen + iSampleIdx];
		}
	}
    return AnyTable;
}
