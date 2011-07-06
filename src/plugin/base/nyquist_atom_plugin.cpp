/******************************************************************************/
/*                                                                            */
/*                              gabor_atom.cpp                                */
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


/*************************************************/
/*                                               */
/* gabor_atom.cpp: methods for gabor atoms       */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "nyquist_atom_plugin.h"

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
MP_Atom_c* MP_Nyquist_Atom_Plugin_c::nyquist_atom_create_empty(void)
    {

      return new MP_Nyquist_Atom_Plugin_c;

    }

/*************************/
/* File factory function */
/*****************************/
/* Specific factory function */
MP_Atom_c* MP_Nyquist_Atom_Plugin_c::create( FILE *fid, MP_Dict_c *dict, const char mode )
{

  const char* func = "MP_Nyquist_Atom_c::init(numChans)";
  MP_Nyquist_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Nyquist_Atom_Plugin_c();
  if ( newAtom == NULL )
    {
      mp_error_msg( func, "Failed to create a new Nyquist atom.\n" );
      return( NULL );
    }

  	if ( dict->numBlocks != 0 )
		newAtom->dict = dict;

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
MP_Nyquist_Atom_Plugin_c::MP_Nyquist_Atom_Plugin_c( void )
    :MP_Atom_c()
{}

/********************/
/* File reader      */
int MP_Nyquist_Atom_Plugin_c::read( FILE *fid, const char mode )
{

  const char* func = "MP_Nyquist_Atom_c(file)";

  /* Go up one level */
  if ( MP_Atom_c::read( fid, mode ) )
    {
      mp_error_msg( func, "Reading of Nyquist atom fails at the generic atom level.\n" );
      return( 1 );
    }

  return( 0 );
}


/**************/
/* Destructor */
MP_Nyquist_Atom_Plugin_c::~MP_Nyquist_Atom_Plugin_c()
{}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Nyquist_Atom_Plugin_c::write( FILE *fid, const char mode )
{

  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Nothing to print as nyquist-specific parameters */

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/*************/
/* Type name */
const char * MP_Nyquist_Atom_Plugin_c::type_name(void)
{
  return ("nyquist");
}

/**********************/
/* Readable text dump */
int MP_Nyquist_Atom_Plugin_c::info( FILE *fid )
{

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
int MP_Nyquist_Atom_Plugin_c::info()
{

  unsigned int i = 0;
  int nChar = 0;

  nChar += (int)mp_info_msg( "NYQUIST ATOM", "[%d] channel(s)\n", numChans );
  for ( i=0; i<numChans; i++ )
    {
      nChar += (int)mp_info_msg( "        |-", "(%d/%d)\tSupport= %lu %lu\tAmp %g\n", i+1, numChans, support[i].pos, support[i].len, (double)amp[i] );
    }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Nyquist_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer )
{

  MP_Chan_t chanIdx;
  unsigned long int len;
  unsigned long int t;
  MP_Real_t value;
  MP_Real_t *atomBuffer;
  MP_Real_t *atomBufferStart;

  for (chanIdx = 0, atomBufferStart = outBuffer;
       chanIdx < numChans;
       chanIdx++ )
    {

      len = support[chanIdx].len;
      value = (MP_Real_t) (1/sqrt( (double)len )) * (MP_Real_t) amp[chanIdx];

      for ( t = 0, atomBuffer = atomBufferStart;
            t<len;
            t+=2, atomBuffer+=2 )
        {
          *atomBuffer = value;
        }
      for ( t = 1, atomBuffer = atomBufferStart + 1;
            t<len;
            t+=2, atomBuffer+=2 )
        {
          *atomBuffer = -value;
        }

      atomBufferStart += len;
    }
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Nyquist_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char /* tfmapType */ )
{

  /* YOUR code */
  mp_error_msg( "MP_Nyquist_Atom_c::add_to_tfmap","This function is not implemented for nyquist atoms.\n" );
  tfmap = NULL;

  return( 0 );
}

int MP_Nyquist_Atom_Plugin_c::has_field( int field )
{

  if ( MP_Atom_c::has_field( field ) ) 
	  return (MP_TRUE);
  else 
	  return(MP_FALSE);
}

MP_Real_t MP_Nyquist_Atom_Plugin_c::get_field( int field , MP_Chan_t chanIdx )
{

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) 
	  return (MP_Atom_c::get_field(field,chanIdx));
  else 
	  x = 0.0;
  return( x );
}



/******************************************************/
/* Registration of new atom (s) in the atoms factory */


DLL_EXPORT void registry(void)
{
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom_empty("nyquist",&MP_Nyquist_Atom_Plugin_c::nyquist_atom_create_empty);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom("nyquist",&MP_Nyquist_Atom_Plugin_c::create);
}
