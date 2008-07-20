/******************************************************************************/
/*                                                                            */
/*                  	      mptk_getinfo_exp.cpp                            */
/*                                                                            */
/*          				mptkMEX toolbox									  */
/*                                                                            */
/* Remi Gribonval                                           	 July 13 2008 */
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
/*
 * $Version 0.5.3$
 * $Date 05/22/2007$
 */

//#include "mex.h"
//#include "mptk.h"
#include "mptk4matlab.h"
#include "matrix.h"
#include <map>
#include <vector>

void fillWindowNameVector(vector <string>*windowNameVector,vector<bool>*windowNeedsOptionVector)
{
	for(int type=0; type<DSP_NUM_WINDOWS; type++)
	{
		bool needs_option;
		char *name = NULL;
		if (((unsigned char)0)!=window_type_is_ok(type))
		{
			needs_option = window_needs_option(type);
			name = window_name(type);
			windowNameVector->push_back(string(name));
			windowNeedsOptionVector->push_back(needs_option);
		}
	}
}


/*
 *
 *     MAIN MEX FUNCTION
 *
 */
void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) {
    
    mxArray *tmp = NULL;
	
    InitMPTK4Matlab(mexFunctionName());

    /* Check input arguments */
    if (nrhs > 0) {
        mexPrintf("%s error -- bad number of input arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        mexErrMsgTxt("Aborting");
		return;
    }
    
    // Gets the list of all registered blocks
    vector< string >* atomNameVector  = new vector< string >();
    vector< string >* blockNameVector = new vector< string >();
    vector< string>* windowNameVector = new vector< string>();
    vector< bool >* windowNeedsOptionVector = new vector< bool >();
    vector< string >* pathNameVector  = new vector< string >();
    // DEBUG
//    mexPrintf("Loading the list of plugins: ...\n"); 
    MP_Atom_Factory_c::get_atom_factory()->get_registered_atom_name( atomNameVector );
    MP_Block_Factory_c::get_block_factory()->get_registered_block_name( blockNameVector );
    fillWindowNameVector(windowNameVector,windowNeedsOptionVector);
    // DEBUG
//    mexPrintf("Loading the path names: ...\n"); 
    MPTK_Env_c::get_env()->get_registered_path_name( pathNameVector );
    // DEBUG
//    mexPrintf(" done\n");

    // ATOMS
    //
    // Create the atoms structure
    // atoms(numAtomTypes).type
    int numAtomFieldNames  = 1;
    const char *atomFieldNames[] = {"type"};
    mxArray *mexAtomInfo = mxCreateStructMatrix((mwSize)atomNameVector->size(),(mwSize)1,
						numAtomFieldNames,atomFieldNames);
    // Gets the information for all atoms
    for (unsigned int i= 0; i < atomNameVector->size(); i++)
      {
	// Gets the name of the atom  
	tmp = mxCreateString(atomNameVector->at(i).c_str());
	mxSetField(mexAtomInfo,(mwIndex)i,"type",tmp);
      }

    // WINDOWS
    //
    // Create the windows structure
    // windows(numWindows).type
    // windows(numWindows).needsOption
    int numWindowsFieldNames  = 2;
    const char *windowsFieldNames[] = {"type","needsOption"};
    mxArray *mexWindowsInfo = mxCreateStructMatrix((mwSize)windowNameVector->size(),(mwSize)1,
						   numWindowsFieldNames,windowsFieldNames);
    // Gets the information for all windows
    for (unsigned int i= 0; i < windowNameVector->size(); i++)
      {
	// Gets the name of the window
	tmp = mxCreateString(windowNameVector->at(i).c_str());
	mxSetField(mexWindowsInfo,(mwIndex)i,"type",tmp);
	tmp = mxCreateDoubleScalar((double)(windowNeedsOptionVector->at(i)));
	mxSetField(mexWindowsInfo,(mwIndex)i,"needsOption",tmp);
      }
		
    // BLOCKS
    //
    // Create the blocks structure
    // blocks(numBlockTypes).type
    // blocks(numBlockTypes).parameters
    int numBlockFieldNames  = 2;
    const char *blockFieldNames[] = {"type","parameters"};
    mxArray *mexBlockInfo = mxCreateStructMatrix((mwSize)blockNameVector->size(),(mwSize)1,
						numBlockFieldNames,blockFieldNames);

    // Prepare for storing the block parameters information
    int numBlockInfoFieldNames         = 4;
    const char *blockInfoFieldNames[]  = {"name","type","info","default"};
    mxArray *paramName     = NULL;
    mxArray *paramInfo     = NULL;
    mxArray *paramType     = NULL;
    mxArray *paramDefault  = NULL;
	
    // Gets the information for all blocks
    for (unsigned int i= 0; i < blockNameVector->size(); i++)
      {
	// Gets the map with default / types / info on parameters 
	map<string, string, mp_ltstring>* defaultMap  = new map<string, string, mp_ltstring>();
	map<string, string, mp_ltstring>* typeMap     = new map<string, string, mp_ltstring>();
	map<string, string, mp_ltstring>* infoMap     = new map<string, string, mp_ltstring>();
	MP_Block_Factory_c::get_block_factory()->get_block_default_map(blockNameVector->at(i).c_str())(defaultMap);
	MP_Block_Factory_c::get_block_factory()->get_block_type_map(   blockNameVector->at(i).c_str())(typeMap);
	MP_Block_Factory_c::get_block_factory()->get_block_info_map(   blockNameVector->at(i).c_str())(infoMap);
	
	// Gets the name of the block  
	tmp = mxCreateString(blockNameVector->at(i).c_str());
	mxSetField(mexBlockInfo,(mwIndex)i,"type",tmp);
	// Debug
	//mexPrintf("%s\t",blockNameVector->at(i).c_str());
	// Creates the struct array to store the parameters and their information
        mxArray *mexParameters = mxCreateStructMatrix((mwSize)infoMap->size(),(mwSize)1,
							numBlockInfoFieldNames,blockInfoFieldNames);

	// Create a new entry for each parameter name
	map< string, string, mp_ltstring>::iterator iter,tmpiter;   
	int j = 0;
	for(iter = (*infoMap).begin(); iter != (*infoMap).end(); iter++,j++) 
	  {
	    // Debug
	    //mexPrintf("name    : %s\n",iter->first.c_str());
	    //mexPrintf("info    : %s\n",iter->second.c_str());
	    //mexPrintf("type    : %s\n",typeMap->find(iter->first)->second.c_str());
	    //mexPrintf("default : %s\n",defaultMap->find(iter->first)->second.c_str());
	    // Fill type, information, and default value
	    paramName = mxCreateString(iter->first.c_str());
	    mxSetField(mexParameters,j,"name",paramName);
	    paramInfo = mxCreateString(iter->second.c_str());
	    mxSetField(mexParameters,j,"info",paramInfo);
	    paramType = mxCreateString(typeMap->find(iter->first)->second.c_str());
	    mxSetField(mexParameters,j,"type",paramType);
	    paramDefault = mxCreateString(defaultMap->find(iter->first)->second.c_str());	
	    mxSetField(mexParameters,j,"default",paramDefault);
	  }
	// Delete maps
	if (defaultMap) delete(defaultMap);
	if (typeMap) delete(typeMap);
	if (infoMap) delete(infoMap);	
	// Store parameter information for the current block
	mxSetField(mexBlockInfo,(mwIndex)i,"parameters",mexParameters);	
      }


    // PATH information from config file :
    mxArray *mexPathInfo = mxCreateStructMatrix((mwSize)1,(mwSize)1,0,NULL);
    
    // Add path.name according to content of config_path
    for (unsigned int i= 0; i < pathNameVector->size(); i++)
      {
		// Gets the name of the path  
		const char *pathName = pathNameVector->at(i).c_str();
		// DEBUG
//		mexPrintf("Found path %s with value %s\n",pathName,MPTK_Env_c::get_env()->get_config_path(pathName));
		if (NULL!=MPTK_Env_c::get_env()->get_config_path(pathName)) {
			mxAddField(mexPathInfo,pathName);
			tmp = mxCreateString(MPTK_Env_c::get_env()->get_config_path(pathName));
			mxSetField(mexPathInfo,(mwIndex)0,pathName,tmp);
		}
	}
    
    // Create the output information structure 
    // info.atoms   = atoms(numAtomTypes)
    // info.blocks  = blocks(numBlockTypes)
    // info.windows = windows(numWindowsTypes)
    // info.path    = path
    int numInfoFieldNames    = 4;
    const char *infoFieldNames[] = {"atoms","blocks","windows","path"};
    mxArray *mexInfo = mxCreateStructMatrix((mwSize)1,(mwSize)1,numInfoFieldNames,infoFieldNames);
    // Store information for atoms, blocks and windows in output variable
    mxSetField(mexInfo,(mwIndex)0,"atoms",mexAtomInfo);
    mxSetField(mexInfo,(mwIndex)0,"blocks",mexBlockInfo);
    mxSetField(mexInfo,(mwIndex)0,"windows",mexWindowsInfo);
    mxSetField(mexInfo,(mwIndex)0,"path",mexPathInfo);

    
    // mexPrintf("Done\n");
    
    plhs[0] = mexInfo;
    
}
