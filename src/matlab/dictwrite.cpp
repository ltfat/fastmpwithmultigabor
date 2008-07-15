#include "mex.h"
#include "mptk.h"
#include <iostream>
#include <sstream>

void msgfunc(char *msge) {
	mexPrintf("%s",msge);
}

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
	double isvalid   = 0.0;
	double iswritten = 0.0;
	mxArray *mxBlockCell = NULL;
	mxArray *mxTmp = NULL;
	MP_Dict_c *dict = NULL;

	/* Check input arguments */
    if (nrhs<2 || nrhs>2) {
        mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        mexErrMsgTxt("Aborting");
		return;
    }
	
	/* Check output arguments */
    if (nlhs>2) {
        mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        mexErrMsgTxt("Aborting");
		return;
    }

    
	/* Load the MPTK environment if not loaded */
    if (!MPTK_Env_c::get_env()->get_environment_loaded()) {
		if (MPTK_Env_c::get_env()->load_environment("")==false) {
				mexPrintf("%s error -- could not load the MPTK environment.\n",mexFunctionName());
				mexPrintf("The most common reason is a missing or erroneous MPTK_CONFIG_FILENAME variable.\n");
				mexPrintf("This environment variable can be set by typing\n");
				mexPrintf("     'setenv('MPTK_CONFIG_FILENAME','<path to configuration file.xml>')");
				mexPrintf(" from the Matlab command line\n");
				mexErrMsgTxt("Aborting");
		}
	}

	MPTK_Server_c::get_msg_server()->register_display_function("info_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("error_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("warning_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("progress_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("debug_message_display", &msgfunc);
	
	/* Check that the input dictionary structure has the right fields */
	mxBlockCell = mxGetField(prhs[0],0,"block");
	if (NULL==mxBlockCell) {
		mexWarnMsgTxt("The dict.block field is missing. Aborting");
		if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
		if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	int nBlocks = (int)mxGetNumberOfElements(mxBlockCell);
	if (nBlocks<=0) {
		mexWarnMsgTxt("The number of blocks should be at least one. Aborting");
		if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
		if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	if(!mxIsCell(mxBlockCell))
	{
		mexWarnMsgTxt("The dict.block is not a cell array. Aborting");
		if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
		if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	/* Reach all blocks */ 
	for (int i = 0; i < nBlocks; i++ ) {
		mxArray *mxBlock = mxGetCell(mxBlockCell,i);
		if (NULL==mxBlock) {
			mexPrintf("dict.block{%d} could not be retrieved",i);
			mexWarnMsgTxt("Aborting");
			if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			return;
		}
		size_t numFields = mxGetNumberOfFields(mxBlock);
		if (numFields<=0) {
			mexPrintf("The number of fields %d should be at least one in dict.block{%d}.",numFields,i);
			mexWarnMsgTxt("Aborting");
			if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			return;
		}
 	  
		/* Reach all fields of the block and put them in a map */
		map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
		for (int j= 0; j <numFields ; j++)
        {
			const char * fieldName = mxGetFieldNameByNumber(mxBlock,j);
			if (NULL==fieldName) {
				mexPrintf("Field number %d in block %d could not be retrieved",j,i);
				mexWarnMsgTxt("Aborting");		
				if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
				if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
				return;
			}
			/* Retrieve the field value and store in the map */
			mxTmp = mxGetField(mxBlock,0,fieldName);
			char * fieldValue = mxArrayToString(mxTmp);
			(*paramMap)[string(fieldName)]=string(fieldValue);
			mxFree(fieldValue);
			fieldValue = NULL;
		}
		
		/* Retrieve the block creator */
		MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
		blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*paramMap)["type"].c_str());
		if (NULL == blockCreator) 
		{
			mexPrintf("The block %d of type %s is not registered in the block factory.",i,(*paramMap)["type"].c_str());
			mexWarnMsgTxt("Aborting");
			if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			delete paramMap;
			return;
        }
		/* Create the block */
		MP_Block_c *block =  blockCreator(NULL, paramMap);

		if (NULL == block)
		{
			mexPrintf("The block %d of type %s was not successfully created.",i,(*paramMap)["type"].c_str());
			mexWarnMsgTxt("Aborting");
			if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			delete paramMap;
			return;
        }
		
		/* Create the dictionary if needed */
		if (NULL==dict) {
			dict = MP_Dict_c::init();
			if (NULL==dict)
			{
				mexWarnMsgTxt("Failed to create an empty dictionary. Aborting.");
				if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
				if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
				delete paramMap;
				delete block;
				return;
			}
		}
		
		/* Add the block to the dictionary */
		dict->add_block(block);
		delete paramMap;
	}

	/* The dictionary is valid, and we can write the first output if needed */
	isvalid = 1.0;
	if(nlhs>0) plhs[0] = mxCreateDoubleScalar((double)isvalid);
	/* If a filename was provided, we need to try to write to file */
    if(nrhs>1) {
		char *fileName = mxArrayToString(prhs[1]);
		if (NULL==fileName) {
			mexWarnMsgTxt("The file name could not be retrieved from the input. Aborting.");
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			return;
		}
		if (dict->print(fileName)) {
			mexWarnMsgTxt("The dictionary could not be written.");
			if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
			return;
		}
		iswritten = 1.0;
		mxFree(fileName);
	}
	/* If needed, write the second output */
	if(nlhs>1) plhs[1] = mxCreateDoubleScalar((double)iswritten);
	
	// We need to understand why deleting the dictionary crashes !!!!
	// delete dict;
	
	return;
}