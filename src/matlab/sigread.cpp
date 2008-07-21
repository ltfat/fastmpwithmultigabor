/******************************************************************************/
/*                                                                            */
/*                  	          sigread.cpp                       	      */
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
/*
 * $Version 0.5.3$
 * $Date 05/22/2007$
 */

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{

  InitMPTK4Matlab(mexFunctionName());

  // Check input arguments
  if (nrhs !=1) {
    mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  if ( !mxIsChar(prhs[0])) {
    mexPrintf("%s error -- the argument filename should be a string\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;        
  }
  // Check output arguments
  if (nlhs>2) {
    mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  
  // Get the filename 
  char *fileName = mxArrayToString(prhs[0]);
  if (NULL==fileName) {
    mexErrMsgTxt("The file name could not be retrieved from the input. Aborting.");
    return;
  }
  // Try to load the signal
  MP_Signal_c *signal = MP_Signal_c::init(fileName);
  if (NULL==signal) {
    mexPrintf("Failed to create a signal from file [%s].\n", fileName);
    // Clean the house
    mxFree(fileName);
    mexErrMsgTxt("Aborting");
    return;
  }
  // Clean the house
  mxFree(fileName);

  // Load signal object in Matlab structure
  mxArray *mxSignal = mp_create_mxSignal_from_signal(signal);
  if(NULL==mxSignal) {
    mexPrintf("Failed to convert a signal from MPTK to Matlab.\n");
    mexErrMsgTxt("Aborting");
    return;
  }
  if(nlhs>0)  plhs[0] = mxSignal;
  if(nlhs>1)  plhs[1] = mxCreateDoubleScalar((double)signal->sampleRate);
}
