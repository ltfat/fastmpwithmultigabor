/******************************************************************************/
/*                                                                            */
/*                             harmonic_block.h                         */
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

/***********************************************************/
/*                                                         */
/* DEFINITION OF THE MP_Harmonic BLOCK CLASS         */
/* RELEVANT TO THE Harmonic TIME-FREQUENCY TRANSFORM */
/*                                                         */
/***********************************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __harmonic_block_h_
#define __harmonic_block_h_


/***************************/
/* HARMONIC BLOCK CLASS    */
/***************************/

/** \brief Blocks used to generate Harmonic atoms.
 *
 * As described in R. Gribonval and E. Bacry, 
 * <A HREF="http://www.irisa.fr/metiss/gribonval/Papers/2003/IEEESP/harmonicMP.pdf"> 
 * Harmonic Decomposition of Audio Signals with Matching Pursuit </A>, 
 * IEEE Trans. on Signal Proc., Vol. 51, No. 1, January 2003, pp 101-111.
 *
 * \sa MP_Harmonic_Atom_c::build_waveform()
 *
 * A harmonic block is the union of a standard Gabor block
 * with a set of harmonic subspaces spanned by Gabor atoms at discrete frequencies 
 * \f[
 * f_k \approx k*f_0, 1 \leq k \leq \mbox{maxNumPartials},
 * \f]
 * with the constraint \f$f_k\f$ < \b fft->fftRealSize. The fundamental frequency
 * \f$f_0 = \ell/\mbox{fft.fftCplxSize}\f$ spans the domain
 * \f[
 * 1 \leq \mbox{minFundFreqIdx} \leq \ell
 * < \mbox{minFundFreqIdx}+\mbox{numFundFreqIdx} <= \mbox{fft.fftRealSize}
 * \f]
 *
 * Thus, \b numFilters = \b fft->fftRealSize + \b numFundFreqIdx
 *
 */

class MP_Harmonic_Block_c:public MP_Gabor_Block_c {

  /********/
  /* DATA */
  /********/

public:
  
  /** \brief Minimum fundamental frequency bin */
  unsigned long int minFundFreqIdx;
  /** \brief Number of fundamental frequency bins */
  unsigned long int numFundFreqIdx;
  /** \brief Maximum number of partials per harmonic subspace. 
   *  The actual number of partials in a given subspace might be smaller */
  unsigned int maxNumPartials;

  /** \brief An array of size \b fft->fftRealSize which holds the frame-wise sum 
   * of FFT results across channels
   * \sa mag */
  MP_Real_t *sum;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Constructor that allocates the storage space and inits it to zero, 
   * and set up the FFT interface
   *
   * The size of the complex FFT which is performed depends on \a setWindowSize and 
   * \a setFftRealSize (== the number of frequency bins) and is typically
   * 2*(fftRealSize-1), so for speed reasons it might be preferable
   * to use \a setFftRealSize of the form 2^n+1
   *
   *  \param setWindowSize size of the window
   * \param setFilterShift shift, in samples, between two consecutive frames. 
   * Typically, use \a setFilterShift = \a setWindowSize / 2 to get 50 percent overlap between windows
   * \param setFftRealSize number of plain Gabor atoms (frequency bins) per frame. 
   * Typically, use \a setFftRealSize = \a setWindowSize / 2 + 1 to have the block compute
   * windowed FFTs without zero padding.
   * \param s the signal on which the block will work
   * \param setWindowType type of the window  (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
   * \param setWindowOption optional shaping parameter of the windows
   * \param setMinFundFreqIdx minimum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a setFftRealSize-1 (Nyquist)
   * \param setMaxFundFreqIdx maximum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a setFftRealSize-1 (Nyquist)
   * \param setMaxNumPartials maximum number of partials to be considered in each harmonic subspace
   *
   * \warning the behaviour is undefined if the following conditions are not satisfied:
   * -  \a setFftRealSize is at least \a setWindowSize / 2 + 1, 
   * -  \a setMaxNumPartials is at least 2,
   * -  \f$1 \leq \mbox{setMinFundFreqIdx} \leq \mbox{setMaxFundFreqIdx} < \mbox{setFftRealSize}\f$
   *
   * \sa add_harmonic_block()
   * \sa MP_Gabor_Block_c::MP_Gabor_Block_c()
   * \sa make_window()
   */
  MP_Harmonic_Block_c( MP_Signal_c *s,
		       const unsigned long int setWindowSize,
		       const unsigned long int setFilterShift,
		       const unsigned long int setFftRealSize,
		       const unsigned char setWindowType,
		       const double setWindowOption,
		       const unsigned long int setMinFundFreqIdx,
		       const unsigned long int setMaxFundFreqIdx,
		       const unsigned int  setMaxNumPartials);

  /* Destructor */
  virtual ~MP_Harmonic_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text output */
  virtual int info( FILE *fid );

  /** \brief update the inner products with a minimum number of arithmetic operations
   * and indicates which frames have been updated.
   *
   * \param touch Multi-channel support (i.e., array[s->numChans] of MP_Support_t)
   * that identifies which part of each channel of the signal is different from what it was
   * when the block was last updated, so as to update only the IP at places
   * where they might have been touched.
   * \return a support indicating which frames have been touched by the inner products' update 
   * \remark Pass touch == NULL to force a full update. 
   *
   * On each frame, this method computes the energy of Gabor atoms plus the energy of
   * projections over harmonic molecules at fundamental frequency 
   * \f$f_0 = \ell/\mbox{fft.fftCplxSize}\f$ as
   * \f[
   * \sum_{k=1}^{K} \mbox{sum}[k*\ell]
   * \f]
   * with K the largest integer no larger than \b maxNumPartials which 
   * satisfies \f$K \ell < \mbox{fft.fftRealSize}\f$.
   */
  MP_Support_t update_ip( const MP_Support_t *touch );
  
  /** \brief Creates a new Harmonic atom (or a plain Gabor atom) 
   * corresponding to \a atomIdx = \a frameIdx * \b numFilters + \a filterIdx.
   * \param atom a pointer to a reference to the returned atom object
   * \param atomIdx the index of the desired atom
   * \return the number of created atoms (one upon success, zero otherwise)
   *
   * - if \a filterIdx < \b fft->fftRealSize the result is a Gabor atom 
   * at frequency \a freq = \a filterIdx / \b fft->fftCplxSize.
   * - otherwise the created atom is a Harmonic atom at fundamental frequency 
   * \f[
   * \mbox{freq} = (\mbox{filterIdx}-\mbox{fft.fftRealSize}+\mbox{minFundFreqIdx})/\mbox{fft.fftCplxSize}
   * \f]
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int atomIdx );

};


/*************/
/* FUNCTIONS */
/*************/

/** \brief Add a harmonic block to a dictionary
 *
 * \param dict The dictionnary to which the block will be added
*  \param windowSize size of the window
 * \param filterShift shift, in samples, between two consecutive frames. 
 * Typically, use \a filterShift = \a windowSize / 2 to get 50 percent overlap between windows
 * \param fftRealSize number of plain Gabor atoms (frequency bins) per frame. 
 * Typically, use \a fftRealSize = \a windowSize / 2 + 1 to have the block compute
 * windowed FFTs without zero padding.
 * \param windowType type of the window  (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
 * \param windowOption optional shaping parameter of the windows
 * \param minFundFreq minimum allowed fundamental frequency of the harmonic subspaces, between 0 (DC) and 0.5 (Nyquist)
 * \param maxFundFreq maximum allowed fundamental frequency of the harmonic subspaces, between 0 (DC) and 0.5 (Nyquist)
 * \param maxNumPartials maximum number of partials to be considered in each harmonic subspace
 * \return one upon success, zero otherwise 
 *
 * \remark If \a fftRealSize is smaller than \a windowSize / 2 + 1, or \a maxNumPartials smaller than 2,
 * or if there is no frequency bin in the range [\a minFundFreq , \a maxFundFreq], 
 * no Gabor block is added!
 *
 * \sa add_gabor_block()
 * \sa add_harmonic_blocks()
 * \sa MP_Harmonic_Block_c::MP_Harmonic_Block_c()
 * \sa make_window()
 */
int add_harmonic_block( MP_Dict_c *dict,
			const unsigned long int windowSize,
			const unsigned long int filterShift,
			const unsigned long int fftRealSize,
			const unsigned char windowType,
			const double windowOption,
			const MP_Real_t minFundFreq,
			const MP_Real_t maxFundFreq,
			const unsigned int  maxNumPartials);

/** \brief Add a family of harmonic blocks to a dictionary.
 *
 * The added blocks correspond to window sizes (= \a filterLen) that are
 * powers of two up to a maximum window size
 * \param dict The dictionnary to which the blocks will be added
 * \param maxWindowSize Determines which dyadic window sizes are used
 * \f[2 \leq \mbox{windowSize} = 2^n \leq \mbox{maxWindowSize}.\f]
 * \param timeDensity Determines the shift between windows (= \a filterShift) as a function of
 * the window size (= \a filterLen):
 * \f[ \mbox{filterShift} = \frac{\mbox{windowSize}}{\mbox{timeDensity}}\f]
 * The larger \a timeDensity, the smaller the shift between adjacent windows, e.g., typically use
 * \a timeDensity = 2 to have a 50 percent overlap.
 *
 * \param freqDensity Determines the number of frequency bins 
 * (= \a fftRealSize)  as a function of the window size (= \a filterLen):
 * \f[\mbox{fftRealSize} = (\mbox{filterLen/2+1}) \times \mbox{freqDensity}\f]
 * If \a freqDensity exceeds one, zero padding will be performed to have
 * an increased frequency resolution.
 *
 * \param setWindowType type of the windows (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
 * \param setWindowOption optional shaping parameter of the windows
 * \param minFundFreq minimum allowed fundamental frequency of the harmonic subspaces, between 0 (DC) and 0.5 (Nyquist)
 * \param maxFundFreq maximum allowed fundamental frequency of the harmonic subspaces, between 0 (DC) and 0.5 (Nyquist)
 * \param maxNumPartials maximum number of partials to be considered in each harmonic subspace
 *
 * \return the number of added blocks
 *
 * \remark \a timeDensity should preferably be no smaller than one to avoid gaps
 * betwen adjacent windows. 
 * \warning the behaviour is undefined if \a timeDensity is zero or negative
 * \warning the behaviour is undefined if  \a freqDensity is less than one.

 *
 * \sa add_harmonic_block()
 * \sa make_window()
 */
int add_harmonic_blocks( MP_Dict_c *dict,
			 const unsigned long int maxWindowSize,
			 const MP_Real_t timeDensity,
			 const MP_Real_t freqDensity, 
			 const unsigned char setWindowType,
			 const double setWindowOption,
			 const MP_Real_t minFundFreq,
			 const MP_Real_t maxFundFreq,
			 const unsigned int  maxNumPartials);


#endif /* __harmonic_block_h_ */
