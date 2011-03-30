/******************************************************************************/
/*                                                                            */
/*                             fft_interface.cpp                              */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
 * $Author: broy $
 * $Date: 2007-06-29 18:03:53 +0200 (Fri, 29 Jun 2007) $
 * $Revision: 1084 $
 *
 */

/**************************************************************/
/*                                                            */
/* fft_interface.cpp: generic interface for FFT libraries     */
/*                                                            */
/**************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include <fstream>


/*********************************/
/*                               */
/* GENERIC INTERFACE             */
/*                               */
/*********************************/

/***************************/
/* FACTORY METHOD          */
/***************************/
MP_FFT_Interface_c* MP_FFT_Interface_c::init( const unsigned long int setWindowSize, const unsigned char setWindowType, const double setWindowOption, const unsigned long int setFftSize )
{
	MP_FFT_Interface_c* fft = NULL;

	if ( setFftSize < setWindowSize )
    {
		mp_error_msg( "MP_FFT_Interface_c::init()","Can't create a FFT of size %lu smaller than the window size %lu. Returning a NULL fft object.\n", setFftSize, setWindowSize);
		return( NULL );
    }

	/* Create the adequate FFT and check the returned address */
#ifdef USE_FFTW3
	fft = (MP_FFT_Interface_c*) new MP_FFTW_Interface_c( setWindowSize, setWindowType, setWindowOption, setFftSize );
	if ( fft == NULL ) 
		mp_error_msg( "MP_FFT_Interface_c::init()", "Instanciation of FFTW_Interface failed. Returning a NULL fft object.\n" );
#elif defined(USE_MAC_FFT)
	fft = (MP_FFT_Interface_c*) new MP_MacFFT_Interface_c( setWindowSize, setWindowType, setWindowOption, setFftSize );
	if ( fft == NULL ) 
		mp_error_msg( "MP_FFT_Interface_c::init()","Instanciation of MacFFT_Interface failed."" Returning a NULL fft object.\n" );
#else
#  error "No FFT implementation was found !"
#endif

	if ( fft == NULL)
	{
		mp_error_msg( "MP_FFT_Interface_c::init()","FFT window is NULL. Returning a NULL fft object.\n");
		return( NULL );
	}

	// Check the internal buffers:
	// - window :
	if ( fft->window == NULL )
    {
		mp_error_msg( "MP_FFT_Interface_c::init()","FFT window is NULL. Returning a NULL fft object.\n");
		delete( fft );
		return( NULL );
    }
	// - other buffers:
	if ( ( fft->bufferRe  == NULL ) || ( fft->bufferIm  == NULL ) || ( fft->buffer2Re == NULL ) || ( fft->buffer2Im == NULL ) || ( fft->inDemodulated == NULL ))
    {
		mp_error_msg( "MP_FFT_Interface_c::init()","One or several of the internal FFT buffers are NULL. Returning a NULL fft object.\n");
		delete( fft );
		return( NULL );
    }

	// If everything went OK, just pop it !
	return( fft );
}

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/***********************************/
/* Constructor with a typed window */
MP_FFT_Interface_c::MP_FFT_Interface_c( const unsigned long int setWindowSize, const unsigned char setWindowType, const double setWindowOption, const unsigned long int setFftSize )
{
	/* Set values */
	windowSize = setWindowSize;
	windowType = setWindowType;
	windowOption = setWindowOption;
	fftSize = setFftSize;
	numFreqs = (fftSize >> 1) + 1;

	assert( fftSize >= setWindowSize );

	// Compute the window and get its center point
	windowCenter = MPTK_Server_c::get_win_server()->get_window( &window, windowSize, windowType, windowOption );

	// Allocate some other buffers
	bufferRe = (MP_Real_t*) malloc(sizeof(MP_Real_t)*numFreqs);
	bufferIm = (MP_Real_t*) malloc(sizeof(MP_Real_t)*numFreqs);

	buffer2Re = (MP_Real_t*) malloc(sizeof(MP_Real_t)*numFreqs);
	buffer2Im = (MP_Real_t*) malloc(sizeof(MP_Real_t)*numFreqs);

	inDemodulated = (MP_Real_t*) malloc(sizeof(MP_Real_t)*windowSize);
}


/**************/
/* Destructor */
MP_FFT_Interface_c::~MP_FFT_Interface_c( )
{
	if ( bufferRe )      free( bufferRe );
	if ( bufferIm )      free( bufferIm );
	if ( buffer2Re )     free( buffer2Re );
	if ( buffer2Im )     free( buffer2Im );
	if ( inDemodulated ) free( inDemodulated );
}

/***************************/
/* OTHER METHODS           */
/***************************/

/***************************/
/* EXECUTION METHODS       */
/***************************/

/**************************/
/* Get the magnitude only */
void MP_FFT_Interface_c::exec_mag( MP_Real_t *in, MP_Real_t *mag )
{
	unsigned long int i;
	double re, im;

	// Simple buffer check
	assert( in  != NULL );
	assert( mag != NULL );

	// Execute the FFT
	exec_complex( in, bufferRe, bufferIm );

	// Get the resulting magnitudes
	for ( i=0; i<numFreqs; i++ )
    {
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
void MP_FFT_Interface_c::exec_complex_demod( MP_Real_t *in, MP_Real_t *demodFuncRe, MP_Real_t *demodFuncIm, MP_Real_t *re, MP_Real_t *im )
{
	unsigned long int i;

	// Simple buffer check
	assert( in != NULL );
	assert( demodFuncRe != NULL );
	assert( demodFuncIm != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// RE
	// Demodulate the input signal with the real part of the demodulation function
	for ( i=0; i<windowSize; i++ )
    {
		*(inDemodulated+i) = (double)(*(demodFuncRe+i)) * (double)(*(in+i));
    }
	// Execute the FFT
	exec_complex( inDemodulated, bufferRe, bufferIm );

	// IM
	// Demodulate the input signal with the imaginary part of the demodulation function
	for ( i=0; i<windowSize; i++ )
    {
		*(inDemodulated+i) = (double)(*(demodFuncIm+i)) * (double)(*(in+i));
    }
	// Execute the FFT
	exec_complex( inDemodulated, buffer2Re, buffer2Im );

	// COMBINATION
	// Combine both parts to get the final result
	for ( i=0; i<numFreqs; i++ )
    {
		*(re+i) = bufferRe[i] - buffer2Im[i];
		*(im+i) = bufferIm[i] + buffer2Re[i];
    }
}

void MP_FFT_Interface_c::exec_complex_inverse_demod(MP_Real_t* re, MP_Real_t* im, MP_Real_t *demodFuncRe, MP_Real_t *demodFuncIm, MP_Real_t* output)
{
    unsigned int t,f;

    // separate the odd and even parts
	//    *bufferRe = *re;
	//    *bufferIm = *im;
	//    *buffer2Re = 0;
	//    *buffer2Im = 0;
    
	for (f = 0; f < numFreqs; f++)
	{
        bufferRe[f] = re[f];
        bufferIm[f] = im[f];
        buffer2Re[f] = im[f];
        buffer2Im[f] = re[f];
    }
    
    // compute inverse fft. The real part will be in inDemodulated, the imaginary part in output
    exec_complex_inverse(bufferRe, bufferIm, inDemodulated);
    exec_complex_inverse(buffer2Re, buffer2Im, output);
    
    // modulate.
    //WARNING: this formula is only right with unitary demodulation functions.
    for (t=0; t<windowSize; t++)
        *(output+t) = *(demodFuncRe+t)*(*(inDemodulated+t)) + *(demodFuncIm+t)*(*(output+t));
}
      

/*********************************/
/*                               */
/*             GENERIC TEST      */
/*                               */
/*********************************/
int MP_FFT_Interface_c::test( const double presicion, const unsigned long int setWindowSize, const unsigned char windowType, const double windowOption, MP_Real_t *samples)
{
	MP_FFT_Interface_c* fft = MP_FFT_Interface_c::init( setWindowSize, windowType, windowOption, setWindowSize );
	unsigned long int i;
	MP_Real_t amp,energy1,energy2,tmp;

	// -1- Compute the energy of the analyzed signal multiplied by the analysis window
	energy1 = 0.0;
	for (i=0; i < setWindowSize; i++)
    {
		amp = samples[i]*(fft->window[i]);
		energy1 += amp*amp;
    }
	
	// -2- The resulting complex FFT should be of the same energy multiplied by windowSize
	energy2 = 0.0;
	fft->exec_complex(samples,fft->bufferRe,fft->bufferIm);
	amp = fft->bufferRe[0];
	energy2 += amp*amp;
	for (i=1; i< (fft->numFreqs-1); i++)
    {
		amp = fft->bufferRe[i];
		energy2 += 2*amp*amp;
		amp = fft->bufferIm[i];
		energy2 += 2*amp*amp;
    }
	amp = fft->bufferRe[fft->numFreqs-1];
	energy2 += amp*amp;

	tmp = fabsf((float)energy2 /(setWindowSize*(float)(energy1))-1);
	if ( tmp < presicion )
    {
		mp_info_msg( "MP_FFT_Interface_c::test()","SUCCESS for FFT size [%ld] energy in/out = 1+/-%g\n",setWindowSize,tmp);
		delete(fft);
		return(0);
    }
	else
    {
		mp_error_msg( "MP_FFT_Interface_c::test()", "FAILURE for FFT size [%ld] energy |in/out-1|= %g > %g\n", setWindowSize, tmp, presicion);
		delete(fft);
		return(1);
    }
}


/*********************************/
/*                               */
/* FFTW-DEPENDENT IMPLEMENTATION */
/*                               */
/*********************************/

/* Utilities to save and load FFT library config ("wisdom files")*/
static void my_fftw_write_char(char c, void *f) 
{ 
	fputc(c, (FILE *) f); 
}
#define fftw_export_wisdom_to_file(f) fftw_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwf_export_wisdom_to_file(f) fftwf_export_wisdom(my_fftw_write_char, (void*) (f))
#define fftwl_export_wisdom_to_file(f) fftwl_export_wisdom(my_fftw_write_char, (void*) (f))

static int my_fftw_read_char(void *f) 
{ 
	return fgetc((FILE *) f); 
}
#define fftw_import_wisdom_from_file(f) fftw_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwf_import_wisdom_from_file(f) fftwf_import_wisdom(my_fftw_read_char, (void*) (f))
#define fftwl_import_wisdom_from_file(f) fftwl_import_wisdom(my_fftw_read_char, (void*) (f))

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/
#ifdef USE_FFTW3
/****/
/* Constructor where the window is actually generated */
MP_FFTW_Interface_c::MP_FFTW_Interface_c( const unsigned long int setWindowSize, const unsigned char setWindowType, const double setWindowOption, const unsigned long int setFftSize ):MP_FFT_Interface_c( setWindowSize, setWindowType, setWindowOption, setFftSize )
{
	// FFTW takes integer FFT sizes => check if the cast (int)(fftCplxSize) will overflow.
	assert( fftSize <= INT_MAX );
	// Allocate the necessary buffers
	inPrepared =       (double*) fftw_malloc( sizeof(double)       * fftSize );
	out        = (fftw_complex*) fftw_malloc( sizeof(fftw_complex) * numFreqs );

	// Create plans
	p = fftw_plan_dft_r2c_1d( (int)(fftSize), inPrepared, out, FFTW_MEASURE );
	iP = fftw_plan_dft_c2r_1d( (int)(fftSize), out, inPrepared, FFTW_MEASURE );
}

/**************/
/* Destructor */
MP_FFTW_Interface_c::~MP_FFTW_Interface_c()
{
	fftw_free( inPrepared );
	fftw_free( out );
	fftw_destroy_plan( p );
	fftw_destroy_plan( iP );
}


/***************************/
/* OTHER METHODS           */
/***************************/

/**************************/
/* Get the complex result */
void MP_FFTW_Interface_c::exec_complex( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im )
{
	unsigned long int i;
	double re_out, im_out;

	// Simple buffer check
	assert( in != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Did anyone hook some buffers ?
	assert( in  != NULL );
	assert( window  != NULL );

	// Copy and window the input signal
	for ( i=0; i<windowSize; i++ )
    {
		*(inPrepared+i) = (double)(*(window+i)) * (double)(*(in+i));      
    }
	// Perform the zero padding
	for ( i=windowSize; i<fftSize; i++ )
    {
		*(inPrepared+i) = 0.0;
    }

	// Execute the FFT described by plan "p" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( p );

	// Cast and copy the result
	for ( i=0; i<numFreqs; i++ )
    {
		re_out = out[i][0];
		im_out = out[i][1];
		*(re+i) = (MP_Real_t)( re_out );
		*(im+i) = (MP_Real_t)( im_out );
    }
	// Ensure that the imaginary part of the DC and Nyquist frequency components are zero
	*(im) = (MP_Real_t)0.0;
	*(im+numFreqs-1) = (MP_Real_t)0.0;
}

void MP_FFTW_Interface_c::exec_complex_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im )
{
	unsigned long int i;
	double re_out, im_out;

	// Simple buffer check
	assert( in != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Did anyone hook some buffers ?
	assert( in  != NULL );
	assert( window  != NULL );

	// Copy and window the input signal
	for ( i=0; i<windowSize; i++ )
    {
		*(inPrepared+i) = (double)(*(in+i));
    }
	// Perform the zero padding
	for ( i=windowSize; i<fftSize; i++ )
    {
		*(inPrepared+i) = 0.0;
    }

	// Execute the FFT described by plan "p" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( p );

	// Cast and copy the result
	for ( i=0; i<numFreqs; i++ )
    {
		re_out = out[i][0];
		im_out = out[i][1];
		*(re+i) = (MP_Real_t)( re_out );
		*(im+i) = (MP_Real_t)( im_out );
    }
	// Ensure that the imaginary part of the DC and Nyquist frequency components are zero
	*(im) = (MP_Real_t)0.0;
	*(im+numFreqs-1) = (MP_Real_t)0.0;
}

void MP_FFTW_Interface_c::exec_complex_flip( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im )
{

	unsigned long int i;
	double re_out, im_out;

	// Simple buffer check
	assert( in != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Did anyone hook some buffers ?
	assert( in  != NULL );
	assert( window  != NULL );

	// Copy and window the input signal
	// test the window and if rectangular do not multiply by window +i
	for ( i=0; i<windowSize; i++ )
    {
		*(inPrepared+(windowSize-1-i)) = (double)(*(window+i)) * (double)(*(in+i));      
    }
	// Perform the zero padding
	for ( i=windowSize; i<fftSize; i++ )
    {
		*(inPrepared+i) = 0.0;
    }

	// Execute the FFT described by plan "p" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( p );

	// Cast and copy the result
	for ( i=0; i<numFreqs; i++ )
    {
		re_out = out[i][0];
		im_out = out[i][1];
		*(re+i) = (MP_Real_t)( re_out );
		*(im+i) = (MP_Real_t)( im_out );
    }
	// Ensure that the imaginary part of the DC and Nyquist frequency components are zero
	*(im) = (MP_Real_t)0.0;
	*(im+numFreqs-1) = (MP_Real_t)0.0;
}

void MP_FFTW_Interface_c::exec_complex_flip_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im )
{
	unsigned long int i;
	double re_out, im_out;

	// Simple buffer check
	assert( in != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Did anyone hook some buffers ?
	assert( in  != NULL );
	assert( window  != NULL );

	// Copy and window the input signal
	// test the window and if rectangular do not multiply by window +i
	for ( i=0; i<windowSize; i++ )
    {
		*(inPrepared+(windowSize-1-i)) = (double)(*(in+i));
    }
	// Perform the zero padding
	for ( i=windowSize; i<fftSize; i++ )
    {
		*(inPrepared+i) = 0.0;
    }

	// Execute the FFT described by plan "p" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( p );

	// Cast and copy the result
	for ( i=0; i<numFreqs; i++ )
    {
		re_out = out[i][0];
		im_out = out[i][1];
		*(re+i) = (MP_Real_t)( re_out );
		*(im+i) = (MP_Real_t)( im_out );
    }
	// Ensure that the imaginary part of the DC and Nyquist frequency components are zero
	*(im) = (MP_Real_t)0.0;
	*(im+numFreqs-1) = (MP_Real_t)0.0;
}

/***********************/
/* Get the real result */
void MP_FFTW_Interface_c::exec_complex_inverse( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output )
{
	unsigned long int i;

	// Simple buffer check
	assert( output != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Copy and window the input frequency components
	for ( i=0; i<numFreqs; i++ )
    {
		out[i][0] = (double)(*(window+i)) * (double)(*(re+i));
		out[i][1] = (double)(*(window+i)) * (double)(*(im+i));
    }
  
	// Execute the inverse FFT described by plan "iP" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( iP );

	// Cast and copy the result
	for ( i=0; i<fftSize; i++ )
    {
		*(output+i) = (MP_Real_t)( *(inPrepared+i) );
    }
}

void MP_FFTW_Interface_c::exec_complex_inverse_without_window( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output )
{
	unsigned long int i;
  
	// Simple buffer check
	assert( output != NULL );
	assert( re != NULL );
	assert( im != NULL );

	// Copy and window the input frequency components
	for ( i=0; i<numFreqs; i++ )
    {
		out[i][0] = (double)(*(re+i));
		out[i][1] = (double)(*(im+i));
    }
  
	// Execute the inverse FFT described by plan "iP" (which itself points to the right input/ouput buffers, such as buffer inPrepared etc.)
	fftw_execute( iP );

	// Cast and copy the result
	for ( i=0; i<fftSize; i++ )
    {
		*(output+i) = (MP_Real_t)( *(inPrepared+i) );
    }
}
#endif

/* Init FFT library config */
bool MP_FFT_Interface_c::init_fft_library_config()
{
#ifdef USE_FFTW3
	const char	*func =  "MP_FFT_Interface_c::init_fft_library_config()";
	int			wisdom_status;
	FILE		*wisdomFile = NULL;
	const char	*filename;

	// Check if file path is defined in env variable
	filename = MPTK_Env_c::get_env()->get_config_path("fftw_wisdomfile");
	
	if (NULL != filename)
		wisdomFile= fopen(filename,"r");
	// Check if file exists
	if (wisdomFile!=NULL)
    {
		// Try to load the wisdom file for creating fftw plan
		wisdom_status = fftw_import_wisdom_from_file(wisdomFile);
      
		// Check if wisdom file is well formed
		if (wisdom_status==0)
        {
			mp_error_msg( func, "wisdom file is ill formed\n");
			// Close the file anyway
			fclose(wisdomFile);
			return false;
        }
		else
        {
			MPTK_Env_c::get_env()->set_fftw_wisdom_loaded();
			// Close the file
			fclose(wisdomFile);
			return true;
        }
    }
	else
	{
		mp_warning_msg( func, "fftw wisdom file with path %s  doesn't exist.\n", filename);
		mp_warning_msg( func, "It will be created.\n");
		mp_warning_msg( func, "NB: An fftw wisdom file allows MPTK to run slighty faster,\n");
		mp_warning_msg( func, "    however its absence is otherwise harmless.\n");
		mp_warning_msg( func, "    YOU CAN SAFELY IGNORE THIS WARNING MESSAGE.\n");
		return false;
	}
#else
	return false;
#endif
}

bool MP_FFT_Interface_c::save_fft_library_config()
{
#ifdef USE_FFTW3
	FILE		*wisdomFile = NULL;
	const char	*filename;
	
	// Check if fftw wisdom file has to be saved and if the load of this files  succeed when init the fft library config
	filename = MPTK_Env_c::get_env()->get_config_path("fftw_wisdomfile");
	if (NULL!=filename && !MPTK_Env_c::get_env()->get_fftw_wisdom_loaded())
		wisdomFile = fopen(filename,"w");
	// Check if file exists or if the files could be created
	if (wisdomFile!=NULL)
    {
		// Export the actual wisdom to the file
		fftw_export_wisdom_to_file(wisdomFile);
		// Close the file
		fclose(wisdomFile);
		return true;
    }
	else
    {
		return false;
	}
#else
	return false;
#endif
}
