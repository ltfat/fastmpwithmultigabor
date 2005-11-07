/******************************************************************************/
/*                                                                            */
/*                              dirac_atom.cpp                             */
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

/*******************************************/
/*                                         */
/* dirac_atom.cpp: methods for dirac atoms */
/*                                         */
/*******************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Dirac_Atom_c::MP_Dirac_Atom_c( void )
  :MP_Atom_c() {

  amp = NULL;
}

/************************/
/* Specific constructor */
MP_Dirac_Atom_c::MP_Dirac_Atom_c( unsigned int setNChan )
  :MP_Atom_c( setNChan ) {

  if ( (amp = (MP_Real_t*)calloc( numChans, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Dirac_Atom_c() - Can't allocate the amp array for a new atom; amp stays NULL.\n" );
  } 
}

/********************/
/* File constructor */
MP_Dirac_Atom_c::MP_Dirac_Atom_c( FILE *fid, const char mode )
  :MP_Atom_c( fid, mode ) {
  
  char str[MP_MAX_STR_LEN];
  double fidAmp;
  int i, iRead;
  
  /* Allocate and initialize the amplitudes */
  if ( (amp = (MP_Real_t*)calloc( numChans, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Dirac_Atom_c(file) - Can't allocate the amp array for a new atom; amp stays NULL.\n" );
  } 
  
  switch ( mode ) {
    
  case MP_TEXT:
    for (i = 0; i<numChans; i++) {
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<par type=\"amp\" chan=\"%d\">%lg</par>\n", &iRead,&fidAmp ) != 2 ) ) {
	fprintf(stderr, "mplib warning -- MP_Dirac_Atom_c(file) - Cannot scan amp on channel %d.\n", i );
      } else if ( iRead != i ) {
 	fprintf(stderr, "mplib warning -- MP_Dirac_Atom_c(file) - Potential shuffle in the parameters"
		" of a dirac atom. (Index \"%d\" read, \"%d\" expected.)\n",
		iRead, i );
      } else {
	*(amp+i) = (MP_Real_t)fidAmp;
      }
    }
    break;
      
  case MP_BINARY:
    if ( mp_fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      fprintf(stderr, "mplib warning -- MP_Dirac_Atom_c(file) - Failed to read the amp array.\n" );     
      for ( i=0; i<numChans; i++ ) *(amp+i) = 0.0;
    }
    
    break;
    
  default:
    fprintf( stderr, "mplib error -- MP_Dirac_Atom_c(file,mode) -"
	     " Unknown read mode met in MP_Dirac_Atom_c( fid , mode )." );
    break;
  }
  
}


/**************/
/* Destructor */
MP_Dirac_Atom_c::~MP_Dirac_Atom_c() {

  if (amp) free(amp);

}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Dirac_Atom_c::write( FILE *fid, const char mode ) {
  
  int i, nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other dirac-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    for (i = 0; i<numChans; i++) {
      nItem += fprintf(fid, "\t\t<par type=\"amp\" chan=\"%d\">%lg</par>\n", i, (double)amp[i]);
    }
    break;

  case MP_BINARY:
    nItem += mp_fwrite( amp,   sizeof(MP_Real_t), numChans, fid );
    break;

  default:
    break;
  }

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/*************/
/* Type name */
char * MP_Dirac_Atom_c::type_name(void) {
  return ("dirac");
}

/**********************/
/* Readable text dump */
int MP_Dirac_Atom_c::info( FILE *fid ) {

  int i, nChar = 0;

  nChar += fprintf( fid, "mplib info -- DIRAC ATOM:");
  nChar += fprintf( fid, " [%d] channel(s)\n", numChans );
  for ( i=0; i<numChans; i++ ) {
    nChar += fprintf( fid, "mplib info -- (%d/%d)\tSupport=", i+1, numChans );
    nChar += fprintf( fid, " %lu %lu ", support[i].pos, support[i].len );
    nChar += fprintf( fid, "\tAmp %g",(double)amp[i]);
    nChar += fprintf( fid, "\n" );
  }

  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Dirac_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  int chanIdx;
  for (chanIdx = 0 ; chanIdx < numChans; chanIdx++ ) 
    outBuffer[chanIdx] = amp[chanIdx];
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
char MP_Dirac_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap ) {

  unsigned char chanIdx;
  MP_Real_t tMin,tMax,fMin,fMax;
  int nMin,kMin,nMax,kMax;
  int iMin,jMin,iMax,jMax;
  int i,j;
  MP_Real_t t,f;
  MP_Real_t *column;
  char flag = 0;
  MP_Real_t value;

#ifndef NDEBUG
  assert(numChans==tfmap->numChans);
#endif

  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {
#ifndef NDEBUG
    fprintf(stderr,"Displaying channel [%u] to tfmap\n",chanIdx);
#endif
    /* 1/ Determine the support (extremities included) of the atom in standard coordinates */
    tMin = support[chanIdx].pos;
    tMax = (MP_Real_t)(support[chanIdx].pos+support[chanIdx].len)-1;
    fMin = 0.0;
    fMax = 0.5;
#ifndef NDEBUG
    fprintf(stderr,"Atom 'rectangle' in tf  coordinates [%g %g]x[%g %g]\n",tMin,tMax,fMin,fMax);
#endif
    /* 2/ Convert this in pixel coordinates */
    tfmap->pixel_coordinates(tMin,fMin,&nMin,&kMin);
    tfmap->pixel_coordinates(tMax,fMax,&nMax,&kMax);
#ifndef NDEBUG
    fprintf(stderr,"Atom rectangle in pix coordinates [%d %d]x[%d %d]\n",nMin,nMax,kMin,kMax);
#endif

    /* 3/ Detect the cases where there is nothing to display */
    if (nMin >= tfmap->numCols || nMax < 0 || kMin >= tfmap->numRows || kMax < 0) {
#ifndef NDEBUG
      fprintf(stderr,"Atom rectangle does not intersect tfmap\n");
#endif
      continue;
    }
    else {
      flag = 1;
    }
    /* 4/ Compute the rectangle (lower extremity included, upper one excluded) of the pixel map that needs to be filled in */
    if (nMin < 0) iMin = 0;
    else          iMin = nMin;
    if (nMax >= tfmap->numCols) iMax = tfmap->numCols;
    else                        iMax = nMax+1;
    if (kMin < 0) jMin = 0;
    else          jMin = kMin;
    if (kMax >= tfmap->numRows) jMax = tfmap->numRows;
    else                        jMax = kMax+1;
#ifndef NDEBUG
    fprintf(stderr,"Filled rectangle in pix coordinates [%d %d[x[%d %d[\n",iMin,iMax,jMin,jMax);
#endif
		    
    /* 4/ Fill the TF map */
    for (i = iMin; i < iMax; i++) { /* loop on columns */
      column = 	tfmap->channel[chanIdx] + i*tfmap->numRows; /* seek the column data */
      /* Fill the column */
      for (j = jMin; j < jMax; j++) {
	tfmap->tf_coordinates(i,j,&t,&f);
	value = amp[chanIdx]*amp[chanIdx];
	column[j] += value;
      }
    }
  }
  return(flag);
}


int MP_Dirac_Atom_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
  case MP_AMP_PROP:
    return( MP_TRUE );
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_Dirac_Atom_c::get_field( int field , int chanIdx ) {

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field) {
  case MP_AMP_PROP :
    return (amp[chanIdx]);
  default:
    x = 0.0;
  }
  return( x );
}

