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
}

/************************/
/* Specific constructor */
MP_Dirac_Atom_c::MP_Dirac_Atom_c( unsigned int setNChan )
  :MP_Atom_c( setNChan ) {
}

/********************/
/* File constructor */
MP_Dirac_Atom_c::MP_Dirac_Atom_c( FILE *fid, const char mode )
  :MP_Atom_c( fid, mode ) {
  
  const char* func = "MP_Dirac_Atom_c(file)";
  char str[MP_MAX_STR_LEN];
  double fidAmp;
  unsigned int i, iRead;
  
  switch ( mode ) {
    
  case MP_TEXT:
    for (i = 0; i<numChans; i++) {
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<par type=\"amp\" chan=\"%d\">%lg</par>\n", &iRead,&fidAmp ) != 2 ) ) {
	mp_warning_msg( func, "Cannot scan amp on channel %u.\n", i );
      } else if ( iRead != i ) {
 	mp_warning_msg( func, "Potential shuffle in the parameters"
			" of a dirac atom. (Index \"%u\" read, \"%u\" expected.)\n",
		iRead, i );
      } else {
	*(amp+i) = (MP_Real_t)fidAmp;
      }
    }
    break;
      
  case MP_BINARY:
    if ( mp_fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_warning_msg( func, "Failed to read the amp array.\n" );     
      for ( i=0; i<numChans; i++ ) *(amp+i) = 0.0;
    }
    
    break;
    
  default:
    mp_error_msg( func, " Unknown read mode met in MP_Dirac_Atom_c( fid , mode )." );
    break;
  }
  
}


/**************/
/* Destructor */
MP_Dirac_Atom_c::~MP_Dirac_Atom_c() {
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Dirac_Atom_c::write( FILE *fid, const char mode ) {
  
  unsigned int i = 0;
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other dirac-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    for (i = 0; i<numChans; i++) {
      nItem += fprintf(fid, "\t\t<par type=\"amp\" chan=\"%u\">%lg</par>\n", i, (double)amp[i]);
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
  
  int nChar = 0;
  FILE* bakStream;
  void (*bakHandler)( void );

  /* Backup the current stream/handler */
  bakStream = get_info_stream();
  bakHandler = get_info_handler();
  /* Redirect to the given file */
  set_info_stream( fid );
  set_info_handler( MP_FLUSH );
  /* Launch the info output */
  nChar += info();
  /* Reset to the previous stream/handler */
  set_info_stream( bakStream );
  set_info_handler( bakHandler );

  return( nChar );
}

/**********************/
/* Readable text dump */
int MP_Dirac_Atom_c::info() {

  unsigned int i = 0;
  int nChar = 0;

  nChar += mp_info_msg( "DIRAC ATOM", "[%d] channel(s)\n", numChans );
  for ( i=0; i<numChans; i++ ) {
    nChar += mp_info_msg( "        |-", "(%d/%d)\tSupport= %lu %lu\tAmp %g\n",
			  i+1, numChans, support[i].pos, support[i].len,
			  (double)amp[i] );
  }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Dirac_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Chan_t chanIdx;
  for (chanIdx = 0 ; chanIdx < numChans; chanIdx++ ) 
    outBuffer[chanIdx] = amp[chanIdx];
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Dirac_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char /* tfmapType */ ) {

  unsigned char chanIdx;
  unsigned long int tMin, nMin, nMax;
  MP_Tfmap_t *column;
  MP_Real_t val;
  unsigned long int i,j;

  assert( numChans == tfmap->numChans );

  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {

    /* 1/ Is the support inside the tfmap ? */
    /* Time: */
    tMin = support[chanIdx].pos;
    if ( (tMin > tfmap->tMax) || (tMin < tfmap->tMin) ) return( 0 );

    /* 2/ Convert the real coordinates into pixel coordinates */
    nMin = tfmap->time_to_pix( tMin );
    //    nMax = tfmap->time_to_pix( tMin+1);
    nMax = nMin +1;

    /* 3/ Fill the TF map: */
    for ( i = nMin; i < nMax; i++) {
      /* Seek the column */
      column = tfmap->channel[chanIdx] + i*tfmap->numRows;
      for ( j = 0; j < tfmap->numRows; j++ ) {
	val = (MP_Real_t)(column[j]) + amp[chanIdx]*amp[chanIdx];
	column[j] = (MP_Tfmap_t)( val );
	/* Test the min/max */
	if ( tfmap->ampMax < val ) tfmap->ampMax = val;
	if ( tfmap->ampMin > val ) tfmap->ampMin = val;
      } /* End for each row */
    } /* End for each column */
  } /* End foreach channel */

  return( 0 );
}


MP_Real_t MP_Dirac_Atom_c::dist_to_tfpoint( MP_Real_t time, MP_Real_t /* freq */, MP_Chan_t chanIdx ) {
  MP_Real_t deltat = (time-(MP_Real_t)(support[chanIdx].pos));
  return(deltat*deltat); 
}


int MP_Dirac_Atom_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_Dirac_Atom_c::get_field( int field , MP_Chan_t chanIdx ) {

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field) {
  default:
    x = 0.0;
  }
  return( x );
}

