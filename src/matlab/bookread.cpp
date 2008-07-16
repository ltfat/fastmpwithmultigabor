/******************************************************************************/
/*                                                                            */
/*                  	          bookread.c                       	      */
/*                                                                            */
/*				mptkMEX toolbox			      	      */
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
void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {

	/* Declarations */
	int tmpcharlen,n,m;
	char filename[1000];
	unsigned long int nAtomUser;
	int numFields = 6;
	const char *field_names[] = {"format", "numAtoms","numChans","numSamples","sampleRate","atom"};
	int numFields2 = 7;
	const char *field_names2[] = {"type","pos","len","amp","freq","phase","chirp"};
	mwSize dims[2] = {1, 1};
	mwSize dims2[2];
	MP_Book_c *book;
	mxArray *tmp, *type,*atom,*pos,*len,*amp,*freq,*phase,*chirp;
	unsigned long int nAtom;
	unsigned short int numChans;

	/* Input */
	tmpcharlen = mxGetN(prhs[0])+1;
	mxGetString(prhs[0],&filename[0],tmpcharlen);
	if (nrhs==2) {
		nAtomUser = (unsigned long int)mxGetScalar(prhs[1]);
	}
	
	/* Load the MPTK environment if not loaded */
    if (!MPTK_Env_c::get_env()->get_environment_loaded())MPTK_Env_c::get_env()->load_environment("");
   
	/* Output */
	plhs[0] = mxCreateStructArray(2 , dims, numFields,  field_names);
    /* Create new book */

	/* Create new book */
	book = MP_Book_c::create();

	/* Load the book */
	book->load(filename);
	
	/* Header */
	tmp = mxCreateString("0.1 (ravelli)");
	mxSetField(plhs[0], 0, "format", tmp);
	tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) book->numAtoms;
	mxSetField(plhs[0], 0, "numAtoms", tmp); 
	tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) book->numChans;
	mxSetField(plhs[0], 0, "numChans", tmp); 
	tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) book->numSamples;
	mxSetField(plhs[0], 0, "numSamples", tmp); 
	tmp = mxCreateDoubleMatrix(1, 1, mxREAL); *mxGetPr( tmp ) = (double) book->sampleRate;
	mxSetField(plhs[0], 0, "sampleRate", tmp);
	if (nrhs==2&&nAtomUser<book->numAtoms) {
		nAtom = nAtomUser;
	} else {
		nAtom = book->numAtoms;
	}
	numChans = book->numChans;

	/* Init */
	atom = mxCreateStructArray(2 , dims, 7,  field_names2);
	dims2[0] = nAtom; dims2[1] = 1;
	type = mxCreateCellArray(2, dims2 );
	pos = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);
	len = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);
	amp = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);
	freq = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);
	phase = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);
	chirp = mxCreateDoubleMatrix(nAtom, numChans, mxREAL);

	/* Atoms */
	for ( n=0 ; n<nAtom ; n++ ) {
		
		/* Type */
		tmp = mxCreateString(book->atom[n]->type_name());
		mxSetCell(type, n, tmp);

		/* Pos */
		for ( m=0 ; m<numChans ; m++ ) {
			*(mxGetPr( pos )+ m*nAtom + n) = book->atom[n]->support[m].pos;
		}
		
		/* Len */
		for ( m=0 ; m<numChans ; m++ ) {
			*(mxGetPr( len )+ m*nAtom + n) = book->atom[n]->support[m].len;
		}

		/* Amp */
		for ( m=0 ; m<numChans ; m++ ) {
			*(mxGetPr( amp )+ m*nAtom + n) = book->atom[n]->amp[m];
		}

		/* Freq */
		if ( book->atom[n]->has_field( MP_FREQ_PROP ) ) {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( freq )+ m*nAtom + n) = book->atom[n]->get_field( MP_FREQ_PROP, m);
			}
		} else {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( freq )+ m*nAtom + n) = mxGetNaN();
			}
		}
	
		/* Phase */
		if ( book->atom[n]->has_field( MP_PHASE_PROP ) ) {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( phase )+ m*nAtom + n) = book->atom[n]->get_field( MP_PHASE_PROP, m);
			}
		} else {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( phase )+ m*nAtom + n) = mxGetNaN();
			}
		}	
	
		/* Chirp */
		if ( book->atom[n]->has_field( MP_CHIRP_PROP ) ) {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( chirp )+ m*nAtom + n) = book->atom[n]->get_field( MP_CHIRP_PROP, m);
			}
		} else {
			for ( m=0 ; m<numChans ; m++ ) {
				*(mxGetPr( chirp )+ m*nAtom + n) = mxGetNaN();
			}
		}	
	}

	/* Set */
	mxSetField(atom, 0, "type", type);
	mxSetField(atom, 0, "pos", pos);
	mxSetField(atom, 0, "len",  len);
	mxSetField(atom, 0, "amp", amp);
	mxSetField(atom, 0, "freq", freq);
	mxSetField(atom, 0, "phase", phase);
	mxSetField(atom, 0, "chirp", chirp);
	mxSetField(plhs[0], 0, "atom", atom);
    
    book->info();
}
