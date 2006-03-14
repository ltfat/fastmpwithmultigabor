/******************************************************************************/
/*                                                                            */
/*                        anywave_hilbert_atom.cpp                            */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Mar 07 2006 */
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
 * $Date$
 * $Revision$
 *
 */

/***************************************************************/
/*                                                             */
/* anywave_hilbert_atom.cpp: methods for anywave hilbert atoms */
/*                                                             */
/***************************************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;


/************************/
/* Factory function     */
MP_Anywave_Hilbert_Atom_c* MP_Anywave_Hilbert_Atom_c::init( const MP_Chan_t setNChan ) {
  
  const char* func = "MP_Anywave_Hilbert_Atom_c::init(numChan)";
  
  MP_Anywave_Hilbert_Atom_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Anywave_Hilbert_Atom_c();
  if ( newAtom == NULL ) {
    mp_error_msg( func, "Failed to create a new Anywave Hilbert atom. Returning a NULL.\n" );
    return( NULL );
  }

  /* Allocate and check */
  if ( newAtom->global_alloc( setNChan ) ) {
    mp_error_msg( func, "Failed to allocate some vectors in the new Anywave Hilbert atom."
		  " Returning a NULL atom.\n" );
    delete( newAtom );
    return( NULL );
  }

  return( newAtom );
}


/*************************/
/* File factory function */
MP_Anywave_Hilbert_Atom_c* MP_Anywave_Hilbert_Atom_c::init( FILE *fid, const char mode ) {
  
  const char* func = "MP_Anywave_Hilbert_Atom_c::init(fid,mode)";
  
  MP_Anywave_Hilbert_Atom_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Anywave_Hilbert_Atom_c();
  if ( newAtom == NULL ) {
    mp_error_msg( func, "Failed to create a new atom.\n" );
    return( NULL );
  }

  /* Read and check */
  if ( newAtom->read( fid, mode ) ) {
    mp_error_msg( func, "Failed to read the new Anywave atom.\n" );
    delete( newAtom );
    return( NULL );
  }
  
  return( newAtom );
}

/********************/
/* Void constructor */
MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c( void )
  :MP_Anywave_Atom_c() {
    meanPart = NULL;
    nyquistPart = NULL;
    realPart = NULL;
    hilbertPart = NULL;

    anywaveRealTable = NULL;
    realTableIdx = 0;
    anywaveHilbertTable = NULL;
    hilbertTableIdx = 0;
}

/************************/
/* Global allocations   */
int MP_Anywave_Hilbert_Atom_c::global_alloc( const MP_Chan_t setNChan ) {

  const char* func = "MP_Anywave_Hilbert_Atom_c::global_alloc(numChans)";

  /* Go up one level */
  if ( MP_Anywave_Atom_c::global_alloc( setNChan ) ) {
    mp_error_msg( func, "Allocation of Anywave Hilbert atom failed at the Anywave atom level.\n" );
    return( 1 );
  }
  if ( init_parts() ) {
    return( 1 );
  }

  return( 0 );
}


int MP_Anywave_Hilbert_Atom_c::init_parts(void) {

  unsigned short int chanIdx;
  
  if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t) <= 1.0) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c", "numChans [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the meanPart array. meanPart is set to NULL\n", numChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
    meanPart = NULL;
    nyquistPart = NULL;
    realPart = NULL;
    hilbertPart = NULL;
    return(1);
  } else {
    if ( (meanPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the meanPart array for a new atom;"
		    " amp stays NULL.\n" );
      return(1);
    }
    if ( (nyquistPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the nyquistPart array for a new atom;"
		    " amp stays NULL.\n" );
      return(1);
    }
    if ( (realPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the realPart array for a new atom;"
		    " amp stays NULL.\n" );
      return(1);
    }
    if ( (hilbertPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the hilbertPart array for a new atom;"
		    " amp stays NULL.\n" );
      return(1);
    }
  }
    
  /* Initialize */
  if ( (meanPart!=NULL) ) {
    for (chanIdx = 0; chanIdx<numChans; chanIdx++) {
      *(meanPart +chanIdx) = 0.0;
    }
  } else {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter meanPart"
		  " for the new atom are left un-initialized.\n" );
    return(1);
  }
  if ( (nyquistPart!=NULL) ) {
    for (chanIdx = 0; chanIdx<numChans; chanIdx++) {
      *(nyquistPart +chanIdx) = 0.0;
    }
  } else {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter nyquistPart"
		  " for the new atom are left un-initialized.\n" );
    return(1);
  }
  if ( (realPart!=NULL) ) {
    for (chanIdx = 0; chanIdx<numChans; chanIdx++) {
      *(realPart +chanIdx) = 0.0;
    }
  } else {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter realPart"
		  " for the new atom are left un-initialized.\n" );
    return(1);
  }
  if ( (hilbertPart!=NULL) ) {
    for (chanIdx = 0; chanIdx<numChans; chanIdx++) {
      *(hilbertPart +chanIdx) = 0.0;
    }
  } else {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter hilbertPart"
		  " for the new atom are left un-initialized.\n" );
    return(1);
  }
  return(0);
}

int MP_Anywave_Hilbert_Atom_c::init_tables( void ) {

  extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;  
  char* str;

  if ( ( str = (char*) malloc( MP_MAX_STR_LEN * sizeof(char) ) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::init_tables()","The string str cannot be allocated.\n" );    
    return(1);
  }

  /* create the real table if needed */  
  strcpy(str, MP_GLOBAL_ANYWAVE_SERVER.get_filename( tableIdx ));
  str = strcat(str,"_real");
  realTableIdx = MP_GLOBAL_ANYWAVE_SERVER.get_index( str );
  if (realTableIdx == MP_GLOBAL_ANYWAVE_SERVER.numTables) {
    anywaveRealTable = anywaveTable->copy();
    anywaveRealTable->center_and_denyquist();
    anywaveRealTable->normalize();
    anywaveRealTable->set_table_file_name(str);
    realTableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( anywaveRealTable );
  } else {
    anywaveRealTable = MP_GLOBAL_ANYWAVE_SERVER.tables[realTableIdx];
  }

  /* create the hilbert table if needed */
  strcpy(str, MP_GLOBAL_ANYWAVE_SERVER.get_filename( tableIdx ));
  str = strcat(str,"_hilbert");
  hilbertTableIdx = MP_GLOBAL_ANYWAVE_SERVER.get_index( str );
  if (hilbertTableIdx == MP_GLOBAL_ANYWAVE_SERVER.numTables) {
    /* need to create a new table */
    anywaveHilbertTable = anywaveTable->create_hilbert_dual(str);
    anywaveHilbertTable->normalize();    
    hilbertTableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( anywaveHilbertTable );
  } else {
    anywaveHilbertTable = MP_GLOBAL_ANYWAVE_SERVER.tables[hilbertTableIdx];
  }

  return(0);
}

/********************/
/* File reader      */
int MP_Anywave_Hilbert_Atom_c::read( FILE *fid, const char mode ) {
    
  double fidParam;
  unsigned short int readChanIdx, chanIdx;
  MP_Real_t* pParam;
  MP_Real_t* pParamEnd;
  char* str;

  /* Go up one level */
  if ( MP_Anywave_Atom_c::read( fid, mode ) ) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::init_tables()", "Allocation of Anywave Hilbert atom failed at the Anywave atom level.\n" );
    return( 1 );
  }

  if ( ( str = (char*) malloc( MP_MAX_STR_LEN * sizeof(char) ) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::init_tables()","The string str cannot be allocated.\n" );    
    return(1);
  }

  /* init tables */
  if ( init_tables() ) {
    return(1);
  }

  /* Allocate and initialize */
  if ( init_parts() ) {
    return(1);
  }

  /* Try to read the param */
  switch (mode ) {
    
  case MP_TEXT:

    for (chanIdx = 0;
	 chanIdx < numChans;
	 chanIdx ++) {
      /* Opening tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<anywavePar chan=\"%hu\">\n", &readChanIdx ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan channel index in atom.\n" );
	return(1);
      }
      else if ( readChanIdx != chanIdx ) {
 	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Potential shuffle in the parameters"
		" of an anywave atom. (Index \"%hu\" read, \"%hu\" expected.)\n",
		readChanIdx, chanIdx );
	return(1);
      }
      /* mean part */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t\t<par type=\"meanPart\">%lg</par>\n", &fidParam ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan the mean part on channel %u.\n", chanIdx );
	return(1);
      }
      else {
	*(meanPart + chanIdx) = (MP_Real_t)fidParam;
      }
      /* nyquist part */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t\t<par type=\"nyquistPart\">%lg</par>\n", &fidParam ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan nyquist part on channel %u.\n", chanIdx );
	return(1);
      }
      else {
	*(nyquistPart + chanIdx) = (MP_Real_t)fidParam;
      }
      /* real part */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t\t<par type=\"realPart\">%lg</par>\n", &fidParam ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan real part on channel %u.\n", chanIdx );
	return(1);
      }
      else {
	*(realPart + chanIdx) = (MP_Real_t)fidParam;
      }
      /* hilbert part */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t\t<par type=\"hilbertPart\">%lg</par>\n", &fidParam ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan hilbert part on channel %u.\n", chanIdx );
	return(1);
      }
      else {
	*(hilbertPart + chanIdx) = (MP_Real_t)fidParam;
      }
      /* Closing tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( strcmp( str , "\t\t</anywavePar>\n" ) ) ) {
	mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan the closing parameter tag"
		" in anywave atom, channel %hu.\n", chanIdx );
	return(1);
      }
    }
    break;
    
  case MP_BINARY:
    /* Try to read the mean part */
    if ( mp_fread( meanPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the meanPart array.\n" );     
      pParamEnd = meanPart + numChans;
      for ( pParam = meanPart;
	    pParam < pParamEnd;
	    pParam ++ ) {
	*pParam = 0.0;
      }
      return(1);
    }
    /* Try to read the nyquist part */
    if ( mp_fread( nyquistPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the nyquistPart array.\n" );     
      pParamEnd = nyquistPart + numChans;
      for ( pParam = nyquistPart;
	    pParam < pParamEnd;
	    pParam ++ ) {
	*pParam = 0.0;
      }
      return(1);
    }
    /* Try to read the real part */
    if ( mp_fread( realPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the realPart array.\n" );     
      pParamEnd = realPart + numChans;
      for ( pParam = realPart;
	    pParam < pParamEnd;
	    pParam ++ ) {
	*pParam = 0.0;
      }
      return(1);
    }
    /* Try to read the hilbert part */
    if ( mp_fread( hilbertPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the hilbertPart array.\n" );     
      pParamEnd = hilbertPart + numChans;
      for ( pParam = hilbertPart;
	    pParam < pParamEnd;
	    pParam ++ ) {
	*pParam = 0.0;
      }
      return(1);
    }
    break;
    
  default:
    break;
  }

  return(0);
}


/**************/
/* Destructor */
MP_Anywave_Hilbert_Atom_c::~MP_Anywave_Hilbert_Atom_c() {
  if (meanPart)   free( meanPart );
  if (nyquistPart)   free( nyquistPart );
  if (realPart)   free( realPart );
  if (hilbertPart)   free( hilbertPart );
}


/***************************/
/* OUTPUT METHOD           */
/***************************/


int MP_Anywave_Hilbert_Atom_c::write( FILE *fid, const char mode ) {
  
  int nItem = 0;
  unsigned short int chanIdx = 0;

  /* Call the parent's write function */
  nItem += MP_Anywave_Atom_c::write( fid, mode );

  /* Print the other anywave-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* print the ampHilbert */
    for (chanIdx = 0; 
	 chanIdx < numChans; 
	 chanIdx++) {
      nItem += fprintf( fid, "\t\t<anywavePar chan=\"%u\">\n", chanIdx );
      nItem += fprintf( fid, "\t\t\t<par type=\"meanPart\">%g</par>\n",   (double)meanPart[chanIdx] );
      nItem += fprintf( fid, "\t\t\t<par type=\"nyquistPart\">%g</par>\n",   (double)nyquistPart[chanIdx] );
      nItem += fprintf( fid, "\t\t\t<par type=\"realPart\">%g</par>\n",   (double)realPart[chanIdx] );
      nItem += fprintf( fid, "\t\t\t<par type=\"hilbertPart\">%g</par>\n",   (double)hilbertPart[chanIdx] );
      nItem += fprintf( fid, "\t\t</anywavePar>\n" );    
    }
    break;

  case MP_BINARY:

    /* Binary parameters */
    if(meanPart) nItem += mp_fwrite( meanPart, sizeof(MP_Real_t), (size_t)numChans, fid );
    if(nyquistPart) nItem += mp_fwrite( nyquistPart, sizeof(MP_Real_t), (size_t)numChans, fid );
    if(realPart) nItem += mp_fwrite( realPart, sizeof(MP_Real_t), (size_t)numChans, fid );
    if(hilbertPart) nItem += mp_fwrite( hilbertPart, sizeof(MP_Real_t), (size_t)numChans, fid );
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
char * MP_Anywave_Hilbert_Atom_c::type_name(void) {
  return ("anywavehilbert");
}

/**********************/
/* Readable text dump */
int MP_Anywave_Hilbert_Atom_c::info() {

  unsigned short int chanIdx = 0;
  int nChar = 0;
  nChar += mp_info_msg( "ANYWAVE HILBERT ATOM", "[%d] channel(s)\n", numChans );
  
  nChar += mp_info_msg( "           |-", "\tFilename %s\tanywaveIdx %li\n",
			anywaveTable->tableFileName, anywaveIdx );
  
  for ( chanIdx = 0;
	chanIdx < numChans; 
	chanIdx ++ ) {
    nChar += mp_info_msg( "           |-", "(%u/%u)\tSupport= %lu %lu\tAmp %g\tmeanPart %g\tnyquistPart %g\trealPart %g\thilbertPart %g\n",
			  chanIdx+1, numChans, support[chanIdx].pos, support[chanIdx].len,
			  (double)amp[chanIdx], (double)meanPart[chanIdx], (double)nyquistPart[chanIdx], (double)realPart[chanIdx], (double)hilbertPart[chanIdx]);
  }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Anywave_Hilbert_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Sample_t *atomBuffer;
  MP_Sample_t *atomBufferStart;
  unsigned short int chanIdx;
  unsigned long int len;
  MP_Sample_t* waveBuffer;
  MP_Sample_t* waveHilbertBuffer;
  double dAmpMean;
  double dAmpNyquist;
  double dAmpReal;
  double dAmpHilbert;

  unsigned long int t;

  if ( outBuffer == NULL ) {
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::build_waveform", "The output buffer shall have been allocated before calling this function. Now, it is NULL. Exiting from this function.\n");
    return;
  }

  for ( chanIdx = 0, atomBufferStart = outBuffer; 
	chanIdx < numChans;
	chanIdx++ ) {

    /* Dereference the atom length in the current channel once and for all */
    len = support[chanIdx].len;
    /* Dereference the arguments once and for all */
    dAmpMean       = (double)(   amp[chanIdx] ) * (double) meanPart[chanIdx] / sqrt((double) len);
    dAmpReal       = (double)(   amp[chanIdx] ) * ( double ) realPart[chanIdx] ;
    dAmpHilbert    = (double)(   amp[chanIdx] ) * ( double ) hilbertPart[chanIdx] ;

    if (numChans == anywaveRealTable->numChans) {
      /* multichannel filter */
      waveBuffer = anywaveRealTable->wave[anywaveIdx][chanIdx];
      waveHilbertBuffer = anywaveHilbertTable->wave[anywaveIdx][chanIdx];
    } else {
      /* monochannel filter */
      waveBuffer = anywaveRealTable->wave[anywaveIdx][0];
      waveHilbertBuffer = anywaveHilbertTable->wave[anywaveIdx][0];
    }
    for ( t = 0, atomBuffer = atomBufferStart;
	  t < len; 
	  t++, atomBuffer++, waveBuffer++, waveHilbertBuffer++ ) {
      /* Compute the waveform samples */
      (*atomBuffer) = dAmpMean + (*waveBuffer) * dAmpReal + (*waveHilbertBuffer) * dAmpHilbert;
    }
    if ((len>>2)<<2 == len) {
      dAmpNyquist    = (double)(   amp[chanIdx] ) * (double) nyquistPart[chanIdx] / sqrt((double) len);
      for ( t = 0, atomBuffer = atomBufferStart;
	    t < len; 
	    t+=2, atomBuffer+=2 ) {
	/* Compute the waveform samples */
	(*atomBuffer) += dAmpNyquist;
	(*(atomBuffer+1)) -= dAmpNyquist;
      }      
    }

    atomBufferStart += len;
  }
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Anywave_Hilbert_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType  ) {

  int flag = 0;

  /* YOUR code */
  mp_error_msg( "MP_Anywave_Hilbert_Atom_c::add_to_tfmap","This function is not implemented for anywave atoms.\n" );
  tfmap = NULL;
  if (tfmapType) {;}

  return( flag );
}



int MP_Anywave_Hilbert_Atom_c::has_field( int field ) {

  if ( MP_Anywave_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
  case MP_HILBERT_TABLE_IDX_PROP :  return( MP_TRUE );
  case MP_ANYWAVE_HILBERT_TABLE_PROP :  return( MP_TRUE );
  case MP_REAL_TABLE_IDX_PROP :  return( MP_TRUE );
  case MP_ANYWAVE_REAL_TABLE_PROP :  return( MP_TRUE );
  case MP_MEAN_PART_PROP : return( MP_TRUE );
  case MP_NYQUIST_PART_PROP : return( MP_TRUE );
  case MP_REAL_PART_PROP : return( MP_TRUE );
  case MP_HILBERT_PART_PROP : return( MP_TRUE );
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_Anywave_Hilbert_Atom_c::get_field( int field , int chanIdx ) {

  MP_Real_t x;

  if ( MP_Anywave_Atom_c::has_field( field ) ) return (MP_Anywave_Atom_c::get_field(field,chanIdx));
  else switch (field) {
  case MP_HILBERT_TABLE_IDX_PROP :
    x = (MP_Real_t)(hilbertTableIdx);
    break;
  case MP_REAL_TABLE_IDX_PROP :
    x = (MP_Real_t)(realTableIdx);
    break;
  case MP_MEAN_PART_PROP :
    x = (MP_Real_t)(meanPart[chanIdx]);
    break;
  case MP_NYQUIST_PART_PROP :
    x = (MP_Real_t)(nyquistPart[chanIdx]);
    break;
  case MP_REAL_PART_PROP :
    x = (MP_Real_t)(realPart[chanIdx]);
    break;
  case MP_HILBERT_PART_PROP :
    x = (MP_Real_t)(hilbertPart[chanIdx]);
    break;
  default:
    mp_error_msg( "MP_Anywave_Hilbert_Atom_c::get_field","Unknown field. Returning ZERO." );
    x = 0.0;
  }
  return( x );
}
