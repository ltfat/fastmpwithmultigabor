/******************************************************************************/
/*                                                                            */
/*                  	          mpdecomp.cpp                       	      */
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
/* Decomposes a signal with MPTK											  */
/*																			  */
/* Usage: [book,residual,decay] = mpdecomp(signal,sampleRate,dict,numIter)	  */
/*																			  */
/* Inputs:																	  */
/*  - signal    : a numSamples x numChans signal (each column is a channel)	  */
/*  - sampleRate: the sampleRate of the signal								  */
/*  - dict      : either a dictionary Matlab strucure, or a filename		  */
/*  - numIter   : the number of iterations to perform						  */
/*																			  */
/* Outputs:																	  */
/*  - book      : the book with the resulting decomposition					  */
/*  - residual  : the residual obtained after the iterations				  */
/*  - decay     : numIterx1 vector with the residual energy at each iteration */
/******************************************************************************/

#include "mptk4matlab.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) {
  
  // Check input arguments
  if (nrhs!=4) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  if ( !mxIsNumeric(prhs[0]) ) {
      mexPrintf("!!! %s error -- The first argument shoud be a signal matrix\n",mexFunctionName());
      mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  if ( !mxIsNumeric(prhs[1]) ||  1!=mxGetNumberOfElements(prhs[1]) ) {
      mexPrintf("!!! %s error -- The second argument shoud be a scalar value\n",mexFunctionName());
      mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  if ( !mxIsStruct(prhs[2]) && !mxIsChar(prhs[2])) {
    mexPrintf("!!! %s error -- The third argument shoud be a dictionary structure or a filename\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  if ( !mxIsNumeric(prhs[3]) || 1!=mxGetNumberOfElements(prhs[3])) {
    mexPrintf("!!! %s error -- The fourth argument shoud be a scalar\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
    
  // Check output args
  if (nlhs > 3) {
    mexPrintf("!!! %s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  
  // Load the MPTK environment if not loaded
  InitMPTK4Matlab(mexFunctionName());
  
  // Load signal object from Matlab structure
  const mxArray *mxSignal = prhs[0];
  MP_Signal_c *signal = mp_create_signal_from_mxSignal(mxSignal);
  if(NULL==signal) {
    mexPrintf("Failed to convert a signal from Matlab to MPTK.\n");
    mexErrMsgTxt("Aborting");
    return;
  }
  // Load sampleRate
  signal->sampleRate = (int)mxGetScalar(prhs[1]);

  // Load dictionary object from Matlab structure
  const mxArray *mxDict = prhs[2];
  MP_Dict_c *dict = NULL;
  if( mxIsStruct(mxDict) ) {
    dict = mp_create_dict_from_mxDict(mxDict);
  } else {
    char *fileName = mxArrayToString(mxDict);
    if(NULL==fileName) {
      mexPrintf("Cannot retrieve filename string from inputs\n");
      mexErrMsgTxt("Aborting");
      return;
    }
    dict = MP_Dict_c::init(fileName);
    // Clean the house
    mxFree(fileName);
  }
  if(NULL==dict) {
    mexPrintf("Failed to convert a dict from Matlab to MPTK or to read it from file.\n");
    // Clean the house
    delete signal;
    mexErrMsgTxt("Aborting");
    return;
  }

  // Read number of iterations
  unsigned long int numIter = (unsigned long int) mxGetScalar(prhs[3]);
    
  // Creating book and core
  MP_Book_c *book = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );
  if ( NULL == book )  {
    mexPrintf("Failed to create a book object.\n" );
    // Clean the house
    delete signal;
    delete dict;
    mexErrMsgTxt("Aborting");
    return;
  }

  MP_Mpd_Core_c *mpdCore =  MP_Mpd_Core_c::create( signal, book, dict );
  if ( NULL == mpdCore )  {
    mexPrintf("Failed to create a MPD core object.\n" );
    // Clean the house
    delete signal;
    delete dict;
    delete book;
    mexErrMsgTxt("Aborting");
    return;
  }

	// To parameterize:
	unsigned long int reportHit = 10;
  // Set stopping condition
  mpdCore->set_iter_condition( numIter );
  mpdCore->set_save_hit(ULONG_MAX,NULL,NULL,NULL);
  mpdCore->set_report_hit(reportHit);

  // If decay wanted, 
  if(nlhs>2) mpdCore->set_use_decay();
  // Verbose mode
  mpdCore->set_verbose();
  // Display some information
  mexPrintf("The dictionary contains %d blocks\n",dict->numBlocks);
  mexPrintf("The signal has:\n");
  signal->info();
  mpdCore->info_conditions();
  mexPrintf("Initial signal energy is %g\n",mpdCore->get_initial_energy());
  mexPrintf("Starting to iterate\n");
  // Run
  //fflush(NULL);
mexEvalString("pause(.001);"); // to dump string and flush
  mpdCore->run();
  mpdCore->info_state();
  mpdCore->info_result();

  // Get results 
  if(nlhs>0) { // Book
    mxArray *mxBook = mp_create_mxBook_from_book(book);
    if(NULL==mxBook) {
      mexPrintf("%s could not convert book to Matlab\n",mexFunctionName());
      // Clean the house
      delete signal;
      delete dict;
      delete book;
      delete mpdCore;
      mexErrMsgTxt("Aborting");
      return;
    }
    plhs[0] = mxBook;
  }
  if(nlhs>1) { // Residual
    mxArray *mxSignal = mp_create_mxSignal_from_signal(signal);
    if(NULL==mxSignal) {
      mexPrintf("%s could not convert signal to Matlab\n",mexFunctionName());
      // Clean the house
      delete signal;
      delete dict;
      delete book;
      delete mpdCore;
      mexErrMsgTxt("Aborting");
      return;
    }
    plhs[1] = mxSignal;
  }
  if(nlhs>2) { // Decay
  	MP_Var_Array_c<double> decay = mpdCore->get_current_decay_vec(); 
    mxArray *mxDecay = mxCreateDoubleMatrix((mwSize)decay.nElem,(mwSize)1,mxREAL);
    if(NULL==mxDecay) {
      mexPrintf("%s could not allocate decay vector\n",mexFunctionName());
      // Clean the house
      delete signal;
      delete dict;
      delete book;
      delete mpdCore;
      mexErrMsgTxt("Aborting");
      return;
    }
    // Fill the decay vector
	// DEBUG
//	mexPrintf("decay vector :\n");
    for (unsigned long int i = 0; i< decay.nElem; i++)  {
      * ( mxGetPr(mxDecay) + i) = decay.elem[i];
	  // DEBUG
//	  mexPrintf("%f ",decay.elem[i]);
    }
	// DEBUG
//	mexPrintf("\n");
    plhs[2] = mxDecay;
  }

  // Clean the house
  delete signal;
  delete dict;
  delete book;
  //delete mpdCore;
}
