/******************************************************************************/
/*                                                                            */
/*                  	      bookreadGil.c                                   */
/*                                                                            */
/*          				mptkMEX toolbox			      */
/*                                                                            */
/* Emmanuel Ravelli                                            	  May 22 2007 */
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

#include "mex.h"
#include "mptk.h"
#include "matrix.h"
#include "mxBook.h"

#include <map>
#include <vector>

/*
 *
 *     MAIN MEX FUNCTION
 *
 */
void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {
    
    /* Check input arguments */
    if (nrhs < 1) {
        mexPrintf("!!! %s error -- bad number of input arguments\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        return;
    }
    
    if ( !mxIsChar(prhs[0])) {
        mexPrintf("!!! %s error -- At least one argument has a wrong type\n",mexFunctionName());
        mexPrintf("    see help %s\n",mexFunctionName());
        return;        
    }
    
    /** Get Book filename */
    string bookName(mxArrayToString(prhs[0]));

   /* Get optionnal number of atom to read */
    unsigned long int nAtomUser; 
    if (nrhs==2) {
        nAtomUser = (unsigned long int)mxGetScalar(prhs[1]);
    }
    
    /* Load the MPTK environment if not loaded */
    if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
    
    /** Create empty book */
    MP_Book_c * mpBook;
    mpBook = MP_Book_c::create();
    
    /* Fill it loading book file */
    mpBook->load(bookName.c_str());
    
    mexPrintf("Book file: %s \n successfully loaded (%ld atoms)\n",bookName.c_str(),mpBook->numAtoms);
    
    /** Load book structure in object */
    mxBook * matBook;
    matBook = new mxBook(mpBook);
    
    plhs[0] = mxDuplicateArray(matBook->mexbook);
    
}
