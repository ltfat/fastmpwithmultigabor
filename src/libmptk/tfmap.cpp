/******************************************************************************/
/*                                                                            */
/*                                 tfmap.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
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
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 13:38:03 $
 * $Revision: 1.2 $
 *
 */

/********************************************/
/*                                          */
/* tfmap.cpp: methods for class MP_TF_Map_c */
/*                                          */
/********************************************/

#include "mptk.h"
#include "system.h"


/* Constructor */
MP_TF_Map_c::MP_TF_Map_c( int setNCols, int setNRows, unsigned char setNChans, 
			  MP_Real_t setTMin, MP_Real_t setFMin,
			  MP_Real_t setTMax, MP_Real_t setFMax ) {
#ifndef NDEBUG
  printf("new tfmap\n");
  assert (setTMin < setTMax);
  assert (setFMin < setFMax);
  assert (setFMin >= 0.0);
  assert (setFMax <= MP_PI);
#endif

  /* Try to allocate storage */
  if ( (storage = (MP_Real_t*) calloc(setNChans*setNCols*setNRows,sizeof(MP_Real_t))) == NULL ) {
    fprintf( stderr, "Warning: can't allocate storage in tfmap with size [%d]x[%u]x[%u]. "
	     "Storage and columns are left NULL.\n", (int)setNChans,setNCols, setNRows );
    numCols  = 0;
    numRows  = 0;
    numChans = 0;
    storage  = NULL;
    channel  = NULL;
  }
  /* "Fold" the storage space into separate channels */
  else {
    if ( (channel = (MP_Real_t**) malloc( numChans*sizeof(MP_Real_t*) ) )==NULL) {
      fprintf( stderr, "Warning: can't allocate [%d] channels in tfmap. "
	       "Storage and channels are left NULL.\n", (int)setNChans);
      numCols  = 0;
      numRows  = 0;
      numChans = 0;
      free( storage );
      storage = NULL;
      channel  = NULL;
    }
    /* If everything went OK fold the storage space */
    else {
      numCols = setNCols;
      numRows = setNRows;
      numChans = setNChans;
      unsigned int i;
      for (i=0; i < numChans; i++) channel[i] = storage + i*numRows*numCols;
      tMin = setTMin;
      fMin = setFMin;
      dt   = (setTMax-setTMin)/numCols;
      df = (setFMax-setFMin)/numRows;
    }
  }
}


/* Destructor */
MP_TF_Map_c::~MP_TF_Map_c() {
#ifndef NDEBUG
  printf("delete tfmap\n");
#endif
  if (storage) free(storage);
  if (channel) free(channel);
}

int MP_TF_Map_c::info( FILE *fid ) {
  int nChar = 0;
  nChar += fprintf(fid,"Number of channels : %d\n",(int)numChans);
  nChar += fprintf(fid,"Number of columns  : %d\n",(int)numCols);
  nChar += fprintf(fid,"Number of rows     : %d\n",(int)numRows);
  nChar += fprintf(fid,"Time range         : [%g %g[ (dt = %g)\n",tMin,tMin+numCols*dt,dt);
  nChar += fprintf(fid,"freq range         : [%g %g[ (df = %g)\n",fMin,fMin+numRows*df,df);
  return(nChar);
}

/**  Write to a file as raw data */
char MP_TF_Map_c::dump_to_float_file( const char *fName , char flagUpsideDown) {
  FILE *fid;
  float buffer[numCols*numRows];
  long int nWrite = 0;
  MP_Real_t *column;
  int i,j,k;
  unsigned char chanIdx;
  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL ) {
    fprintf( stderr, "Error: Can't open file [%s] for writing a tfmap.\n", fName );
    return(0);
  }

  /* for each channel */
  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {
    /* Cast the samples */
    if (flagUpsideDown==0) {
      for ( i=0; i< numRows*numCols; i++ ) { *(buffer+i) = (float)(*(channel[chanIdx]+i)); }
    } else {
      for ( i=0, k=0; i< numCols; i++ ) { 
	column = channel[chanIdx]+i*numRows;
	for (j = 0; j < numRows; j++, k++) {
	  *(buffer+k) = (float)(*(column+numRows-j-1));
	}
      }
    }

    /* Write to the file,
       and emit a warning if we can't write all the signal samples: */
    if ( (nWrite = fwrite( buffer, sizeof(float), numRows*numCols, fid ))
	 != numRows*numCols ) {
      fprintf( stderr, "Warning: Can't write more than [%ld] pixels to file [%s] "
	       "from tfmap with [%d]rows*[%d]columns=[%d] pixels.\n",
	       nWrite, fName, numRows, numCols, numRows*numCols );
    }
  }

  /* Clean the house */
  fclose(fid);
  return(nWrite);
}

void MP_TF_Map_c::pixel_coordinates(MP_Real_t t,MP_Real_t f, int *n, int *k) {
  *n = (int) round( (t-tMin)/dt );
  *k = (int) round( (f-fMin)/df );
}

void MP_TF_Map_c::tf_coordinates(int n, int k, MP_Real_t *t,MP_Real_t *f) {
  *t = tMin+n*dt;
  *f = fMin+k*df;
}

