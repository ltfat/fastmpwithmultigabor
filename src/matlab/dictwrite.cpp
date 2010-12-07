/******************************************************************************/
/*                                                                            */
/*                  	          dictwrite.cpp                       	      */
/*                                                                            */
/*								mptk4matlab toolbox							  */
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
/*						MPTK - Matlab interface								  */
/* Export a dictionary description from Matlab to a file, using MPTK		  */
/*																			  */
/* Usage: isvalid = dictwrite(dict[,filename])								  */
/*																			  */
/* Inputs:																	  */
/*  - dict     : a dictionary description with the following structure		  */
/*						dict.block{i} = block								  */
/*					where, for example										  */
/*						block.type = 'dirac'								  */
/*					and block may have other field names					  */
/*  - filename : the filename where to write the dictionary description in	  */
/*               XML if ommited, we just check if the syntax of dict is valid */
/*																			  */
/* Outputs:																	  */
/*  - isvalid   : indicates if the dictionary structure was correctly formed  */ 
/******************************************************************************/

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
  char *fileName   = NULL;

  InitMPTK4Matlab(mexFunctionName());
  
  // Check input arguments
  if (nrhs<1 || nrhs>2) {
    mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  if(nrhs==2) {
    if(!mxIsChar(prhs[1])) {
      mexPrintf("%s error -- the optional second argument filename should be a string\n",mexFunctionName());
      mexPrintf("    see help %s\n",mexFunctionName());
      mexErrMsgTxt("Aborting");
      return;        
    }
    else {
      fileName = mxArrayToString(prhs[1]);
      if (NULL==fileName) {
	mexPrintf("%s error -- the optional second argument filename could not be retrieved from the input\n",mexFunctionName());
	mexErrMsgTxt("Aborting");
	return;
      }
    }
  }

  // Check output arguments
  if (nlhs>1) {
    mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }

  // Converting dictionary
  const mxArray* mxDict = prhs[0];
  MP_Dict_c *dict = mp_create_dict_from_mxDict(mxDict);

  // If the dictionary is not valid
  if(NULL==dict) {
    double isvalid   = 0.0;
    if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
    return;
  }
  // If it is valid
  double isvalid = 1.0;
  if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
  // If a filename was provided, we need to try to write to file
  if(NULL!=fileName) {
    if (dict->print(fileName)) {
      mexPrintf("%s error --the dictionary could not be written to file %s\n",mexFunctionName(),fileName);
      // Clean the house
      mxFree(fileName);
      mexErrMsgTxt("Aborting");
      return;
    } 
    else {
      // Clean the house
      mxFree(fileName);
    }
  }
  // Clean the house
  delete dict;
}
