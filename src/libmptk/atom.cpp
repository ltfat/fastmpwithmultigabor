/******************************************************************************/
/*                                                                            */
/*                                atom.cpp                                    */
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

/******************************************/
/*                                        */
/* atoms.cpp: methods for class MP_Atom_c */
/*                                        */
/******************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Atom_c::MP_Atom_c( void ) {
  numChans = 0;
  support  = NULL;
  numSamples = 0;
  amp = NULL;
  totalChanLen = 0;
}


/***********************/
/* Internal allocation */
int MP_Atom_c::local_alloc( const MP_Chan_t setNChan ) {

  const char* func = "MP_Atom_c::internal_alloc(numChan)";

  /* Check the allocation size */
  if ((double)MP_MAX_SIZE_T / (double)setNChan / (double)sizeof(MP_Real_t) <= 1.0) {
    mp_error_msg( "MP_Anywave_Atom_c::MP_Anywave_Atom_c", "numChans [%lu] x sizeof(MP_Real_t) [%lu]"
		  " is greater than the maximum value for a size_t [%lu]. Cannot use calloc"
		  " for allocating space for the arrays. The arrays stay NULL.\n",
		  setNChan, sizeof(MP_Real_t), MP_MAX_SIZE_T);
    return( 1 );
  }

  /* Allocate the support array */
  if ( ( support = (MP_Support_t*) calloc( setNChan, sizeof(MP_Support_t) )) == NULL ) {
    mp_warning_msg( "MP_Atom_c::internal_alloc(numChans)", "Can't allocate support array"
		    " in atom with [%u] channels. Support array and param array"
		    " are left NULL.\n", setNChan );
    return( 1 );
  }
  else numChans = setNChan;

  /* Allocate the amp array */
  if ( (amp = (MP_Real_t*)calloc( numChans, sizeof(MP_Real_t)) ) == NULL ) {
    mp_warning_msg( func, "Failed to allocate the amp array for a new atom;"
		    " amp stays NULL.\n" );
    return( 1 );
  }

  return( 0 );
}


/************************/
/* Global allocations   */
int MP_Atom_c::global_alloc( const MP_Chan_t setNChan ) {

  const char* func = "MP_Atom_c::global_alloc(numChans)";

  /* Alloc at local level */
  if ( local_alloc( setNChan ) ) {
    mp_error_msg( func, "Allocation of generic atom failed at the local level.\n" );
    return( 1 );
  }

  return( 0 );
}


/********************/
/* File reader      */
int MP_Atom_c::read( FILE *fid, const char mode ) {

  const char* func = "MP_Atom_c::read(fid,mode)";
  unsigned long int nItem = 0;
  char str[MP_MAX_STR_LEN];
  double fidAmp;
  MP_Chan_t i, iRead;
  unsigned long int val;

  /* Read numChans */
  switch ( mode ) {

  case MP_TEXT:
    if ( fscanf( fid, "\t\t<par type=\"numChans\">%hu</par>\n", &numChans ) != 1 ) {
      mp_error_msg( func, "Cannot scan numChans.\n");
      return( 1 );
    }
    break;

  case MP_BINARY:
    if ( mp_fread( &numChans,   sizeof(int), 1, fid ) != 1 ) {
      mp_error_msg( func, "Cannot read numChans.\n");
      return( 1 );
    }
    break;

  default:
    mp_error_msg( func, "Unknown mode in file reader.\n");
    return( 1 );
    break;
  }

  /* Allocate the storage space... */
  if ( local_alloc( numChans ) ) {
    mp_error_msg( func, "Failed to allocate some vectors in the new atom.\n" );
    return( 1 );
  }

  /* ... and upon success, read the support and amp information */
  switch ( mode ) {
    
  case MP_TEXT:
    /* Support */
    for ( i=0, nItem = 0; i<(unsigned int)numChans; i++ ) {
      fscanf( fid, "\t\t<support chan=\"%hu\">", &iRead );
      nItem += fscanf( fid, "<p>%lu</p><l>%lu</l></support>\n",
		       &(support[i].pos), &(support[i].len) );
      if ( iRead != i ) {
	mp_warning_msg( func, "Supports may be shuffled. "
			"(Index \"%u\" read where \"%u\" was expected).\n",
			iRead, i );
      }
    }
    /* Amp */
    for (i = 0; i<numChans; i++) {
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<par type=\"amp\" chan=\"%hu\">%lg</par>\n", &iRead,&fidAmp ) != 2 ) ) {
	mp_error_msg( func, "Cannot scan the amplitude on channel %hu.\n", i );
	return( 1 );

      } else *(amp+i) = (MP_Real_t)fidAmp;

      if ( iRead != i ) {
 	mp_warning_msg( func, "Potential shuffle in the amplitudes"
			" of a generic atom. (Index \"%hu\" read, \"%hu\" expected.)\n",
			iRead, i );
      }
    }
    break;
    
  case MP_BINARY:
    /* Support */
    for ( i=0, nItem = 0; i<(unsigned int)numChans; i++ ) {
      nItem += mp_fread( &(support[i].pos), sizeof(unsigned long int), 1, fid );
      nItem += mp_fread( &(support[i].len), sizeof(unsigned long int), 1, fid );
    }
    /* Amp */
    if ( mp_fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
      mp_error_msg( func, "Failed to read the amp array.\n" );
      return( 1 );
    }
    break;
    
  default:
    break;
  }

  /* Check the support information */
  if ( nItem != ( 2 * (unsigned long int)( numChans ) ) ) {
    mp_error_msg( func, "Problem while reading the supports :"
		  " %lu read, %lu expected.\n",
		  nItem, 2 * (unsigned long int )( numChans ) );
    return( 1 );
  }
  
  /* Compute the totalChanLen and the numSamples */
  for ( i=0, totalChanLen = 0; i<(unsigned int)numChans; i++ ) {
    val = support[i].pos + support[i].len;
    if (numSamples < val ) numSamples = val;
    totalChanLen += support[i].len;
  }

  return( 0 );
}

/**************/
/* Destructor */
MP_Atom_c::~MP_Atom_c() {
  if (support) free(support);
  if (amp) free(amp);
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Atom_c::write( FILE *fid, const char mode ) {

  MP_Chan_t i;
  int nItem = 0;
  
  switch ( mode ) {

  case MP_TEXT:
    /* numChans */
    nItem += fprintf( fid, "\t\t<par type=\"numChans\">%d</par>\n", numChans );
    /* Support */
    for ( i=0; i<(unsigned int)numChans; i++ )
      nItem += fprintf( fid, "\t\t<support chan=\"%u\"><p>%lu</p><l>%lu</l></support>\n",
			i, support[i].pos,support[i].len );
    /* Amp */
    for (i = 0; i<numChans; i++) {
      nItem += fprintf(fid, "\t\t<par type=\"amp\" chan=\"%hu\">%lg</par>\n", i, (double)amp[i]);
    }
    break;

  case MP_BINARY:
    /* numChans */
    nItem += mp_fwrite( &numChans, sizeof(int), 1, fid );
    /* Support */
    for ( i=0; i<(unsigned int)numChans; i++ ) {
      nItem += mp_fwrite( &(support[i].pos), sizeof(unsigned long int), 1, fid );
      nItem += mp_fwrite( &(support[i].len), sizeof(unsigned long int), 1, fid );
    }
    /* Amp */
    nItem += mp_fwrite( amp,   sizeof(MP_Real_t), numChans, fid );
    break;

  default:
    mp_warning_msg( "MP_Atom_c::write()", "Unknown write mode. No output written.\n" );
    nItem = 0;
    break;
  }

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/***************/
/* Name output */
char * MP_Atom_c::type_name( void ) {
  return ("base_atom_class");
}


/**********************************************/
/* Substract / add an atom from / to signals. */
void MP_Atom_c::substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd ) {

  const char* func = "MP_Atom_c::substract_add(...)";
  MP_Sample_t *sigIn;
  MP_Chan_t chanIdx;
  unsigned int t;
  double sigEBefAdd = 0.0;
  double sigEAftAdd = 0.0;
  double sigEBefSub = 0.0;
  double sigEAftSub = 0.0;
  double sigVal;

  MP_Sample_t totalBuffer[totalChanLen];
  MP_Sample_t *atomIn;
  MP_Sample_t *ps;
  MP_Sample_t *pa;
  unsigned long int len;
  unsigned long int pos;
  unsigned long int tmpLen;

  /* Check that the addition / substraction can take place :
     the signal and atom should have the same number of channels */
  if ( ( sigSub ) && ( sigSub->numChans != numChans ) ) {
    mp_error_msg( func, "Incompatible number of channels between the atom and the subtraction"
		  " signal. Returning without any addition or subtraction.\n" );
    return;
  }
  if ( ( sigAdd ) && ( sigAdd->numChans != numChans ) ) {
    mp_error_msg( func, "Incompatible number of channels between the atom and the addition"
		  " signal. Returning without any addition or subtraction.\n" );
    return;
  }

  /* build the atom waveform */
  build_waveform( totalBuffer );
  
  /* loop on channels, seeking the right location in the totalBuffer */
  for ( chanIdx = 0 , atomIn = totalBuffer; chanIdx < numChans; chanIdx++ ) {

    /* Dereference the atom support in the current channel once and for all */
    len = support[chanIdx].len;
    pos = support[chanIdx].pos;

    /* SUBTRACT the atomic waveform from the first signal */
    if ( (sigSub) && (pos < sigSub->numSamples) ) {

      /* Avoid to write outside of the signal array */
      //assert( (pos + len) <= sigSub->numSamples );
      tmpLen = sigSub->numSamples - pos;
      tmpLen = ( len < tmpLen ? len : tmpLen ); /* min( len, tmpLen ) */

      /* Seek the right location in the signal */
      sigIn  = sigSub->channel[chanIdx] + pos;

      /* Waveform SUBTRACTION */
      for ( t = 0,   ps = sigIn, pa = atomIn;
	    t < tmpLen;
	    t++,     ps++,       pa++ ) {
	/* Dereference the signal value */
	sigVal   = (double)(*ps);
	/* Accumulate the energy before the subtraction */
	sigEBefSub += (sigVal*sigVal);
	/* Subtract the atom sample from the signal sample */
	sigVal   = sigVal - *pa;
	/* Accumulate the energy after the subtraction */
	sigEAftSub += (sigVal*sigVal);
	/* Record the new signal sample value */
	*ps = (MP_Sample_t)(sigVal);
      }

    } /* end SUBTRACT */
    
    /* ADD the atomic waveform to the second signal */
    if ( (sigAdd) && (pos < sigAdd->numSamples) ) {

      /* Avoid to write outside of the signal array */
      //assert( (pos + len) <= sigAdd->numSamples );
      tmpLen = sigAdd->numSamples - pos;
      tmpLen = ( len < tmpLen ? len : tmpLen ); /* min( len, tmpLen ) */

      /* Seek the right location in the signal */
      sigIn  = sigAdd->channel[chanIdx] + pos;

      /* Waveform ADDITION */
      for ( t = 0,   ps = sigIn, pa = atomIn;
	    t < len;
	    t++,     ps++,       pa++ ) {
	/* Dereference the signal value */
	sigVal   = (double)(*ps);
	/* Accumulate the energy before the subtraction */
	sigEBefAdd += (sigVal*sigVal);
	/* Add the atom sample to the signal sample */
	sigVal   = sigVal + *pa;
	/* Accumulate the energy after the subtraction */
	sigEAftAdd += (sigVal*sigVal);
	/* Record the new signal sample value */
	*ps = (MP_Sample_t)(sigVal);
      }

    } /* end ADD */

    /* Go to the next channel */
    atomIn += len;

  } /* end for chanIdx */

  /* Update the energies of the signals */
  if ( sigSub ) sigSub->energy = sigSub->energy - sigEBefSub + sigEAftSub;
  if ( sigAdd ) sigAdd->energy = sigAdd->energy - sigEBefAdd + sigEAftAdd;

}


/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   across all channels */
int MP_Atom_c::satisfies( int field, int test, MP_Real_t val ) {
  
  MP_Chan_t chanIdx;
  int retVal = MP_TRUE;
  
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ ) {
    retVal = ( retVal && satisfies( field, test, val, chanIdx ) );
  }
  
  return( retVal );
}


/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   along one channel */
int MP_Atom_c::satisfies( int field, int test, MP_Real_t val, MP_Chan_t chanIdx ) {
  
  const char* func = "MP_Atom_c::satisfies(...)";
  MP_Real_t x;
  int has = has_field ( field );
  
  if ( test == MP_HAS ){
    return ( has );
  }
  else {
    if ( has == MP_FALSE ) {
      mp_warning_msg( func, "Unknown field. Returning TRUE." );
      return( MP_TRUE );
    } else {
      x = (MP_Real_t) get_field( field , chanIdx);
      switch ( test ) {
      case MP_SUPER:
	return( x > val );
      case MP_SUPEQ:
	return( x >= val );
      case MP_EQ:
	return( x == val );
      case MP_INFEQ:
	return( x <= val );
      case MP_INFER:
	return( x < val );
      default :
	mp_warning_msg( func, "Unknown test. Returning TRUE." );
	return( MP_TRUE );
      }
    }
  }
}

int MP_Atom_c::has_field( int field ) {
  switch (field) {
  case MP_LEN_PROP :   return( MP_TRUE );
  case MP_POS_PROP :   return( MP_TRUE );
  case MP_AMP_PROP :   return( MP_TRUE );
  default : return( MP_FALSE );
  }
}

MP_Real_t MP_Atom_c::get_field( int field , MP_Chan_t chanIdx ) {
  MP_Real_t x;
  switch (field) {
  case MP_LEN_PROP :
    x = (MP_Real_t)(support[chanIdx].len);
    break;
  case MP_POS_PROP :
    x = (MP_Real_t)(support[chanIdx].pos);
    break;
  case MP_AMP_PROP :
    x = amp[chanIdx];
    break;
  default :
    x = (MP_Real_t)0.0;
  }
  return(x);
}


MP_Real_t MP_Atom_c::dist_to_tfpoint( MP_Real_t /* time */, MP_Real_t /* freq */, MP_Chan_t /* chanIdx */)
{
  return(1e6);
}
