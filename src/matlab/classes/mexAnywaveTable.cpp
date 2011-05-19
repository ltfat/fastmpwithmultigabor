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
	const char			*func = "mp_create_mxAnywaveTable_from_anywave_table";
  unsigned long int	filterIdx, chanIdx, sampleIdx;
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
  
	//-----------------------------------------
	// Adding the datas into the output matrix
	//-----------------------------------------
	// Create the waveform array
	mwDimension = new mwSize[3];
	mwDimension[0] = AnyTable->filterLen;
	mwDimension[1] = AnyTable->numChans;
	mwDimension[2] = AnyTable->numFilters;
	mxWaveArray = mxCreateNumericArray(3, mwDimension, mxDOUBLE_CLASS, mxREAL);
	if(!mxWaveArray)
	{
		mp_error_msg(func,"could not create waveform array\n");
		return(NULL);
	}
  
	//Fill in the waveform array
	for (filterIdx = 0 ; filterIdx < AnyTable->numFilters ; filterIdx++)
		for (chanIdx = 0 ; chanIdx < AnyTable->numChans ; chanIdx++)
			for (sampleIdx = 0; sampleIdx < AnyTable->filterLen ; sampleIdx++)
				mxGetPr(mxWaveArray)[(filterIdx*AnyTable->numChans + chanIdx)*AnyTable->filterLen +  sampleIdx] = (double) (AnyTable->wave[filterIdx][chanIdx][sampleIdx]);
  
	// Clean the house
	delete[]mwDimension;

	return mxWaveArray;
}

/********************************************************************************/
/*  Function :		mp_get_anywave_datas_from_mxAnywaveTable					*/
/*  Description :	Transforms the Matlab array mxTable into an Anywave table	*/
/*					AnyTable													*/
/*  Input :																		*/
/*		+ mxArray *mxTable : The input Maltab table								*/
/*  Output :																	*/
/*		+ MP_Anywave_Table_c *AnyTable : The output Anywave table				*/
/*																				*/
/********************************************************************************/
double *mp_get_anywave_datas_from_mxAnywaveTable(const mxArray *mxTable)
{
    const char				*func = "mp_create_anywave_table_from_mxAnywaveTable";
	mxArray					*mxTempMatrix;
	const mwSize			*mwDimension;
	unsigned long int		iFilterIdx, iSampleIdx;
	MP_Chan_t				iChanIdx;
	int						iIndexStorage = 0;
	unsigned long int		iFilterLen = 0, iNumFilters = 0;
	MP_Chan_t				iNumChans = 0;
	MP_Real_t				*dTable;

	//-------------------------------------
	// Getting fields of the Anywave table
	//-------------------------------------
	// Getting the wave field
	mxTempMatrix = mxGetField(mxTable,0,"data");
	if (!mxTempMatrix) 
	{
		mp_error_msg(func,"the table.wave field is missing\n");
		return(NULL);
	}
	mwDimension = mxGetDimensions(mxTempMatrix);
	iFilterLen = (unsigned long int)mwDimension[0];
	iNumChans = (MP_Chan_t)mwDimension[1];
	iNumFilters = (unsigned long int)mwDimension[2];
  
    for (iFilterIdx = 0 ; iFilterIdx < iNumFilters ; iFilterIdx++)
		for (iChanIdx = 0 ; iChanIdx < iNumChans ; iChanIdx++)
			for (iSampleIdx = 0; iSampleIdx < iFilterLen ; iSampleIdx++)
				dTable[iIndexStorage++] = mxGetPr(mxTempMatrix)[(iFilterIdx * iNumChans + iChanIdx) * iFilterLen + iSampleIdx];

    return dTable;
}
