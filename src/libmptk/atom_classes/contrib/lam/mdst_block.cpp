/******************************************************************************/
/*                                                                            */
/*                         mdst_block.cpp      		                      */
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
/*                                               		*/
/* mdst_block.cpp: methods for mclt blocks			*/
/*                                               		*/
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"


/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***********************************************/
/* Factory function  for a strict mdst block   */
MP_Mdst_Block_c* MP_Mdst_Block_c::init( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned char setWindowType,
					  const double setWindowOption,
                                          const unsigned long int setBlockOffset ) {

  const char* func = "MP_Mdst_Block_c::init()";

  /* Parameters for a strict mdst */
  const unsigned long int setFilterShift = setFilterLen / 2;
  const unsigned long int setFftSize = setFilterLen;
  char* name = window_name(setWindowType);
  if ( strcmp(name,"rectangle") & strcmp(name,"cosine") & strcmp(name,"kbd") ) {
  	mp_error_msg( func, "Wrong window type. It has to be: rectangle, cosine or kbd.\n" );
  }

  /* Call the factory function of a generalized mdst */
  MP_Mdst_Block_c *newBlock = init(setSignal,setFilterLen,setFilterShift,setFftSize,setWindowType,setWindowOption,setBlockOffset);

  return( newBlock );
}

/****************************************************/
/* Factory function  for a generalized mdst block   */
MP_Mdst_Block_c* MP_Mdst_Block_c::init( MP_Signal_c *setSignal,
					  const unsigned long int setFilterLen,
					  const unsigned long int setFilterShift,
					  const unsigned long int setFftSize,
					  const unsigned char setWindowType,
					  const double setWindowOption,
                                          const unsigned long int setBlockOffset ) {

  const char* func = "MP_Mdst_Block_c::init()";
  MP_Mdst_Block_c *newBlock = NULL;

  /* Instantiate and check */
  newBlock = new MP_Mdst_Block_c();
  if ( newBlock == NULL ) {
    mp_error_msg( func, "Failed to create a new mdst block.\n" );
    return( NULL );
  }

  /* Set the block parameters (that are independent from the signal) */
  if ( newBlock->init_parameters( setFilterLen, setFilterShift, setFftSize,
				  setWindowType, setWindowOption,setBlockOffset ) ) {
    mp_error_msg( func, "Failed to initialize some block parameters in the new mdst block.\n" );
    delete( newBlock );
    return( NULL );
  }

  /* Set the signal-related parameters */
  if ( newBlock->plug_signal( setSignal ) ) {
    mp_error_msg( func, "Failed to plug a signal in the new mdst block.\n" );
    delete( newBlock );
    return( NULL );
  }

  return( newBlock );
}

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Mdst_Block_c::init_parameters( const unsigned long int setFilterLen,
				       const unsigned long int setFilterShift,
				       const unsigned long int setFftSize,
				       const unsigned char setWindowType,
				       const double setWindowOption,
                                       const unsigned long int setBlockOffset ) {

const char* func = "MP_Mdst_Block_c::init_parameters()";

  MP_Mclt_Abstract_Block_c::init_parameters( setFilterLen, setFilterShift,setFftSize, setWindowType,setWindowOption,setBlockOffset);

 /* Allocate the atom's energy */
  if ( alloc_energy( &atomEnergy ) ) {
    mp_error_msg( func, "Failed to allocate the atom energy.\n" );
    fftSize = numFreqs = 0;
    free( fftRe ); free( fftIm );
    delete( fft );
    return( 1 );
  }

  /* Tabulate the atom's energy */
  if ( fill_energy( atomEnergy ) ) {
    mp_error_msg( func, "Failed to tabulate the atom energy.\n" );
    fftSize = numFreqs = 0;
    free( fftRe ); free( fftIm );
    free( atomEnergy );
    delete( fft );
    return( 1 );
  }

  return( 0 );
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Mdst_Block_c::plug_signal( MP_Signal_c *setSignal ) {

  MP_Mclt_Abstract_Block_c::plug_signal( setSignal );

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Mdst_Block_c::nullify_signal( void ) {

  MP_Mclt_Abstract_Block_c::nullify_signal();

}

/********************/
/* NULL constructor */
MP_Mdst_Block_c::MP_Mdst_Block_c( void )
  :MP_Mclt_Abstract_Block_c() {

   atomEnergy = NULL;

}


/**************/
/* Destructor */
MP_Mdst_Block_c::~MP_Mdst_Block_c() {

 if ( atomEnergy  ) free( atomEnergy  );


}


/***************************/
/* OTHER METHODS           */
/***************************/

/********/
/* Type */
char * MP_Mdst_Block_c::type_name() {
  return ("mdst");
}

/**********************/
/* Readable text dump */
int MP_Mdst_Block_c::info( FILE *fid ) {

  int nChar = 0;

  nChar += mp_info_msg( fid, "mdst BLOCK", "%s window (window opt=%g)"
			" of length [%lu], shifted by [%lu] samples,\n",
			window_name( fft->windowType ), fft->windowOption,
			filterLen, filterShift );
  nChar += mp_info_msg( fid, "         |-", "projected on [%lu] frequencies;\n",
			numFilters );
  nChar += mp_info_msg( fid, "         O-", "The number of frames for this block is [%lu], "
			"the search tree has [%lu] levels.\n", numFrames, numLevels );

  return( nChar );
}

/********************************************/
/* Frame-based update of the inner products */
void MP_Mdst_Block_c::update_frame(unsigned long int frameIdx, 
				    MP_Real_t *maxCorr, 
				    unsigned long int *maxFilterIdx)
{
  unsigned long int inShift;
  unsigned long int i;

  MP_Sample_t *in;
  MP_Real_t *magPtr;

  double sum;
  double max;
  unsigned long int maxIdx;

  int chanIdx;
  int numChans;

  double energy;

  int j;

  assert( s != NULL );
  numChans = s->numChans;
  assert( mag != NULL );

  inShift = frameIdx*filterShift + blockOffset;

  /*----*/
  /* Fill the mag array: */
  for ( chanIdx = 0, magPtr = mag;    /* <- for each channel */
	chanIdx < numChans;
	chanIdx++,   magPtr += numFreqs ) {
    
    assert( s->channel[chanIdx] != NULL );
    
    /* Hook the signal and the inner products to the fft */
    in  = s->channel[chanIdx] + inShift;
    
    /* Compute the energy */
    MP_Mclt_Abstract_Block_c::compute_transform( in );
    if ( filterLen == fftSize ) { 
    	for ( j = 0 ; j < numFreqs ; j++ ) {
    		energy = mcltOutIm[j] * mcltOutIm[j] / atomEnergy[j];
        	*(magPtr+j) = (MP_Real_t)(energy);
    	}
    } else {
	*(magPtr) = (MP_Real_t)(0.0);
	for ( j = 1 ; j < numFreqs ; j++ ) {
    		energy = mcltOutIm[j] * mcltOutIm[j] / atomEnergy[j];
        	*(magPtr+j) = (MP_Real_t)(energy);
    	}
    }
    
  } /* end foreach channel */
  /*----*/
  
  /*----*/
  /* Make the sum and find the maxcorr: */
  /* --Element 0: */
  /* - make the sum */
  sum = (double)(*(mag));                     /* <- channel 0      */
  for ( chanIdx = 1, magPtr = mag+numFreqs; /* <- other channels */
	chanIdx < numChans;
	chanIdx++,   magPtr += numFreqs )   sum += (double)(*(magPtr));
  /* - init the max */
  max = sum; maxIdx = 0;
  /* -- Following elements: */
  for ( i = 1; i<numFreqs; i++) {
    /* - make the sum */
    sum = (double)(*(mag+i));                     /* <- channel 0      */
    for ( chanIdx = 1, magPtr = mag+numFreqs+i; /* <- other channels */
	  chanIdx < numChans;
	  chanIdx++,   magPtr += numFreqs ) sum += (double)(*(magPtr));
    /* - update the max */
    if ( sum > max ) { max = sum; maxIdx = i; }
  }
  *maxCorr = (MP_Real_t)max;
  *maxFilterIdx = maxIdx;
}


/***************************************/
/* Output of the ith atom of the block */
unsigned int MP_Mdst_Block_c::create_atom( MP_Atom_c **atom,
					    const unsigned long int frameIdx,
					    const unsigned long int freqIdx ) {

  const char* func = "MP_mdst_Block_c::create_atom(...)";
  MP_Mdst_Atom_c *matom = NULL;
  /* Time-frequency location: */
  unsigned long int pos = frameIdx*filterShift + blockOffset;
  /* Parameters for a new FFT run: */
  MP_Sample_t *in;
  /* Parameters for the atom waveform : */
  double amp;
  /* Misc: */
  int chanIdx;

  /* Check the position */
  if ( (pos+filterLen) > s->numSamples ) {
    mp_error_msg( func, "Trying to create an atom out of the support of the current signal."
		  " Returning a NULL atom.\n" );
    *atom = NULL;
    return( 0 );
  }

  /* Allocate the atom */
  *atom = NULL;
  if ( (matom = MP_Mdst_Atom_c::init( s->numChans, fft->windowType, fft->windowOption )) == NULL ) {
    mp_error_msg( func, "Can't create a new mdst atom in create_atom()."
		  " Returning NULL as the atom reference.\n" );
    return( 0 );
  }

  /* 1) set the frequency of the atom */
  if ( filterLen == fftSize ) { 
  	matom->freq  = (MP_Real_t)( (double)(freqIdx + 0.5 ) / (double)(fft->fftSize) );
  } else {
	matom->freq  = (MP_Real_t)( (double)(freqIdx) / (double)(fft->fftSize) );
  }
  matom->numSamples = pos + filterLen;

  /* For each channel: */
  for ( chanIdx=0; chanIdx < s->numChans; chanIdx++ ) {

    /* 2) set the support of the atom */
    matom->support[chanIdx].pos = pos;
    matom->support[chanIdx].len = filterLen;
    matom->totalChanLen += filterLen;

    /* 3) seek the right location in the signal */
    in  = s->channel[chanIdx] + pos;
    
    /* 4) recompute the inner product of the atom */
    MP_Mclt_Abstract_Block_c::compute_transform( in );

    /* 5) set the amplitude */
    amp = (double)( *(mcltOutIm + freqIdx) ) / (double)( *(atomEnergy + freqIdx) ); 

    /* 6) fill in the atom parameters */
    matom->amp[chanIdx]   = (MP_Real_t)( amp   );

  }

  *atom = matom;

  return( 1 );

}

/*****************************************/
/* Allocation of the atom energy	 */
int MP_Mdst_Block_c::alloc_energy( MP_Real_t **atomEnergy ) {

  const char* func = "MP_mdst_Block_c::alloc_energy(...)";

  /* Allocate the memory for the energy and init it to zero */
  *atomEnergy = NULL;
  
  if ( ( *atomEnergy = (MP_Real_t *) calloc( numFreqs , sizeof(MP_Real_t)) ) == NULL) {
    mp_error_msg( func, "Can't allocate storage space for the energy"
		  " of the atom. It is left un-initialized.\n");
    return( 1 );
  }

  return( 0 );
}

/******************************************************/
/** Fill the atom energy array
 */
int MP_Mdst_Block_c::fill_energy( MP_Real_t *atomEnergy ) {

  const char* func = "MP_mdst_Block_c::fill_energy(...)";
  double e;
  int k,l;

  assert( atomEnergy != NULL );

  /* Fill : */
  for ( k = 0;  k < (int)(fftSize/2);  k++ ) {
    e = 0;
    for ( l = 0; l < (int)(filterLen);  l++ ) {
	if ( filterLen == fftSize ) { 
		e += pow( *(fft->window+l) * sin( MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k + 0.5 ) ), 2);
	} else {
		e += pow( *(fft->window+l) * sin( MP_2PI/fftSize * (  l + 0.5 + filterLen*0.25 ) * ( k ) ), 2);
	}
    } 
    (*(atomEnergy + k)) = e;

  }

  return( 0 );
}
