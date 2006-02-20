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
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/********************************************/
/*                                          */
/* tfmap.cpp: methods for class MP_TF_Map_c */
/*                                          */
/********************************************/

#include "mptk.h"
#include "mp_system.h"


/***************/
/* Constructor */
MP_TF_Map_c::MP_TF_Map_c( const unsigned long int setNCols,  const unsigned long int setNRows,
			  const int setNChans, 
			  const unsigned long int setTMin,   const unsigned long int setTMax,
			  const MP_Real_t setFMin,           const MP_Real_t setFMax ) {

  const char* func = "MP_TF_Map_c::MP_TF_Map_c()";

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "new tfmap\n");

  assert (setTMin < setTMax);
  assert (setFMin < setFMax);
  assert (setFMin >= 0.0);
  assert (setFMax <= MP_PI);

  /* Initialize */
  numCols = numRows = 0;
  numChans = 0;
  storage = NULL;
  channel = NULL;
  tMin = tMax = 0;
  fMin = fMax = 0.0;
  dt = df = 0.0;
#define BIG_FLOAT 1e100
  ampMin = BIG_FLOAT;
  ampMax = 0.0;

  /* Try to allocate storage */
  if ( (storage = (MP_Tfmap_t*)calloc(setNChans*setNCols*setNRows,sizeof(MP_Tfmap_t))) == NULL ) {
    mp_warning_msg( func, "Can't allocate storage in tfmap with size [%d]x[%lu]x[%lu]. "
	     "Storage and columns are left NULL.\n", setNChans, setNCols, setNRows );
  }
  /* "Fold" the storage space into separate channels */
  else {
    if ( (channel = (MP_Tfmap_t**) malloc( setNChans*sizeof(MP_Tfmap_t*) ) ) == NULL) {
      mp_warning_msg( func, "Can't allocate [%d] channels in tfmap. "
	       "Storage and channels are left NULL.\n", setNChans);
      free( storage ); storage = NULL;
    }
    /* If everything went OK fold the storage space */
    else {
      int i;
      unsigned long int size = (setNRows*setNCols);
      for ( i = 0; i < setNChans; i++) channel[i] = storage + i*size;

      numCols = setNCols;
      numRows = setNRows;
      numChans = setNChans;

      tMin = setTMin;
      tMax = setTMax;

      fMin = setFMin;
      fMax = setFMax;

      dt   = (MP_Real_t)(setTMax-setTMin) / (MP_Real_t)(numCols);
      df   = (setFMax-setFMin) / (MP_Real_t)(numRows);

    }
  }
}


/**************/
/* Destructor */
MP_TF_Map_c::~MP_TF_Map_c() {

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_TF_Map_c::~MP_TF_Map_c()",
		"Deleting tfmap...\n");

  if (storage) free(storage);
  if (channel) free(channel);

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_TF_Map_c::~MP_TF_Map_c()",
		"Done.\n");
}


/**************************/
/* Reset the storage to 0 */
void MP_TF_Map_c::reset( void ) {
  unsigned long int i;
  for ( i = 0; i < (numChans*numCols*numRows); i++ ) storage[i] = 0;
  ampMin = BIG_FLOAT;
  ampMax = 0.0;
}


/***********************/
/* Human-readable info */
int MP_TF_Map_c::info( FILE *fid ) {
  int nChar = 0;
  nChar += mp_info_msg( fid, "TFMAP","Number of channels : %d\n", numChans );
  nChar += mp_info_msg( fid, "   |-","Number of columns  : %lu\n", numCols  );
  nChar += mp_info_msg( fid, "   |-","Number of rows     : %lu\n", numRows  );
  nChar += mp_info_msg( fid, "   |-","Time range         : [%lu %lu[ (dt = %g)\n", tMin, tMax, dt );
  nChar += mp_info_msg( fid, "   O-","freq range         : [%g %g[ (df = %g)\n", fMin, fMax, df );
  return(nChar);
}


/*******************************/
/* Write to a file as raw data */
unsigned long int MP_TF_Map_c::dump_to_file( const char *fName , char flagUpsideDown ) {

  const char* func = "MP_TF_Map_c::dump_to_file(..)";
  FILE *fid;
  unsigned long int nWrite = 0;
  MP_Tfmap_t *ptrColumn;
  unsigned long int i;
  MP_Tfmap_t column[numRows];
  MP_Tfmap_t *endStorage = storage + ( numChans*numRows*numCols );

  /* Open the file in write mode */
  if ( ( fid = fopen( fName, "w" ) ) == NULL ) {
    mp_error_msg( func, "Can't open file [%s] for writing a tfmap.\n", fName );
    return( 0 );
  }

  /* Write the values */
  if ( flagUpsideDown == 0 ) {
    nWrite = mp_fwrite( storage, sizeof(MP_Tfmap_t), numChans*numRows*numCols, fid );
  }
  /* If flagUpsideDown is set, rotate the picture */
  else {
    for( ptrColumn = storage; ptrColumn < endStorage; ptrColumn += numRows ) {
      for ( i = 0; i < numRows; i++ ) column[i] = *(ptrColumn+numRows-i-1);
      nWrite += mp_fwrite( column, sizeof(MP_Tfmap_t), numRows, fid );
    }
  }

  /* Clean the house */
  fclose(fid);

  return( nWrite );
}

/*************************************************************/
/* Convert between real coordinates and discrete coordinates */
/* Time: */
unsigned long int MP_TF_Map_c::time_to_pix( unsigned long int t ) {
  return( (unsigned long int)( floor( (double)(t-tMin) / (double)(dt) ) ) );
}
unsigned long int MP_TF_Map_c::pix_to_time( unsigned long int n ) {
  return( tMin + (unsigned long int)( floor( (double)(n) * dt ) ) );
}
/* Freq: */
unsigned long int MP_TF_Map_c::freq_to_pix( MP_Real_t f ) {
  return( (unsigned long int)( floor( (double)(f-fMin) / (double)(df) ) ) );
}
MP_Real_t MP_TF_Map_c::pix_to_freq( unsigned long int k ) {
  return( (MP_Real_t)( fMin + ((MP_Real_t)k)*df ) );
}
