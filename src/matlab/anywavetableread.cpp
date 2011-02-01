/******************************************************************************/
/*                                                                            */
/*                                  mptk.h                                    */
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

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
	char				*szFileName;
	MP_Anywave_Table_c	*xTable;
	mxArray				*mxTable;
	
	//------------------------
	// Matlab initialisation
	//------------------------
	InitMPTK4Matlab(mexFunctionName());

	//--------------------------
	// Checking input arguments
	//--------------------------
	if (nrhs != 1) 
	{
		mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;
	}
	if (!mxIsChar(prhs[0])) 
	{
		mexPrintf("%s error -- the argument filename should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;        
	}
	
	//---------------------------
	// Checking output arguments
	//---------------------------
	if (nlhs>1) 
	{
		mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;
	}
  
	//-----------------------
	// Getting the filename 
	//-----------------------
	szFileName = mxArrayToString(prhs[0]);
	if (szFileName == NULL) 
	{
		mexErrMsgTxt("The file name could not be retrieved from the input. Aborting.");
		return;
	}
 
	//-------------------
	// Loading the table
	//-------------------
	xTable = new MP_Anywave_Table_c(szFileName);
	if (!xTable) 
	{
		mexPrintf("Failed to create an anywave table from binary file [%s].\n", szFileName);
		mexErrMsgTxt("Aborting");
		mxFree(szFileName);
		return;
	}

	//-----------------------------------------------
	// Loading dictionary object in Matlab structure
	//-----------------------------------------------
	mxTable = mp_create_mxAnywaveTable_from_anywave_table(xTable);
	if(!mxTable) 
	{
		mexPrintf("Failed to convert an anywave table from MPTK to Matlab.\n");
		mexErrMsgTxt("Aborting");
		return;
	}
	if (nlhs>0) 
		plhs[0] = mxTable;
	
	// Clean the house
	mxFree(szFileName);
	free(xTable);
}
