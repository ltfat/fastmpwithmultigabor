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
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

/*************************************************/
/*                                               */
/* anywave_atom.cpp: methods for anywave atoms */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "anywave_atom_plugin.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/************************/
/* Factory function     */
MP_Atom_c  * MP_Anywave_Atom_Plugin_c::anywave_atom_create_empty(void){
return new MP_Anywave_Atom_Plugin_c;
}


/*************************/
/* File factory function */
MP_Atom_c* MP_Anywave_Atom_Plugin_c::create( FILE *fid, const char mode )
{

  const char* func = "MP_Anywave_Atom_c::init(fid,mode)";

  MP_Anywave_Atom_Plugin_c* newAtom = NULL;

  /* Instantiate and check */
  newAtom = new MP_Anywave_Atom_Plugin_c();
  if ( newAtom == NULL )
    {
      mp_error_msg( func, "Failed to create a new atom.\n" );
      return( NULL );
    }

  /* Read and check */
  if ( newAtom->read( fid, mode ) )
    {
      mp_error_msg( func, "Failed to read the new Anywave atom.\n" );
      delete( newAtom );
      return( NULL );
    }

  return( (MP_Atom_c*)newAtom );
}


/********************/
/* Void constructor */
MP_Anywave_Atom_Plugin_c::MP_Anywave_Atom_Plugin_c( void )
    :MP_Atom_c()
{
  tableIdx = 0;
  anywaveTable = NULL;
  anywaveIdx = 0;
}

/********************/
/* File reader      */
int MP_Anywave_Atom_Plugin_c::read( FILE *fid, const char mode )
{

  const char* func = "MP_Anywave_Atom_c::read(fid,mode)";
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];

  /* Go up one level */
  if ( MP_Atom_c::read( fid, mode ) )
    {
      mp_error_msg( func, "Reading of Anywave atom fails at the generic atom level.\n" );
      return( 1 );
    }

  /* Read at the local level */
  switch ( mode )
    {

    case MP_TEXT:
      /* Read the filename */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
           ( read_filename_txt(line,"\t\t<par type=\"filename\">%s",str) == false ) )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to read the filename.\n");
        }
      else
        {
          /* if the table corresponding to filename is already in the anywave
          server, update tableIdx and anywaveTable. If not, add the table
          and update tableIdx and anywaveTable */
          /* create a new table */
          tableIdx = MPTK_Server_c::get_anywave_server()->add( str );
          anywaveTable = MPTK_Server_c::get_anywave_server()->tables[tableIdx];
        }
      /* Read the anywave number */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
           ( sscanf( line, "\t\t<par type=\"filterIdx\">%lu</par>\n", &anywaveIdx ) != 1 ) )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c", "Failed to read the anywave number.\n");
        }
      else if ( anywaveIdx >= anywaveTable->numFilters )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Anywave index is bigger"
                        " than the number of anywaves in the table.\n");
        }
      break;

    case MP_BINARY:
      /* Try to read the filename */
      if ( read_filename_bin( fid, str ) == false )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to scan the atom's table filename.\n");
        }
      else
        {
          /* if the table corresponding to filename is already in the anywave
          server, update tableIdx and anywaveTable. If not, add the table
          and update tableIdx and anywaveTable*/
          /* create a new table */
          tableIdx = MPTK_Server_c::get_anywave_server()->add( str );
          anywaveTable = MPTK_Server_c::get_anywave_server()->tables[tableIdx];
        }
      /* Try to read the anywave number */
      if ( mp_fread( &anywaveIdx, sizeof(long int), 1, fid ) != 1 )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Failed to scan the atom number.\n");
        }
      else if (anywaveIdx >= anywaveTable->numFilters )
        {
          mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Anywave index is bigger than"
                        " the number of anywaves in the table.\n");
        }
      break;

    default:
      mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c","Unknown read mode met"
                    " in MP_Anywave_Atom_c( fid , mode )." );
      break;
    }

  return( 0 );
}


/**************/
/* Destructor */
MP_Anywave_Atom_Plugin_c::~MP_Anywave_Atom_Plugin_c()
{}


/***************************/
/* OUTPUT METHOD           */
/***************************/

/* Test */
bool MP_Anywave_Atom_Plugin_c::test( char* filename )
{

  unsigned long int sampleIdx;
  MP_Chan_t chanIdx;
  MP_Real_t* buffer;

  fprintf( stdout, "\n-- Entering MP_Anywave_Atom_c::test \n" );

  /* add this table to the anywave server */
  MPTK_Server_c::get_anywave_server()->add(filename);

  /* create an anywave atom corresponding to the first filter of the first table in the anywave server */
  MP_Anywave_Atom_Plugin_c* atom = NULL;
  MP_Atom_c* (*emptyAtomCreator)( void ) = MP_Atom_Factory_c::get_atom_factory()->get_empty_atom_creator("anywave");
  if (NULL == emptyAtomCreator) {
  mp_error_msg( "MP_Anywave_Atom_c::test", "Anywave atom is not registred in the atom factory" );
  return( false );
  }
  
  if ( (atom =  (MP_Anywave_Atom_Plugin_c *)(*emptyAtomCreator)())  == NULL )
    {
      mp_error_msg( "MP_Anywave_Atom_c::test", "Can't create a new Gabor atom in create_atom()."
                    " Returning NULL as the atom reference.\n" );
      return( false );
    }
  if ( atom->alloc_atom_param(MPTK_Server_c::get_anywave_server()->tables[0]->numChans) )
    {
      mp_error_msg( "MP_Anywave_Atom_c::test", "Failed to allocate some vectors in the new Gabor atom. Returning a NULL atom.\n" );
      return( false );

    }

  if (atom == NULL)
    {
      return(false);
    }
  /* set numChans */
  atom->anywaveTable = MPTK_Server_c::get_anywave_server()->tables[0];
  atom->tableIdx = 0;

  /* set amp to 2.0 on the first channel, 3.0 on the second channel, etc... */
  for (chanIdx = 0;
       chanIdx < atom->numChans;
       chanIdx ++)
    {
      atom->amp[chanIdx] = (double)(chanIdx + 2);
    }

  /* select the first waveform in the table */
  atom->anywaveIdx = 0;

  /* set the right support */
  /* Initialize the support array */
  atom->totalChanLen = 0;
  for (chanIdx = 0;
       chanIdx < atom->numChans;
       chanIdx++)
    {
      atom->support[chanIdx].pos = 0;
      atom->support[chanIdx].len = atom->anywaveTable->filterLen;
      atom->totalChanLen += atom->support[chanIdx].len;
    }



  /* build the waveform */

if ((double)MP_MAX_SIZE_T / (double)atom->anywaveTable->filterLen / (double) atom->anywaveTable->numChans / (double)sizeof(MP_Real_t)
 <= 1.0)
    {
      mp_error_msg( "MP_Anywave_Atom_c::test", "filterLen [%lu] . numChans [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for the buffer. it is set to NULL\n", atom->anywaveTable->filterLen, atom->anywaveTable-> numChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
      buffer = NULL;
      return(false);
    }
  else if ( ( buffer = (MP_Real_t*) malloc( atom->anywaveTable->filterLen * atom->anywaveTable->numChans * sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Atom_c::test","Can't allocate the buffer before using build_waveform.\n" );
      return(false);
    }
  atom->build_waveform( buffer );

  /* print the 10 first samples of each channel of this atom */
  for (chanIdx = 0;
       chanIdx < atom->numChans;
       chanIdx ++)
    {
      fprintf( stdout, "---- Printing the 10 first samples of channel [%hu] of the waveform obtained by build_waveform:\n", chanIdx );

      fprintf( stdout, "------ Required:\n" );
      for ( sampleIdx = 0;
            (sampleIdx < atom->anywaveTable->filterLen) && (sampleIdx < 10);
            sampleIdx ++)
        {
          fprintf( stdout, "%lf ", (chanIdx + 2) * *(atom->anywaveTable->wave[0][0]+sampleIdx));
        }
      fprintf( stdout, "\n------ Result:\n" );
      for ( sampleIdx = 0;
            (sampleIdx < atom->anywaveTable->filterLen) && (sampleIdx < 10);
            sampleIdx ++)
        {
          fprintf( stdout, "%lf ", *(buffer+sampleIdx));
        }
    }
  /* destroy the atom */
  delete(atom);

  fprintf( stdout, "\n-- Exiting MP_Anywave_Atom_c::test \n" );
  fflush( stdout );

  return(true);
}


int MP_Anywave_Atom_Plugin_c::write( FILE *fid, const char mode )
{

  int nItem = 0;
  unsigned long int numChar;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other anywave-specific parameters */
  switch ( mode )
    {

    case MP_TEXT:
      /* Filename of the table containing the waveform */
      nItem += fprintf( fid, "\t\t<par type=\"filename\">%s</par>\n", anywaveTable->tableFileName );
      /* print the anywaveIdx */
      nItem += fprintf( fid, "\t\t<par type=\"filterIdx\">%li</par>\n",  anywaveIdx );
      break;

    case MP_BINARY:
      /* Filename of the table containing the waveform */
      numChar = (unsigned long int) strlen(anywaveTable->tableFileName)+1;
      nItem += mp_fwrite( &numChar,  sizeof(unsigned long int), 1, fid );
      nItem += fprintf( fid, "%s\n", anywaveTable->tableFileName );

      /* Binary parameters */
      nItem += mp_fwrite( &anywaveIdx,  sizeof(unsigned long int), 1, fid );
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
char * MP_Anywave_Atom_Plugin_c::type_name(void)
{
  return ("anywave");
}

/**********************/
/* Readable text dump */
int MP_Anywave_Atom_Plugin_c::info( FILE *fid )
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
int MP_Anywave_Atom_Plugin_c::info()
{

  MP_Chan_t chanIdx = 0;
  int nChar = 0;
  nChar += mp_info_msg( "ANYWAVE ATOM", "[%d] channel(s)\n", numChans );
  nChar += mp_info_msg( "           |-", "\tFilename %s\tfilterIdx %li\n",
                        anywaveTable->tableFileName, anywaveIdx );
  for ( chanIdx = 0;
        chanIdx < numChans;
        chanIdx ++ )
    {
      nChar += mp_info_msg( "           |-", "(%u/%u)\tSupport= %lu %lu\tAmp %g\n",
                            chanIdx+1, numChans, support[chanIdx].pos, support[chanIdx].len,
                            (double)amp[chanIdx] );
    }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Anywave_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer )
{

  MP_Real_t *atomBuffer;
  MP_Chan_t chanIdx;
  unsigned long int len;
  MP_Real_t* waveBuffer;
  double dAmp;
  unsigned long int t;

  if ( outBuffer == NULL )
    {
      mp_error_msg( "MP_Anywave_Atom_c::build_waveform", "The output buffer shall have been allocated before calling this function. Now, it is NULL. Exiting from this function.\n");
      return;
    }

  for ( chanIdx = 0, atomBuffer = outBuffer;
        chanIdx < numChans;
        chanIdx++  )
    {
      /* Dereference the atom length in the current channel once and for all */
      len = support[chanIdx].len;
      /* Dereference the arguments once and for all */
      dAmp       = (double)(   amp[chanIdx] );

      if (numChans == anywaveTable->numChans)
        {
          /* multichannel filter */
          waveBuffer = anywaveTable->wave[anywaveIdx][chanIdx];
        }
      else
        {
          /* monochannel filter */
          waveBuffer = anywaveTable->wave[anywaveIdx][0];
        }
      for ( t = 0;
            t < len;
            t++, atomBuffer++, waveBuffer++ )
        {
          /* Compute the waveform samples */
          (*atomBuffer) = (*waveBuffer) * dAmp;
        }
    }
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Anywave_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType  )
{

  int flag = 0;

  /* YOUR code */
  mp_error_msg( "MP_Anywave_Atom_c::add_to_tfmap","This function is not implemented for anywave atoms.\n" );
  tfmap = NULL;
  if (tfmapType)
    {
      ;
    }

  return( flag );
}



int MP_Anywave_Atom_Plugin_c::has_field( int field )
{

  if ( MP_Atom_c::has_field( field ) ) return (MP_TRUE);
  else switch (field)
      {
      case MP_TABLE_IDX_PROP :
        return( MP_TRUE );
      case MP_ANYWAVE_TABLE_PROP :
        return( MP_TRUE );
      case MP_ANYWAVE_IDX_PROP :
        return( MP_TRUE );
      default:
        return( MP_FALSE );
      }
}

MP_Real_t MP_Anywave_Atom_Plugin_c::get_field( int field , MP_Chan_t chanIdx )
{

  MP_Real_t x;

  if ( MP_Atom_c::has_field( field ) ) return (MP_Atom_c::get_field(field,chanIdx));
  else switch (field)
      {
      case MP_TABLE_IDX_PROP :
        x = (MP_Real_t)(tableIdx);
        break;
      case MP_ANYWAVE_IDX_PROP :
        x = (MP_Real_t)(anywaveIdx);
        break;
      default:
        mp_error_msg( "MP_Anywave_Atom_c::get_field","Unknown field. Returning ZERO." );
        x = 0.0;
      }
  return( x );
}

bool MP_Anywave_Atom_Plugin_c::read_filename_txt( char* line, char* pattern, char* outputStr)
{
  char* pch;
  char tempStr[MP_MAX_STR_LEN];
  if (sscanf( line, pattern, tempStr ) != 1)
    {
      return(false);
    }
  else
    {
      pch = strchr( tempStr, '<' );
      if (pch == NULL)
        {
          return(false);
        }
      else
        {
          strncpy(outputStr,tempStr,pch-tempStr);
          outputStr[pch-tempStr] = '\0';
          return(true);
        }
    }
}

bool MP_Anywave_Atom_Plugin_c::read_filename_bin( FILE* fid, char* outputStr)
{

  size_t numChar;
  fread(&numChar, sizeof(unsigned long int), 1, fid );
  fread(outputStr, sizeof(char), numChar, fid);
  outputStr[numChar-1] = '\0';
  return(true);
}



