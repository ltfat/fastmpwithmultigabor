/******************************************************************************/
/*                                                                            */
/*                  	          cmpdecomp.cpp                       	      */
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
	const mxArray			*mxSignalFromMatlab,*mxDict;
	mxArray					*mxBook,*mxSignalForResidual,*mxDecay;
	MP_Signal_c				*signal;
	MP_Dict_c				*dict;
	MP_Book_c				*book;
	MP_CMpd_Core_c			*cmpdCore;
	char					*fileName;
	unsigned long int		iIndex,numIter,reportHit;
	MP_Var_Array_c<double>	decay;
	float					dB_cycle_improve,setSnr,dB_augment_improve,dB_augment_no_cycle;
	unsigned long int		num_cycles,num_augment,num_augment_no_cycle;
	bool					hold_flag;
	char					*szTmpArgument = NULL;
	bool					numIterSelected = false, setSnrSelected = false, num_cyclesSelected = false;
	bool					dB_cycle_improveSelected = false, num_augmentSelected = false, dB_augment_improveSelected = false;
	bool					num_augment_no_cycleSelected = false, dB_augment_no_cycleSelected = false, hold_flagSelected = false;

	//---------------------------
	// 1) Check input arguments
	//---------------------------
	if (nrhs < 3 && ((nrhs % 2) == 0)) 
	 {
		 mexPrintf("!!! %s error -- bad number of input arguments %i pfiouu\n",mexFunctionName(),nrhs);
		 mexPrintf("    see help %s\n",mexFunctionName());
		 return;
	 }
	 // Signal argument
	 if ( !mxIsNumeric(prhs[0]) ) 
	 {
		 mexPrintf("!!! %s error -- The 1st argument shoud be a signal matrix\n",mexFunctionName());
		 mexPrintf("    see help %s\n",mexFunctionName());
		 return;        
	 }
	 // Sample rate argument
	 if ( !mxIsNumeric(prhs[1]) ||  1!=mxGetNumberOfElements(prhs[1]) ) 
	 {
		 mexPrintf("!!! %s error -- The 2nd argument shoud be a scalar value\n",mexFunctionName());
		 mexPrintf("    see help %s\n",mexFunctionName());
		 return;        
	 }
	 // Dictionary argument
	 if ( !mxIsStruct(prhs[2]) && !mxIsChar(prhs[2])) 
	 {
		 mexPrintf("!!! %s error -- The 3rd argument shoud be a dictionary structure or a filename\n",mexFunctionName());
		 mexPrintf("    see help %s\n",mexFunctionName());
		 return;        
	 }

	for(iIndex = 3 ; iIndex < nrhs ; iIndex+=2)
	{
		// String argument
		if ( !mxIsChar(prhs[iIndex]) ) 
		{
			mexPrintf("!!! %s error -- The argument %i shoud be a string argument like '-n'\n",mexFunctionName(),iIndex);
			mexPrintf("    see help %s\n",mexFunctionName());
			return;        
		}
		// Value associated argument
		if ( !mxIsNumeric(prhs[iIndex+1])) 
		{
			mexPrintf("!!! %s error -- The argument %i shoud be a scalar value\n",mexFunctionName(),iIndex);
			mexPrintf("    see help %s\n",mexFunctionName());
			return;        
		}
	}

	//---------------------------
	// 2) Check output args
	//--------------------------- 
	if (nlhs > 3) 
	{
		mexPrintf("!!! %s error -- bad number of output arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;
	}
  
	//--------------------------------------------
	// 3) Load the MPTK environment if not loaded
	//--------------------------------------------
	InitMPTK4Matlab(mexFunctionName());
  
	//--------------------------------------------
	// 4) Load signal object from Matlab structure
	//--------------------------------------------
	mxSignalFromMatlab = prhs[0];
	signal = mp_create_signal_from_mxSignal(mxSignalFromMatlab);
	if(signal == NULL) 
	{
		mexPrintf("Failed to convert a signal from Matlab to MPTK.\n");
		mexErrMsgTxt("Aborting");
		return;
	}
	// Load sampleRate
	signal->sampleRate = (int)mxGetScalar(prhs[1]);

	//--------------------------------------------
	// 5) Load dictionary object from Matlab structure
	//--------------------------------------------
	mxDict = prhs[2];
	dict = NULL;
	if( mxIsStruct(mxDict) ) 
		dict = mp_create_dict_from_mxDict(mxDict);
	else 
	{
		fileName = mxArrayToString(mxDict);
		if(NULL==fileName) 
		{
			mexPrintf("Cannot retrieve filename string from inputs\n");
			mexErrMsgTxt("Aborting");
			return;
		}
		dict = MP_Dict_c::init(fileName);
		// Clean the filename
		mxFree(fileName);
	}
	if(dict == NULL) 
	{
		mexPrintf("Failed to convert a dict from Matlab to MPTK or to read it from file.\n");
		// Clean the house
		delete signal;
		mexErrMsgTxt("Aborting");
		return;
	}

  num_cycles = 1;
  dB_cycle_improve = 0.001;
  num_augment = 1;
  dB_augment_improve = 0;
  num_augment_no_cycle = 10000;
  dB_augment_no_cycle = 60;
  hold_flag = false;
  
	//--------------------------------------------
	// 6) Read parameters
	//--------------------------------------------
	for(iIndex = 3; iIndex < nrhs; iIndex+=2)
	{
		if(!strcmp(mxArrayToString(prhs[iIndex]),"-n"))
			{numIter = (unsigned long int) mxGetScalar(prhs[iIndex+1]); numIterSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-s"))
			{setSnr = (float) mxGetScalar(prhs[iIndex+1]); setSnrSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-L"))
			{num_cycles = (unsigned long int) mxGetScalar(prhs[iIndex+1]); num_cyclesSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-O"))
			{dB_cycle_improve = (float)	mxGetScalar(prhs[iIndex+1]); dB_cycle_improveSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-K"))
			{num_augment = (unsigned long int) mxGetScalar(prhs[iIndex+1]); num_augmentSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-J"))
			{dB_augment_improve = (float) mxGetScalar(prhs[iIndex+1]); dB_augment_improveSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-M"))
			{num_augment_no_cycle = (unsigned long int)	mxGetScalar(prhs[iIndex+1]); num_augment_no_cycleSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-Q"))
			{dB_augment_no_cycle = (float) mxGetScalar(prhs[iIndex+1]); dB_augment_no_cycleSelected = true;}
		else if(!strcmp(mxArrayToString(prhs[iIndex]),"-Z"))
			{hold_flag = (bool)	mxGetScalar(prhs[iIndex+1]); hold_flagSelected = true;}
	}

	//--------------------------------------------
	// 7) Creating book and core
	//--------------------------------------------
	book = MP_Book_c::create(signal->numChans, signal->numSamples, signal->sampleRate );
	if (book == NULL)  
	{
		mexPrintf("Failed to create a book object.\n" );
		// Clean the house
		delete signal;
		delete dict;
		mexErrMsgTxt("Aborting");
		return;
	}

	cmpdCore =  MP_CMpd_Core_c::create( signal, book, dict );
	if (cmpdCore == NULL)  
	{
		mexPrintf("Failed to create a MPD core object.\n" );
		// Clean the house
		delete signal;
		delete dict;
		delete book;
		mexErrMsgTxt("Aborting");
		return;
	}

	//--------------------------------------------
	// 8) Parameters
	//--------------------------------------------
	// Report hits
	reportHit = 10;
	// Stopping condition
	cmpdCore->set_iter_condition( numIter );
	if(setSnrSelected)
		cmpdCore->set_snr_condition(setSnr);
	cmpdCore->set_settings (num_cycles, dB_cycle_improve, num_augment, dB_augment_improve, num_augment_no_cycle, dB_augment_no_cycle, hold_flag);
	cmpdCore->set_save_hit(ULONG_MAX,NULL,NULL,NULL);
	cmpdCore->set_report_hit(reportHit);
	// If decay wanted, 
	if(nlhs>2) 
		cmpdCore->set_use_decay();
	// Verbose mode
	cmpdCore->set_verbose();
	// Display some information
	mexPrintf("The dictionary contains %d blocks\n",dict->numBlocks);
	mexPrintf("The signal has:\n");
	signal->info();
	cmpdCore->info_conditions();
	mexPrintf("Initial signal energy is %g\n",cmpdCore->get_initial_energy());
	mexPrintf("Starting to iterate\n");

	
	//--------------------------------------------
	// 8) Running
	//--------------------------------------------
	mexEvalString("pause(.001);"); // to dump string and flush
	cmpdCore->run();
	cmpdCore->info_state();
	cmpdCore->info_result();

	//--------------------------------------------
	// 9) Getting resutls
	//--------------------------------------------
	if(nlhs>0) 
	{
		// Arg 1 : Book
		mxBook = mp_create_mxBook_from_book(book);
		if(mxBook == NULL) 
		{
			mexPrintf("%s could not convert book to Matlab\n",mexFunctionName());
			// Clean the house
			delete signal;
			delete dict;
			delete book;
			delete cmpdCore;
			mexErrMsgTxt("Aborting");
			return;
		}
		plhs[0] = mxBook;
	}
	
	// Arg 2 : Residual
	if(nlhs>1) 
	{
		mxSignalForResidual = mp_create_mxSignal_from_signal(signal);
		if(mxSignalForResidual == NULL) 
		{
			mexPrintf("%s could not convert signal to Matlab\n",mexFunctionName());
			// Clean the house
			delete signal;
			delete dict;
			delete book;
			delete cmpdCore;
			mexErrMsgTxt("Aborting");
			return;
		}
		plhs[1] = mxSignalForResidual;
	}
	
	// Arg 3 : Decay
	if(nlhs>2) 
	{
		decay = cmpdCore->get_current_decay_vec(); 
		mxDecay = mxCreateDoubleMatrix((mwSize)decay.nElem,(mwSize)1,mxREAL);
		if(mxDecay == NULL) 
		{
			mexPrintf("%s could not allocate decay vector\n",mexFunctionName());
			// Clean the house
			delete signal;
			delete dict;
			delete book;
			delete cmpdCore;
			mexErrMsgTxt("Aborting");
			return;
		}
		// Fill the decay vector
		for (unsigned long int i = 0; i< decay.nElem; i++)  
		{
			*(mxGetPr(mxDecay)+i) = decay.elem[i];
		}
		plhs[2] = mxDecay;
  }

  // Clean the house
  delete signal;
  delete dict;
  delete book;
}
