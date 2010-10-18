/******************************************************************************/
/*                                                                            */
/*                         mclt_abstract_block.cpp                            */
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
/* mclt_abstract_block.cpp: methods for mclt_abstract blocks	*/
/*                                               		*/
/****************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mclt_abstract_block_plugin.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/*********************************************************/
/* Initialization of signal-independent block parameters */
int MP_Mclt_Abstract_Block_Plugin_c::init_parameters( const unsigned long int setFilterLen,
				       const unsigned long int setFilterShift,
				       const unsigned long int setFftSize,
				       const unsigned char setWindowType,
				       const double setWindowOption,
				       const unsigned long int setBlockOffset ) {

  const char* func = "MP_mclt_abstract_Block_c::init_parameters(...)";

  /* Check the validity of setFilterLen */
  if ( is_odd(setFilterLen) ) { /* If windowLen is odd: windowLen has to be even! */
    mp_error_msg( func, "windowLen [%lu] is odd: windowLen must be even.\n" ,
		  setFilterLen );
    return( 1 );
  }
  /* Check the validity of setFftSize */
  if ( check_fftsize(setFilterLen, setFftSize) ) { /* fftSize must be equal to windowLen or a
					multiple of 2*windowlen */
    mp_error_msg( func, "fftSize [%lu] must be equal to windowLen or a multiple of 2*windowlen.\n" ,
		  setFftSize );
    return( 1 );
  }
  if ( !(window_type_is_ok(setWindowType)) ) {
    mp_error_msg( func, "Invalid window type.\n" );
    return( 1 );
  }

  /* Go up the inheritance graph */
  if ( MP_Block_c::init_parameters( setFilterLen, setFilterShift, setFftSize/2, setBlockOffset ) ) {
    mp_error_msg( func, "Failed to init the block-level parameters in the new Gabor block.\n" );
    return( 1 );
  }

  /* Set the parameters */
  fftSize = setFftSize;
  numFreqs = fftSize/2;

  /* Create the FFT object */
  fft = (MP_FFT_Interface_c*)MP_FFT_Interface_c::init( filterLen, setWindowType, setWindowOption,
						       fftSize );
  if ( fft == NULL ) {
    mp_error_msg( func, "Failed to init the FFT in the new Gabor block.\n" );
    fftSize = numFreqs = 0;
    return( 1 );
  }

  /* Allocate the complex fft buffers */
  if ( (fftRe = (MP_Real_t*) calloc( numFreqs+1 , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the real part of the fft array.\n", numFreqs );
    fftSize = numFreqs = 0;
    delete( fft );
    return( 1 );
  }
  if ( (fftIm = (MP_Real_t*) calloc( numFreqs+1 , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the imaginary part of the fft array.\n", numFreqs );
    fftSize = numFreqs = 0;
    free( fftRe );
    delete( fft );
    return( 1 );
  }

  /* Allocate the complex mclt output */
  if ( (mcltOutRe = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the real part of the mclt output.\n", numFreqs );
    return( 1 );
  }
  if ( (mcltOutIm = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the imaginary part of the mclt output.\n", numFreqs );
    return( 1 );
  }

  /* Allocate the modulation buffers */
  if ( (preModRe = (MP_Real_t*) calloc( setFilterLen , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the real part of the pre-modulation array.\n", setFilterLen );
    return( 1 );
  }
  if ( (preModIm = (MP_Real_t*) calloc( setFilterLen , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the imaginary part of the post-modulation array.\n", setFilterLen );
    return( 1 );
  }
  if ( (postModRe = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the real part of the fft array.\n", numFreqs );
    return( 1 );
  }
  if ( (postModIm = (MP_Real_t*) calloc( numFreqs , sizeof(MP_Real_t) )) == NULL ) {
    mp_error_msg( func, "Failed to allocate an array of [%lu] MP_Real_t elements"
		  " for the imaginary part of the fft array.\n", numFreqs );
    return( 1 );
  }

  /* Init the transform */
  init_transform( );

  return( 0 );
}

/*******************************************************/
/* Initialization of signal-dependent block parameters */
int MP_Mclt_Abstract_Block_Plugin_c::plug_signal( MP_Signal_c *setSignal ) {

  const char* func = "MP_mclt_abstract_Block_c::plug_signal( signal )";

  /* Reset any potential previous signal */
  nullify_signal();

  if ( setSignal != NULL ) {

    /* Go up the inheritance graph */
    if ( MP_Block_c::plug_signal( setSignal ) ) {
      mp_error_msg( func, "Failed to plug a signal at the block level.\n" );
      nullify_signal();
      return( 1 );
    }
    
    /* Allocate the mag array */
    if ( (mag = (MP_Real_t*) calloc( numFreqs*s->numChans , sizeof(MP_Real_t) )) == NULL ) {
      mp_error_msg( func, "Can't allocate an array of [%lu] MP_Real_t elements"
		    " for the mag array. Nullifying all the signal-related parameters.\n",
		    numFreqs*s->numChans );
      nullify_signal();
      return( 1 );
    }

  }

  return( 0 );
}


/**************************************************/
/* Nullification of the signal-related parameters */
void MP_Mclt_Abstract_Block_Plugin_c::nullify_signal( void ) {

  MP_Block_c::nullify_signal();
  if ( mag ) { free( mag ); mag = NULL; }

}



/********************/
/* NULL constructor */
MP_Mclt_Abstract_Block_Plugin_c::MP_Mclt_Abstract_Block_Plugin_c( void )
  :MP_Block_c() {

  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_mclt_abstract_Block_c::MP_mclt_abstract_Block_c()",
		"Constructing a mclt_abstract block...\n" );

  fft = NULL;
  mag = NULL;

  fftRe = fftIm = NULL;

  mcltOutRe = mcltOutIm = NULL;

  preModRe = preModIm = postModRe = postModIm = NULL;

  fftSize = numFreqs = 0;
  mp_debug_msg( MP_DEBUG_CONSTRUCTION, "MP_mclt_abstract_Block_c::MP_mclt_abstract_Block_c()",
		"Done.\n" );  

}


/**************/
/* Destructor */
MP_Mclt_Abstract_Block_Plugin_c::~MP_Mclt_Abstract_Block_Plugin_c() {

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_mclt_abstract_Block_c::~MP_mclt_abstract_Block_c()", "Deleting mclt_abstract block...\n" );

  if ( fft ) delete( fft );

  if ( mag ) free( mag );

  if ( fftRe ) free( fftRe );
  if ( fftIm ) free( fftIm );

  if ( mcltOutRe ) free( mcltOutRe );
  if ( mcltOutIm ) free( mcltOutIm );

  if ( preModRe ) free( preModRe );
  if ( preModIm ) free( preModIm );

  if ( postModRe ) free( postModRe );
  if ( postModIm ) free( postModIm );

  mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_mclt_abstract_Block_c::~MP_mclt_abstract_Block_c()", "Done.\n" );



}


/***************************/
/* OTHER METHODS           */
/***************************/

/*********************************************/
/* Check the the validity of FftSize	     */
int MP_Mclt_Abstract_Block_Plugin_c::check_fftsize( const unsigned long int setFilterLen,
			       const unsigned long int setFftSize ) {

	double temp;

	if ( setFilterLen == setFftSize ) {
		/* OK: this is the strict MCLT/MDCT/MDST case */
		return( 0 );
	} else {
		temp = setFftSize / setFilterLen;
		temp = floor( temp / 2 ) * 2 - temp;
		if ( temp == 0.0 ) {
			 /* Ok: this is a generalized MCLT/MDCT/MDST
				the fftsize must be a multiple of 2*windowlen */
			return( 0 );
		} else { 
			return( 1 );
		}
	}
}


/*************************************/
/* Init the mclt transform 	     */
void MP_Mclt_Abstract_Block_Plugin_c::init_transform( ) {

	unsigned int i;

	if ( filterLen == fftSize ) {

		for ( i = 0 ; i < filterLen ; i++ ) {
			*(preModRe+i) = cos( MP_PI * i / fftSize );
			*(preModIm+i) = - sin( MP_PI * i / fftSize );
		}

		for ( i = 0 ; i < numFreqs ; i++ ) {
			*(postModRe+i) = cos( MP_PI * (1 + filterLen*0.5 ) * ( i + 0.5 ) / fftSize );
			*(postModIm+i) = - sin( MP_PI * (1 + filterLen*0.5 ) * ( i + 0.5 ) / fftSize );
		}

	} else {
		
		for ( i = 0 ; i < filterLen ; i++ ) {
			*(preModRe+i) = 1;
			*(preModIm+i) = 0;
		}

		for ( i = 0 ; i < numFreqs ; i++ ) {
			*(postModRe+i) = cos( MP_PI * (1 + filterLen*0.5 ) * ( i ) / fftSize );
			*(postModIm+i) = - sin( MP_PI * (1 + filterLen*0.5 ) * ( i ) / fftSize );
		}
	}

}

/*************************************/
/* Compute the transform 	     */
void MP_Mclt_Abstract_Block_Plugin_c::compute_transform( MP_Real_t *in ) {

	unsigned int i;

	fft->exec_complex_demod( in, preModRe, preModIm, fftRe, fftIm);

	for ( i = 0 ; i < numFreqs ; i++ ) {
		*(mcltOutRe+i) = (*(fftRe+i)) * (*(postModRe+i)) - (*(fftIm+i)) * (*(postModIm+i));
		*(mcltOutIm+i) = (*(fftRe+i)) * (*(postModIm+i)) + (*(fftIm+i)) * (*(postModRe+i));
	}

}

void MP_Mclt_Abstract_Block_Plugin_c::compute_inverse_transform(MP_Real_t* out){
    
    unsigned int i;
    
    for ( i = 0 ; i < numFreqs ; i++ ) {
        *(fftRe+i) = (*(mcltOutRe+i)) * (*(postModRe+i)) + (*(mcltOutIm+i)) * (*(postModIm+i));
        *(fftIm+i) = - (*(mcltOutRe+i)) * (*(postModIm+i)) + (*(mcltOutIm+i)) * (*(postModRe+i));
    }
    
    fft->exec_complex_inverse_demod (fftRe, fftIm, preModRe, preModIm, out);
}
