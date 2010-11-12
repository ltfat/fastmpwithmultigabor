/******************************************************************************/
/*                                                                            */
/*                              fft_interface.h                               */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Boris Mailhé                                               Mon 25 Oct 2010 */
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


/* This module provides an abstraction to interface
   the package with any DCT package */


#ifndef __dct_interface_h_
#define __dct_interface_h_

/*-------------------*/
/** \brief A generic interface between MPLib and many possible DCT
 *  implementations.
 *
 * Example code on how to use this class can be found in the file test_dct.cpp
 */
/*-------------------*/

class MP_DCT_Interface_c
  {

    /********/
    /* DATA */
    /********/

  public:

    /** \brief size of the signal on which the FFT is performed (including zero padding).
     *  \remark needs to be even.
     */
    unsigned long int dctSize;
    MP_Real_t* buffer;
    
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
   MPTK_LIB_EXPORT static MP_DCT_Interface_c* init(const unsigned long int dctSize);

    /** \brief A generic method to test if the default instantiation of the FFT class for a given
     * window scales correctly the energy of a signal, which is a clue whether it is correctly implemented */
   MPTK_LIB_EXPORT static int test(const double precision , unsigned long int  dctSize, MP_Real_t *samples);


    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  protected:

    /** \brief A generic constructor used only by non-virtual children classes */
    MPTK_LIB_EXPORT MP_DCT_Interface_c(const unsigned long int dctSize );

  public:
    MPTK_LIB_EXPORT virtual ~MP_DCT_Interface_c();


    /***************************/
    /* OTHER METHODS           */
    /***************************/

  public:

    /** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
     *
     * \param in  input signal buffer, only the first windowSize values are used.
     * \param out  output the DCT coefficients, only the first numFreqs values are filled.
     * The output buffers are filled with the values
     * \f[
     * \mbox{out}[k] = \frac{1}{\sqrt(\mbox{\dctSize})}\sum_{n=0}^{\mbox{dctSize}-1}\ mbox{in}[n] \cdot \cos\left(\frac{\pi}{\mbox{dctSize}}
     * \left(n+\frac{1}{2}\right)\left(k+\frac{1}{2}\right).
     * \f]
     * for \f$0 \leq k < \mbox{dctSize} 
     * where the signal is zero padded beyond the window size if necessary.
     *
     *
     */
    MPTK_LIB_EXPORT virtual void exec_dct( MP_Real_t *in, MP_Real_t *out ) = 0;
    MPTK_LIB_EXPORT inline void exec_dct_inverse(MP_Real_t* in, MP_Real_t* out){
        exec_dct(in, out);
    }

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
   MPTK_LIB_EXPORT static bool init_dct_library_config();

    /** \brief Save the fft library configuration
     * If FFTW3 is used, a wisdom file can be saved if it's necessary, this files will be used
     * to set the FFTW wisdom to fix FFTW plan for another computation
     */
   MPTK_LIB_EXPORT static bool save_dct_library_config();
   
   virtual MP_Real_t test(){return MP_PI;}

  };



/*-----------------------------------*/
/* Choice of the FFT implementation: */
/*-----------------------------------*/

#ifdef USE_FFTW3

#include <fftw3.h>

/*********************************/
/*                               */
/* FFTW-DEPENDENT IMPLEMENTATION */
/*                               */
/*********************************/

/** \brief The implementation of MP_DCT_Interface_c using
 * the Fastest Fourier Transform in the West (FFTW)
 * library. See <A HREF="http://www.fftw.org">http://www.fftw.org</A>
 */
class MP_DCTW_Interface_c:public MP_DCT_Interface_c
  {

    /********/
    /* DATA */
    /********/

  private:

    /* FFTW parameters */
    /** \brief object that performs the FFT computations as fast as it can */
    fftw_plan p;

    /** \brief input signal multiplied by the window and zero padded,
     * used as input by the plan  */
    double *inPrepared;
    /** \brief output of the plan */
    double *out;
    double scale;

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
     * \param dctSize size of the performed, including zero padding.
     * For speed reasons it might be preferable to set dctSize = \f$2^n\f$ for some integer n.
     *
     */
    MP_DCTW_Interface_c( const unsigned long int dctSize );

    ~MP_DCTW_Interface_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/
  public:
    /** \brief Performs the complex DCT of an input signal buffer and puts the result in two output buffers.
     *
     * \param in  input signal buffer, only the first windowSize values are used.
     * \param out  output the DCT coefficients, only the first numFreqs values are filled.
     * The output buffers are filled with the values
     * \f[
     * \mbox{out}[k] = \sum_{n=0}^{\mbox{dctSize}-1}\ mbox{in}[n] \cdot \cos\left(\frac{\pi}{\mbox{dctSize}}
     * \left(n+\frac{1}{2}\right)\left(k+\frac{1}{2}\right).
     * \f]
     * for \f$0 \leq k < \mbox{dctSize} 
     * where the signal is zero padded beyond the window size if necessary.
     *
     *
     */
    MPTK_LIB_EXPORT virtual void exec_dct( MP_Real_t *in, MP_Real_t *out );
    
    MP_Real_t test();

  };

#endif /* USE_FFTW3 */

#endif /* __dct_interface_h_ */
