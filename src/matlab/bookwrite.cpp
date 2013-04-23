/******************************************************************************/
/*                                                                            */
/*                  	          bookwrite.cpp                      	      */
/*                                                                            */
/*								mptk4matlab toolbox							  */
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
/*							MPTK - Matlab interface							  */
/* Exports a binary Matching Pursuit book from Matlab, using MPTK			  */
/*																			  */
/* Usage : bookwrite(book,filename[,writeMode])								  */
/*																			  */
/* Input :																	  */
/*  - book     : a book structure with the following structure				  */
/*  - dict     : a dict structure                                             */
/*  - filename : the filename where to read the book						  */
/*  - writeMode: optional, 'binary' (default) or 'txt'						  */
/*																			  */
/* Known limitations : only the following atom types are supported:			  */
/*    gabor, harmonic, mdct, mclt, dirac.									  */
/******************************************************************************/

#include "mptk4matlab.h"
#include "mxBook.h"

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) 
{
  char *fileName = NULL;
  char *writeModeName = NULL;
  InitMPTK4Matlab(mexFunctionName());
    
  // Check input arguments
  if (nrhs < 3 || nrhs>4) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  if ( !mxIsStruct(prhs[0])) {
    mexPrintf("!!! %s error -- first argument (book) should be a struct\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;        
  }
  if ( !mxIsStruct(prhs[1])) {
    mexPrintf("!!! %s error -- second argument (dict) should be a struct\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    return;
  }
  if(nrhs>=3) {
	if(!mxIsChar(prhs[2])) {
		mexPrintf("!!! %s error -- third argument (fileName) should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		return;        
	} else {
		fileName = mxArrayToString(prhs[2]);
		if (NULL==fileName) {
			mexPrintf("%s error -- third argument (fileName) could not be retrieved from the input\n",mexFunctionName());
			mexErrMsgTxt("Aborting");
			return;
		}
	}
  }
  if(nrhs>=4) {
	if(!mxIsChar(prhs[3])) {
		mexPrintf("!!! %s error -- optional fourth argument (writeMode) should be a string\n",mexFunctionName());
		mexPrintf("    see help %s\n",mexFunctionName());
		// Clean the house
		mxFree(fileName);
		return;        
	} else {
		writeModeName = mxArrayToString(prhs[3]);
		if (NULL==writeModeName) {
			mexPrintf("%s error -- the optional fourth argument writeMode could not be retrieved from the input\n",mexFunctionName());
			mexErrMsgTxt("Aborting");
			// Clean the house
			mxFree(fileName);
			return;
		}
	}
  }
  // Check output arguments
  if (nlhs>0) {
    mexPrintf("%s error -- wrong number of output arguments\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    // Clean the house
    mxFree(fileName);
    if(NULL!=writeModeName) mxFree(writeModeName);
    return;
  }

  // Check write mode
  char writeMode = MP_BINARY;
  if(NULL!=writeModeName) {
    if(!strcmp(writeModeName,"binary"))
      writeMode = MP_BINARY;
    else if(!strcmp(writeModeName,"txt"))
      writeMode = MP_TEXT;
    else {
      mexPrintf("%s error -- writeMode can be either 'binary' or 'txt'\n",mexFunctionName());
      mexErrMsgTxt("Aborting");
      // Clean the house
      mxFree(fileName);
      mxFree(writeModeName);
      return;
    }
  }

  // Converting dictionary
  const mxArray* mxDict = prhs[1];
  MP_Dict_c *dict = mp_create_dict_from_mxDict(mxDict);
  if(NULL==dict) {
    mexPrintf("Failed to convert a dict from Matlab to MPTK.\n");
    mexErrMsgTxt("Aborting");
    // Clean the house
    mxFree(fileName);
    mxFree(writeModeName);
    return;
  }

  // Load book object from Matlab structure
  const mxArray *mexBook = prhs[0];
  MP_Book_c *book = mp_create_book_from_mxBook(mexBook, dict);
  if(NULL==book) {
    mexPrintf("Failed to convert a book from Matlab to MPTK.\n");
    mexErrMsgTxt("Aborting");
    // Clean the house
    mxFree(fileName);
    mxFree(writeModeName);
    delete dict;
    return;
  }
  
  book->print(fileName,writeMode);
  return;
}

