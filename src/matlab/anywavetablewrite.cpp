/******************************************************************************/
/*                                                                            */
/*                  	     anywavetablewrite.cpp                    	      */
/*                                                                            */
/*							  mptk4matlab toolbox							  */
/*                                                                            */
/* Ronan Le Boulch                                             	  Feb 01 2010 */
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
/*							MPTK - Matlab interface							  */
/* Exports a binary Matching Pursuit anywavetable from Matlab, using MPTK	  */
/*																			  */
/* Usage : anywavetablewrite(anywavetable,filename[,writeMode])				  */
/*																			  */
/* Input :																	  */
/*  - anywavetable  : a book structure with the following structure			  */
/*  - filename		: the filename where to read the book					  */
/*  - writeMode		: optional, 'binary' (default) or 'txt'					  */
/*																			  */
/******************************************************************************/

#include "mptk4matlab.h"
#include "mxBook.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) 
{
	char				*szTableFileName = NULL, *szDatasFileName = NULL;
	char				*writeModeName = NULL;
	const mxArray		*mxAnywaveTable;	
	MP_Anywave_Table_c	*mpAnywaveTable;
	
	//------------------------
	// Matlab initialisation
	//------------------------
	InitMPTK4Matlab(mexFunctionName());
    
	//--------------------------
	// Checking input arguments
	//--------------------------
	if (nrhs != 3) 
	{
		mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;
	} 
	if ( !mxIsStruct(prhs[0])) 
	{
		mexPrintf("!!! %s error -- first argument (anywaveTable) should be a struct\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;        
	} 
	if(!mxIsChar(prhs[1])) 
	{
		mexPrintf("!!! %s error -- second argument (szTableFileName) should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;        
	}
	else
	{
		szTableFileName = mxArrayToString(prhs[1]);
		if (szTableFileName == NULL) 
		{
			mexPrintf("%s error -- second argument (szTableFileName) could not be retrieved from the input\n",mexFunctionName());
			mexErrMsgTxt("Aborting");
			return;
		} 
	}
	if(!mxIsChar(prhs[2])) 
	{
		mexPrintf("!!! %s error -- third argument (szDatasFileName) should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;        
	}
	else 
	{
		szDatasFileName = mxArrayToString(prhs[2]);
		if (szDatasFileName == NULL) 
		{
			mexPrintf("%s error -- second argument (szDatasFileName) could not be retrieved from the input\n",mexFunctionName());
			mexErrMsgTxt("Aborting");
			return;
		} 
	}

	//---------------------------
	// Checking output arguments
	//---------------------------
	if (nlhs>0) 
	{
		mexPrintf("%s error -- wrong number of output arguments\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		// Clean the house
		mxFree(szTableFileName);
		mxFree(szDatasFileName);
		return;
	}

	//----------------------------------------
	// Loading anywaveTable object from Matlab
	//----------------------------------------
	mxAnywaveTable = prhs[0];
	mpAnywaveTable = mp_create_anywave_table_from_mxAnywaveTable(mxAnywaveTable);
	if(mpAnywaveTable == NULL) 
	{
		mexPrintf("Failed to convert a book from Matlab to MPTK.\n");
		mexErrMsgTxt("Aborting");
		// Clean the house
		mxFree(szTableFileName);
		mxFree(szDatasFileName);
		return;
	}
  
	mpAnywaveTable->write(szTableFileName,szDatasFileName);

	// Clean the house
	mxFree(szTableFileName);
	mxFree(szDatasFileName);
	return;
}

