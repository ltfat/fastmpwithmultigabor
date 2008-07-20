/******************************************************************************/
/*                                                                            */
/*                  	      bookread_exp.cpp                                */
/*                                                                            */
/*				mptk4matlab toolbox		      	      */
/*                                                                            */
/* Emmanuel Ravelli                                            	  May 22 2007 */
/* Gilles Gonon                                               	  Feb 20 2008 */
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
 * $Version 0.5.3$
 * $Date 05/22/2007$
 */

#include "mptk4matlab.h"
#include "matrix.h"
#include "mxBook.h"

#include <map>
#include <vector>

/*
 *
 *     MAIN MEX FUNCTION
 *
 */
void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[]) {
    
  // Load the MPTK environment if not loaded 
  InitMPTK4Matlab(mexFunctionName());
  
  /* Check input arguments */
  if (1!=nrhs) {
    mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  
  if (!mxIsChar(prhs[0])) {
    mexPrintf("!!! %s error -- The filename argument should be a string\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;        
  }

  // Check output arguments
  if (1<nlhs) {
    mexPrintf("%s error -- bad number of output arguments\n",mexFunctionName());
    mexPrintf("    see help %s\n",mexFunctionName());
    mexErrMsgTxt("Aborting");
    return;
  }
  
  // Get the book filename
  char *fileName = mxArrayToString(prhs[0]);
  if (NULL==fileName) {
    mexErrMsgTxt("The book file name could not be retrieved from the input. Aborting.");
    return;
  }

  // Try to load the book
  MP_Book_c * book;
  book = MP_Book_c::create();
  if (NULL==book) {
    mexPrintf("Failed to create a book.\n");
    // Clean the house 
    mxFree(fileName);
    mexErrMsgTxt("Aborting");
    return;
  }
  if(0==book->load(fileName)) {
    mexPrintf("Failed to load atoms from file [%s].\n",fileName);
    // Clean the house 
    mxFree(fileName);
    mexErrMsgTxt("Aborting");
    return;
  }
  // DEBUG
  mexPrintf("Book file: %s \n successfully loaded (%ld atoms)\n",fileName,book->numAtoms);
  // Clean the house
  mxFree(fileName);


  // Load book object in Matlab structure
  mxBook * mexBook = new mxBook(book); // It used to crashes here!!!!
  
  // 
  plhs[0] = mxDuplicateArray(mexBook->mexbook);

}


/*
  // Load book object in Matlab structure
  mxArray *mxBook = mp_create_mxBook_from_book(book);
  if(NULL==mxBook) {
    mexPrintf("Failed to convert a book from MPTK to Matlab.\n");
    mexErrMsgTxt("Aborting");
    return;
  }
  plhs[0] = mxBook;
}

*/
