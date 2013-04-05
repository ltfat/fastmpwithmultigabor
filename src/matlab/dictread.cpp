/******************************************************************************/
/*                                                                            */
/*                  	          dictread.cpp                       	      */
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
/*							MPTK - Matlab interface							  */
/* Imports a dictionary description from a file to Matlab, using MPTK		  */
/*																			  */
/* Usage: dict = dictread(filename)											  */
/*																			  */
/* Input:																	  */
/*  - filename : the filename where to read the dictionary description in XML */
/*																			  */
/* Output:																	  */
/*  - dict     : a dictionary description with the following structure		  */
/*						dict.block{i} = block								  */
/*				where, for example											  */
/*						block.type = 'dirac'								  */
/*				and block may have other field names						  */
/******************************************************************************/

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
	InitMPTK4Matlab(mexFunctionName());

	// Check input arguments
	if (nrhs !=1) 
	{
		mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;
	}
	if ( !mxIsChar(prhs[0])) 
	{
		mexPrintf("%s error -- the argument filename should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;        
	}
	// Check output arguments
	if (nlhs>1) 
	{
		mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;
	}
  
	// Get the filename 
	char *fileName = mxArrayToString(prhs[0]);
	if (NULL==fileName) 
	{
		mexErrMsgTxt("The file name could not be retrieved from the input. Aborting.");
		return;
	}
	// Try to load the dictionary
	MP_Dict_c *dict = MP_Dict_c::read_from_xml_file(fileName);
	if (NULL==dict) 
	{
		mexPrintf("Failed to create a dictionary from XML file [%s].\n", fileName);
		// Clean the house
		mxFree(fileName);
		mexErrMsgTxt("Aborting");
		return;
	}

	// Clean the house
	mxFree(fileName);

	// Load dict object in Matlab structure
	mxArray *mxDict = mp_create_mxDict_from_dict(dict);
	if(NULL==mxDict) 
	{
		mexPrintf("Failed to convert a dictionary from MPTK to Matlab.\n");
		mexErrMsgTxt("Aborting");
		return;
	}
	if (nlhs>0) 
plhs[0] = mxDict;
}
