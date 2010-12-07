/******************************************************************************/
/*                                                                            */
/*                  	          mprecons.cpp                       	      */
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
/*						MPTK - Matlab interface								  */
/* Reconstructs a signal from a book, using MPTK							  */
/*																			  */
/* Usage: signal = mprecons(book[,residual])								  */
/*																			  */
/* Inputs:																	  */
/*  - book       : a book Matlab structure									  */
/*  - residual   : an optional numSamples x numChans matrix, which dimensions */
/*				   should match the fields book.numSamples, book.numChans	  */
/*																			  */
/* Outputs:																	  */
/*  - signal: the reconstructed signal, a numSamples x numChans matrix		  */
/******************************************************************************/

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) {
    
  // Check input arguments
  if (nrhs<1 || nrhs>2) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  if ( !mxIsStruct(prhs[0])) {
    mexPrintf("!!! %s error -- The first argument shoud be a book structure\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  
  // Check output args
  if (nlhs > 1) {
    mexPrintf("!!! %s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  
  // Load the MPTK environment if not loaded
  InitMPTK4Matlab(mexFunctionName());
  
  // Load book object from Matlab structure
  const mxArray *mexBook = prhs[0];
  MP_Book_c *book = mp_create_book_from_mxBook(mexBook);
  if(NULL==book) {
    mexPrintf("Failed to convert a book from Matlab to MPTK.\n");
    mexErrMsgTxt("Aborting");
    return;
  }

 // Initializing output signal 
  MP_Signal_c *residual = NULL;
  if(nrhs==1) {
    residual = MP_Signal_c::init(book->numChans,book->numSamples,book->sampleRate );
  }
  else {
    const mxArray* mxSignal = prhs[1];
    residual = mp_create_signal_from_mxSignal(mxSignal);
  }
  if(NULL==residual) {
    mexPrintf("%s could not init or convert residual\n",mexFunctionName());
    // Clean the house
    delete book;
    mexErrMsgTxt("Aborting");
    return;
  }
  // Reconstructing 
  book->substract_add(NULL,residual,NULL);
  // Clean the house
  delete book;

  // Load residual object in Matlab structure
  mxArray *mxSignal = mp_create_mxSignal_from_signal(residual);
  // Clean the house
  delete residual;
  if(NULL==mxSignal) {
    mexPrintf("Failed to convert a signal from MPTK to Matlab.\n");
    mexErrMsgTxt("Aborting");
    return;
  }
  if(nlhs>0)  plhs[0] = mxSignal;
}
