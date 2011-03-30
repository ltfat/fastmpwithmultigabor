/******************************************************************************/
/*                                                                            */
/*                               dsp_windows.h                                */
/*                                                                            */
/*                    Digital Signal Processing Windows                       */
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
 * $Author: broy $
 * $Date: 2007-05-25 17:36:47 +0200 (Fri, 25 May 2007) $
 * $Revision: 1056 $
 *
 */

#ifdef _WIN32
#if defined(_MSC_VER)
#define MPTK_LIB_EXPORT __declspec(dllexport)  /* export function out of the lib */
#define MPTK_LIB_IMPORT __declspec(dllimport)  /* import function in the lib */
#else
#define MPTK_LIB_EXPORT
#define MPTK_LIB_IMPORT
#endif
#else
#define MPTK_LIB_EXPORT
#define MPTK_LIB_IMPORT
#endif

#ifndef __dsp_windows_h_
#define __dsp_windows_h_

/********************/
/* Basic data types */
/********************/
/** \brief The type of window samples (i.e., float, or double, etc.) */
#define Dsp_Win_t double

/** \brief A special type of window returned by window_type() when a window name is unknown 
*/
#define DSP_UNKNOWN_WIN     0

/** \brief The most basic window type  
 */
#define DSP_RECTANGLE_WIN   1
/** \brief The type of triangular windows 
 *
 * \f[
 * w[n] \propto
 * \left\{\begin{array}{ll}
 * n & 0 \leq n < \lfloor \frac{N}{2} \rfloor\\
 * (N-1)-n & \lfloor \frac{N}{2} \rfloor \leq n \leq N-1
 * \end{array}\right.
 * \f]
 */
#define DSP_TRIANGLE_WIN    2
/** \brief The type of cosine windows 
 *
 * \f[
 * w[n] \propto \sin\left(\pi \cdot \frac{n}{N-1}\right)
 * \f]
 */
#define DSP_COSINE_WIN      3
/** \brief The type of Hanning windows (also called Hann windows)
 *
 * \f[
 * w[n] \propto 0.5 \left(1-\cos \left(2\pi \frac{n}{N-1}\right)\right)
 * \f]
 */
#define DSP_HANNING_WIN     4
/** \brief The type of Hamming windows
 *
 * \f[
 * w[n] \propto 0.54-0.46 \cos\left(2\pi \frac{n}{N-1}\right)
 * \f]
 */
#define DSP_HAMMING_WIN     5
/** \brief The type of generalized Hamming windows.
 * The \a optional parameter determines\f$\alpha\f$. 
 *
 * \f[
 * w[n] \propto \alpha + (\alpha-1) \cos \left(2\pi \frac{n}{N-1}\right)
 * \f]
 */
#define DSP_HAMGEN_WIN      6 
#define DSP_HAMGEN_DEFAULT_OPT 0.54

/** \brief The type of Blackman windows 
 *
 * \f[
 * w[n] \propto 0.42-0.5 \cos \left(2\pi\frac{n}{N-1}\right)+0.08 \cos \left(4\pi\frac{n}{N-1}\right)
 * \f]
 */
#define DSP_BLACKMAN_WIN    7
/** \brief The type of Flat top windows 
 *
 * \f[
 * w[n] \propto 0.2156 -
 * 0.4160 \cos\left(2\pi \frac{n}{N-1}\right) +
 * 0.2781 \cos\left(4\pi \frac{n}{N-1}\right) -
 * 0.0836 \cos\left(6\pi \frac{n}{N-1}\right) +
 * 0.0069 \cos\left(8\pi \frac{n}{N-1}\right)
 * \f]
 */
#define DSP_FLATTOP_WIN     8
/** \brief The type of Gaussian windows compatible with LastWave 'stft' package. 
 * The \a optional parameter determines the \f$\alpha\f$ parameter (the default
 * value for compatibility with LastWave is \f$\alpha = 0.02\f$) which should
 * always be strictly positive.
 *
 * \f[
 * w[n] \propto 
 * \exp\left(
 * -\frac{\left(n-(N-1)/2\right)^2}{2 \alpha (N+1)^2}\right)
 * \f]
 * \todo Replace N+1 with N-1 in expression once compatibility with LastWave is checked?
 */
#define DSP_GAUSS_WIN       9
#define DSP_GAUSS_DEFAULT_OPT 0.02

/** \brief The type of exponential windows. 
 * It is \b asymetric, and the \a optional parameter specifies the value of \f$\alpha\f$,
 * which should be strictly positive. A typical value would be \f$\alpha = \ln 10^4 \f$
 *
 * \f[
 * w[n] \propto \exp\left(- \frac{\alpha \ n}{N-1}\right)
 * \f]
 **/
#define DSP_EXPONENTIAL_WIN 10
#define DSP_EXPONENTIAL_DEFAULT_OPT log(1e4)

/** \brief The type of FoF (Formant wave functions) windows. 
 * It is \b asymetric. 
 *
 * \f[
 * w[n] \propto
 * \left\{\begin{array}{ll}
 * 0.5
 * \left(1-\cos\left(\frac{4\pi (n+1)}{N+1}\right)\right) \cdot
 * \exp\left(-\frac{\alpha\ (n+1)}{N+1}\right)
 * & 0 \leq n < \lfloor \frac{N+1}{4}\rfloor\\ 
 * \exp\left(-\frac{\alpha\ n}{N+1}\right) & \lfloor\frac{N+1}{4}\rfloor \leq n < N
 * \end{array}\right.
 * \f]
 * where \f$\alpha = \ln 10^5\f$ and \f$\lfloor \cdot \rfloor\f$ denotes the integer part.
 * \sa This is the window described in Gribonval, R. and Bacry, E.
 * Harmonic Decomposition of Audio Signals with Matching Pursuit, IEEE Trans. Signal Proc.,
 * Vol. 51, No. 1, pp 101-111, January 2003 (Eq. (23)) and previously used in LastWave.
 **/
#define DSP_FOF_WIN         11

/** \brief The type of Kaiser Bessel Derived windows. 
 * It is \b symetric, and the \a optional parameter should be positive.
 *
 **/
#define DSP_KBD_WIN	    12

/** \brief Cosine window which satisfy the Princen-Bradley condition
*
* \f[
* w[n] \propto 0.5 \left(1-\cos \left(2\pi \frac{n+0.5}{N}\right)\right)
* \f]
*/
#define DSP_PBCOSINE_WIN      13

/** \brief A gamma function window
  * It is \b asymmetric, and the \a filterorder and the damping parameter is packed into the optional argument. 
  * The standard mapping is indicated with an argument > 0 and is defined as:
  * filterorder = integer part, damping = (fractional part)*1000
  * e.g. 4.00714 -> damping = 7.14 and filterorder = 4
  * To be able to use noninteger filterorder a special mapping possible using a negative parameter:
  * filterorder = (integer part)/-100 and  damping = (fractional part)*1000
  * e.g. -425.00714 -> damping = 7.14 and filterorder = 4.25
  *
  * \f$\lambda\f$ damping factor, \f$n\f$ filterorder 
  * 
  * \f[
  * w[n] \propto n^{c-1}e^{-\lambda n}
  * \f]
  **/
#define DSP_GAMMA_WIN 14
#define DSP_GAMMA_DEFAULT_FILTERORDER 4
#define DSP_GAMMA_DEFAULT_DAMPING 850

/** \brief A special type of windows.
 * It is \b reserved for future use with windows that do not necessarily
 * have an analytic expression.
 *
 * \todo Check compatibility with the function make_window
 */
#define DSP_TABULATED_WINDOW 255


/** \brief The maximum number of available windows
 */
#define DSP_NUM_WINDOWS 256


/********************/
/* Procedures       */
/********************/
#ifdef __cplusplus
extern "C" {
#endif

  /**
   * \fn unsigned long int make_window(Dsp_Win_t *out,  const unsigned long int length, const unsigned char type, double optional)
   * \brief A function to fill in an array with various classical windows of unit energy. 
   *
   * The documentation of each window type DSP_*_WIN gives the analytic
   * expression \f$ w[n], 0 \leq n < N \f$ of the window and the meaning of the
   * \a optional parameter when necessary.
   * \param out The array where the generated window will be stored. 
   * \param length The number \f$N\f$ of samples in the window. 
   * A one point window of any type is always filled with the value 1.
   * \param type The type of the window. 
   * \param optional An optional parameter to describe the shape of certain types of windows. 
   * \return The offset of the sample at the 'center' of the window compared to the first sample.
   * By convention, for most symmetric windows wich are bump functions, the 'center' is
   * the first sample at which the absolute maximum of the window is reached.
   * \remark  When the window type is not a known one, an error message is printed to stderr and out is not filled.
   * \sa window_type_is_ok() can be used before calling make_window() to check if the window type is known
   */
  MPTK_LIB_EXPORT unsigned long int make_window( Dsp_Win_t *out,
				 const unsigned long int length,
				 const unsigned char type,
				 double optional );

  /** \brief Check if a window type is a known one 
   *
   * \param type The type to be checked
   * \return DSP_UNKNOWN_WIN if the type is unknown, type otherwise
   **/
   MPTK_LIB_EXPORT unsigned char     window_type_is_ok(const unsigned char type);

  /** \brief Check if a window needs the optional parameter
   *
   * \param type The type to be checked
   * \return true if the type requires the optional argument,
   * false otherwise (including when the window type is unknown).
   **/
   MPTK_LIB_EXPORT unsigned char     window_needs_option(const unsigned char type);

  /** \brief Convert a window name into a window type
   *
   * \param name The name to be converted
   * \return DSP_UNKNOWN_WIN if the type is unknown, the desired type otherwise
   **/
  MPTK_LIB_EXPORT unsigned char     window_type(const char * name);

  /** \brief Convert a window type into a window name
   *
   * \param type The type to be converted
   * \return The string "unknown" if the type is unknown, the desired name otherwise
   **/
   MPTK_LIB_EXPORT  char*             window_name(const unsigned char type);

  /** \brief Compute the zeroth order modified Bessel function of the first kind
  */
  MPTK_LIB_EXPORT double BesselI0(double x);

#ifdef __cplusplus
}
#endif

#endif /* __dsp_windows_h_ */
