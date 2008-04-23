#include "mex.h"
#include "mptk.h"
#include <iostream>
#include <sstream>


void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
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
