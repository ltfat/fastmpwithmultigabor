/******************************************************************************/
/*                                                                            */
/*                  	          bookwrite.c                       	      */
/*                                                                            */
/*				mptkMEX toolbox			      	      */
/*                                                                            */
/* Emmanuel Ravelli                                            	   Jan 1 2007 */
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
 * $Version 0.5.2$
 * $Date 01/01/2007$
 */

#include "mex.h" 
#include <string.h> 

void mexFunction(int nlhs, mxArray *plhs[],
int nrhs, const mxArray *prhs[]) {

    /* Declarations */
    mxArray *tmp,*atoms,*atom;
    char *tmpchar,*VERSION,*type;
    double tmpdouble;
    double *tmparray,*tmparray2;
    int n,i,tmpcharlen,sampleRate;
	unsigned short int numChans;
    unsigned int numPartials;
    unsigned long int nAtom,numSamples,tmplint,tmplint2;
	FILE *fid;
    
    /* Open the file */
    tmpcharlen = mxGetN(prhs[1])+1;
    tmpchar = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
    mxGetString(prhs[1],tmpchar,tmpcharlen);
     if ( (fid = fopen(tmpchar , "w" ) ) == NULL ) {
        mexErrMsgTxt( "Can't open file");
    }
    mxFree(tmpchar);
    
    /* Header */
    tmp = mxGetField(prhs[0],0,"numAtoms");
    nAtom = (unsigned long int)mxGetScalar(tmp);
    tmp = mxGetField(prhs[0],0,"numChans");
    numChans = (unsigned int)mxGetScalar(tmp);
    tmp = mxGetField(prhs[0],0,"numSamples");
    numSamples = (unsigned long int)mxGetScalar(tmp);
    tmp = mxGetField(prhs[0],0,"sampleRate");
    sampleRate = (unsigned int)mxGetScalar(tmp);
    tmp = mxGetField(prhs[0],0,"libVersion");
    tmpcharlen = mxGetN(tmp)+1;
    VERSION = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
    mxGetString(tmp,VERSION,tmpcharlen);
    fprintf( fid, "bin\n" );
    fprintf( fid, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
	   " sampleRate=\"%d\" libVersion=\"%s\">\n",
	   nAtom, numChans, numSamples, sampleRate, VERSION );
    mxFree(VERSION);
    
    /* Atoms */
    atoms = mxGetField(prhs[0],0,"atom");
    for ( n = 0; n < nAtom; n++ ) {
        atom = mxGetCell(atoms,n);
        
        /* Atom type */
        tmp = mxGetField(atom,0,"type");
        tmpcharlen = mxGetN(tmp)+1;
        type = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
        mxGetString(tmp,type,tmpcharlen);
        fprintf( fid, "%s\n", type );
        
        /* numChans */
        fwrite( &numChans, sizeof(unsigned short int), 1, fid );
        
        /* Support */
        tmp = mxGetField(atom,0,"pos");
        tmparray = mxGetPr(tmp);
        tmp = mxGetField(atom,0,"len");
        tmparray2 = mxGetPr(tmp);
        for ( i=0; i<numChans; i++ ) {
          tmplint = (unsigned long int)(*(tmparray+i));
          fwrite( &tmplint, sizeof(unsigned long int), 1, fid );
          tmplint2 = (unsigned long int)(*(tmparray2+i));            
          fwrite( &tmplint2, sizeof(unsigned long int), 1, fid );
        }
        
        /* Amp */
        tmp = mxGetField(atom,0,"amp");
        tmparray = mxGetPr(tmp);
        fwrite( tmparray,   sizeof(double), numChans, fid );
        
        /* - Gabor atom: */
        if ( !strcmp(type,"gabor") ) {
            
            /* Window name */
            tmp = mxGetField(atom,0,"windowType");
            tmpcharlen = mxGetN(tmp)+1;
            tmpchar = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
            mxGetString(tmp,tmpchar,tmpcharlen);
            fprintf( fid, "%s\n", tmpchar );
            mxFree(tmpchar);
            /* Window option */
            tmp = mxGetField(atom,0,"windowOpt");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            /* Binary parameters */
            tmp = mxGetField(atom,0,"freq");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            tmp = mxGetField(atom,0,"chirp");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble, sizeof(double), 1, fid );
            tmp = mxGetField(atom,0,"phase");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray,   sizeof(double), numChans, fid );
            
        } 
        /* - Harmonic atom: */
        else if ( !strcmp(type,"harmonic") ) {
            
            /* Window name */
            tmp = mxGetField(atom,0,"windowType");
            tmpcharlen = mxGetN(tmp)+1;
            tmpchar = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
            mxGetString(tmp,tmpchar,tmpcharlen);
            fprintf( fid, "%s\n", tmpchar );
            mxFree(tmpchar);
            /* Window option */
            tmp = mxGetField(atom,0,"windowOpt");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            /* Binary parameters */
            tmp = mxGetField(atom,0,"freq");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            tmp = mxGetField(atom,0,"chirp");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble, sizeof(double), 1, fid );
            tmp = mxGetField(atom,0,"phase");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray,   sizeof(double), numChans, fid );
            /* Number of partials */
            tmp = mxGetField(atom,0,"numPartials");
            numPartials = (unsigned int) mxGetScalar(tmp);
            fwrite( &numPartials,  sizeof(unsigned int), 1, fid );
            /* Binary parameters */
            tmp = mxGetField(atom,0,"harmonicity");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray,   sizeof(double), numPartials, fid );
            tmp = mxGetField(atom,0,"partialAmpStorage");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray,   sizeof(double), numChans*numPartials, fid );
            tmp = mxGetField(atom,0,"partialPhaseStorage");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray, sizeof(double), numChans*numPartials, fid );
            
        }
        /* - Mdct atom : */
        else if ( !strcmp(type,"mdct") ) {
            
            /* Window name */
            tmp = mxGetField(atom,0,"windowType");
            tmpcharlen = mxGetN(tmp)+1;
            tmpchar = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
            mxGetString(tmp,tmpchar,tmpcharlen);
            fprintf( fid, "%s\n", tmpchar );
            mxFree(tmpchar);
            /* Window option */
            tmp = mxGetField(atom,0,"windowOpt");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            /* Binary parameters */
            tmp = mxGetField(atom,0,"freq");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            
        } 
        /* - Mclt atom: */
        else if ( !strcmp(type,"mclt") ) {
            
            /* Window name */
            tmp = mxGetField(atom,0,"windowType");
            tmpcharlen = mxGetN(tmp)+1;
            tmpchar = (char*)mxCalloc(tmpcharlen, sizeof(char)); 
            mxGetString(tmp,tmpchar,tmpcharlen);
            fprintf( fid, "%s\n", tmpchar );
            mxFree(tmpchar);
            /* Window option */
            tmp = mxGetField(atom,0,"windowOpt");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            /* Binary parameters */
            tmp = mxGetField(atom,0,"freq");
            tmpdouble = mxGetScalar(tmp);
            fwrite( &tmpdouble,  sizeof(double), 1, fid );
            tmp = mxGetField(atom,0,"phase");
            tmparray = mxGetPr(tmp);
            fwrite( tmparray,   sizeof(double), numChans, fid );
        }
        /* - Dirac atom: */
        else if ( !strcmp(type,"dirac") ) {
        }
        /* - Unknown atom type: */
        else { 
            mexErrMsgTxt("Cannot write atom type");
        }
    }

    /* print the closing </book> tag */
    fprintf( fid, "</book>\n"); 
    
    /* Close the file */
    fclose( fid );
    
    /* Free */
    mxFree(type);
    
}
