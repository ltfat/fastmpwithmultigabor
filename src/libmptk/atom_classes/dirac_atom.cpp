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
int MP_Dirac_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType ) {

  unsigned char chanIdx;
  unsigned long int tMin, nMin;
  MP_Tfmap_t *column;
  unsigned long int j;

  assert( numChans == tfmap->numChans );

  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {

    /* 1/ Is the support inside the tfmap ? */
    /* Time: */
    tMin = support[chanIdx].pos;
    if ( (tMin > tfmap->tMax) || (tMin < tfmap->tMin) ) return( 0 );

    /* 2/ Convert the real coordinates into pixel coordinates */
    nMin = tfmap->time_to_pix( tMin );

    /* 3/ Fill the TF map: */
    column = tfmap->channel[chanIdx] + nMin*tfmap->numRows; /* Seek the column */
    for ( j = 0; j < tfmap->numRows; j++ ) column[j] += tfmap->linmap( amp[chanIdx] );

  } /* End foreach channel */

  return( 0 );
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

