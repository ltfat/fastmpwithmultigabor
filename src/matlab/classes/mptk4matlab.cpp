/******************************************************************************/
/*                                                                            */
/*                  	      mptk4matlab.cpp                                 */
/*                                                                            */
/*          				mptk4matlab toolbox                   */
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

#include "mptk4matlab.h"

/* Define a function which displays error messages within Matlab */
void msgfunc(char *msge) {
	mexPrintf("%s",msge);
}

void FoundMPTK4Matlab(void)
{
  mexPrintf("Found MPTK4Matlab\n");
}

/* The function to be called at the beginning of each mex file */
void InitMPTK4Matlab(const char *functionName) {
  /* Register the display function */
  /* We could set a flag variable to do it only once */
    MPTK_Server_c::get_msg_server()->register_display_function("info_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("error_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("warning_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("progress_message_display", &msgfunc);
    MPTK_Server_c::get_msg_server()->register_display_function("debug_message_display", &msgfunc);
	
    /* Load the MPTK environment if not loaded */
    if (!MPTK_Env_c::get_env()->get_environment_loaded()) {
      if (MPTK_Env_c::get_env()->load_environment("")==false) {
	mexPrintf("%s error -- could not load the MPTK environment.\n",functionName);
	mexPrintf("The most common reason is a missing or erroneous MPTK_CONFIG_FILENAME variable.\n");
	mexPrintf("This environment variable can be set by typing\n");
	mexPrintf("     'setenv('MPTK_CONFIG_FILENAME','<path to configuration file.xml>')");
	mexPrintf(" from the Matlab command line\n");
	mexErrMsgTxt("Aborting");
      }
    }
}
