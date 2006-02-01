/******************************************************************************/
/*                                                                            */
/*                              anywave_atom.cpp                              */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Thu Nov 03 2005 */
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

/*************************************************/
/*                                               */
/* anywave_atom.cpp: methods for anywave atoms */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "mp_system.h"

//#include <dsp_windows.h>

/* YOUR includes go here */

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;
/********************/
/* Void constructor */
MP_Anywave_Atom_c::MP_Anywave_Atom_c( void )
  :MP_Atom_c() {
    tableIdx = 0;
    anywaveTable = NULL;
    anywaveIdx = 0;
    amp = NULL;
}

/**************************/
/* Specific constructor 1 */
MP_Anywave_Atom_c::MP_Anywave_Atom_c( unsigned short int setNumChans ) 
  :MP_Atom_c(setNumChans) {
    tableIdx = 0;
    anywaveTable = NULL;
    anywaveIdx = 0;
    
    unsigned short int chanIdx;

    /* amp */
    if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t) <= 1.0) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c", "numChans [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the amplitudes array. amp is set to NULL\n", numChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
      amp = NULL;
    } else if ( (amp = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Can't allocate the amp array for a new atom;"
	       " amp stays NULL.\n" );
    }
    
    /* Initialize */
    if ( (amp!=NULL) ) {
      for (chanIdx = 0; chanIdx<numChans; chanIdx++) {
	*(amp  +chanIdx) = 0.0;
      }
    }
    else mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","The parameter arrays"
		  " for the new atom are left un-initialized.\n" );
}

/********************/
/* File constructor */
MP_Anywave_Atom_c::MP_Anywave_Atom_c( FILE *fid, const char mode )
  :MP_Atom_c( fid, mode ) {

  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  double fidAmp;
  unsigned short int readChanIdx, chanIdx;
  MP_Real_t* pAmp;
  MP_Real_t* pAmpEnd;

  extern MP_Anywave_Server_c MP_GLOBAL_ANYWAVE_SERVER;  
  
  switch ( mode ) {

  case MP_TEXT:
    /* Read the filename */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( read_filename_txt(line,"\t\t<par type=\"filename\">%s",str) == false ) ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to read the filename.\n");
    }
    else {
      /* if the table corresponding to filename is already in the anywave
	 server, update tableIdx and anywaveTable. If not, add the table
	 and update tableIdx and anywaveTable */
      /* create a new table */
      tableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( str );
      anywaveTable = MP_GLOBAL_ANYWAVE_SERVER.tables[tableIdx];
    }
    break;

  case MP_BINARY:
    /* Try to read the filename */
    if ( read_filename_bin( fid, str ) == false ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to scan the atom's table filename.\n");
    } else {
      /* if the table corresponding to filename is already in the anywave
	 server, update tableIdx and anywaveTable. If not, add the table
	 and update tableIdx and anywaveTable*/
      /* create a new table */
      tableIdx = MP_GLOBAL_ANYWAVE_SERVER.add( str );      
      anywaveTable = MP_GLOBAL_ANYWAVE_SERVER.tables[tableIdx];
    }
    break;

  default:
    mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Unknown read mode met in MP_Anywave_Atom_c( fid , mode )." );
    break;
  }

  /* Allocate and initialize */
  /* amp */
  if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t) <= 1.0) {
    mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c", "numChans [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the amplitudes array. amp is set to NULL\n", numChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
    amp = NULL;
  } else if ( (amp = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Can't allocate the amp array for a new atom; amp stays NULL.\n" );
  } else {
    /* Initialize */
    pAmpEnd = amp + numChans;
    for (pAmp = amp;
	 pAmp < pAmpEnd;
	 pAmp ++) {
      *pAmp = 0.0;
    }  
  }

  switch ( mode ) {

  case MP_TEXT:
    /* Read the anywave number */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t\t<par type=\"anywaveIdx\">%lu</par>\n", &anywaveIdx ) != 1 ) ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to read the anywave number.\n");      
    } else if ( anywaveIdx >= anywaveTable->numFilters ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Anywave index is bigger than the number of anywaves in the table.\n");
    }
    break;
  case MP_BINARY:
    /* Try to read the anywave number */    
    if ( mp_fread( &anywaveIdx, sizeof(long int), 1, fid ) != 1 ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to scan the atom number.\n");      
    } else if (anywaveIdx >= anywaveTable->numFilters ) {
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Anywave index is bigger than the number of anywaves in the table.\n");
    }
    break;

  default:
    mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Unknown read mode met in MP_Anywave_Atom_c( fid , mode )." );
    break;
  }
  /* Try to read the amp */
  switch (mode ) {
    
  case MP_TEXT:

    for (chanIdx = 0;
	 chanIdx < numChans;
	 chanIdx ++) {
      /* Opening tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<anywavePar chan=\"%hu\">\n", &readChanIdx ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Cannot scan channel index in atom.\n" );
      }
      else if ( readChanIdx != chanIdx ) {
 	mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Potential shuffle in the parameters"
		" of an anywave atom. (Index \"%hu\" read, \"%hu\" expected.)\n",
		readChanIdx, chanIdx );
      }
      /* amp */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t\t<par type=\"amp\">%lg</par>\n", &fidAmp ) != 1 ) ) {
	mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Cannot scan amp on channel %u.\n", chanIdx );
      }
      else {
	*(amp + chanIdx) = (MP_Real_t)fidAmp;
      }
      /* Closing tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( strcmp( str , "\t\t</anywavePar>\n" ) ) ) {
	mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Cannot scan the closing parameter tag"
		" in anywave atom, channel %hu.\n", chanIdx );
      }
    }
    break;
    
  case MP_BINARY:
    /* Try to read the amp */
    if ( mp_fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
 	mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c()","Failed to read the amp array.\n" );     
	pAmpEnd = amp + numChans;
	for ( pAmp = amp;
	      pAmp < pAmpEnd;
	      pAmp ++ ) {
	  *pAmp = 0.0;
	}
    }
    break;
    
  default:
    break;
  }
}


/**************/
/* Destructor */
MP_Anywave_Atom_c::~MP_Anywave_Atom_c() {
  if (amp)   free( amp );
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

/* Test */
bool MP_Anywave_Atom_c::test( char* filename ) {

  unsigned long int sampleIdx;
  unsigned short int chanIdx;
  MP_Sample_t* buffer;

  fprintf( stdout, "\n-- Entering MP_Anywave_Atom_c::test \n" );

  /* add this table to the anywave server */
  MP_GLOBAL_ANYWAVE_SERVER.add(filename);

  /* create an anywave atom corresponding to the first filter of the first table in the anywave server */
  MP_Anywave_Atom_c* atom = new MP_Anywave_Atom_c( MP_GLOBAL_ANYWAVE_SERVER.tables[0]->numChans );
  
  if (atom == NULL) {
    return(false);
  }
  /* set numChans */
  atom->anywaveTable = MP_GLOBAL_ANYWAVE_SERVER.tables[0];
  atom->tableIdx = 0;

  /* set amp to 2.0 on the first channel, 3.0 on the second channel, etc... */
  for (chanIdx = 0;
       chanIdx < atom->numChans;
       chanIdx ++) {
    atom->amp[chanIdx] = (double)(chanIdx + 2);
  }

  /* select the first waveform in the table */
  atom->anywaveIdx = 0;

  /* set the right support */
  /* Initialize the support array */
  atom->totalChanLen = 0;
  for (chanIdx = 0; 
       chanIdx < atom->numChans; 
       chanIdx++) {
    atom->support[chanIdx].pos = 0;
    atom->support[chanIdx].len = atom->anywaveTable->filterLen;
    atom->totalChanLen += atom->support[chanIdx].len;
  }



  /* build the waveform */

  if ((double)MP_MAX_SIZE_T / (double)atom->anywaveTable->filterLen / (double) atom->anywaveTable->numChans / (double)sizeof(MP_Sample_t) <= 1.0) {
    mp_error_msg( "MP_Anywave_Atom_c::test", "filterLen [%lu] . numChans [%lu] . sizeof(MP_Sample_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the buffer. it is set to NULL\n", atom->anywaveTable->filterLen, atom->anywaveTable-> numChans, sizeof(MP_Sample_t), MP_MAX_SIZE_T);
    buffer = NULL;
    return(false);
  } else if ( ( buffer = (MP_Sample_t*) malloc( atom->anywaveTable->filterLen * atom->anywaveTable->numChans * sizeof(MP_Sample_t) ) ) == NULL ) {
    mp_error_msg( "MP_Anywave_Atom_c::test","Can't allocate the buffer before using build_waveform.\n" );
    return(false);
  }
  atom->build_waveform( buffer );
  
  /* print the 10 first samples of each channel of this atom */
  for (chanIdx = 0;
       chanIdx < atom->numChans;
       chanIdx ++) {
    fprintf( stdout, "---- Printing the 10 first samples of channel [%hu] of the waveform obtained by build_waveform:\n", chanIdx );
    
    fprintf( stdout, "------ Required:\n" );
    for ( sampleIdx = 0;
	  (sampleIdx < atom->anywaveTable->filterLen) && (sampleIdx < 10);
	  sampleIdx ++) {
      fprintf( stdout, "%lf ", (chanIdx + 2) * *(atom->anywaveTable->wave[0][0]+sampleIdx));
    }
    fprintf( stdout, "\n------ Result:\n" );    
    for ( sampleIdx = 0;
	  (sampleIdx < atom->anywaveTable->filterLen) && (sampleIdx < 10);
	  sampleIdx ++) {
      fprintf( stdout, "%lf ", *(buffer+sampleIdx));
    }
  }
  /* destroy the atom */
  delete(atom);

  fprintf( stdout, "\n-- Exiting MP_Anywave_Atom_c::test \n" );
  fflush( stdout );
  
  return(true);
}


int MP_Anywave_Atom_c::write( FILE *fid, const char mode ) {
  
  int nItem = 0;
  unsigned short int chanIdx = 0;
  int numChar;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other anywave-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* Filename of the table containing the waveform */
    nItem += fprintf( fid, "\t\t<par type=\"filename\">%s</par>\n", anywaveTable->tableFileName );
    /* print the anywaveIdx, amp */
    nItem += fprintf( fid, "\t\t<par type=\"waveIdx\">%li</par>\n",  anywaveIdx );
    for (chanIdx = 0; 
	 chanIdx < numChans; 
	 chanIdx++) {
      nItem += fprintf( fid, "\t\t<anywavePar chan=\"%u\">\n", chanIdx );
      nItem += fprintf( fid, "\t\t\t<par type=\"amp\">%g</par>\n",   (double)amp[chanIdx] );
      nItem += fprintf( fid, "\t\t</anywavePar>\n" );    
    }
    break;

  case MP_BINARY:
    /* Filename of the table containing the waveform */
    numChar = strlen(anywaveTable->tableFileName)+1;
    nItem += mp_fwrite( &numChar,  sizeof(long int), 1, fid );
    nItem += fprintf( fid, "%s\n", anywaveTable->tableFileName );

    /* Binary parameters */
    nItem += mp_fwrite( &anywaveIdx,  sizeof(long int), 1, fid );
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
char * MP_Anywave_Atom_c::type_name(void) {
  return ("anywave");
}

/**********************/
/* Readable text dump */
int MP_Anywave_Atom_c::info( FILE *fid ) {

  unsigned short int chanIdx = 0;
  int nChar = 0;
  nChar += fprintf( fid, "mplib info -- ANYWAVE ATOM" );
  nChar += fprintf( fid, " [%d] channel(s)\n", numChans );
  nChar += fprintf( fid, "\tFilename %s\tanywaveIdx %li\n", anywaveTable->tableFileName, anywaveIdx );
  for ( chanIdx = 0;
	chanIdx < numChans; 
	chanIdx ++ ) {
    nChar += fprintf( fid, "mplib info -- (%u/%u)\tSupport=", chanIdx+1, numChans );
    nChar += fprintf( fid, " %lu %lu ", support[chanIdx].pos, support[chanIdx].len );
    nChar += fprintf( fid, "\tAmp %g",(double)amp[chanIdx] );
    nChar += fprintf( fid, "\n" );
  }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Anywave_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Sample_t *atomBuffer;
  unsigned short int chanIdx;
  unsigned long int len;
  MP_Sample_t* waveBuffer;
  double dAmp;
  unsigned long int t;

  if ( outBuffer == NULL ) {
    mp_error_msg( "MP_Anywave_Atom_c::build_waveform", "The output buffer shall have been allocated before calling this function. Now, it is NULL. Exiting from this function.\n");
    return;
  }

  for ( chanIdx = 0, atomBuffer = outBuffer; 
	chanIdx < numChans;
	chanIdx++  ) {
    /* Dereference the atom length in the current channel once and for all */
    len = support[chanIdx].len;
    /* Dereference the arguments once and for all */
    dAmp       = (double)(   amp[chanIdx] );

    if (numChans == anywaveTable->numChans) {
      /* multichannel filter */
      waveBuffer = anywaveTable->wave[anywaveIdx][chanIdx];
    } else {
      /* monochannel filter */
      waveBuffer = anywaveTable->wave[anywaveIdx][0];
    }
    for ( t = 0;
	  t < len; 
	  t++, atomBuffer++, waveBuffer++ ) {
      /* Compute the waveform samples */
      (*atomBuffer) = (*waveBuffer) * dAmp;
    }
  }
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Anywave_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType  ) {

  int flag = 0;

  /* YOUR code */
  mp_error_msg( "MP_Anywave_Atom_c::add_to_tfmap","This function is not implemented for anywave atoms.\n" );
  tfmap = NULL;
  if (tfmapType) {;}

  return( flag );
}



int MP_Anywave_Atom_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field) {
  case MP_TABLE_IDX_PROP :  return( MP_TRUE );
  case MP_ANYWAVE_TABLE_PROP :  return( MP_TRUE );
  case MP_ANYWAVE_IDX_PROP :   return( MP_TRUE );
  case MP_AMP_PROP : return( MP_TRUE );
  default:
    return( MP_FALSE );
  }
}

MP_Real_t MP_Anywave_Atom_c::get_field( int field , int chanIdx ) {

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field) {
  case MP_TABLE_IDX_PROP :
    x = (MP_Real_t)(tableIdx);
    break;
  case MP_ANYWAVE_IDX_PROP :
    x = (MP_Real_t)(anywaveIdx);
    break;
  case MP_AMP_PROP :
    x = (MP_Real_t)(amp[chanIdx]);
    break;
  default:
    mp_error_msg( "MP_Anywave_Atom_c::get_field","Unknown field. Returning ZERO." );
    x = 0.0;
  }
  return( x );
}

bool MP_Anywave_Atom_c::read_filename_txt( char* line, char* pattern, char* outputStr) {
  char* pch;
  char tempStr[MP_MAX_STR_LEN];
  if (sscanf( line, pattern, tempStr ) != 1) {
    return(false);
  } else {
    pch = strchr( tempStr, '<' );
    if (pch == NULL) {
      return(false);
    } else {
      strncpy(outputStr,tempStr,pch-tempStr);
      outputStr[pch-tempStr] = '\0';
      return(true);
    }
  }  
}
 
bool MP_Anywave_Atom_c::read_filename_bin( FILE* fid, char* outputStr) {
  int charIdx;
  char tempChar;
  for (charIdx = 0;
       ((charIdx < MP_MAX_STR_LEN) && ((tempChar = getc(fid)) != '\0'));
       charIdx ++) {
    outputStr[charIdx] = tempChar;
  }
  outputStr[charIdx+1] = '\0';
  return(true);
}
 
