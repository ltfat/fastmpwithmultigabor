#include "mex.h"
#include "mptk.h"

void msgfunc(char *msge) {
	mexPrintf("%s",msge);
}

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
	/* Check input arguments */
	if (nrhs !=1) {
		mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		mexErrMsgTxt("Aborting");
		return;
	}
	
	/* Check output arguments */
	if (nlhs>1) {
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

	/* Get the filename and try to load the dictionary */
	char *fileName = mxArrayToString(prhs[0]);
	if (NULL==fileName) {
		mexWarnMsgTxt("The file name could not be retrieved from the input. Aborting.");
		return;
	}
	MP_Dict_c *dict = MP_Dict_c::init(fileName);
	if (NULL==dict)
	{
	  mexPrintf("Failed to create a dictionary from XML file [%s].\n", fileName);
	  mexWarnMsgTxt("Aborting");
	  return;
	}
	mxFree(fileName);

	// Create the output information structure 
	// dict.block{numBlocks}
	int numDictFieldNames    = 1;
	const char *dictFieldNames[] = {"block"};
	mxArray *mxDict = mxCreateStructMatrix((mwSize)1,(mwSize)1,numDictFieldNames,dictFieldNames);

	// Create the block cell
	mxArray *mxBlockCell = mxCreateCellMatrix((mwSize)dict->numBlocks,(mwSize)1);
	
	// Loop to create each block
	for (unsigned int i=0; i < dict->numBlocks; i++)
	{
		MP_Block_c *block = dict->block[i];
		map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
		paramMap = block->get_block_parameters_map();
		if (NULL==paramMap) 
		{
			mexPrintf("Empty paramMap for block %d\n", i);
			mexWarnMsgTxt("Aborting");
			return;
		}
		/* Create mxBlock */
		mxArray *mxBlock = mxCreateStructMatrix((mwSize)1,(mwSize)1,0,NULL);
		/* Add all fields */
		map<string, string, mp_ltstring>::const_iterator iter;
		for ( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ )
		{
			/* Add the field */
			mxAddField(mxBlock,iter->first.c_str());
			/* Set the field value */
			/* Here we may want to convert according to type of value */
            mxArray *mxTmp = mxCreateString(iter->second.c_str());
			mxSetField(mxBlock,0,iter->first.c_str(),mxTmp);
		}
		/* Put the mxBlock in the mxBlockCell */
		mxSetCell(mxBlockCell,i,mxBlock);
		/* Delete tmp variables */
		delete paramMap;
	}
	mxSetField(mxDict,0,"block",mxBlockCell);
	plhs[0] = mxDict;
}




