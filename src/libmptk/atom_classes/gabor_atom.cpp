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
/*
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/*************************************************/
/*                                               */
/* gabor_atom.cpp: methods for gabor atoms       */
/*                                               */
/*************************************************/

#include "mptk.h"
#include "system.h"

#include <dsp_windows.h>


/*************/
/* CONSTANTS */
/*************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Gabor_Atom_c::MP_Gabor_Atom_c( void )
  :MP_Atom_c() {
  windowType = DSP_UNKNOWN_WIN;
  windowOption = 0.0;
  freq  = 0.0;
  chirp = 0.0;
  amp   = NULL;
  phase = NULL;
}

/************************/
/* Specific constructor */
MP_Gabor_Atom_c::MP_Gabor_Atom_c( const unsigned int setNChan,
				  const unsigned char setWindowType,
				  const double setWindowOption )
  :MP_Atom_c( setNChan ) {
  
  int i;
  
  windowType   = setWindowType;
  windowOption = setWindowOption;
  
  /* default freq and chirp */
  freq  = 0.0;
  chirp = 0.0;

  /* amp */
  if ( (amp = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c() - Can't allocate the amp array for a new atom;"
	     " amp stays NULL.\n" );
  }
  /* phase */
  if ( (phase = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c() - Can't allocate the phase array for a new atom;"
	     " phase stays NULL.\n" );
  }
  /* Initialize */
  if ( (amp!=NULL) && (phase!=NULL) ) {
    for (i = 0; i<numChans; i++) {
      *(amp  +i) = 0.0;
      *(phase+i) = 0.0;
    }
  }
  else fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c() - The parameter arrays"
	      " for the new atom are left un-initialized.\n" );

}

/********************/
/* File constructor */
MP_Gabor_Atom_c::MP_Gabor_Atom_c( FILE *fid, const char mode )
  :MP_Atom_c( fid, mode ) {

  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
  double fidFreq,fidChirp,fidAmp,fidPhase;
  int i, iRead;

  switch ( mode ) {

  case MP_TEXT:
    /* Read the window type */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "\t\t<window type=\"%[a-z]\" opt=\"%lg\"></window>\n", str, &windowOption ) != 2 ) ) {
      fprintf(stderr,"mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the window type"
	      " and/or option in a Gabor atom structure.\n");
      windowType = DSP_UNKNOWN_WIN;
    }
    else {
      /* Convert the window type string */
      windowType = window_type( str );
    }
    break;

  case MP_BINARY:
    /* Try to read the atom window */
    if ( ( fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||
	 ( sscanf( line, "%[a-z]\n", str ) != 1 ) ) {
      fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to scan the atom's window type.\n");
      windowType = DSP_UNKNOWN_WIN;
    }
    else {
      /* Convert the window type string */
      windowType = window_type( str );
    }
    /* Try to read the window option */
    if ( fread( &windowOption,  sizeof(double), 1, fid ) != 1 ) {
      fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the atom's window option.\n");
      windowOption = 0.0;
    }
   break;

  default:
    fprintf( stderr, "mplib error -- MP_Gabor_Atom_c(file) - Unknown read mode met in MP_Gabor_Atom_c( fid , mode )." );
    break;
  }

  /* Allocate and initialize */
  /* amp */
  if ( (amp = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Can't allocate the amp array for a new atom; amp stays NULL.\n" );
  }
  /* phase */
  if ( (phase = (MP_Real_t*)malloc( numChans*sizeof(MP_Real_t)) ) == NULL ) {
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Can't allocate the phase array for a new atom; phase stays NULL.\n" );
  }
  /* Initialize */
  if ( (amp!=NULL) && (phase!=NULL) ) {
    for (i = 0; i<numChans; i++) {
      *(amp  +i) = 0.0;
      *(phase+i) = 0.0;
    }  
  }
  else {
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c(file) - The parameter arrays for the new atom are left un-initialized.\n" );
  }

  /* Try to read the freq, chirp, amp, phase */
  switch (mode ) {
    
  case MP_TEXT:

    /* freq */
    if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	 ( sscanf( str, "\t\t<par type=\"freq\">%lg</par>\n", &fidFreq ) != 1 ) ) {
      fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan freq.\n" );
    }
    else {
      freq = (MP_Real_t)fidFreq;
    }
    /* chirp */
    if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	 ( sscanf( str, "\t\t<par type=\"chirp\">%lg</par>\n", &fidChirp ) != 1 ) ) {
      fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan chirp.\n" );
    }
    else {
      chirp = (MP_Real_t)fidChirp;
    }
  
    for (i = 0; i<numChans; i++) {
      /* Opening tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<gaborPar chan=\"%d\">\n", &iRead ) != 1 ) ) {
	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan channel index in atom.\n" );
      }
      else if ( iRead != i ) {
 	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Potential shuffle in the parameters"
		" of a gabor atom. (Index \"%d\" read, \"%d\" expected.)\n",
		iRead, i );
      }
      /* amp */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<par type=\"amp\">%lg</par>\n", &fidAmp ) != 1 ) ) {
	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan amp on channel %d.\n", i );
      }
      else {
	*(amp +i) = (MP_Real_t)fidAmp;
      }
      /* phase */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( sscanf( str, "\t\t<par type=\"phase\">%lg</par>\n", &fidPhase ) != 1 ) ) {
	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan phase on channel %d.\n", i );
      }
      else {
	*(phase +i) = (MP_Real_t)fidPhase;
      }
      /* Closing tag */
      if ( ( fgets( str, MP_MAX_STR_LEN, fid ) == NULL  ) ||
	   ( strcmp( str , "\t\t</gaborPar>\n" ) ) ) {
	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Cannot scan the closing parameter tag"
		" in gabor atom, channel %d.\n", i );
      }
    }
    break;
    
  case MP_BINARY:
    /* Try to read the freq, chirp, amp, phase */
    if ( fread( &freq,  sizeof(MP_Real_t), 1 , fid ) != (size_t)1 ) {
 	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the freq.\n" );     
	freq = 0.0;
    }
    if ( fread( &chirp, sizeof(MP_Real_t), 1, fid ) != (size_t)1 ) {
 	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the chirp.\n" );     
	chirp = 0.0;
    }
    if ( fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
 	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the amp array.\n" );     
	for ( i=0; i<numChans; i++ ) *(amp+i) = 0.0;
    }
    if ( fread( phase, sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
 	fprintf(stderr, "mplib warning -- MP_Gabor_Atom_c(file) - Failed to read the phase array.\n" );     
	for ( i=0; i<numChans; i++ ) *(phase+i) = 0.0;
    }
    break;
    
  default:
    break;
  }

}


/**************/
/* Destructor */
MP_Gabor_Atom_c::~MP_Gabor_Atom_c() {
  if (amp)   free( amp );
  if (phase) free( phase );
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Gabor_Atom_c::write( FILE *fid, const char mode ) {
  
  int i;
  int nItem = 0;

  /* Call the parent's write function */
  nItem += MP_Atom_c::write( fid, mode );

  /* Print the other Gabor-specific parameters */
  switch ( mode ) {
    
  case MP_TEXT:
    /* Window name */
    nItem += fprintf( fid, "\t\t<window type=\"%s\" opt=\"%g\"></window>\n", window_name(windowType), windowOption );
    /* print the freq, chirp, amp, phase */
    nItem += fprintf( fid, "\t\t<par type=\"freq\">%g</par>\n",  (double)freq );
    nItem += fprintf( fid, "\t\t<par type=\"chirp\">%g</par>\n", (double)chirp );
    for (i = 0; i<numChans; i++) {
      nItem += fprintf( fid, "\t\t<gaborPar chan=\"%d\">\n", i );
      nItem += fprintf( fid, "\t\t<par type=\"amp\">%g</par>\n",   (double)amp[i] );
      nItem += fprintf( fid, "\t\t<par type=\"phase\">%g</par>\n", (double)phase[i] );
      nItem += fprintf( fid, "\t\t</gaborPar>\n" );    
    }
    break;

  case MP_BINARY:
    /* Window name */
    nItem += fprintf( fid, "%s\n", window_name(windowType) );
    /* Window option */
    nItem += fwrite( &windowOption,  sizeof(double), 1, fid );
    /* Binary parameters */
    nItem += fwrite( &freq,  sizeof(MP_Real_t), 1, fid );
    nItem += fwrite( &chirp, sizeof(MP_Real_t), 1, fid );
    nItem += fwrite( amp,   sizeof(MP_Real_t), numChans, fid );
    nItem += fwrite( phase, sizeof(MP_Real_t), numChans, fid );
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
char * MP_Gabor_Atom_c::type_name(void) {
  return ("gabor");
}

/**********************/
/* Readable text dump */
int MP_Gabor_Atom_c::info( FILE *fid ) {

  int i, nChar = 0;

  nChar += fprintf( fid, "mplib info -- GABOR ATOM: %s window (window opt=%g),", window_name(windowType), windowOption );
  nChar += fprintf( fid, " [%d] channel(s)\n", numChans );
  nChar += fprintf( fid, "\tFreq %g\tChirp %g\n", (double)freq, (double)chirp);
  for ( i=0; i<numChans; i++ ) {
    nChar += fprintf( fid, "mplib info -- (%d/%d)\tSupport=", i+1, numChans );
    nChar += fprintf( fid, " %lu %lu ", support[i].pos, support[i].len );
    nChar += fprintf( fid, "\tAmp %g\tPhase %g",(double)amp[i], (double)phase[i] );
    nChar += fprintf( fid, "\n" );
  }
  return( nChar );
}

/********************/
/* Waveform builder */
void MP_Gabor_Atom_c::build_waveform( MP_Sample_t *outBuffer ) {

  MP_Real_t *window;
  MP_Sample_t *atomBuffer;
  unsigned long int windowCenter = 0;
  /* Parameters for the atom waveform : */
  int chanIdx;
  unsigned int t;
  unsigned long int len;
  double dHalfChirp, dAmp, dFreq, dPhase, dT;
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
    dHalfChirp = (double)( chirp ) * MP_PI;
    dFreq      = (double)(  freq ) * MP_2PI;
    dPhase     = (double)( phase[chanIdx] );
    dAmp       = (double)(   amp[chanIdx] );

    /** Multiply by the desired modulation to get
     * \f[
     * \mbox{window}(t) \cdot \mbox{amp} \cdot 
     * \cos\left(2\pi \left (\mbox{chirp} \cdot \frac{t^2}{2}
     *      + \mbox{freq} \cdot t\right)+ \mbox{phase}\right)
     * \f]
     */
    for ( t = 0; t<len; t++ ) {

      /* Compute the cosine's argument */
      dT = (double)(t);
      argument = (dHalfChirp*dT + dFreq)*dT + dPhase;
      /* The above does:
	   argument = dHalfChirp*dT*dT + dFreq*dT + dPhase
	 but saves a multiplication.
      */
      /* Compute the waveform samples */
      *(atomBuffer+t) = (MP_Sample_t)( (double)(*(window+t)) * dAmp * cos(argument) );

    }

    /* Go to the next channel */
    atomBuffer += len;
  }

}

#ifdef LASTWAVE
/*
 * The Wigner-Ville distribution of a Gaussian atom is
 *  GWV(2*(u/2^o)) x GWV(2*(2*pi*\sigma^2* k*2^o/GABOR_MAX_FREQID))
 *
 *  GWV(x) = e^{-x^2/4\sigma^2}
 */
static float *GaussianWignerVille(int sizeGWV)
{
    float *GWV;
    float c = 1/(4*theGaussianSigma2*sizeGWV*sizeGWV);
    int i;
        
    /* Allocation */
    if ((GWV = FloatAlloc(sizeGWV)) == NULL) 
        Errorf("GaussianWignerVilleTime : Mem. Alloc. failed!");
        
    /* Computation of e^{-x^2/\sigma^2}, x = i/sizeGWV*/
    for (i = 0; i < sizeGWV; i++) 
    {
        GWV[i] = exp(-c*i*i);
    }
    return(GWV);
}

/* 
 * The Wigner-Ville distribution of a FoF atom is
 *  FWV(2*(u/2^o)) x GWV(2*(2*pi*\sigma^2* k*2^o/GABOR_MAX_FREQID))
 *
 *  FWV(x) = ???
 */
static float *FoFWignerVille(int sizeFWV)
{
    float *FWV;
    float a,expon,beta,max;
    int i;
        
    /* Allocation */
    if ((FWV = FloatAlloc(sizeFWV)) == NULL) 
        Errorf("FoFWignerVilleTime : Mem. Alloc. failed!");
    
    /* Damping factor */
    a = log(decayFoF);
    
    /* scale */
    expon = a/sizeFWV; 
    beta = betaFoF/sizeFWV;        
    
    max=0.0;

    /* Computation of FoF window */
    for (i = 0; i <= M_PI/beta; i++) 
    {
        FWV[i] = 0.5*(1-cos(beta*i))*exp(-expon*i);
	if (FWV[i] > max) max=FWV[i];
    }
    for (; i<sizeFWV; i++)
    {
	FWV[i] = exp(-expon*i);
	if (FWV[i] > max) max=FWV[i];
    }

    /* Normalizing to the maximum */
    for (i = 0; i <sizeFWV; i++) 
    {
        FWV[i]=FWV[i]/max;
    }
    return(FWV);
}
#endif

MP_Real_t wigner_ville(MP_Real_t t, MP_Real_t f, unsigned char windowType) {

  static double factor = 1/MP_PI;
  double x = t-0.5;
  
  switch(windowType) {
  case DSP_GAUSS_WIN :
    return(factor*exp(-(x*x)/DSP_GAUSS_DEFAULT_OPT -f*f*DSP_GAUSS_DEFAULT_OPT));
  default :
    return(1);
  }
}

/* Adds a pseudo Wigner-Ville of the atom to a time-frequency map */
char MP_Gabor_Atom_c::add_to_tfmap( MP_TF_Map_c *tfmap ) {

  unsigned char chanIdx;
  MP_Real_t tMin,tMax,df,fMin,fMax;
  int nMin,kMin,nMax,kMax;
  int iMin,jMin,iMax,jMax;
  int i,j;
  MP_Real_t t,f;
  MP_Real_t *column;
  char flag = 0;
  MP_Real_t value;

#ifndef NDEBUG
  assert(numChans==tfmap->numChans);
#endif

  for (chanIdx = 0; chanIdx < numChans; chanIdx++) {
#ifndef NDEBUG
    fprintf(stderr,"Displaying channel [%u] to tfmap\n",chanIdx);
#endif
    /* 1/ Determine the support (extremities included) of the atom in standard coordinates */
    tMin = support[chanIdx].pos;
    tMax = (MP_Real_t)(support[chanIdx].pos+support[chanIdx].len)-1;
    df   = 1/((MP_Real_t)support[chanIdx].len); /* TODO : determine a constant factor */
    fMin = freq-df/2;
    fMax = freq+df/2+chirp*(tMax-tMin);
#ifndef NDEBUG
    fprintf(stderr,"Atom 'rectangle' in tf  coordinates [%g %g]x[%g %g]\n",tMin,tMax,fMin,fMax);
#endif

    /* 2/ Convert this in pixel coordinates */
    tfmap->pixel_coordinates(tMin,fMin,&nMin,&kMin);
    tfmap->pixel_coordinates(tMax,fMax,&nMax,&kMax);
#ifndef NDEBUG
    fprintf(stderr,"Atom rectangle in pix coordinates [%d %d]x[%d %d]\n",nMin,nMax,kMin,kMax);
#endif

    /* 3/ Detect the cases where there is nothing to display */
    if (nMin >= tfmap->numCols || nMax < 0 || kMin >= tfmap->numRows || kMax < 0) {
#ifndef NDEBUG
      fprintf(stderr,"Atom rectangle does not intersect tfmap\n");
#endif
      continue;
    }
    else {
      flag = 1;
    }
    /* 4/ Compute the rectangle (lower extremity included, upper one excluded) of the pixel map that needs to be filled in */
    if (nMin < 0) iMin = 0;
    else          iMin = nMin;
    if (nMax >= tfmap->numCols) iMax = tfmap->numCols;
    else                        iMax = nMax+1;
    if (kMin < 0) jMin = 0;
    else          jMin = kMin;
    if (kMax >= tfmap->numRows) jMax = tfmap->numRows;
    else                        jMax = kMax+1;
#ifndef NDEBUG
    fprintf(stderr,"Filled rectangle in pix coordinates [%d %d[x[%d %d[\n",iMin,iMax,jMin,jMax);
#endif
		    
    /* 4/ Fill the TF map */
    for (i = iMin; i < iMax; i++) { /* loop on columns */
      column = 	tfmap->channel[chanIdx] + i*tfmap->numRows; /* seek the column data */
      /* Fill the column */
      for (j = jMin; j < jMax; j++) {
	tfmap->tf_coordinates(i,j,&t,&f);
	value = amp[chanIdx]*amp[chanIdx]
	  *wigner_ville((t-tMin)/support[chanIdx].len,
			(f-freq-chirp*(t-tMin))*support[chanIdx].len,
			windowType);
	column[j] += value;
      }
    }
  }
  return(flag);
}



/***********************************************************************/
/* Sorting function which characterizes various properties of the atom,
   along one channel */
int MP_Gabor_Atom_c::has_field( int field ) {

  if ( MP_Atom_c::has_field( field ) ) return ( MP_TRUE );
  else switch (field) {
  case MP_FREQ_PROP :  return( MP_TRUE );
  case MP_AMP_PROP :   return( MP_TRUE );
  case MP_PHASE_PROP : return( MP_TRUE );
  case MP_CHIRP_PROP : return( MP_TRUE );
  default : return( MP_FALSE );
  }
}

MP_Real_t MP_Gabor_Atom_c::get_field( int field, int chanIdx ) {
  MP_Real_t x;
  if ( MP_Atom_c::has_field( field ) ) return ( MP_Atom_c::get_field(field,chanIdx) );
  else switch (field) {
  case MP_POS_PROP :
    x = (MP_Real_t)(support[chanIdx].pos);
    break;
  case MP_FREQ_PROP :
    x = freq;
    break;
  case MP_AMP_PROP :
    x = amp[chanIdx];
    break;
  case MP_PHASE_PROP :
    x = phase[chanIdx];
    break;
  case MP_CHIRP_PROP :
    x = chirp;
    break;
  default :
    fprintf( stderr, "mplib warning -- MP_Gabor_Atom_c::get_field -- Unknown field. Returning ZERO." );
    x = 0.0;
  }

  return( x );

}

