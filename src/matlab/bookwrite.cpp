/******************************************************************************/
/*                                                                            */
/*                  	          bookwrite.cpp                      	      */
/*                                                                            */
/*				mptk4matlab toolbox		      	      */
/*                                                                            */
/* Gilles Gonon                                                	  Feb 21 2008 */
/* Remi Gribonval                                              	  July   2008 */
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

#include "mptk4matlab.h"
#include "mxBook.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) 
{
  char *fileName = NULL;
  
  InitMPTK4Matlab(mexFunctionName());
    
  // Check input arguments
  if (nrhs < 2 ) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  if ( !mxIsStruct(prhs[0]) || !mxIsChar(prhs[1])) {
    mexPrintf("!!! %s error -- At least one argument has a wrong type\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
    
  // Load book structure in object 
  mxBook mybook(prhs[0]);
    
  // Get Book filename
  string bookName(mxArrayToString(prhs[1]));
    
  // Check write mode */
  char writeMode = MP_TEXT;  // Change to MP_BINARY after debug
  if (nrhs==3) {
    writeMode = (char) mxGetScalar(prhs[2]);
  }
    
  mybook.MP_BookWrite(bookName, writeMode);
    
  return;
}
