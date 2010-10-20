/******************************************************************************/
/*                                                                            */
/*                  	          sigwrite.cpp                       	      */
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

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
  char *fileName   = NULL;

  InitMPTK4Matlab(mexFunctionName());
  
  // Check input arguments
  if (3!=nrhs) {
    mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  if(!mxIsChar(prhs[1])) {
    mexPrintf("%s error -- the second argument filename should be a string\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;        
  }
  fileName = mxArrayToString(prhs[1]);
  if (NULL==fileName) {
    mexPrintf("%s error -- the second argument filename could not be retrieved from the input\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  if(!mxIsNumeric(prhs[2])) {
    mexPrintf("%s error -- the third argument sampleRate should be a positive number\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  double sampleRate = mxGetScalar(prhs[2]);

  // Check output arguments
  if (nlhs>0) {
    mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    // Clean the house
    mxFree(fileName);
    mexErrMsgTxt("Aborting");
    return;
  }

  // Converting signal
  const mxArray* mxSignal = prhs[0];
  MP_Signal_c *signal = mp_create_signal_from_mxSignal(mxSignal);
  if(NULL==signal) {
    mexPrintf("%s could not convert given signal\n",mexFunctionName());
    // Clean the house
    mxFree(fileName);
    mexErrMsgTxt("Aborting");
    return;
  }
  signal->sampleRate = (int)sampleRate;

  // Writing. 
  if (0==signal->wavwrite(fileName)) {
    mexPrintf("%s error --the signal could not be written to WAV file %s\n",mexFunctionName(),fileName);
    // Clean the house
    mxFree(fileName);
    return;
  } 

  // Clean the house
  delete signal;
}
