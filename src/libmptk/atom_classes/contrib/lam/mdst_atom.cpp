/******************************************************************************/
/*                                                                            */
/*                          mdst_atom.cpp    	                    	      */
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


/****************************************************************/
/*                                                              */
/* mdst_atom.cpp: methods for mclt atoms			*/
/*                                                              */
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"

#include <dsp_windows.h>


/*************/
/* CONSTANTS */
/*************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/************************/
/* Factory function     */
MP_Mdst_Atom_c * MP_Mdst_Atom_c::init( const MP_Chan_t setNumChans,
                                       const unsigned char setWindowType,
                                       const double setWindowOption ) {
  
  const char* func = "MP_Mdst_Atom_c::MP_Mdst_Atom_c(...)";
  
  MP_Mdst_Atom_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Mdst_Atom_c();
  if ( newAtom == NULL ) {
    mp_error_msg( func, "Failed to create a new MDST atom. Returning a NULL.\n" );
    return( NULL );
  }

  /* Set the window-related values */
  if ( window_type_is_ok( setWindowType ) ) {
    newAtom->windowType   = setWindowType;
    newAtom->windowOption = setWindowOption;
  }
  else {
    mp_error_msg( func, "The window type is unknown. Returning a NULL atom.\n" );
    delete( newAtom );
    return( NULL );
  }
  
  /* Set the default freq */
  newAtom->freq  = 0.0;

  /* Allocate and check */
  if ( newAtom->global_alloc( setNumChans ) ) {
    mp_error_msg( func, "Failed to allocate some vectors in the new MDST atom. Returning a NULL atom.\n" );
    delete( newAtom );
    return( NULL );
  }

  return( newAtom );
}

/*************************/
/* File factory function */
MP_Mdst_Atom_c* MP_Mdst_Atom_c::init( FILE *fid, const char mode ) {
  
  const char* func = "MP_Mdst_Atom_c::init(fid,mode)";
  
  MP_Mdst_Atom_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Mdst_Atom_c();
  if ( newAtom == NULL ) {
    mp_error_msg( func, "Failed to create a new atom.\n" );
    return( NULL );
  }

  /* Read and check */
  if ( newAtom->read( fid, mode ) ) {
    mp_error_msg( func, "Failed to read the new MDST atom.\n" );
    delete( newAtom );
    return( NULL );
  }
  
  return( newAtom );
}

/********************/
/* Void constructor */
MP_Mdst_Atom_c::MP_Mdst_Atom_c( void )
  :MP_Atom_c() {
  windowType = DSP_UNKNOWN_WIN;
  windowOption = 0.0;
  freq  = 0.0;
  amp   = NULL;
}


/************************/
/* Local allocations    */
int MP_Mdst_Atom_c::local_alloc( const MP_Chan_t /* setNumChans */ ) {

  // const char* func = "MP_Mdst_Atom_c::local_alloc(numChans)";

  /* No vectors at this level. */

  return( 0 );
}


/************************/
/* Global allocations   */
int MP_Mdst_Atom_c::global_alloc( const MP_Chan_t setNumChans ) {

  const char* func = "MP_Mdst_Atom_c::global_alloc(numChans)";

  /* Go up one level */
  if ( MP_Atom_c::global_alloc( setNumChans ) ) {
    mp_error_msg( func, "Allocation of MDST atom failed at the generic atom level.\n" );
    return( 1 );
  }

  /* Alloc at local level */
  /* if ( local_alloc( setNumChans ) ) {
    mp_error_msg( func, "Allocation of MDST atom failed at the local level.\n" );
    return( 1 );
    } */
  /* Note: no alloc needs to be performed at the local level. */

  return( 0 );
}


/********************/
/* File reader      */
int MP_Mdst_Atom_c::read( FILE *fid, const char mode ) {

  const char* func = "MP_Mdst_Atom_c::MP_Mdst_Atom_c(fid,mode)";
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  double fidFreq;

  /* Go up one level */
  if ( MP_Atom_c::read( fid, mode ) ) {
    mp_error_msg( func, "Reading of MDST atom fails at the generic atom level.\n" );
    return( 1 );
  }

  /* Alloc at local level */
  /* if ( local_alloc( numChans ) ) {
    mp_error_msg( func, "Allocation of MDST atom failed at the local level.\n" );
    return( 1 );
    }*/
  /* NOTE: no local alloc needed here because no vectors are used at this level. */

  /* Then read this level's info */
  switch ( mode ) {

  case MP_TEXT:
    /* Read the window type */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t\t<window type=\"%[a-z]\" opt=\"%lg\"></window>\n", str, &windowOption ) != 2 ) ) {
      mp_error_msg( func, "Failed to read the window type and/or option in a Mdst atom structure.\n");
      windowType = DSP_UNKNOWN_WIN;
        return( 1 );
    }
    else {
      /* Convert the window type string */
      windowType = window_type( str );
    }
    /* freq */
    if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	 ( sscanf( str, "\t\t<par type=\"freq\">%lg</par>\n", &fidFreq ) != 1 ) ) {
      mp_error_msg( func, "Cannot scan freq.\n" );
        return( 1 );
    }
    else {
      freq = (MP_Real_t)fidFreq;
    }
    break;

  case MP_BINARY:
    /* Try to read the atom window */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "%[a-z]\n", str ) != 1 ) ) {
      mp_error_msg( func, "Failed to scan the atom's window type.\n");
      windowType = DSP_UNKNOWN_WIN;
        return( 1 );
    }
    else {
      /* Convert the window type string */
      windowType = window_type( str );
    }
    /* Try to read the window option */
    if ( mp_fread( &windowOption,  sizeof(double), 1, fid ) != 1 ) {
      mp_error_msg( func, "Failed to read the atom's window option.\n");
      windowOption = 0.0;
        return( 1 );
    }
    /* Try to read the freq */
    if ( mp_fread( &freq,  sizeof(MP_Real_t), 1 , fid ) != (size_t)1 ) {
      mp_error_msg( func, "Failed to read the freq.\n" );     
      freq = 0.0;
        return( 1 );
    }
   break;

  default:
    mp_error_msg( func, "Unknown read mode met in MP_Mdst_Atom_c( fid , mode )." );
        return( 1 );
    break;
  }

                   return( 0 );
}


/**************/
/* Destructor */
MP_Mdst_Atom_c::~MP_Mdst_Atom_c() {  
}

/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Mdst_Atom_c::write( FILE *fid, const char mode ) {
  
  unsigned int i;
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other MDST-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* Window name */
    nItem += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%g\"></window>\n", window_name(windowType), windowOption );
    /* print the freq */
    nItem += fprintf( fid, "\t\t<par type=\"freq\">%g</par>\n",  (double)freq );
    break;

  case MP_BINARY:
    /* Window name */
    nItem += fprintf( fid, "%s\n", window_name(windowType) );
    /* Window option */
    nItem += mp_fwrite( &windowOption,  sizeof(double), 1, fid );
    /* Binary parameters */
    nItem += mp_fwrite( &freq,  sizeof(MP_Real_t), 1, fid );
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
char * MP_Mdst_Atom_c::type_name(void) {
  return ("mdst");
}

/**********************/
/* Readable text dump */
int MP_Mdst_Atom_c::info( FILE *fid ) {
  
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
int MP_Mdst_Atom_c::info() {

  unsigned int i = 0;
  int nChar = 0;

  nChar += mp_info_msg( "MDST ATOM", "%s window (window opt=%g)\n", window_name(windowType), windowOption );
  nChar += mp_info_msg( "        |-", "[%d] channel(s)\n", numChans );
  nChar += mp_info_msg( "        |-", "Freq %g\n", (double)freq);
  for ( i=0; i<numChans; i++ ) {
    nChar += mp_info_msg( "        |-", "(%d/%d)\tSupport= %lu %lu\tAmp %g\n",
			  i+1, numChans, support[i].pos, support[i].len,
			  (double)amp[i]);
  }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Mdst_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Real_t *window;
  MP_Sample_t *atomBuffer;
  unsigned long int windowCenter = 0;
  /* Parameters for the atom waveform : */
  unsigned int chanIdx;
  unsigned int t;
  unsigned long int len;
  double dAmp, dFreq, dPhase, dT;
  double argument;

  extern MP_Win_Server_c MP_GLOBAL_WIN_SERVER;

  assert( outBuffer != NULL );

  for ( chanIdx = 0 , atomBuffer = outBuffer; 
	chanIdx < numChans; 
	chanIdx++  ) {
    /* Dereference the atom length in the current channel once and for all */
    len = support[chanIdx].len;

    /** make the window */
    windowCenter = MP_GLOBAL_WIN_SERVER.get_window( &window, len, windowType, windowOption );
    assert( window != NULL );
    
    /* Dereference the arguments once and for all */
    dFreq      = (double)(  freq ) * MP_2PI;
    dAmp       = (double)(   amp[chanIdx] );

    /* Compute the phase */
    dPhase     = dFreq * ( 0.5 + len*0.25 ) + MP_PI*0.5;

    for ( t = 0; t<len; t++ ) {

      /* Compute the cosine's argument */
      dT = (double)(t);
      argument = dFreq*dT + dPhase;
      
      /* Compute the waveform samples */
      *(atomBuffer+t) = (MP_Sample_t)( (double)(*(window+t)) * dAmp * cos(argument) );
	
    }

    /* Go to the next channel */
    atomBuffer += len;
  }

}

/********************************************************/
/** Addition of the atom to a time-frequency map */
int MP_Mdst_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType ) {

  MP_Gabor_Atom_c *gatom = NULL; 
  int chanIdx;
  int flag;

  gatom = MP_Gabor_Atom_c::init(numChans,windowType, windowOption);

  gatom->numChans = numChans;
  gatom->windowType = windowType;
  gatom->windowOption = windowOption;

  // Setting the support
  for(chanIdx=0; chanIdx< numChans; chanIdx++) {
    gatom->support[chanIdx].pos = support[chanIdx].pos;
    gatom->support[chanIdx].len = support[chanIdx].len;
  }

  // Setting the freq
  gatom->freq = freq;
  gatom->chirp = 0.0;
  // Setting the amplitude on each channel
  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {
    gatom->amp[chanIdx] = amp[chanIdx];
    gatom->phase[chanIdx] = freq * ( 1/2 + support[chanIdx].len/4 ) + MP_PI*0.5;
  }
  // Plotting
  flag = gatom->add_to_tfmap(tfmap, tfmapType);

  return( flag );
}
