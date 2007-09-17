#include "mex.h"
#include "mptk.h"


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



