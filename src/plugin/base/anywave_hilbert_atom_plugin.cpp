/******************************************************************************/
/*                                                                            */
/*                              anywave_hilbert_atom.cpp                      */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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

/*******************************************************/
/*                                                     */
/* anywave_hilbert_atom.cpp: methods for anywave atoms */
/*                                                     */
/*******************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "anywave_hilbert_atom_plugin.h"


/***************************************************************/
/*                                                             */
/* anywave_hilbert_atom.cpp: methods for anywave hilbert atoms */
/*                                                             */
/***************************************************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/


/************************/
/* Factory function     */
MP_Atom_c  * MP_Anywave_Hilbert_Atom_Plugin_c::anywave_hilbert_atom_create_empty(void)
{
	return new MP_Anywave_Hilbert_Atom_Plugin_c;
}

/*************************/
/* File factory function */
MP_Atom_c* MP_Anywave_Hilbert_Atom_Plugin_c::create( FILE *fid, const char mode )
{
	const char* func = "MP_Anywave_Hilbert_Atom_c::init(fid,mode)";

	MP_Anywave_Hilbert_Atom_Plugin_c* newAtom = NULL;

	// Instantiate and check
	newAtom = new MP_Anywave_Hilbert_Atom_Plugin_c();
	if ( newAtom == NULL )
    {
		mp_error_msg( func, "Failed to create a new atom.\n" );
		return( NULL );
    }

	// Read and check
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
MP_Anywave_Hilbert_Atom_Plugin_c::MP_Anywave_Hilbert_Atom_Plugin_c( void ):MP_Anywave_Atom_Plugin_c()
{
	realPart = NULL;
	hilbertPart = NULL;
	anywaveRealTable = NULL;
	anywaveHilbertTable = NULL;
	realTableIdx = 0;
	hilbertTableIdx = 0;
}

/************************/
/* Global allocations   */
int MP_Anywave_Hilbert_Atom_Plugin_c::alloc_hilbert_atom_param( const MP_Chan_t setNumChans )
{
  if (init_parts())
      return 1;
  return 0;
}


int MP_Anywave_Hilbert_Atom_Plugin_c::init_parts(void)
{
	unsigned short int chanIdx;

	if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t) <= 1.0)
    {
		mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c","numChans [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu].Cannot use malloc for allocating space for the realPart and the hilbertPart array. realPart and hilbertPart are set to NULL\n", numChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
		realPart = NULL;
		hilbertPart = NULL;
		return(1);
    }
	if ( (realPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL )
    {
		mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the realPart array for a new atom; amp stays NULL.\n" );
		return(1);
    }
	if ( (hilbertPart = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL )
    {
		mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Can't allocate the hilbertPart array for a new atom; amp stays NULL.\n" );
		return(1);
    }

	// Initialize
	if ( (realPart!=NULL) )
    {
		for (chanIdx = 0; chanIdx<numChans; chanIdx++)
        {
			*(realPart +chanIdx) = 0.0;
        }
    }
	else
    {
		mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter realPart for the new atom are left un-initialized.\n" );
		return 1;
    }
	if ( (hilbertPart!=NULL) )
    {
		for (chanIdx = 0; chanIdx<numChans; chanIdx++)
        {
			*(hilbertPart +chanIdx) = 0.0;
        }
    }
	else
    {
		mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","The parameter hilbertPart for the new atom are left un-initialized.\n" );
		return 1;
    }
	return 0;
}

int MP_Anywave_Hilbert_Atom_Plugin_c::init_tables( void )
{
	const char	*func = "MP_Anywave_Hilbert_Atom_Plugin_c::init_tables()";
	int			iKeyNameSize = 0;
	char		*szBeforeEncodage;
	char		*szAfterEncodage;

	/* create the real table if needed */
	iKeyNameSize = MPTK_Server_c::get_anywave_server()->get_keyname_size();

	if ( ( szBeforeEncodage = (char*) calloc( iKeyNameSize , sizeof(char) ) ) == NULL )
    {
      mp_error_msg( func,"The string szStrBeforeEncodage cannot be allocated.\n" );
	  return 1;
    }
	if ( ( szAfterEncodage = (char*) calloc( iKeyNameSize , sizeof(char) ) ) == NULL )
    {
      mp_error_msg( func,"The string szStrAfterEncodage cannot be allocated.\n" );
	  return 1;
    }

	// Retrieve the keyName
	memcpy(szBeforeEncodage, MPTK_Server_c::get_anywave_server()->get_keyname( tableIdx ),iKeyNameSize);
	// Encode the szBeforeEncodage to szAfterEncodage
	MPTK_Server_c::get_anywave_server()->encodeMd5( szBeforeEncodage, iKeyNameSize, szAfterEncodage );
	// Get the new encoded string index	
	realTableIdx = MPTK_Server_c::get_anywave_server()->get_index( szAfterEncodage );
	// If the new encoded string does not exist
	if (realTableIdx == MPTK_Server_c::get_anywave_server()->numTables)
    {
      anywaveRealTable = anywaveTable->copy();
      anywaveRealTable->center_and_denyquist();
      anywaveRealTable->normalize();
      anywaveRealTable->set_key_table(szAfterEncodage);
      realTableIdx = MPTK_Server_c::get_anywave_server()->add( anywaveRealTable );
    }
	else
    {
      anywaveRealTable = MPTK_Server_c::get_anywave_server()->tables[realTableIdx];
    }

	// Retrieve the keyName
	memcpy(szBeforeEncodage, szAfterEncodage,iKeyNameSize);
	memset(szAfterEncodage, 0,iKeyNameSize);
	// Encode the szBeforeEncodage to szAfterEncodage
	MPTK_Server_c::get_anywave_server()->encodeMd5( szBeforeEncodage, iKeyNameSize, szAfterEncodage );
	// Get the new encoded string index	
	hilbertTableIdx = MPTK_Server_c::get_anywave_server()->get_index( szAfterEncodage );
	if (hilbertTableIdx == MPTK_Server_c::get_anywave_server()->numTables)
    {
		/* need to create a new table */
		anywaveHilbertTable = anywaveTable->create_hilbert_dual(szAfterEncodage);
		anywaveHilbertTable->normalize();
		hilbertTableIdx = MPTK_Server_c::get_anywave_server()->add( anywaveHilbertTable );
    }
	else
    {
		anywaveHilbertTable = MPTK_Server_c::get_anywave_server()->tables[hilbertTableIdx];
    }

	if (szBeforeEncodage != NULL) 
	{
		free(szBeforeEncodage);
		szBeforeEncodage = NULL;
	}
	if (szAfterEncodage != NULL) 
	{
		free(szAfterEncodage);
		szAfterEncodage = NULL;
	}
	return 0;
}

/********************/
/* File reader      */
int MP_Anywave_Hilbert_Atom_Plugin_c::read( FILE *fid, const char mode )
{
 const char *func = "MP_Anywave_Hilbert_Atom_c::read";
  double fidParam;
  unsigned short int readChanIdx, chanIdx;
  MP_Real_t* pParam;
  MP_Real_t* pParamEnd;
  char* str;

  /* Go up one level */
  if ( MP_Anywave_Atom_Plugin_c::read( fid, mode ) )
    {
      mp_error_msg( func, "Allocation of Anywave Hilbert atom failed at the Anywave atom level.\n" );
      return( 1 );
    }

  if ( ( str = (char*) malloc( MP_MAX_STR_LEN * sizeof(char) ) ) == NULL )
    {
      mp_error_msg( func,"The string str cannot be allocated.\n" );
      return(1);
    }

  /* init tables */
  if ( init_tables() )
    {
      return(1);
    }

  /* Allocate and initialize */
  if ( init_parts() )
    {
      return(1);
    }

  /* Try to read the param */
  switch (mode )
    {

    case MP_TEXT:

      for (chanIdx = 0;
           chanIdx < numChans;
           chanIdx ++)
        {
          /* Opening tag */
          if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
               ( sscanf( str, "\t\t<anywavePar chan=\"%hu\">\n", &readChanIdx ) != 1 ) )
            {
              mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan channel index in atom.\n" );
              return(1);
            }
          else if ( readChanIdx != chanIdx )
            {
              mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Potential shuffle in the parameters"
                            " of an anywave atom. (Index \"%hu\" read, \"%hu\" expected.)\n",
                            readChanIdx, chanIdx );
              return(1);
            }

          /* real part */
          if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
               ( sscanf( str, "\t\t\t<par type=\"realPart\">%lg</par>\n", &fidParam ) != 1 ) )
            {
              mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan real part on channel %u.\n", chanIdx );
              return(1);
            }
          else
            {
              *(realPart + chanIdx) = (MP_Real_t)fidParam;
            }
          /* hilbert part */
          if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
               ( sscanf( str, "\t\t\t<par type=\"hilbertPart\">%lg</par>\n", &fidParam ) != 1 ) )
            {
              mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan hilbert part on channel %u.\n", chanIdx );
              return(1);
            }
          else
            {
              *(hilbertPart + chanIdx) = (MP_Real_t)fidParam;
            }
          /* Closing tag */
          if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
               ( strcmp( str , "\t\t</anywavePar>\n" ) ) )
            {
              mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Cannot scan the closing parameter tag"
                            " in anywave atom, channel %hu.\n", chanIdx );
              return(1);
            }
        }
      break;

    case MP_BINARY:

      /* Try to read the real part */
      if ( mp_fread( realPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans )
        {
          mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the realPart array.\n" );
          pParamEnd = realPart + numChans;
          for ( pParam = realPart;
                pParam < pParamEnd;
                pParam ++ )
            {
              *pParam = 0.0;
            }
          return(1);
        }
      /* Try to read the hilbert part */
      if ( mp_fread( hilbertPart,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans )
        {
          mp_error_msg( "MP_Anywave_Hilbert_Atom_c::MP_Anywave_Hilbert_Atom_c()","Failed to read the hilbertPart array.\n" );
          pParamEnd = hilbertPart + numChans;
          for ( pParam = hilbertPart;
                pParam < pParamEnd;
                pParam ++ )
            {
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
MP_Anywave_Hilbert_Atom_Plugin_c::~MP_Anywave_Hilbert_Atom_Plugin_c()
{
  if (realPart)   free( realPart );
  if (hilbertPart)   free( hilbertPart );
}


/***************************/
/* OUTPUT METHOD           */
/***************************/


int MP_Anywave_Hilbert_Atom_Plugin_c::write( FILE *fid, const char mode )
{

  int nItem = 0;
  unsigned short int chanIdx = 0;

  /* Call the parent's write function */
  nItem += MP_Anywave_Atom_Plugin_c::write( fid, mode );

  /* Print the other anywave-specific parameters */
  switch ( mode )
    {

    case MP_TEXT:
      /* print the ampHilbert */
      for (chanIdx = 0;
           chanIdx < numChans;
           chanIdx++)
        {
          nItem += fprintf( fid, "\t\t<anywavePar chan=\"%u\">\n", chanIdx );
          nItem += fprintf( fid, "\t\t\t<par type=\"realPart\">%g</par>\n",   (double)realPart[chanIdx] );
          nItem += fprintf( fid, "\t\t\t<par type=\"hilbertPart\">%g</par>\n",   (double)hilbertPart[chanIdx] );
          nItem += fprintf( fid, "\t\t</anywavePar>\n" );
        }
      break;

    case MP_BINARY:

      /* Binary parameters */
      if (realPart) nItem += (int)mp_fwrite( realPart, sizeof(MP_Real_t), (size_t)numChans, fid );
      if (hilbertPart) nItem += (int)mp_fwrite( hilbertPart, sizeof(MP_Real_t), (size_t)numChans, fid );
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
const char * MP_Anywave_Hilbert_Atom_Plugin_c::type_name(void)
{
  return("anywavehilbert");
}

/**********************/
/* Readable text dump */
int MP_Anywave_Hilbert_Atom_Plugin_c::info()
{

  unsigned short int chanIdx = 0;
  int nChar = 0;
  nChar += (int)mp_info_msg("ANYWAVE HILBERT ATOM", "[%d] channel(s)\n", numChans );

  nChar += (int)mp_info_msg( "           |-", "\tFilename %s\tfilterIdx %li\n", anywaveTable->tableFileName, anywaveIdx );

  for ( chanIdx = 0; chanIdx < numChans; chanIdx ++ )
    {
      nChar += (int)mp_info_msg( "           |-", "(%u/%u)\tSupport= %lu %lu\tAmp %g\trealPart %g\thilbertPart %g\n",
                            chanIdx+1, numChans, support[chanIdx].pos, support[chanIdx].len,
                            (double)amp[chanIdx], (double)realPart[chanIdx], (double)hilbertPart[chanIdx]);
    }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Anywave_Hilbert_Atom_Plugin_c::build_waveform( MP_Real_t *outBuffer )
{

  MP_Real_t *atomBuffer;
  MP_Real_t *atomBufferStart;
  unsigned short int chanIdx;
  unsigned long int len;
  MP_Real_t* waveBuffer;
  MP_Real_t* waveHilbertBuffer;
  double dAmpReal;
  double dAmpHilbert;

  unsigned long int t;

  if ( outBuffer == NULL )
    {
      mp_error_msg( "MP_Anywave_Hilbert_Atom_c::build_waveform", "The output buffer shall have been allocated before calling this function. Now, it is NULL. Exiting from this function.\n");
      return;
    }

  for ( chanIdx = 0, atomBufferStart = outBuffer;
        chanIdx < numChans;
        chanIdx++ )
    {

      /* Dereference the atom length in the current channel once and for all */
      len = support[chanIdx].len;
      /* Dereference the arguments once and for all */

      dAmpReal       = (double)(   amp[chanIdx] ) * ( double ) realPart[chanIdx] ;
      dAmpHilbert    = (double)(   amp[chanIdx] ) * ( double ) hilbertPart[chanIdx] ;

      if (numChans == anywaveRealTable->numChans)
        {
          /* multichannel filter */
          waveBuffer = anywaveRealTable->wave[anywaveIdx][chanIdx];
          waveHilbertBuffer = anywaveHilbertTable->wave[anywaveIdx][chanIdx];
        }
      else
        {
          /* monochannel filter */
          waveBuffer = anywaveRealTable->wave[anywaveIdx][0];
          waveHilbertBuffer = anywaveHilbertTable->wave[anywaveIdx][0];
        }

      for ( t = 0, atomBuffer = atomBufferStart;
            t < len;
            t++, atomBuffer++, waveBuffer++, waveHilbertBuffer++ )
        {
          /* Compute the waveform samples */
          (*atomBuffer) = (*waveBuffer) * dAmpReal + (*waveHilbertBuffer) * dAmpHilbert;
        }

      atomBufferStart += len;
    }
}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
int MP_Anywave_Hilbert_Atom_Plugin_c::add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType  )
{

  int flag = 0;

  /* YOUR code */
  mp_error_msg( "MP_Anywave_Hilbert_Atom_c::add_to_tfmap","This function is not implemented for anywave atoms.\n" );
  tfmap = NULL;
  if (tfmapType)
    {
      ;
    }

  return( flag );
}



int MP_Anywave_Hilbert_Atom_Plugin_c::has_field( int field )
{

  if ( MP_Anywave_Atom_Plugin_c::has_field( field ) ) return (MP_TRUE);
  else switch (field)
      {
      case MP_HILBERT_TABLE_IDX_PROP :
        return( MP_TRUE );
      case MP_ANYWAVE_HILBERT_TABLE_PROP :
        return( MP_TRUE );
      case MP_REAL_TABLE_IDX_PROP :
        return( MP_TRUE );
      case MP_ANYWAVE_REAL_TABLE_PROP :
        return( MP_TRUE );
      case MP_REAL_PART_PROP :
        return( MP_TRUE );
      case MP_HILBERT_PART_PROP :
        return( MP_TRUE );
      default:
        return( MP_FALSE );
      }
}

MP_Real_t MP_Anywave_Hilbert_Atom_Plugin_c::get_field( int field , MP_Chan_t chanIdx )
{
	const char *func =  "MP_Anywave_Hilbert_Atom_c::get_field";
  MP_Real_t x;
  mp_debug_msg(MP_DEBUG_FUNC_ENTER, func,"Entering\n");

  if ( MP_Anywave_Atom_Plugin_c::has_field( field ) ) return (MP_Anywave_Atom_Plugin_c::get_field(field,chanIdx));
  else switch (field)
      {
      case MP_HILBERT_TABLE_IDX_PROP :
        x = (MP_Real_t)(hilbertTableIdx);
        break;
      case MP_REAL_TABLE_IDX_PROP :
        x = (MP_Real_t)(realTableIdx);
        break;
      case MP_REAL_PART_PROP :
        x = (MP_Real_t)(realPart[chanIdx]);
        break;
      case MP_HILBERT_PART_PROP :
        x = (MP_Real_t)(hilbertPart[chanIdx]);
        break;
      default:
        mp_error_msg( func,"Unknown field %d in atom of type %s. Returning ZERO.\n", field, type_name());
        x = 0.0;
      }
  mp_debug_msg(MP_DEBUG_FUNC_EXIT, func,"Entering\n");

  return( x );
}


/*****************************************************/
/* Registration of new atom (s) in the atoms factory */


DLL_EXPORT void registry(void)
{
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom_empty("anywave",&MP_Anywave_Atom_Plugin_c::anywave_atom_create_empty);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom("anywave",&MP_Anywave_Atom_Plugin_c::create);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom_empty("anywavehilbert",&MP_Anywave_Hilbert_Atom_Plugin_c::anywave_hilbert_atom_create_empty);
  MP_Atom_Factory_c::get_atom_factory()->register_new_atom("anywavehilbert",&MP_Anywave_Hilbert_Atom_Plugin_c::create);
 
}
