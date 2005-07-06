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
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 13:38:01 $
 * $Revision: 1.2 $
 *
 */

/******************************************/
/*                                        */
/* atoms.cpp: methods for class MP_Atom_c */
/*                                        */
/******************************************/

#include "mptk.h"
#include "system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Atom_c::MP_Atom_c( void ) {
  numChans = 0;
  support  = NULL;
  totalChanLen = 0;
}

/*************************/
/* Constructor/allocator */
MP_Atom_c::MP_Atom_c( const unsigned int setNChan ) {

  alloc_atom( setNChan );

}

/********************/
/* File constructor */
MP_Atom_c::MP_Atom_c( FILE *fid, const char mode ) {

  unsigned long int nItem = 0;
  unsigned int i, iRead;

  /* Read numChans */
  switch ( mode ) {

  case MP_TEXT:
    if ( fscanf( fid, "\t\t<par type=\"numChans\">%d</par>\n", &numChans ) != 1 ) {
      fprintf(stderr, "mplib warning -- MP_Atom_c(file) - Cannot scan numChans. Atom will remain void.\n");
      numChans = 0;
    }
    break;

  case MP_BINARY:
    if ( fread( &numChans,   sizeof(int), 1, fid ) != 1 ) {
      fprintf(stderr, "mplib warning -- MP_Atom_c(file) - Cannot read numChans. Atom will remain void.\n");
      numChans = 0;
    }
    break;

  default:
    fprintf(stderr, "mplib warning -- MP_Atom_c(file) - Bad mode in file-atom contructor. Atom will remain void.\n");
    numChans = 0;
    break;
  }

  /* Allocate the storage space... */
  if ( alloc_atom( numChans ) ) {

    /* ... and upon success, read the support information */
    switch ( mode ) {
      
    case MP_TEXT:
      for ( i=0, nItem = 0; i<(unsigned int)numChans; i++ ) {
	fscanf( fid, "\t\t<support chan=\"%u\">", &iRead );
	nItem += fscanf( fid, "<p>%lu</p><l>%lu</l></support>\n",
			 &(support[i].pos), &(support[i].len) );
	if ( iRead != i ) {
	  fprintf(stderr, "mplib warning -- MP_Atom_c(file) - Supports may be shuffled. "
		  "(Index \"%u\" read where \"%u\" was expected).\n",
		  iRead, i );
	}
      }
      break;
      
    case MP_BINARY:
      for ( i=0, nItem = 0; i<(unsigned int)numChans; i++ ) {
	nItem += fread( &(support[i].pos), sizeof(unsigned long int), 1, fid );
	nItem += fread( &(support[i].len), sizeof(unsigned long int), 1, fid );
      }
      break;
      
    default:
      break;
    }
    /* Check the support information */
    if ( nItem != ( 2 * (unsigned long int)( numChans ) ) ) {
      fprintf(stderr, "mplib warning -- MP_Atom_c(file) - Problem while reading the supports :"
	      " %lu read, %lu expected.\n", nItem, 2 * (unsigned long int )( numChans ) );
    }

    /* Compute the totalChanLen */
    for ( i=0, totalChanLen = 0; i<(unsigned int)numChans; i++ ) {
      totalChanLen += support[i].len;
    }
  }
  /* If allocation failed, do nothing and support will stay NULL */

}

/**************/
/* Destructor */
MP_Atom_c::~MP_Atom_c() {
  if (support) free(support);
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Atom_c::write( FILE *fid, const char mode ) {

  unsigned int i;
  int nItem = 0;
  
  switch ( mode ) {

  case MP_TEXT:
    nItem += fprintf( fid, "\t\t<par type=\"numChans\">%d</par>\n", numChans );
    for ( i=0; i<(unsigned int)numChans; i++ )
      nItem += fprintf( fid, "\t\t<support chan=\"%u\"><p>%lu</p><l>%lu</l></support>\n",
			i, support[i].pos,support[i].len );
    break;

  case MP_BINARY:
    nItem += fwrite( &numChans, sizeof(int), 1, fid );
    for ( i=0; i<(unsigned int)numChans; i++ ) {
      nItem += fwrite( &(support[i].pos), sizeof(unsigned long int), 1, fid );
      nItem += fwrite( &(support[i].len), sizeof(unsigned long int), 1, fid );
    }
    break;

  default:
    fprintf(stderr, "mplib warning -- MP_Atom_c::write() - Unknown write mode. No output written.\n" );
    nItem = 0;
    break;
  }

  return( nItem );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/***********************/
/* Internal allocation */
int MP_Atom_c::alloc_atom( const unsigned int setNChan ) {

  unsigned int i;

  /* Allocate the support array */
  if ( ( support = (MP_Support_t*) malloc( setNChan*sizeof(MP_Support_t) )) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Atom_c::alloc_atom() - Can't allocate support array in atom with [%u] channels. "
             "Support array and param array are left NULL.\n", setNChan );
    numChans = 0;
    return( 0 );
  }
  else {
    /* Initialize the support array */
    for (i = 0; i < setNChan; i++) {
      support[i].pos = 0;
      support[i].len = 0;
    }
    numChans = setNChan;
  }
  totalChanLen = 0;

  return( 1 );
}


/***************/
/* Name output */
char * MP_Atom_c::type_name( void ) {
  return ("base_atom_class");
}


/**********************************************/
/* Substract / add an atom from / to signals. */
void MP_Atom_c::substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd ) {

  MP_Sample_t *sigIn;
  int chanIdx;
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

  /* Check that the addition / substraction can take place :
     the signal and atom should have the same number of channels */
#ifndef NDEBUG
  if ( sigSub != NULL ) assert( sigSub->numChans == numChans );
  if ( sigAdd != NULL ) assert( sigAdd->numChans == numChans );
#endif

  /* build the atom waveform */
  build_waveform( totalBuffer );
  
  /* loop on channels, seeking the right location in the totalBuffer */
  for ( chanIdx = 0 , atomIn = totalBuffer; chanIdx < numChans; chanIdx++ ) {

    /* Dereference the atom support in the current channel once and for all */
    len = support[chanIdx].len;
    pos = support[chanIdx].pos;

    /* SUBTRACT the atomic waveform from the first signal */
    if ( sigSub ) {

      /* Assert that we don't try to write outside of the signal array */
      assert( (pos + len) <= sigSub->numSamples );

      /* Seek the right location in the signal */
      sigIn  = sigSub->channel[chanIdx] + pos;

      /* Waveform SUBTRACTION */
      for ( t = 0,   ps = sigIn, pa = atomIn;
	    t < len;
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
    if ( sigAdd ) {

      /* Assert that we don't try to write outside of the signal array */
      assert( (pos + len) <= sigAdd->numSamples );

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
char MP_Atom_c::satisfies( char field, char test, MP_Real_t val ) {
  
  int chanIdx;
  char retVal = MP_TRUE;
  
  for ( chanIdx = 0; chanIdx < numChans; chanIdx++ ) {
    retVal = ( retVal && satisfies( field, test, val, chanIdx ) );
  }
  
  return( retVal );
}


/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   along one channel */
char MP_Atom_c::satisfies( char field, char test, MP_Real_t val, int chanIdx ) {
  
  MP_Real_t x;
  char has = has_field ( field );
  
  if ( test == MP_HAS ){
    return ( has );
  }
  else {
    if ( has == MP_FALSE ) {
      fprintf( stderr, "mplib warning -- MP_Atom_c::satisfies -- Unknown field. Returning TRUE." );
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
	fprintf( stderr, "mplib warning -- MP_Atom_c::satisfies -- Unknown test. Returning TRUE." );
	return( MP_TRUE );
      }
    }
  }
}

char MP_Atom_c::has_field( char field ) {
  switch (field) {
  case MP_LEN_PROP :   return( MP_TRUE );
  case MP_POS_PROP :   return( MP_TRUE );
  default : return( MP_FALSE );
  }
}

MP_Real_t MP_Atom_c::get_field( char field , int chanIdx ) {
  MP_Real_t x;
  switch (field) {
  case MP_LEN_PROP :
    x = (MP_Real_t)(support[chanIdx].len);
    break;
  case MP_POS_PROP :
    x = (MP_Real_t)(support[chanIdx].pos);
    break;
  default :
    x = (MP_Real_t)0.0;
  }
  return(x);
}
