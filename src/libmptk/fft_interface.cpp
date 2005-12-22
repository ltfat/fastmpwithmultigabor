/******************************************************************************/
/*                                                                            */
/*                             fft_interface.cpp                              */
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

/**************************************************************/
/*                                                            */
/* fft_interface.cpp: generic interface for FFT libraries     */
/*                                                            */
/**************************************************************/

#include "mptk.h"
#include "mp_system.h"


/*********************************/
/*                               */
/* GENERIC INTERFACE             */
/*                               */
/*********************************/

/***************************/
/* FACTORY METHOD          */
/***************************/
MP_FFT_Interface_c* MP_FFT_Interface_c::init( const unsigned long int setWindowSize,
					      const unsigned char setWindowType,
					      const double setWindowOption,
					      const unsigned long int setFftRealSize ) {
  MP_FFT_Interface_c* fft = NULL;

  if( 2*(setFftRealSize-1) < setWindowSize ) {
    mp_error_msg( "MP_FFT_Interface_c::init()",
		  "Can't create a FFT of size %ld smaller than the window size %ld."
		  " Returning a NULL fft object.\n", 2*(setFftRealSize-1), setWindowSize);
    return( NULL );
  }

  /* Create the adequate FFT and check the returned address */
#ifdef USE_FFTW3
  fft = (MP_FFT_Interface_c*) new MP_FFTW_Interface_c( setWindowSize, setWindowType, setWindowOption,
						       setFftRealSize );
  if ( fft == NULL ) mp_error_msg( "MP_FFT_Interface_c::init()",
				   "Instanciation of FFTW_Interface failed."
				   " Returning a NULL fft object.\n" );
#elif defined(USE_MAC_FFT)
  fft = (MP_FFT_Interface_c*) new MP_MacFFT_Interface_c( setWindowSize, setWindowType, setWindowOption,
							 setFftRealSize );
  if ( fft == NULL ) mp_error_msg( "MP_FFT_Interface_c::init()",
				   "Instanciation of MacFFT_Interface failed."
				   " Returning a NULL fft object.\n" );
#else
#  error "No FFT implementation was found !"
#endif

  if ( fft == NULL) return( NULL );

  /* Check the internal buffers: */
  /* - window: */
  if ( fft->window == NULL ) {
    mp_error_msg( "MP_FFT_Interface_c::init()",
		  "FFT window is NULL. Returning a NULL fft object.\n");
    delete( fft );
    return( NULL );
  }
  /* - other buffers: */
  if ( ( fft->bufferRe  == NULL ) || ( fft->bufferIm  == NULL ) ||
       ( fft->buffer2Re == NULL ) || ( fft->buffer2Im == NULL ) ||
       ( fft->inDemodulated == NULL ) ) {
    mp_error_msg( "MP_FFT_Interface_c::init()",
		  "One or several of the internal FFT buffers are NULL. Returning a NULL fft object.\n");
    delete( fft );
    return( NULL );
  }

  /* If everything went OK, just pop it ! */
  return( fft );
}

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***********************************/
/* Constructor with a typed window */
MP_FFT_Interface_c::MP_FFT_Interface_c( const unsigned long int setWindowSize,
					const unsigned char setWindowType,
					const double setWindowOption,
					const unsigned long int setFftRealSize ) {
  extern MP_Win_Server_c MP_GLOBAL_WIN_SERVER;

  /* Check if fftCplxSize will overflow in the expression fftCplxSize = 2*(fftRealSize-1) */
  assert( setFftRealSize    <= (ULONG_MAX >> 1) );

  /* Set values */
  windowSize = setWindowSize;
  windowType = setWindowType;
  windowOption = setWindowOption;
  fftRealSize = setFftRealSize;
  fftCplxSize = (fftRealSize-1)<<1;
  assert( fftCplxSize >= setWindowSize );

  /* Compute the window and get its center point */
  windowCenter = MP_GLOBAL_WIN_SERVER.get_window( &window, windowSize, windowType, windowOption );

  /* Allocate some other buffers */
  bufferRe = (MP_Real_t*) malloc(sizeof(MP_Real_t)*fftRealSize);  
  bufferIm = (MP_Real_t*) malloc(sizeof(MP_Real_t)*fftRealSize);

  buffer2Re = (MP_Real_t*) malloc(sizeof(MP_Real_t)*fftRealSize);  
  buffer2Im = (MP_Real_t*) malloc(sizeof(MP_Real_t)*fftRealSize);

  inDemodulated = (MP_Sample_t*) malloc(sizeof(MP_Sample_t)*windowSize);
}


/**************/
/* Destructor */
MP_FFT_Interface_c::~MP_FFT_Interface_c( ) {

  if ( bufferRe )  free( bufferRe );
  if ( bufferIm )  free( bufferIm );
  if ( buffer2Re )  free( buffer2Re );
  if ( buffer2Im )  free( buffer2Im );
  if ( inDemodulated )  free( inDemodulated );

}



/***************************/
/* OTHER METHODS           */
/***************************/

/***************************/
/* EXECUTION METHODS       */
/***************************/

/**************************/
/* Get the magnitude only */
void MP_FFT_Interface_c::exec_mag( MP_Sample_t *in, MP_Real_t *mag ) {

  unsigned long int i;
  double re, im;

  /* Simple buffer check */
  assert( in  != NULL );
  assert( mag != NULL );

  /* Execute the FFT */
  exec_complex( in, bufferRe, bufferIm );

  /* Get the resulting magnitudes */
  for ( i=0; i<fftRealSize; i++ ) {
    re = bufferRe[i];
    im = bufferIm[i];

#ifdef MP_MAGNITUDE_IS_SQUARED
    *(mag+i) = (MP_Real_t)( re*re+im*im );
#else
    *(mag+i) = (MP_Real_t)( sqrt( re*re+im*im ) );
#endif
  }

}


/****************************************************/
/* Get the complex result with a demodulated signal */
void MP_FFT_Interface_c::exec_complex_demod( MP_Sample_t *in,
					     MP_Sample_t *demodFuncRe, MP_Sample_t *demodFuncIm,
					     MP_Real_t *re, MP_Real_t *im ) {

  unsigned long int i;

  /* Simple buffer check */
  assert( in != NULL );
  assert( demodFuncRe != NULL );
  assert( demodFuncIm != NULL );
  assert( re != NULL );
  assert( im != NULL );

  /* RE */
  /* Demodulate the input signal with the real part of the demodulation function */
  for ( i=0; i<windowSize; i++ ) {
    *(inDemodulated+i) = (double)(*(demodFuncRe+i)) * (double)(*(in+i));
  }
  /* Execute the FFT */
  exec_complex( inDemodulated, bufferRe, bufferIm );

  /* IM */
  /* Demodulate the input signal with the imaginary part of the demodulation function */
  for ( i=0; i<windowSize; i++ ) {
    *(inDemodulated+i) = (double)(*(demodFuncIm+i)) * (double)(*(in+i));
  }
  /* Execute the FFT */
  exec_complex( inDemodulated, buffer2Re, buffer2Im );

  /* COMBINATION */
  /* Combine both parts to get the final result */
  for ( i=0; i<fftRealSize; i++ ) {
    *(re+i) = bufferRe[i] - buffer2Im[i];
    *(im+i) = bufferIm[i] + buffer2Re[i];
  }
  /* Ensure that the imaginary part of the DC and Nyquist frequency components are zero */
  *(im) = 0.0;
  *(im+fftRealSize-1) = 0.0;

}


/*********************************/
/*                               */
/*             GENERIC TEST      */
/*                               */
/*********************************/
int MP_FFT_Interface_c::test( const unsigned long int setWindowSize , 
			      const unsigned char windowType,
			      const double windowOption,
			      MP_Sample_t *samples) {

  MP_FFT_Interface_c* fft = MP_FFT_Interface_c::init( setWindowSize, windowType, windowOption, setWindowSize/2+1 );
  unsigned long int i;
  MP_Real_t amp,energy1,energy2,tmp;

  /* -1- Compute the energy of the analyzed signal multiplied by the analysis window */
  energy1 = 0.0;
  for (i=0; i < setWindowSize; i++) {
    amp = samples[i]*(fft->window[i]);
    energy1 += amp*amp;
  }
  /* -2- The resulting complex FFT should be of the same energy multiplied by windowSize */
  energy2 = 0.0;
  fft->exec_complex(samples,fft->bufferRe,fft->bufferIm);
  amp = fft->bufferRe[0];
  energy2 += amp*amp;
  for (i=1; i< (fft->fftRealSize-1); i++) {
    amp = fft->bufferRe[i];
    energy2 += 2*amp*amp;
    amp = fft->bufferIm[i];
    energy2 += 2*amp*amp;
  }
  amp = fft->bufferRe[fft->fftRealSize-1];
  energy2 += amp*amp;

  tmp = fabsf((energy2/(setWindowSize*energy1))-1);
  if ( tmp < MP_FFT_TEST_PRECISION ) {
    printf("FFT size [%ld] energy in/out = 1+/-%g OK\n",
	   setWindowSize,tmp);  
    return(0);
  }
  else {
    printf("FFT size [%ld] energy |in/out-1|= %g > %g!!!\n",
	   setWindowSize, tmp, MP_FFT_TEST_PRECISION);  
    return(1);
  }

}


/*********************************/
/*                               */
/* FFTW-DEPENDENT IMPLEMENTATION */
/*                               */
/*********************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/****/
/* Constructor where the window is actually generated */
MP_FFTW_Interface_c::MP_FFTW_Interface_c( const unsigned long int setWindowSize,
					  const unsigned char setWindowType,
					  const double setWindowOption,
					  const unsigned long int setFftRealSize )
  :MP_FFT_Interface_c( setWindowSize, setWindowType, setWindowOption, setFftRealSize ) {

  /* FFTW takes integer FFT sizes => check if the cast (int)(fftCplxSize) will overflow. */
  assert( fftCplxSize <= INT_MAX );

  /* Allocate the necessary buffers */
  inPrepared =       (double*) fftw_malloc( sizeof(double)       * fftCplxSize );
  out        = (fftw_complex*) fftw_malloc( sizeof(fftw_complex) * fftRealSize );
  /* Call the FFTW planning utility */
  p = fftw_plan_dft_r2c_1d( (int)(fftCplxSize), inPrepared, out, FFTW_MEASURE );
  
}


/**************/
/* Destructor */
MP_FFTW_Interface_c::~MP_FFTW_Interface_c() {

  fftw_free( inPrepared );
  fftw_free( out );
  fftw_destroy_plan( p );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/**************************/
/* Get the complex result */
void MP_FFTW_Interface_c::exec_complex( MP_Sample_t *in, MP_Real_t *re, MP_Real_t *im ) {

  unsigned long int i;
  double re_out, im_out;

  /* Simple buffer check */
  assert( in != NULL );
  assert( re != NULL );
  assert( im != NULL );

  /* Did anyone hook some buffers ? */
  assert( in  != NULL );
  assert( window  != NULL );

  /* Copy and window the input signal */
  for ( i=0; i<windowSize; i++ ) {
    *(inPrepared+i) = (double)(*(window+i)) * (double)(*(in+i));
  }
  /* Perform the zero padding */
  for ( i=windowSize; i<fftCplxSize; i++ ) {
    *(inPrepared+i) = 0.0;
  }

  /* Execute the FFT described by plan "p"
     (which itself points to the right input/ouput buffers,
     such as buffer inPrepared etc.) */
  fftw_execute( p );

  /* Cast and copy the result */
  for ( i=0; i<fftRealSize; i++ ) {
    re_out = out[i][0];
    im_out = out[i][1];
    *(re+i) = (MP_Real_t)( re_out );
    *(im+i) = (MP_Real_t)( im_out );
  }
  /* Ensure that the imaginary part of the DC and Nyquist frequency components are zero */
  *(im) = 0.0;
  *(im+fftRealSize-1) = 0.0;

}
