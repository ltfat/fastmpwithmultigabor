/******************************************************************************/
/*                                                                            */
/*                  	          mextest.cpp                       	      */
/*                                                                            */
/*				mptk4matlab toolbox		      	      */
/*                                                                            */
/* Remi Gribonval                                              	 July 17 2008 */
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
 * $Version 0.5.4$
 * $Date 21/02/2008$
 */

#include "mex.h"

#include <string.h>

#include "mptk.h"

void msgfunc(char *msge) {
  if(NULL==msge) 
  {
    mexPrintf("msgfunc : trying to print NULL msge!");
  }
  else
    {
	mexPrintf("%s",msge);
    }
}

void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {
  char * func = "mextest";
    /* Check input arguments */
    if (nrhs >0) {
        mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        return;
    }
    if (nlhs >0) {
        mexPrintf("!!! %s error -- bad number of output arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        return;
    }
    mexPrintf("This test MEX file is correctly launched\n");

    mexPrintf("Now registering the MPTK display functions\n");
	MPTK_Server_c::get_msg_server()->register_display_function("info_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("error_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("warning_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("progress_message_display", &msgfunc);
	MPTK_Server_c::get_msg_server()->register_display_function("debug_message_display", &msgfunc);
	mexPrintf("Done\n");
	mexPrintf("Testing the MPTK info_message_display functions\n");
	mp_info_msg(func,"test succesfull %d\n",0);
	mexPrintf("Testing the MPTK error_message_display functions\n");
	mp_error_msg(func,"test succesfull %d\n",1);
	mexPrintf("Testing the MPTK warning_message_display functions\n");
	mp_warning_msg(func,"test succesfull %d\n",2);
	mexPrintf("Testing the MPTK progress_message_display functions\n");
	mp_progress_msg(func,"test succesfull %d\n",3);
	mexPrintf("Testing the MPTK debug_message_display functions (should not display if compiled in release mode)\n");
	mp_debug_msg(func,"test succesfull %d\n",4);
	mexPrintf("Now checking if the MPTK environment is loaded\n");
    /* Load the MPTK environment if not loaded */
    if (!MPTK_Env_c::get_env()->get_environment_loaded())
      {
	mexPrintf("the environment was not loaded, now loading it\n");
	if (MPTK_Env_c::get_env()->load_environment("")==false) {
	mexPrintf("%s error -- could not load the MPTK environment.\n",mexFunctionName());
	mexPrintf("The most common reason is a missing or erroneous MPTK_CONFIG_FILENAME variable.\n");
	mexPrintf("This environment variable can be set by typing\n");
	mexPrintf("     'setenv('MPTK_CONFIG_FILENAME','<path to configuration file.xml>')");
	mexPrintf(" from the Matlab command line\n");
	mexErrMsgTxt("Aborting");
	}
	else {
	  mexPrintf("the environment is now loaded\n");
	}
      }
    else {
      mexPrintf("the environment was already loaded\n");
    }
    mexPrintf("End of the test, bye!\n");
    return;
}
