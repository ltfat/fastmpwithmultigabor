/******************************************************************************/
/*                                                                            */
/*                  	          atomrecons.cpp                       	      */
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
#include "mxBook.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) {
    
  // Check input arguments
  if (1!=nrhs) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  //if ( !mxIsStruct(prhs[0])) {
  if ( !mxIsChar(prhs[0])) {
    // mexPrintf("!!! %s error -- The argument shoud be an atom structure\n",mexFunctionName());
    mexPrintf("!!! %s error -- The argument shoud be an atom name\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  double sampleRate = 1.0;
  
  // Check output args
  if (nlhs > 1) {
    mexPrintf("!!! %s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  
  // Load the MPTK environment if not loaded
  InitMPTK4Matlab(mexFunctionName());
  
  // Load atom object from Matlab 
  const mxArray *mxAtom = prhs[0];
  MP_Atom_c *atom = mp_create_atom_from_mxAtom(mxAtom); // Currently creates a default atom from a string
  if(NULL==atom) {
    mexPrintf("Failed to convert an atom from Matlab to MPTK.\n");
    mexErrMsgTxt("Aborting");
    return;
  }

 // Initializing output signal 
  MP_Signal_c *signal = MP_Signal_c::init(atom->numChans,atom->numSamples,sampleRate );
  if(NULL==signal) {
    mexPrintf("%s could not init output signal\n",mexFunctionName());
    // Clean the house
    delete atom;
    mexErrMsgTxt("Aborting");
    return;
  }

  // Checking compatibility ????
  // Reconstructing : this is where it crashes! Apparently, in more details, it happens in buildwaveform
  // Maybe this comes from a bad atom conversion ????
  mexPrintf("Succesfully generated atom\n");
  atom->write(stdout,MP_TEXT);
// return;
  atom->substract_add(NULL,signal);
  mexPrintf("Succesfully built to signal\n");
  // Clean the house
  delete atom;

  // Load signal object in Matlab structure
  mxArray *mxSignal = mp_create_mxSignal_from_signal(signal);
  // Clean the house
  delete signal;
  if(NULL==mxSignal) {
    mexPrintf("Failed to convert a signal from MPTK to Matlab.\n");
    mexErrMsgTxt("Aborting");
    return;
  }
  if(nlhs>0)  plhs[0] = mxSignal;
}
