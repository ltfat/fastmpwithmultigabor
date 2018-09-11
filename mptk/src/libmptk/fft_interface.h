/******************************************************************************/
/*                                                                            */
/*                              fft_interface.h                               */
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

/*****************************************/
/*                                       */
/* DEFINITION OF THE FFT INTERFACE CLASS */
/*                                       */
/*****************************************/
/*
 * SVN log:
 *
 * $Author: slesage $
 * $Date: 2007-06-28 16:48:30 +0200 (Thu, 28 Jun 2007) $
 * $Revision: 1083 $
 *
 */

/* This module provides an abstraction to interface the package with any FFT package */


#ifndef __fft_interface_h_
#define __fft_interface_h_

/** \brief A macro which says that the method exec_mag should return
 *  the squared magnitude (i.e., the square root is avoided in the computation
 * of the magnitude). When this macro is NOT defined the square root is used instead.
 */
#define MP_MAGNITUDE_IS_SQUARED

/* Inheritance graph: all the interfaces inherit from
   the generic interface (MP_FFT_Interface_c):
	MP_FFT_Interface_c |-> MP_FFTW_Interface_c
 */


/*-------------------*/
/** \brief A generic interface between MPLib and many possible FFT
 *  implementations.
 *
 * Example code on how to use this class can be found in the file test_fft.cpp
 */
/*-------------------*/

class MP_FFT_Interface_c
{
	/********/
    /* DATA */
    /********/
	public:
		/** \brief type of window used before performing the FFT
		 * (Hamming, Gauss etc.).
		 * 
		 * \sa make_window() */
		unsigned char windowType;
		/** \brief optional window parameter
		 * (applies to Gauss, generalized Hamming and exponential windows).
		 * 
		 * \sa make_window() */
		double windowOption;
		/** \brief size of the window used before performing the FFT.
		 * Combined with numFreqs it determines how much zero padding is performed.
		 *
		 * It is also often called the frame size
		 * \sa make_window() 
		 */
		unsigned long int windowSize;
		/** \brief offset between the sample considered as the 'center'
		 * of the window and its first sample. 
		 *
		 * For most symmetric windows wich are bump functions, the 'center' is the first sample
		 * at which the absolute maximum of the window is reached.
		 * \sa make_window()
		 */
		unsigned long int windowCenter;
		/** \brief size of the signal on which the FFT is performed (including zero padding). */
		unsigned long int fftSize;
		/** \brief size of the output buffers filled by exec_complex() and exec_mag().
		 * It is deduced from fftSize as numFreqs = ( fftSize/2 + 1 )
		 *
		 * \sa exec_complex() 
		 */
		unsigned long int numFreqs;
		/** \brief Pointer on a tabulated window.(DO NOT MALLOC OR FREE IT.) */
		MP_Real_t *window;
	protected:
		/** \brief Four buffers of size numFreqs to store the output of exec_complex() when generic methods
		 * such as fill_correl() or exec_mag() need it */
		MP_Real_t *bufferRe;
		MP_Real_t *bufferIm;
		
		MP_Real_t *buffer2Re;
		MP_Real_t *buffer2Im;

		/** \brief A buffer of size windowSize to multiply the input signal by a demodulation function */
		MP_Real_t *inDemodulated;
    
    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* FACTORY METHOD          */
    /***************************/
	
	public:
		/** \brief The method which should be called to instantiate an FFT_Interface object
		 * for a given window (specified by its type and size) and FFT resolution.
		 *
		 * \param windowSize size of the window
		 * \param windowType type of the window
		 * \param windowOption optional shaping parameter of the window
		 * \param fftSize size of the performed FFT. For speed reasons it might be
		 * preferable to set fftSize = \f$2^n\f$ for some integer n.
		 *
		 * \sa make_window(), exec_complex().
		 */
		MPTK_LIB_EXPORT static MP_FFT_Interface_c* init( const unsigned long int windowSize,
                                     const unsigned char windowType,
                                     const double windowOption,
                                     const unsigned long int fftSize );

		/** \brief A generic method to test if the default instantiation of the FFT class for a given
		 * window scales correctly the energy of a signal, which is a clue whether it is correctly implemented */
		MPTK_LIB_EXPORT static int test(const double presicion , const unsigned long int windowSize,
									const unsigned char windowType,
									const double windowOption,
									MP_Real_t *samples);


	/***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
	protected:
		/** \brief A generic constructor used only by non-virtual children classes */
		MPTK_LIB_EXPORT MP_FFT_Interface_c( const unsigned long int windowSize,
									const unsigned char windowType,
									const double windowOption,
									const unsigned long int fftSize );

	public:
		/** \brief A generic destructor */
		MPTK_LIB_EXPORT virtual ~MP_FFT_Interface_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/
	public:
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		MPTK_LIB_EXPORT virtual void exec_complex( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im ) = 0;
		MPTK_LIB_EXPORT virtual void exec_complex_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im ) = 0;
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 * The input signal is inverted for processing, used by MP_Convolution_FFT_c
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		MPTK_LIB_EXPORT virtual void exec_complex_flip( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im ) = 0;
		MPTK_LIB_EXPORT virtual void exec_complex_flip_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im ) = 0;
	    /** \brief Performs the complex inverse FFT of two input buffers and puts the result in an output signal buffer.
		 *
		 * \param re  input FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  input FFT imaginary part buffer, only the first numFreqs values are filled.
		 * \param output output signal buffer, only the first windowSize values are used.
		 *
		 * These input values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 *
		 * The output buffer is filled with the values
		 * \f[
		 * \mbox{out}[n] = \frac{1}{fftCplxSize}\sum_{k=0}^{\mbox{fftCplxSize}-1} \mbox{window}[k]
		 * \cdot (\mbox{re}[k] + i \mbox{im}[k])  \cdot \exp\left(\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq n < \mbox{fftCplxSize}\f$, 
		 * where the frequency components upon the Nyquist frequency of re
		 * and im are completed, symmetrically, with the conjugate
		 * components of the frequency components below the Nyquist
		 * frequency.
		 */
		MPTK_LIB_EXPORT virtual void exec_complex_inverse( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output ) = 0;
		MPTK_LIB_EXPORT virtual void exec_complex_inverse_without_window( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output ) = 0;
		/** \brief Performs the complex FFT of an input signal buffer multiplied by a
		 * demodulation function, and puts the result in two output buffers.
		 */
		MPTK_LIB_EXPORT virtual void exec_complex_demod( MP_Real_t *in,
                                     MP_Real_t *demodFuncRe, MP_Real_t *demodFuncIm,
                                     MP_Real_t *re, MP_Real_t *im );
                                     
		/** \brief Performs the inverse fft of two input buffers and divide the result by a demodulation function
		 *  \remark the modulus of the demodulation function is assumed to be 1 for each sample. 
		 */
		MPTK_LIB_EXPORT virtual void exec_complex_inverse_demod(MP_Real_t* re, MP_Real_t* im, 
									MP_Real_t *demodFuncRe, MP_Real_t *demodFuncIm,
									MP_Real_t* output);

		/** \brief Computes the power spectrum of an input signal buffer and puts it
		 * in an output magnitude buffer.
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param mag output FFT magnitude buffer, only the first numFreqs values are filled.
		 *
		 * The output buffer is filled with
		 * \f$\mbox{re}[k]^2+\mbox{im}[k]^2\f$ for \f$0 \leq k < \mbox{numFreqs}\f$
		 * unless the macro \a MP_MAGNITUDE_IS_SQUARED is undefined, in which case
		 * the square root is computed.
		 *
		 * \sa The documentation of exec_complex() gives the expression of
		 * \f$(\mbox{re}[k],\mbox{im}[k])\f$ and details about zero padding
		 * and the use of numFreqs.
		 */
		MPTK_LIB_EXPORT virtual void exec_mag( MP_Real_t *in, MP_Real_t *mag );

		/** \brief Initialise the fft library configuration
		 * If FFTW3 is used, a wisdom file is loaded in order to fix the plan used for
		 *  the FFT computation. This configuration allows the reproducibility of the computation 
		 */
		MPTK_LIB_EXPORT static bool init_fft_library_config();

		/** \brief Save the fft library configuration
		 * If FFTW3 is used, a wisdom file can be saved if it's necessary, this files will be used
		 * to set the FFTW wisdom to fix FFTW plan for another computation
		 */
		MPTK_LIB_EXPORT static bool save_fft_library_config();
   
		virtual MP_Real_t test(){return MP_PI;}
};

/*-----------------------------------*/
/* Choice of the FFT implementation: */
/*-----------------------------------*/
#ifdef HAVE_FFTW3
#  define USE_FFTW3 1
#else
#  error "No FFT implementation was found !"
#endif

#ifdef USE_FFTW3
#include <fftw3.h>

/*********************************/
/*                               */
/* FFTW-DEPENDENT IMPLEMENTATION */
/*                               */
/*********************************/

/** \brief The implementation of MP_FFT_Interface_c using
 * the Fastest Fourier Transform in the West (FFTW)
 * library. See <A HREF="http://www.fftw.org">http://www.fftw.org</A>
 */
class MP_FFTW_Interface_c:public MP_FFT_Interface_c
{
	/********/
    /* DATA */
    /********/
	private:
		/* FFTW parameters */
		/** \brief object that performs the FFT computations as fast as it can */
		fftw_plan p;
		/** \brief object that performs the FFT computations as fast as it can */
		fftw_plan iP;
		/** \brief input signal multiplied by the window and zero padded, used as input by the plan  */
		double *inPrepared;
		/** \brief output of the plan */
		fftw_complex *out;

    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
	public:
		/** \brief A constructor which takes as input a window specified
		 * by its type and size.
		 *
		 * \param windowSize size of the window
		 * \param windowType type of the window
		 * \param windowOption optional shaping parameter of the window
		 * \param fftSize size of the performed, including zero padding.
		 * For speed reasons it might be preferable to set fftSize = \f$2^n\f$ for some integer n.
		 *
		 * \sa make_window(), exec_complex().
		 */
		MP_FFTW_Interface_c( const unsigned long int windowSize,
                         const unsigned char windowType,
                         const double windowOption,
                         const unsigned long int fftSize );

		/** \brief A generic destructor */
		~MP_FFTW_Interface_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/
	public:
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 * The input signal is inverted for processing, used by MP_Convolution_FFT_c
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		void exec_complex( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im );
		void exec_complex_flip( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im );
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 * The input signal is inverted for processing, used by MP_Convolution_FFT_c
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		void exec_complex_inverse( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output );
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 * The input signal is inverted for processing, used by MP_Convolution_FFT_c
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		void exec_complex_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im );
		void exec_complex_flip_without_window( MP_Real_t *in, MP_Real_t *re, MP_Real_t *im );
		/** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
		 * The input signal is inverted for processing, used by MP_Convolution_FFT_c
		 *
		 * \param in  input signal buffer, only the first windowSize values are used.
		 * \param re  output FFT real part buffer, only the first numFreqs values are filled.
		 * \param im  output FFT imaginary part buffer, only the first numFreqs values are filled.
		 *
		 * The output buffers are filled with the values
		 * \f[
		 * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
		 * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
		 * \f]
		 * for \f$0 \leq k < \mbox{numFreqs} = \mbox{fftCplxSize}/2+1\f$, 
		 * where the signal is zero padded beyond the window size if necessary.
		 *
		 * These output values correspond to the frequency components between the DC
		 * component and the Nyquist frequency, inclusive.
		 */
		void exec_complex_inverse_without_window( MP_Real_t *re, MP_Real_t *im, MP_Real_t *output );
  };

#endif /* USE_FFTW3 */
#endif /* __fft_interface_h_ */
