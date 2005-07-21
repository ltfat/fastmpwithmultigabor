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
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

/* This module provides an abstraction to interface
   the package with any FFT package */


#ifndef __fft_interface_h_
#define __fft_interface_h_


#include <fftw3.h>


/*--------------------------*/
/* Choice of the interface: */
/*--------------------------*/
/** \brief A macro that defines which particular FFT interface is used
 * in the rest of the package.
 */
#define MP_FFT_Interface_c MP_FFTW_Interface_c 

/** \brief A macro which says that the method exec_mag should return
 *  the squared magnitude (i.e., the square root is avoided in the computation
 * of the magnitude). When this macro is NOT defined the square root is used instead.
 */
#define MP_MAGNITUDE_IS_SQUARED

/* Inheritance graph: all the interfaces inherit from
   the generic interface (MP_FFT_Generic_Interface_c):

       MP_FFT_Generic_Interface_c |-> MP_FFTW_Interface_c
 */


/*-------------------*/
/** \brief A generic interface between MPLib and many possible FFT
 *  implementations.
 * 
 * Example code on how to use this class can be found in the file test_fft.cpp
 */
/*-------------------*/

class MP_FFT_Generic_Interface_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief type of window used before performing the FFT 
   * (Hamming, Gauss etc.).
   * 
   * \sa make_window() */
  unsigned char     windowType;   
  /** \brief optional window parameter
   * (applies to Gauss, generalized Hamming and exponential windows).
   * 
   * \sa make_window() */
  double windowOption;   
  /** \brief size of the window used before performing the FFT.
   * Combined with fftRealSize it determines how much zero padding is performed.
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

  /** \brief size of the output buffers filled by exec_complex(), exec_mag() and exec_energy(). 
   * Combined with windowSize it determines how much zero padding is performed.
   *
   * \sa exec_complex() 
   */
  unsigned long int fftRealSize; 

  /** \brief size of the signal on which the FFT is performed (including zero padding).
   * It is deduced from fftRealSize as fftCplxSize = 2*( fftRealSize-1 )
   */
  unsigned long int fftCplxSize;

  /** \brief Pointer on a tabulated window.(DO NOT MALLOC OR FREE IT.)
   */
  MP_Real_t *window;    
  
  /** \brief Storage space for the real part of the quantity 
   * \f[
   * (\mbox{reCorrel}[k],\mbox{imCorrel[k]}) = 
   * \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}^2[n] \cdot 
   * \exp \left(\frac{2i\pi \cdot (2k)\cdot n}{\mbox{fftCplxSize}}\right)
   * \f]
   * which measures the correlation between complex atoms and their conjugate.
   * (DO NOT MALLOC OR FREE IT.)
   * \sa imCorrel
   */
  MP_Real_t *reCorrel;
  /** \brief Storage space for the imaginary part of the correlation between
   *  complex atoms and  their conjugate. (DO NOT MALLOC OR FREE IT.) 
   * \sa reCorrel */
  MP_Real_t *imCorrel;
  /** \brief Storage space for the squared modulus of the correlation between
   * complex atoms and their conjugate. (DO NOT MALLOC OR FREE IT.) 
   * \sa reCorrel
   *
   * \f$ \mbox{sqCorrel} = \mbox{reCorrel}^2+\mbox{imCorrel}^2 \f$
   */
  MP_Real_t *sqCorrel;
  /** \brief Storage space for a useful constant related to the atoms'
   * autocorrelations with their conjugate. (DO NOT MALLOC OR FREE IT.)
   * \sa sqCorrel 
   *
   * \f$ \mbox{cstCorrel} = \frac{2}{1-\mbox{sqCorrel}} \f$
   * */
  MP_Real_t *cstCorrel;


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
   * \param fftRealSize size of the output buffer filled by exec_complex(), exec_mag() and exec_energy(). Since the complex FFT which is performed is of size
   * \a fftCplxSize = 2(fftRealSize-1), for speed reasons it might be
   * preferable to set fftRealSize = \f$2^n+1 \f$ for some integer n.
   * \warning \a fftRealSize MUST satisfy
   * \f$ 2\times(\mbox{fftRealSize}-1) \geq \mbox{windowSize} \f$
   * otherwise the behaviour is undefined.
   * \sa make_window(), exec_complex().
   */
  MP_FFT_Generic_Interface_c( const unsigned long int windowSize,
			      const unsigned char windowType,
			      const double windowOption,
			      const unsigned long int fftRealSize );
  virtual ~MP_FFT_Generic_Interface_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

private:

  /** \brief Allocates the atom's autocorrelation.
   */
  int alloc_correl( void );

  /** \brief Computes and tabulates the atom's autocorrelation.
   */
  virtual int fill_correl( void ) = 0;


public:

  /** \brief Performs the complex FFT of an input signal buffer and puts the result in two output buffers.
   *
   * \param in  input signal buffer, only the first windowSize values are used.
   * \param re  output FFT real part buffer, only the first fftRealSize values are filled.
   * \param im  output FFT imaginary part buffer, only the first fftRealSize values are filled.
   *
   * The output buffers are filled with the values
   * \f[
   * (\mbox{re}[k],\mbox{im}[k]) = \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}[n]
   * \cdot \mbox{in}[n] \cdot \exp\left(-\frac{2i\pi\ k \cdot n}{\mbox{fftCplxSize}}\right).
   * \f]
   * for \f$0 \leq k < \mbox{fftRealSize} = \mbox{fftCplxSize}/2+1\f$, 
   * where the signal is zero padded beyond the window size if necessary.
   *
   * These output values correspond to the frequency components between the DC
   * component and the Nyquist frequency, inclusive.
   *
   */  
  virtual void exec_complex( MP_Sample_t *in, MP_Real_t *re, MP_Real_t *im ) = 0;

  /** \brief Computes the power spectrum of an input signal buffer and puts it
   * in an output magnitude buffer.
   *
   * \param in  input signal buffer, only the first windowSize values are used.
   * \param mag output FFT magnitude buffer, only the first fftRealSize values are filled.
   *
   * The output buffer is filled with
   * \f$\mbox{re}[k]^2+\mbox{im}[k]^2\f$ for \f$0 \leq k < \mbox{fftRealSize}\f$
   * unless the macro \a MP_MAGNITUDE_IS_SQUARED is undefined, in which case
   * the square root is computed.
   *
   * \sa The documentation of exec_complex() gives the expression of
   * \f$(\mbox{re}[k],\mbox{im}[k])\f$ and details about zero padding
   * and the use of fftRealSize.
   */  
  virtual void exec_mag( MP_Sample_t *in, MP_Real_t *mag ) = 0;

  /** \brief Computes a special version of the power spectrum of an input signal buffer and
   * puts it in an output magnitude buffer. This version corresponds to the actual projection
   * of a real valued signal on the space spanned by a complex atom and its conjugate transpose.
   *
   * \param in  input signal buffer, only the first windowSize values are used.
   * \param mag output FFT magnitude buffer, only the first fftRealSize values are filled.
   *
   * As explained in Section 3.2.3 (Eq. 3.30) of the Ph.D. thesis of Remi 
   * Gribonval, for a normalized atom \f$g\f$ with
   * \f$|\langle g,\overline{g}\rangle|<1\f$ we have
   * \f[
   * \mbox{energy} = 
   * \frac{2}{1-|\langle g,\overline{g}\rangle|^2}
   *  \cdot \mbox{Real} \left(
   * |\langle \mbox{sig},g \rangle|^2 -
   * \langle g,\overline{g}\rangle 
   * \langle \mbox{sig},g \rangle^2\right)
   * \f]
   * so with \f$(\mbox{re},\mbox{im}) = \langle \mbox{sig},g \rangle\f$ 
   * and \f$(\mbox{reCorrel},\mbox{imCorrel}) = \langle g,\overline{g}\rangle\f$  
   * and the definition of \a sqCorrel and \a \cstCorrel we have
   *
   * \f[
   * \mbox{energy} = 
   * \mbox{cstCorre} \times
   * \left(\mbox{re}^2+\mbox{im}^2-
   * \mbox{reCorrel} *
   * \left(\mbox{re}^2-\mbox{im}^2\right) +
   * 2 * \mbox{imCorrel} * \mbox{re} * \mbox{im}
   * \right)
   * \f]
   *
   * In the case of a real valued atom (\f$\langle g,\overline{g}\rangle = 1\f$)
   * or when \f$\langle g,\overline{g}\rangle\f$ is very small
   * we simply have
   * \f[
   * \mbox{energy} = \mbox{re}^2+\mbox{im}^2.
   * \f]
   * \sa The documentation of exec_complex() gives the expression of \f$(\mbox{re}[k],\mbox{im}[k])\f$
   * and details about zero padding and the use of fftRealSize.
   */  
  virtual void exec_energy( MP_Sample_t *in, MP_Real_t *mag ) = 0;

};



/*********************************/
/*                               */
/* FFTW-DEPENDENT IMPLEMENTATION */
/*                               */
/*********************************/

/** \brief The implementation of MP_FFT_Interface_c using
 * the Fastest Fourier Transform in the West (FFTW)
 * library. See <A HREF="http://www.fftw.org">http://www.fftw.org</A>
 */
class MP_FFTW_Interface_c:public MP_FFT_Generic_Interface_c {

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
   * \param fftRealSize size of the output buffer filled by exec_complex(), exec_mag() and exec_energy(). Since the complex FFT which is performed is of size
   * \a fftCplxSize = 2(fftRealSize-1), for speed reasons it might be
   * preferable to set fftRealSize = \f$2^n+1 \f$ for some integer n.
   * \warning \a fftRealSize MUST satisfy
   * \f$ 2\times(\mbox{fftRealSize}-1) \geq \mbox{windowSize} \f$
   * otherwise the behaviour is undefined.
   * \sa make_window(), exec_complex().
   */
  MP_FFTW_Interface_c( const unsigned long int windowSize,
		       const unsigned char windowType,
		       const double windowOption,
		       const unsigned long int fftRealSize );

  ~MP_FFTW_Interface_c();

  /***************************/
  /* OTHER METHODS           */
  /***************************/
private:
  inline void common_FFTW_constructor(void);
  int fill_correl( void );
  inline void exec( MP_Sample_t *in );
public:
  void exec_complex( MP_Sample_t *in, MP_Real_t *re, MP_Real_t *im );
  void exec_mag( MP_Sample_t *in, MP_Real_t *mag );
  void exec_energy( MP_Sample_t *in, MP_Real_t *mag );

};


#endif /* __fft_interface_h_ */
