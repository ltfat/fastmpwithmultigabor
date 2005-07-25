/******************************************************************************/
/*                                                                            */
/*                              harmonic_atom.cpp                       */
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

/*************************************************************/
/*                                                           */
/* harmonic_atom.cpp: methods for Harmonic Gabor atoms */
/*                                                           */
/*************************************************************/

#include "mptk.h"
#include "mp_system.h"

#include <dsp_windows.h>

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Harmonic_Atom_c::MP_Harmonic_Atom_c( void )
  :MP_Gabor_Atom_c() {
  numPartials         = 1;
  harmonicity         = NULL;
  partialAmpStorage   = NULL;
  partialPhaseStorage = NULL;
  partialAmp          = NULL;
  partialPhase        = NULL;
}

/************************/
/* Specific constructor */
MP_Harmonic_Atom_c::MP_Harmonic_Atom_c( unsigned int setNChan,  
						    const unsigned char setWindowType,
						    const double setWindowOption,
						    const unsigned int setNumPartials)
  :MP_Gabor_Atom_c( setNChan , setWindowType, setWindowOption) {

  int i;
  unsigned int j;

  assert ( numPartials>=2 );
  numPartials  = setNumPartials;

  /* harmonicity */
  if ( (harmonicity = (MP_Real_t*)calloc(numPartials, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c() - Can't allocate harmonicity for a new atom;"
	     " harmonicity stays NULL.\n" );
  } else {
    for (j = 0; j < numPartials; j++) {*(harmonicity+j) = (MP_Real_t)(j+1);}
  }

  /* amp */
 
  if ( (partialAmpStorage = (MP_Real_t*)calloc(numChans*numPartials, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c() - Can't allocate partialAmpStorage for a new atom;"
	     " partialAmpStorage and partialAmp stay NULL.\n" );
    partialAmp        = NULL;
  } else if  ( (partialAmp = (MP_Real_t**) malloc(numChans*sizeof(MP_Real_t*)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c() - Can't allocate partialAmp array for a new atom;"
	     " partialAmpStorage and partialAmp stay NULL.\n" );
    free(partialAmpStorage);
    partialAmpStorage = NULL;
  } else {
    for (i=0; i < numChans; i++) {partialAmp[i] = partialAmpStorage+i*numPartials;}
  }

  /* phase */
  if ( (partialPhaseStorage = (MP_Real_t*)calloc(numChans*numPartials,sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c() - Can't allocate partialPhaseStorage for a new atom;"
	     " partialPhaseStorage and partialPhase stay NULL.\n" );
    partialPhase = NULL;
  } else if ( (partialPhase = (MP_Real_t**)malloc(numChans*sizeof(MP_Real_t*)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c() - Can't allocate partialPhase for a new atom;"
	     " partialPhaseStorage and partialPhase stay NULL.\n" );
    free(partialPhaseStorage);
    partialPhaseStorage = NULL;
  } else {
    for (i=0; i < numChans; i++) {partialPhase[i] = partialPhaseStorage+i*numPartials;}
  }
}

/********************/
/* File constructor */
MP_Harmonic_Atom_c::MP_Harmonic_Atom_c( FILE *fid, const char mode ) 
  :MP_Gabor_Atom_c( fid, mode ) { 

  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  double fidHarmonicity,fidAmp,fidPhase;
  int i, iRead;
  unsigned int j, jRead;

  switch ( mode ) {

  case MP_TEXT:
    /* Read the numPartials */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t\t<par type=\"numPartials\">%u</par>\n", &numPartials ) != 1 ) ||
	 (numPartials <=1) ) {
      fprintf(stderr,"mplib warning -- MP_Harmonic_Atom_c(file) - Failed to read the number of partials"
	      " in a Harmonic Gabor atom structure.\n");
      numPartials = 0;
    }
    break;

  case MP_BINARY:
    /* Try to read the number of partials */
    if ( ( mp_fread( &numPartials,  sizeof(unsigned int), 1, fid ) != 1) ||
	 (numPartials <=1) ) {
      fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Failed to read the atom's number of partials.\n");
      numPartials = 0;
    }

   break;

  default:
    fprintf( stderr, "mplib error -- MP_Harmonic_Atom_c(file) - Unknown read mode met in MP_Harmonic_Atom_c( fid , mode )." );
    break;
  }

  /* Allocate and initialize */
  /* harmonicity */
  if ( (harmonicity = (MP_Real_t*)calloc(numPartials, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Can't allocate harmonicity for a new atom;"
	     " harmonicity stays NULL.\n" );
  } else {
    for (j = 0; j < numPartials; j++) {*(harmonicity+j) = (MP_Real_t)(j+1);}
  }


  /* amp */
 
  if ( (partialAmpStorage = (MP_Real_t*)calloc(numChans*numPartials, sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Can't allocate partialAmpStorage for a new atom;"
	     " partialAmpStorage and partialAmp stay NULL.\n" );
    partialAmp        = NULL;
  } else if  ( (partialAmp = (MP_Real_t**) malloc(numChans*sizeof(MP_Real_t*)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Can't allocate partialAmp array for a new atom;"
	     " partialAmpStorage and partialAmp stay NULL.\n" );
    free(partialAmpStorage);
    partialAmpStorage = NULL;
  } else {
    for (i=0; i < numChans; i++) {partialAmp[i] = partialAmpStorage+i*numPartials;}
  }

  /* phase */
  if ( (partialPhaseStorage = (MP_Real_t*)calloc(numChans*numPartials,sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Can't allocate partialPhaseStorage for a new atom;"
	     " partialPhaseStorage and partialPhase stay NULL.\n" );
    partialPhase = NULL;
  } else if ( (partialPhase = (MP_Real_t**)malloc(numChans*sizeof(MP_Real_t*)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Can't allocate partialPhase for a new atom;"
	     " partialPhaseStorage and partialPhase stay NULL.\n" );
    free(partialPhaseStorage);
    partialPhaseStorage = NULL;
  } else {
    for (i=0; i < numChans; i++) {partialPhase[i] = partialPhaseStorage+i*numPartials;}
  }

  /* Try to read the harmonicity, partialAmp and partialPhase */
  switch (mode ) {
    
  case MP_TEXT:
    /* Read the harmonicity for each partial */
    for (j=0; j < numPartials; j++) {
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( line, "\t\t<par type=\"harmonicity\" partial=\"%u\">%lg</par>\n", 
		     &jRead,&fidHarmonicity ) != 2 ) ||
	   ( jRead != j )) {
	fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Cannot scan harmonicity for partial [%u].\n",j );
      } else *(harmonicity+j) = (MP_Real_t)fidHarmonicity;
    }
    
    for (i = 0; i<numChans; i++) {
      /* Opening tag */
      if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( line, "\t\t<harmonicPar chan=\"%d\">\n", &iRead ) != 1 ) ) {
	fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Cannot scan channel index in atom.\n" );
      }
      else if ( iRead != i ) {
 	fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Potential shuffle in the parameters"
		" of a gabor atom. (Index \"%d\" read, \"%d\" expected.)\n",
		iRead, i );
      }
      
      else for (j = 0; j < numPartials; j++) { 
	/* partialAmp */
	if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	     ( sscanf( line, "\t\t<par type=\"amp\" partial=\"%u\">%lg</par>\n", &jRead, &fidAmp ) != 2 ) ) {
	  fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Cannot scan amp on channel %d and partial %u.\n", i, j );
	} else if (jRead != j) {
	  fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Potential shuffle in the parameters"
		  " of a harmonic gabor atom. (Partial Index \"%u\" read, \"%u\" expected.)\n",
		  jRead, j );
	}  else {
	  partialAmp[i][j] = (MP_Real_t)fidAmp;
	}
	
	/* phase */
	if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	     ( sscanf( line, "\t\t<par type=\"phase\" partial=\"%u\">%lg</par>\n", &jRead, &fidPhase ) != 2 ) ) {
	  fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Cannot scan phase on channel %d and partial %u.\n", i, j );
	} else if (jRead != j) {
	  fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Potential shuffle in the parameters"
		  " of a harmonic gabor atom. (Partial Index \"%u\" read, \"%u\" expected.)\n",
		  jRead, j );
	} else {
	  partialPhase[i][j] = (MP_Real_t)fidPhase;
	}
      }
      /* Closing tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( strcmp( str , "\t\t</harmo_gaborPar>\n" ) ) ) {
	fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Cannot scan the closing parameter tag"
		" in harmonic gabor atom, channel %d.\n", i );
      }
    }
    break;
    
  case MP_BINARY:
    /* Try to read the harmonicity, partialAmp, partialPhase */
    if ( mp_fread( harmonicity,   sizeof(MP_Real_t), numPartials, fid ) != (size_t)numPartials ) {
      fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Failed to read the harmonicity array.\n" );     
      for ( j=0; j<numPartials; j++ ) *(harmonicity+j) = (MP_Real_t)(j+1);
    }

    if ( mp_fread( partialAmpStorage,   sizeof(MP_Real_t), numChans*numPartials, fid ) != (size_t)(numChans*numPartials) ) {
      fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Failed to read the partialAmp array.\n" );     
      for ( i=0; i<numChans; i++) {
	for ( j=0; j<numPartials; j++) { *(partialAmp[i]+j) = 0.0;}
      }
    }
    if ( mp_fread( partialPhaseStorage, sizeof(MP_Real_t), numChans*numPartials, fid ) != (size_t)(numChans*numPartials) ) {
      fprintf(stderr, "mplib warning -- MP_Harmonic_Atom_c(file) - Failed to read the partialPhase array.\n" );     
      for ( i=0; i<numChans; i++) {
	for ( j=0; j<numPartials; j++) { *(partialPhase[i]+j) = 0.0;}
      }
    }
    break;
    
  default: /* This case is never reached */
    break;
  }
}


/**************/
/* Destructor */
MP_Harmonic_Atom_c::~MP_Harmonic_Atom_c() {
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Harmonic_Atom_c() - Deleting harmonic_atom.\n" );
#endif
  free(harmonicity);
  free(partialAmpStorage);
  free(partialPhaseStorage);
  free(partialAmp);
  free(partialPhase);
#ifndef NDEBUG
  fprintf( stderr, "done.\n" );
#endif
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Harmonic_Atom_c::write( FILE *fid, const char mode ) {
  
  int i, nItem = 0;
  unsigned int j;

  /* Call the parent's write function */
  nItem += MP_Gabor_Atom_c::write( fid, mode );

  /* Print the other harmonic-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* Number of partials */
    nItem += fprintf( fid, "\t\t<par type=\"numPartials\">%u</par>\n", numPartials );
    /* Harmonicity */
    for (j = 0; j < numPartials; j++) { 
      nItem += fprintf( fid, "\t\t<par type=\"harmonicity\" partial=\"%u\">%lg</par>\n",j,(double)harmonicity[j] );
    }
    /* partialAmp and partialPhase */
    for (i = 0; i<numChans; i++) {
      nItem += fprintf( fid, "\t\t<harmo_gaborPar chan=\"%d\">\n", i );
      for (j = 0; j < numPartials; j++) { 
	nItem += fprintf( fid, "\t\t<par type=\"amp\" partial=\"%u\">%lg</par>\n",j,(double)partialAmp[i][j] );
	nItem += fprintf( fid, "\t\t<par type=\"phase\" partial=\"%u\">%lg</par>\n",j,(double)partialPhase[i][j] );
      }
      nItem += fprintf( fid, "\t\t</harmo_gaborPar>\n" );    
    }
    break;

  case MP_BINARY:
    /* Number of partials */
    nItem += mp_fwrite( &numPartials,  sizeof(unsigned int), 1, fid );
    /* Binary parameters */
    nItem += mp_fwrite( harmonicity,   sizeof(MP_Real_t), numPartials, fid );
    nItem += mp_fwrite( partialAmpStorage,   sizeof(MP_Real_t), numChans*numPartials, fid );
    nItem += mp_fwrite( partialPhaseStorage, sizeof(MP_Real_t), numChans*numPartials, fid );

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
char * MP_Harmonic_Atom_c::type_name(void) {
  return ("harmonic");
}

/**********************/
/* Readable text dump */
int MP_Harmonic_Atom_c::info( FILE *fid ) {

  int i, nChar = 0;
  unsigned int j;

  nChar += fprintf( fid, "HARMONIC GABOR, %s window (window opt=%g),", 
		    window_name(windowType), windowOption );
  nChar += fprintf( fid, " [%d] channel(s), [%u] partials\n", numChans, numPartials );
  nChar += fprintf( fid, "\tFreq %g\tChirp %g\n",(double)freq, (double)chirp);
  for ( i=0; i<numChans; i++ ) {
    nChar += fprintf( fid, "(%d/%d)\tSupport=", i+1, numChans );
    nChar += fprintf( fid, " %lu %lu ", support[i].pos, support[i].len );
    nChar += fprintf( fid, "\t Amp %g\t Phase %g\n", (double)amp[i], (double)phase[i] );
    for ( j=0; j<numPartials; j++) {
      nChar += fprintf( fid, "\t[%g]\tAmp %g\tPhase %g",
			(double)harmonicity[j], (double)partialAmp[i][j], (double)partialPhase[i][j] );
    }
    nChar += fprintf( fid, "\n" );
  }
  
  
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Harmonic_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Real_t *window;
  MP_Sample_t *atomBuffer;
  unsigned long int windowCenter = 0;
  /* Parameters for the atom waveform : */
  int chanIdx;
  unsigned int t;
  unsigned long int len;
  unsigned int j;
  double dHalfChirp, dAmp, dFreq, dPhase, dT, dGlobPhase, dGlobAmp;
  double argument;

  extern MP_Win_Server_c MP_GLOBAL_WIN_SERVER;

  assert( outBuffer != NULL );

  /* Dereference the arguments once and for all */
  dHalfChirp = (double)( chirp ) * MP_PI; /* chirp/2 */
  dFreq      = (double)(  freq ) * MP_2PI;

  for ( chanIdx = 0 , atomBuffer = outBuffer; 
	chanIdx < numChans; 
	chanIdx++  ) {
    /* Dereference the atom length in the current channel once and for all */
    len = support[chanIdx].len;

    /* Make the window */
    windowCenter = MP_GLOBAL_WIN_SERVER.get_window( &window, len, windowType, windowOption );
    assert( window != NULL );

    /* Dereference the arguments once and for all */
    dGlobAmp   = (double)(   amp[chanIdx] );
    dGlobPhase = (double)( phase[chanIdx] );
  
    /* 1/ Build the desired modulation (without multiplying by the window) 
     * \f[
     * \sum_{k=1}^{\mbox{numPartials}} a_k
     * \cdot \cos\left(2\pi \lambda_k \left(\mbox{chirp} \cdot \frac{t^2}{2}
     *      + \mbox{freq} \cdot t\right)+ \mbox{phase} + \phi_k\right)
     * \f]
     */
    
    for ( t = 0; t<len; t++ ) {
      
      /* Compute the cosine's argument */
      dT = (double)(t);
      argument = (dHalfChirp*dT + dFreq)*dT;
      /* The above does:
       * argument = dHalfChirp*dT*dT + dFreq*dT but saves a multiplication.
       * \todo save multiplications by integrating twice the second derivative ?
      */
      /* -- first partial */
      dAmp   = dGlobAmp*(double)(   partialAmp[chanIdx][0] );
      dPhase = (double)( partialPhase[chanIdx][0] );
      *(atomBuffer+t) = (MP_Sample_t)( dAmp * cos( (harmonicity[0]*argument) +
						   dGlobPhase+dPhase) );

      /* -- following partials */
      for ( j = 1; j < numPartials; j++) {
	dAmp   = dGlobAmp*(double)(   partialAmp[chanIdx][j] );
	dPhase = (double)( partialPhase[chanIdx][j] );
	*(atomBuffer+t) += (MP_Sample_t)( dAmp * cos( (harmonicity[j]*argument) +
						      dGlobPhase+dPhase) );
      }
    } /* <-- end loop on samples */
    
    /* 2/ multiply by the window and the global amplitude */
    for ( t = 0; t<len; t++ ) {
      *(atomBuffer+t)   *= *(window+t);
    }

    /* Go to the next channel */
    atomBuffer += len;
  }

}


/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
/** \todo TO IMPLEMENT */
char MP_Harmonic_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap ) {

  char flag = 0;

  /* YOUR code */
  tfmap = NULL;

  return( flag );
}


