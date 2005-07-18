/******************************************************************************/
/*                                                                            */
/*                              TEMPLATE_atom.cpp                             */
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
 * $Date: 2005/05/16 14:41:58 $
 * $Revision: 1.1 $
 *
 */

/*************************************************/
/*                                               */
/* TEMPLATE_atom.cpp: methods for TEMPLATE atoms */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "system.h"

#include <dsp_windows.h>

/* YOUR includes go here */

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_TEMPLATE_Atom_c::MP_TEMPLATE_Atom_c( void )
  :MP_Atom_c() {

  /* YOUR code */

}

/************************/
/* Specific constructor */
MP_TEMPLATE_Atom_c::MP_TEMPLATE_Atom_c( unsigned int setNChan /* + YOUR parameters */ )
  :MP_Atom_c( setNChan ) {

  /* YOUR code */

}

/********************/
/* File constructor */
MP_TEMPLATE_Atom_c::MP_TEMPLATE_Atom_c( FILE *fid, const char mode )
  :MP_Atom_c( fid, mode ) {

  /* YOUR variables */

  switch ( mode ) {

  case MP_TEXT:
    /* YOUR code reads text data in sync with the MP_TEMPLATE_Atom_c::write() method. */
    break;

  case MP_BINARY:
    /* YOUR code reads buinary data in sync with the MP_TEMPLATE_Atom_c::write() method. */
    break;

  default:
    fprintf( stderr, "mplib error -- MP_TEMPLATE_Atom_c(file,mode) -"
	     " Unknown read mode met in MP_TEMPLATE_Atom_c( fid , mode )." );
    break;
  }

  /* YOUR code allocates and initialize deep data members */

  switch (mode ) {
    
  case MP_TEXT:
    /* YOUR code reads text data members in sync with the MP_TEMPLATE_Atom_c::write() method. */
    break;
    
  case MP_BINARY:
    /* YOUR code reads binary data members in sync with the MP_TEMPLATE_Atom_c::write() method. */
    break;
    
  default:
    break;
  }

}


/**************/
/* Destructor */
MP_TEMPLATE_Atom_c::~MP_TEMPLATE_Atom_c() {

  /* YOUR code frees all the allocated data members. */
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_TEMPLATE_Atom_c::write( FILE *fid, const char mode ) {
  
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other TEMPLATE-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* YOUR code writes text data and counts the number of written items */
    break;

  case MP_BINARY:
    /* YOUR code writes binary data and counts the number of written items/bytes */
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
char * MP_TEMPLATE_Atom_c::type_name(void) {
  return ("TEMPLATE");
}

/**********************/
/* Readable text dump */
int MP_TEMPLATE_Atom_c::info( FILE *fid ) {

  int nChar = 0;

  /* YOUR code writes the data members in a readable form
     and returns the number of written characters */
  fid = NULL;

  return( nChar );
}

/********************/
/* Waveform builder */
void MP_TEMPLATE_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  /* YOUR code builds a waveform and copies it to the output buffer */
  outBuffer = NULL;
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
char MP_TEMPLATE_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap ) {

  char flag = 0;

  /* YOUR code */
  tfmap = NULL;

  return( flag );
}



int MP_TEMPLATE_Atom_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
    /*    
     * case MP_YOURCode1_PROP:
     * case MP_YOURCode2_PROP:
     * case MP_YOURCode3_PROP:
     *    return ( MP_TRUE );
     */
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_TEMPLATE_Atom_c::get_field( int field , int chanIdx ) {

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field) {
    /*    
     * case MP_TEMPLATE1_PROP:
     *       return(YOUR code);
     * case MP_TEMPLATE2_PROP:
     *       return(YOUR code);
     */
  default:
    x = 0.0;
  }
  return( x );
}

