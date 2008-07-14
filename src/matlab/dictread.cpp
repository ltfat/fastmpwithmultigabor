#include "mex.h"
#include "mptk.h"

/* void msgfunc(char *msge) {
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
	if (nlhs !=1) {
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

	/* Get the filename and try to load the dictionary */
	char *fileName = mxArrayToString(prhs[0]);
	if (NULL==fileName) {
		mexWarnMsgTxt("The file name could not be retrieved from the input. Aborting.");
		return;
	}
	dict = MP_Dict_c::init(fileName);
	if (dict==NULL)
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
	
	for (unsigned int i=0; i < dict->numBlocks; i++)
	{
	
		mxArray *mxBlock = mxCreateStructMatrix((mwSize)1,(mwSize)1,0,{});
		map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
		map<string, string, mp_ltstring>::const_iterator iter;
		MP_Block_c *block = dict->block[i];
		paramMap = block->get_block_parameters_map();
	
	}
}
 */
void erereFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
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
  char*  convertEnd;
  map<string,string,mp_ltstring> *paramMap = new map<string, string, mp_ltstring>();
  map<string, string, mp_ltstring>::const_iterator iter;
  
  /* Load the MPTK environment if not loaded */
  if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");

  /* Input */
  tmpcharlen = mxGetN(prhs[0])+1;
  mxGetString(prhs[0],filename,tmpcharlen);

  /* Output */
  plhs[0] = mxCreateStructArray(2 , dims, 3,  field_names);

  tmp = mxCreateString(filename);
  mxSetField(plhs[0],0,"filename",tmp);
  
  /* Load the dict */
  dict = MP_Dict_c::init(filename);
  if (dict==NULL)
    {
      mp_error_msg(NULL,"Failed to create a dictionary from XML file [%s].\n", filename);
      mxAddField(plhs[0],"load_failed");
      return;
    }

  mxAddField(plhs[0],"load_successful");
  {
    mxArray *tmp = mxCreateDoubleMatrix(1,1,mxREAL);
    if (NULL==tmp) return;
    else
      {
        double  *val = (double *)mxCalloc(1,sizeof(double));
        if (NULL==val) return;
        else
          {
            *val = (double) dict->numBlocks;
            mxSetPr(tmp,val);
            mxSetField(plhs[0],0,"numBlocks",tmp);
          }
      }
  }

  mwSize dimBlockCell[2] = {dict->numBlocks,1};
  mxArray *theblocks=mxCreateCellArray(2,dimBlockCell);

  /* Iterate on all blocks */
  for (i=0; i<dict->numBlocks; i++)
    {
      /* For each blocks, converet the parameterMap in matlab structure */
      block = dict->block[i];
      paramMap = block->get_block_parameters_map();
      map<string,string,mp_ltstring> *paramMapType = new map<string, string, mp_ltstring>();
      MP_Block_Factory_c::get_block_factory()->get_block_type_map((*paramMap)["type"].c_str())(paramMapType);

      mxArray *theblock = mxCreateStructArray(2,dims,0,NULL);
      
      /* For each parameter of the paramMap */
      for ( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ )
        {
      /* Test type and convert*/
          if (!strcmp(((*paramMapType)[iter->first.c_str()]).c_str(),"string"))
            {
              tmp = mxCreateString(iter->second.c_str());
              mxAddField(theblock,iter->first.c_str());
              mxSetField(theblock,0,iter->first.c_str(),tmp);
            }
          else
            {
              mxArray *tmp = mxCreateDoubleMatrix(1,1,mxREAL);
              if (NULL==tmp)
                {
                  mp_error_msg( "convert", "tmp == NULL, cannot create double matrix.\n" );
                  return;
                }
              if (!strcmp(((*paramMapType)[iter->first.c_str()]).c_str(),"real"))
                {
                  double  *doubleVal = (double *)mxCalloc(1,sizeof(double));
                  if (NULL==doubleVal) return;
                  *doubleVal = strtod(iter->second.c_str(), &convertEnd);
                  if (*convertEnd != '\0')
                    {
                      mp_error_msg( "convert", "cannot convert parameter %s in double for value : %s.\n",iter->first.c_str(), iter->second.c_str() );
                      return;
                    }
                  mxSetPr(tmp,doubleVal);
                  mxAddField(theblock,iter->first.c_str());
                  mxSetField(theblock,0,iter->first.c_str(),tmp);
                }
              else if ((!strcmp(((*paramMapType)[iter->first.c_str()]).c_str(),"ulong"))||(!strcmp(((*paramMapType)[iter->first.c_str()]).c_str(),"uint")))
                {
                  double  *doubleVal = (double *)mxCalloc(1,sizeof(double));
                  if (NULL==doubleVal) return;
                  *doubleVal = strtol(iter->second.c_str(), &convertEnd, 10);
                  if (*convertEnd != '\0')
                    {
                      mp_error_msg( "convert", "cannot convert parameter %s in integer for value : %s.\n",iter->first.c_str(), iter->second.c_str()  );
                      return;
                    }
                  mxSetPr(tmp,doubleVal);
                  mxAddField(theblock,iter->first.c_str());
                  mxSetField(theblock,0,iter->first.c_str(),tmp);
                }
            }
        }
      mxSetCell(theblocks,i,theblock);
      delete (paramMapType);
    }
  mxSetField(plhs[0],0,"block",theblocks);
}



