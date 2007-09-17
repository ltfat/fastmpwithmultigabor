/******************************************************************************/
/*                                                                            */
/*                              dirac_atom.cpp                                */
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
 * $Author: broy $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

/*******************************************/
/*                                         */
/* dirac_atom.cpp: methods for dirac atoms */
/*                                         */
/*******************************************/

#include "mptk.h"
#include "dirac_atom_plugin.h"
#include "mp_system.h"

#include <dsp_windows.h>

using namespace std;


/*************/
/* CONSTANTS */
/*************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Atom_c  * MP_Dirac_Atom_Plugin_c::dirac_atom_create_empty(void)
    {

      return new MP_Dirac_Atom_Plugin_c;

    }

/* File factory function */
MP_Atom_c * MP_Dirac_Atom_Plugin_c::create( FILE *fid, const char mode ) {
  
  const char* func = "MP_Dirac_Atom_Plugin_c::create(fid,mode)";
  MP_Dirac_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Dirac_Atom_Plugin_c();
  if ( newAtom == NULL ) {
    mp_error_msg( func, "Failed to create a new atom.\n" );
    return( NULL );
  }

  /* Read and check */
  if ( newAtom->read( fid, mode ) ) {
    mp_error_msg( func, "Failed to read the new Gabor atom.\n" );
    delete( newAtom );
    return( NULL );
  }
  
  return( (MP_Atom_c*)newAtom );
}

/********************/
/* Void constructor */
MP_Dirac_Atom_Plugin_c::MP_Dirac_Atom_Plugin_c( void )
  :MP_Atom_c() {
}

/********************/
/* File reader      */
int MP_Dirac_Atom_Plugin_c::read( FILE *fid, const char mode ) {
  
  const char* func = "MP_Dirac_Atom_c(file)";
  
  /* Go up one level */
  if ( MP_Atom_c::read( fid, mode ) ) {
    mp_error_msg( func, "Reading of Dirac atom fails at the generic atom level.\n" );
    return( 1 );
  }

  return( 0 );
}


/**************/
/* Destructor */
MP_Dirac_Atom_Plugin_c::~MP_Dirac_Atom_Plugin_c() {
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Dirac_Atom_Plugin_c::write( FILE *fid, const char mode ) {
  
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Nothing to print as dirac-specific parameters */

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/*************/
/* Type name */
char * MP_Dirac_Atom_Plugin_c::type_name(void) {
  return ("dirac");
}

/**********************/
/* Readable text dump */
int MP_Dirac_Atom_Plugin_c::info( FILE *fid ) {
  
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
int MP_Dirac_Atom_Plugin_c::info() {

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
void MP_Dirac_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer ) {

  MP_Chan_t chanIdx;
  for (chanIdx = 0 ; chanIdx < numChans; chanIdx++ ) 
    outBuffer[chanIdx] = amp[chanIdx];
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Dirac_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char /* tfmapType */ ) {

  MP_Chan_t chanIdx;
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


MP_Real_t MP_Dirac_Atom_Plugin_c::dist_to_tfpoint( MP_Real_t time, MP_Real_t /* freq */, MP_Chan_t chanIdx ) {
  MP_Real_t deltat = (time-(MP_Real_t)(support[chanIdx].pos));
  return(deltat*deltat); 
}


int MP_Dirac_Atom_Plugin_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_Dirac_Atom_Plugin_c::get_field( int field , MP_Chan_t chanIdx ) {

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field) {
  default:
    x = 0.0;
  }
  return( x );
}



/******************************************************/
/* Registration of new atom (s) in the atoms factory */


DLL_EXPORT void registry(void)
{
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom_empty("DiracAtom",&MP_Dirac_Atom_Plugin_c::dirac_atom_create_empty);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom("dirac",&MP_Dirac_Atom_Plugin_c::create);
 
}
