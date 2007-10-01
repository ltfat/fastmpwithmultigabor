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
 * $Author: sacha $
 * $Date: 2006-07-03 17:27:31 +0200 (lun., 03 juil. 2006) $
 * $Revision: 582 $
 *
 */


#ifndef __harmonic_block_plugin_h_
#define __harmonic_block_plugin_h_


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
 * A harmonic block is the \b union of a standard Gabor block
 * with a set of harmonic subspaces spanned by Gabor atoms at discrete frequencies 
 * \f[
 * f_k \approx k*f_0, 1 \leq k \leq \mbox{maxNumPartials},
 * \f]
 * with the constraint \f$f_k\f$ < \b fft->numFreqs. The fundamental frequency
 * \f$f_0 = \ell/\mbox{fft.fftSize}\f$ spans the domain
 * \f[
 * 1 \leq \mbox{minFundFreqIdx} \leq \ell
 * < \mbox{minFundFreqIdx}+\mbox{numFundFreqIdx}
 * \]
 * where
 * \[
 * \mbox{minFundFreqIdx} \geq ??
 * \]
 * and
 * \[
 * \mbox{minFundFreqIdx}+\mbox{numFundFreqIdx} <= \mbox{fft.numFreqs}
 * \f]
 *
 * Thus, \b numFilters = \b fft->numFreqs + \b numFundFreqIdx 
 * where the first term counts Gabor atoms and the second
 * one harmonic atoms
 *
 */

class MP_Harmonic_Block_Plugin_c:public MP_Gabor_Block_Plugin_c {

  /********/
  /* DATA */
  /********/

public:
  
  /* BLOCK-specific parameters: */

  /** \brief Minimum fundamental frequency, in Hz */
  MP_Real_t f0Min;
  /** \brief Maximum fundamental frequency, in Hz */
  MP_Real_t f0Max;
  /** \brief Maximum number of partials per harmonic subspace. 
   *  The actual number of partials in a given subspace might be smaller */
  unsigned int maxNumPartials;

  /** \brief An array of size \b fft->numFreqs which holds the frame-wise sum 
   * of FFT results across channels
   * \sa mag */
  double *sum;

  /* SIGNAL-related parameters: */

  /** \brief Minimum fundamental frequency, in fft bins */
  unsigned long int minFundFreqIdx;
  /** \brief Number of fundamental frequency bins between f0Min and f0Max */
  unsigned long int numFundFreqIdx;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
			 
  /** \brief Factory function that allocates the storage space and inits it to zero, 
   * and set up the block from the map 
   * 
   * \param s the signal on which the block will work
   * \param paramMap the map containing the parameter to construct the block:
   */
  static MP_Block_c* create( MP_Signal_c *s, map<string, string, mp_ltstring> *paramMap);

  /** \brief an initializer for the parameters which ARE related to the signal */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal
   *  \param setFilterLen size of the window
   *  \param setFilterShift shift, in samples, between two consecutive frames. 
   * Typically, use \a setFilterShift = \a setWindowSize / 2 to get 50 percent overlap between windows
   *  \param setFftSize The size of the executed FFT, including zero padding.
   * Typically, use \a setFftSize = \a setWindowSize to have the block compute
   * windowed FFTs without zero padding.
   *  \param setWindowType type of the window  (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
   *  \param setWindowOption optional shaping parameter of the windows
   *  \param  setF0Min minimum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a numFreqs-1 (Nyquist)
   *  \param setF0Max maximum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a numFreqs-1 (Nyquist)
   *  \param setMaxNumPartials maximum number of partials to be considered in each harmonic subspace
   *  \param setBlockOffset the block offset
   * 
   * \warning the behaviour is undefined if the following conditions are not satisfied:
   * -  \a setFftSize is at least \a setWindowSize;
   * -  \a setMaxNumPartials is at least 2,
   * -  \f$1 \leq \mbox{setMinFundFreqIdx} \leq \mbox{setMaxFundFreqIdx} < \mbox{numFreqs}\f$
   */  

  virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const MP_Real_t setF0Min,
			       const MP_Real_t setF0Max,
			       const unsigned int  setMaxNumPartials,
			       const unsigned long int setBlockOffset );
			       
  /** \brief an initializer for the parameters which ARE NOT related to the signal in a parameter map 
   *  \param setFilterLen size of the window
   *  \param setFilterShift shift, in samples, between two consecutive frames. 
   * Typically, use \a setFilterShift = \a setWindowSize / 2 to get 50 percent overlap between windows
   *  \param setFftSize The size of the executed FFT, including zero padding.
   * Typically, use \a setFftSize = \a setWindowSize to have the block compute
   * windowed FFTs without zero padding.
   *  \param setWindowType type of the window  (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
   *  \param setWindowOption optional shaping parameter of the windows
   *  \param setF0Min minimum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a numFreqs-1 (Nyquist)
   *  \param setF0Max maximum allowed fundamental frequency of the harmonic subspaces, 
   * expressed in frequency bins between 0 (DC) and \a numFreqs-1 (Nyquist)
   *  \param setMaxNumPartials maximum number of partials to be considered in each harmonic subspace
   *  \param setBlockOffset the block offset
   * 
   * \warning the behaviour is undefined if the following conditions are not satisfied:
   * -  \a setFftSize is at least \a setWindowSize;
   * -  \a setMaxNumPartials is at least 2,
   * -  \f$1 \leq \mbox{setMinFundFreqIdx} \leq \mbox{setMaxFundFreqIdx} < \mbox{numFreqs}\f$
   */ 
  virtual int init_parameter_map( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const MP_Real_t setF0Min,
			       const MP_Real_t setF0Max,
			       const unsigned int  setMaxNumPartials,
			       const unsigned long int setBlockOffset );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Harmonic_Block_Plugin_c( void );

public:
  /* Destructor */
  virtual ~MP_Harmonic_Block_Plugin_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text output */
  virtual int info( FILE *fid );

  /** \brief update the inner products of a given frame and return the
   * correlation \a maxCorr and index in the frame \a maxFilterIdx of the
   * maximally correlated atom on the frame
   *
   * \param frameIdx the index of the frame used for the inner products
   *
   * \param maxCorr a MP_Real_t* pointer to return the value of the maximum
   * inner product (or maximum correlation) in this frame
   *
   * \param maxFilterIdx an unsigned long int* pointer to return the index of
   * the maximum inner product
   *
   * On each frame, this method computes the energy of Gabor atoms plus the energy of
   * projections over harmonic molecules at fundamental frequency 
   * \f$f_0 = \ell/\mbox{fft.fftCplxSize}\f$ as
   * \f[
   * \sum_{k=1}^{K} \mbox{sum}[k*\ell]
   * \f]
   * with K the largest integer no larger than \b maxNumPartials which 
   * satisfies \f$K \ell < \mbox{fft.numFreqs}\f$.
   *
   * \sa MP_Block_c::update_frame()
   * \sa MP_Block_c::update_ip()
   */
  virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ); 
  
  /** \brief Creates a new Harmonic atom (or a plain Gabor atom) 
   * corresponding to (frameIdx,filterIdx).
   * \param atom a pointer to a reference to the returned atom object
   * \param frameIdx the index of the desired atom
   * \param filterIdx the index of the desired atom
   * \return the number of created atoms (one upon success, zero otherwise)
   *
   * - if \a filterIdx < \b fft->numFreqs the result is a Gabor atom 
   * at frequency \a freq = \a filterIdx / \b fft->fftSize.
   * - otherwise the created atom is a Harmonic atom at fundamental frequency 
   * \f[
   * \mbox{freq} = (\mbox{filterIdx}-\mbox{fft.numFreqs}+\mbox{minFundFreqIdx})/\mbox{fft.fftSize}
   * \f]
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int frameIdx,
			    const unsigned long int filterIdx );

 /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   * \param parameterMapType the map to fill .
   */
  static void get_parameters_type_map(map< string, string, mp_ltstring>* parameterMapType);
   /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   *
   * \param parameterMapInfo the map to fill.
   */
  static void get_parameters_info_map(map< string, string, mp_ltstring>* parameterMapInfo);
   /** \brief Field a map with the parameter type of the block, the creation and destruction of the map is done by the calling method 
   *
   *
   * \param parameterMapDefault the map to fill.
   */
  static void  get_parameters_default_map(map< string, string, mp_ltstring>* parameterMapDefault);
  
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
 * \param fftSize size of the executed FFT, including zero padding.
 * Typically, use \a fftSize = \a windowSize to have the block compute
 * windowed FFTs without zero padding.
 * \param windowType type of the window  (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
 * \param windowOption optional shaping parameter of the windows
 * \param f0Min minimum allowed fundamental frequency of the harmonic subspaces, in Hz
 * \param f0Max maximum allowed fundamental frequency of the harmonic subspaces, in Hz
 * \param maxNumPartials maximum number of partials to be considered in each harmonic subspace
 * \return one upon success, zero otherwise 
 *
 * \remark If \a fftSize is smaller than \a windowSize , or \a maxNumPartials smaller than 2,
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
			const unsigned long int fftSize,
			const unsigned char windowType,
			const double windowOption,
			const MP_Real_t f0Min,
			const MP_Real_t f0Max,
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
 * (= \a numFreqs)  as a function of the window size (= \a filterLen):
 * \f[\mbox{fftSize} = (\mbox{filterLen}) \times \mbox{freqDensity}\f]
 * If \a freqDensity exceeds one, zero padding will be performed to have
 * an increased frequency resolution.
 *
 * \param setWindowType type of the windows (ex: \b DSP_GAUSS_WIN, \b DSP_HAMMING_WIN, ...)
 * \param setWindowOption optional shaping parameter of the windows
 * \param f0Min minimum allowed fundamental frequency of the harmonic subspaces, in Hz
 * \param f0Max maximum allowed fundamental frequency of the harmonic subspaces, in Hz
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
			 const MP_Real_t f0Min,
			 const MP_Real_t f0Max,
			 const unsigned int  maxNumPartials);


#endif /* __harmonic_block_h_ */
