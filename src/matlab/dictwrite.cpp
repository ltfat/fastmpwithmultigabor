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
    if (nrhs !=2) {
        mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        mexErrMsgTxt("Aborting");
		return;
    }
	
	/* Check output arguments */
    if (nlhs !=2) {
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

	mp_info_msg("mexFunction","this message does not display\n");
	MPTK_Server_c::get_msg_server()->register_display_function("info_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("error_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("warning_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("progress_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("debug_message_display", &msgfunc);
	mp_info_msg("mexFunction","this message displays\n");
	
	/* Check that the input dictionary structure has the right fields */
	mxBlockCell = mxGetField(prhs[0],0,"block");
	if (NULL==mxBlockCell) {
		mexWarnMsgTxt("The dict.block field is missing. Aborting");
		plhs[0] = mxCreateDoubleScalar((double)isvalid);
		plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	int nBlocks = (int)mxGetNumberOfElements(mxBlockCell);
	if (nBlocks<=0) {
		mexWarnMsgTxt("The number of blocks should be at least one. Aborting");
		plhs[0] = mxCreateDoubleScalar((double)isvalid);
		plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	if(!mxIsCell(mxBlockCell))
	{
		mexWarnMsgTxt("The dict.block is not a cell array. Aborting");
		plhs[0] = mxCreateDoubleScalar((double)isvalid);
		plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	/* Reach all blocks */ 
	for (int i = 0; i < nBlocks; i++ ) {
		mxArray *mxBlock = mxGetCell(mxBlockCell,i);
		if (NULL==mxBlock) {
			mexPrintf("dict.block{%d} could not be retrieved",i);
			//if (!mxIsStruct(prhs[0]))
			//  mexPrintf("is not a struct");
			//  if (!mxIsCell(prhs[0]))
			//  mexPrintf("is not a cell");
			//  if (mxGetFieldNumber(prhs[0],"block")<0)
			//  mexPrintf("has no field called 'block'");
			mexWarnMsgTxt("Aborting");
			plhs[0] = mxCreateDoubleScalar((double)isvalid);
			plhs[1] = mxCreateDoubleScalar((double)iswritten);
			return;
		}
		size_t numFields = mxGetNumberOfFields(mxBlock);
		if (numFields<=0) {
			if(!mxIsStruct(mxBlock))
			mexPrintf("dict.block{%d} is not a struct",i);
			if(!mxIsCell(mxBlock))
			mexPrintf("dict.block{%d} is not a cell",i);
			mexPrintf("The number of fields %d should be at least one in dict.block{%d}.",numFields,i);
			mexWarnMsgTxt("Aborting");
			plhs[0] = mxCreateDoubleScalar((double)isvalid);
			plhs[1] = mxCreateDoubleScalar((double)iswritten);
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
				plhs[0] = mxCreateDoubleScalar((double)isvalid);
				plhs[1] = mxCreateDoubleScalar((double)iswritten);
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
			mexPrintf("The block %d of type %s is not registred in the atom factory.",i,(*paramMap)["type"].c_str());
			mexWarnMsgTxt("Aborting");
			plhs[0] = mxCreateDoubleScalar((double)isvalid);
			plhs[1] = mxCreateDoubleScalar((double)iswritten);
			delete paramMap;
			return;
        }
		/* Create the block */
		MP_Block_c *block =  blockCreator(NULL, paramMap);

		if (NULL == block)
		{
			mexPrintf("The block %d of type %s was not successfully created.",i,(*paramMap)["type"].c_str());
			mexWarnMsgTxt("Aborting");
			plhs[0] = mxCreateDoubleScalar((double)isvalid);
			plhs[1] = mxCreateDoubleScalar((double)iswritten);
			delete paramMap;
			return;
        }
		/* Add the block to dictionary */
		if (i==1) {
			dict = MP_Dict_c::init();
			if (NULL==dict)
			{
				mexWarnMsgTxt("Failed to create an empty dictionary. Aborting.");
				plhs[0] = mxCreateDoubleScalar((double)isvalid);
				plhs[1] = mxCreateDoubleScalar((double)iswritten);
				delete paramMap;
				delete block;
				return;
			}
		}
		
		mexPrintf("Block and dictionary successfully created, there remains to add the block...");
		block->info(stdout); // Does not crash
		dict->add_block(block); // Bug is here now: bus error !!!
		mexPrintf("not done\n");
		//delete paramMap;
	}

	isvalid = 1.0;
	char *fileName = mxArrayToString(prhs[1]);
	if (NULL==fileName) {
		mexWarnMsgTxt("The file name could not be retrieved from the input. Aborting.");
		plhs[0] = mxCreateDoubleScalar((double)isvalid);
		plhs[1] = mxCreateDoubleScalar((double)iswritten);
		return;
	}
	// Other bug here when trying to write to file if no add_block has been performed
//	if (dict->print(fileName)) {
//		mexWarnMsgTxt("The dictionary could not be written.");
//		plhs[0] = mxCreateDoubleScalar((double)isvalid);
//		plhs[1] = mxCreateDoubleScalar((double)iswritten);
//		return;
//	}
//	iswritten = 1.0;
	mxFree(fileName);
	plhs[0] = mxCreateDoubleScalar((double)isvalid);
	plhs[1] = mxCreateDoubleScalar((double)iswritten);
	delete dict;
	return;
}



void eerterterteFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
 {

  /* Declarations */
  char filename[1000];
  int tmpcharlen,i;
  const char *field_names[] ={"filename","numBlocks","block"};
  MP_Dict_c* dict = NULL;
  MP_Block_c* block = NULL;
  mwSize dims[2] =
    {
      1,1
    };
  mxArray *tmp = NULL;
  mxArray *mxBlocks= NULL;
  mxArray *mxBlock= NULL;
  char*  convertEnd;
  map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
  map<string,string,mp_ltstring> *paramMapType= new map<string, string, mp_ltstring>();
  std::ostringstream oss1;
  std::ostringstream oss2;
  map<string, string, mp_ltstring>::const_iterator iter;
  
  /* Load the MPTK environment if not loaded */
  if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
  
  /* Create new dict */
  dict = MP_Dict_c::init();
  if (dict==NULL)
    {
      mexPrintf("Failed to create an empty dictionary\n");
      return;
    }

  /* Input */
  if (nrhs!=2)
    {
      mexErrMsgTxt("Two input argument are required");
    }

  tmp = mxGetField(prhs[0],0,"numBlocks");
  int nBlocks = (int)mxGetScalar(tmp);
  mxBlocks = mxGetField(prhs[0],0,"block");
  for (int i = 0; i < nBlocks; i++ )
    {
      mxBlock = mxGetCell(mxBlocks,i);
      size_t mxBlockSize = mxGetNumberOfFields(mxBlock);
      for (int j= 0; j <mxBlockSize ; j++)
        {

          const char * field_name = mxGetFieldNameByNumber(mxBlock,j);
          /* Block type */
          tmp = mxGetField(mxBlock,0,"type");
          tmpcharlen = mxGetN(tmp)+1;
          char * type = (char*)mxCalloc(tmpcharlen, sizeof(char));
          mxGetString(tmp,type,tmpcharlen);
          /*Load the parameter type map for block type*/
          paramMapType = new map<string, string, mp_ltstring>();
          MP_Block_Factory_c::get_block_factory()->get_block_type_map(type)(paramMapType);

          tmp = mxGetFieldByNumber(mxBlock, 0 , j);

          /* Test the type of parameter and fill it in the parameter map */
          if (!strcmp(((*paramMapType)[field_name]).c_str(),"string"))
            {
              tmpcharlen = mxGetN(tmp)+1;
              char * stringbuf = (char*)mxCalloc(tmpcharlen, sizeof(char));
              mxGetString(tmp,stringbuf,tmpcharlen);
              if (!(oss1 << field_name))
                {
                  mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                  return;
                }
              if (!(oss2 << stringbuf))
                {
                  mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                  return;
                }

              (*paramMap)[oss1.str()] = oss2.str();
              oss1.str("");
              oss2.str("");
            }
          else
            {
              if ((!strcmp(((*paramMapType)[field_name]).c_str(),"ulong"))||(!strcmp(((*paramMapType)[field_name]).c_str(),"uint")))
                {
                  unsigned long int value = (unsigned long int)mxGetScalar(tmp);
                  if (!(oss1 << field_name))
                    {
                      mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                      return;
                    }
                  if (!(oss2 << value))
                    {
                      mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                      return;
                    }

                  (*paramMap)[oss1.str()] = oss2.str();
                  oss1.str("");
                  oss2.str("");
                }
              else if (!strcmp(((*paramMapType)[field_name]).c_str(),"real"))
                {
                  double value = (double)mxGetScalar(tmp);
                  if (!(oss1 << field_name))
                    {
                      mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                      return;
                    }
                  if (!(oss2 << value))
                    {
                      mexErrMsgTxt("Cannot convert parameter in string for parameterMap");
                      return;
                    }

                  (*paramMap)[oss1.str()] = oss2.str();
                  oss1.str("");
                  oss2.str("");
                }


            }

        }

      /*create the block */
      MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
      blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*paramMap)["type"].c_str());

      if (NULL == blockCreator)
        {
          mexPrintf("The %s block type is not registred in the atom factory.\n",(*paramMap)["type"].c_str());
          return;
        }
      block =  blockCreator(NULL, paramMap);
      /* Add the block to dictionary */
      dict->add_block(block);

    }

  /* Extract file name from input parameter*/
  tmpcharlen = mxGetN(prhs[1])+1;
  mxGetString(prhs[1],filename,tmpcharlen);
  /* Write dictionary */
  if (dict->print(filename)) mexErrMsgTxt("Failed to write dictionary");
  else mexPrintf("Write dictionary to file '%s':\n", filename);
  
  delete(paramMapType);
  delete(paramMap);
  
}
