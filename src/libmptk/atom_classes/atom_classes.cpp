/******************************************************************************/
/*                                                                            */
/*                             atom_classes.cpp                               */
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

/****************************************/
/*                                      */
/* INTERFACE BETWEEN ATOM CLASSES       */
/* AND THE REST OF THE SOFTWARE         */
/*                                      */
/****************************************/

#include "mptk.h"
#include "mp_system.h"


/******************************/
/* ATOM-RELATED I/O FUNCTIONS */
/******************************/

/*************/
/* Generic factory function to create atoms from streams */
MP_Atom_c* read_atom( FILE *fid, const char mode ) {

  MP_Atom_c* atom;
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];

  /* Try to read the atom header */
  switch ( mode ) {

  case MP_TEXT:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t<atom type=\"%[a-z]\">\n", str ) != 1 ) ) {
      fprintf(stderr, "mplib error -- read_atom() - Cannot scan the atom type (in text mode).\n");
      return( NULL );
    }
    break;

  case MP_BINARY:
    if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "%[a-z]\n", str ) != 1 ) ) {
      fprintf(stderr, "mplib error -- read_atom() - Cannot scan the atom type (in binary mode).\n");
      return( NULL );
    }
    break;

  default:
    fprintf(stderr, "mplib error -- read_atom() - Unknown read mode in read_atom().\n");
    return( NULL );
    break;
  }

  /* Allocate an atom of the right type */
  /* - Gabor atom: */
  if ( !strcmp(str,"gabor") ) atom = new MP_Gabor_Atom_c( fid, mode );
  /* - Harmonic atom: */
  //  else if ( !strcmp(str,"harmonic") ) atom = new MP_Harmonic_Atom_c( fid, mode );
  /* - Dirac atom: */
  //  else if ( !strcmp(str,"dirac") ) atom = new MP_Dirac_Atom_c( fid, mode );
  /* - ADD YOUR ATOMS HERE: */
  //  else if ( !strcmp(str,"TEMPLATE") ) atom = new MP_TEMPLATE_Atom_c( fid, mode );
  /* - Unknown atom type: */
  else { 
    fprintf(stderr,"mplib error -- read_atom() - Cannot read atoms of type '%s'\n",str);
    return( NULL );
  }
  /* In text mode... */
  if ( mode == MP_TEXT ) {
    /* ... try to read the closing atom tag */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( strcmp( line, "\t</atom>\n" ) ) ) {
      fprintf(stderr,"mplib error -- read_atom() - Failed to read the </atom> tag.\n");
      if ( atom ) delete( atom );
      return( NULL );
    }
  }

  return ( atom );
}

/*************/
/* Generic function to write atoms to streams */
int write_atom( FILE *fid, const char mode, MP_Atom_c *atom ) {

  int nItem = 0;

  /* Write the atom type */
  switch ( mode ) {
    
  case MP_TEXT:
    nItem += fprintf( fid, "\t<atom type=\"");
    nItem += fprintf( fid, "%s", atom->type_name() );
    nItem += fprintf( fid, "\">\n" );
    break;

  case MP_BINARY:
    nItem += fprintf( fid, "%s\n", atom->type_name() );
    break;

  default:
    fprintf( stderr, "mplib error -- write_atom() - Unknown write mode." );
    return(0);
    break;
  }

  /* Call the atom's write function */
  nItem += atom->write( fid, mode );

  /* Print the closing tag if needed */
  if ( mode == MP_TEXT ) nItem += fprintf( fid, "\t</atom>\n" );

  return( nItem );

}
