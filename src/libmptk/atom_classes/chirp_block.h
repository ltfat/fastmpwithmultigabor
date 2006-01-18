/******************************************************************************/
/*                                                                            */
/*                             chirp_block.h                               */
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

/*****************************************************/
/*                                                   */
/* DEFINITION OF THE CHIRP BLOCK CLASS               */
/* RELEVANT TO THE CHIRP TIME-FREQUENCY TRANSFORM    */
/*                                                   */
/*****************************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2005-07-25 14:54:55 +0200 (Mon, 25 Jul 2005) $
 * $Revision: 20 $
 *
 */


#ifndef __chirp_block_h_
#define __chirp_block_h_


/* YOUR includes go here. */


/************************/
/* CHIRP BLOCK CLASS    */
/************************/

/** \brief Explain what YOUR block does here. */
class MP_Chirp_Block_c:public MP_Gabor_Block_c {

  /********/
  /* DATA */
  /********/

public:

  /** \brief Number of points used to fit a parabolic logAmp/phase model */
  unsigned int numFitPoints;

  /** \brief Number of iterations used to fit a parabolic logAmp/phase model */
  unsigned int numIter;

private :
  /* totNumFitPoints = 2*numFitPoints+1 */
  unsigned int totNumFitPoints;
  /* Buffer of filterLen complex chirp values */
  MP_Real_t *chirpRe;
  MP_Real_t *chirpIm;
  /* Buffer of analyzed signal multiplied by complex chirp values */
  MP_Real_t *sigChirpRe;
  MP_Real_t *sigChirpIm;
  /* Buffer of energy */
  MP_Real_t *fftEnergy;
  /* Buffer for 2*numFitPoints+1 amplitudes and phases which must be fitted to a parabola */
  MP_Real_t *logAmp;
  MP_Real_t *phase;

  /* Buffer for correlation between complex chirps at various frequencies */
 /** \brief Storage space for the real part of the quantity 
   * \f[
   * (\mbox{reCorrel}[k],\mbox{imCorrel[k]}) = 
   * \sum_{n=0}^{\mbox{fftCplxSize}-1} \mbox{window}^2[n] \cdot 
   * \exp \left(\frac{2i\pi \cdot (2k)\cdot n}{\mbox{fftCplxSize}}\right)
   * \f]
   * which measures the correlation between complex atoms and their conjugate.
   * (DO NOT MALLOC OR FREE IT.)
   * \sa imCorrelChirp
   */
  MP_Real_t *reCorrelChirp;
  /** \brief Storage space for the imaginary part of the correlation between
   *  complex atoms and  their conjugate. (DO NOT MALLOC OR FREE IT.) 
   * \sa reCorrel */
  MP_Real_t *imCorrelChirp;
  /** \brief Storage space for the squared modulus of the correlation between
   * complex atoms and their conjugate. (DO NOT MALLOC OR FREE IT.) 
   * \sa reCorrelChirp
   *
   * \f$ \mbox{sqCorrel} = \mbox{reCorrel}^2+\mbox{imCorrel}^2 \f$
   */
  MP_Real_t *sqCorrelChirp;
  /** \brief Storage space for a useful constant related to the atoms'
   * autocorrelations with their conjugate. (DO NOT MALLOC OR FREE IT.)
   * \sa sqCorrel 
   *
   * \f$ \mbox{cstCorrel} = \frac{2}{1-\mbox{sqCorrel}} \f$
   * */
  MP_Real_t *cstCorrelChirp;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:
  /** \brief A factory function for the chirp blocks
   * 
   * \sa MP_FFT_Interface_c::fftRealSize MP_FFT_Interface_c::exec_complex()
   */
  static MP_Chirp_Block_c* init( MP_Signal_c *s,
				 const unsigned long int filterLen,
				 const unsigned long int filterShift,
				 const unsigned long int fftSize,
				 const unsigned char windowType,
				 const double windowOption,
				 const unsigned int setNumFitPoints,
				 const unsigned int setNumIter );

  /** \brief an initializer for the parameters which ARE related to the signal */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal */
  virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const unsigned int setNumFitPoints,
			       const unsigned int setNumIter );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Chirp_Block_c( void );

public:
  /* Destructor */
  virtual ~MP_Chirp_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text dump */
  virtual int info( FILE* fid );

  /** \brief Creates a new chirp atom corresponding to (frameIdx,filterIdx)
   * 
   *  \todo Describe how the atom is determined here.
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int frameIdx,
			    const unsigned long int filterIdx );


private : 
  /** \brief Sets the complex demodulation signal, and computes and tabulates
      the related atom's autocorrelations.
   */
  virtual int set_chirp_demodulator( MP_Real_t chirprate );

};


/*************/
/* FUNCTIONS */
/*************/

/** \brief Add a chirp block to a dictionary
 *
 * \param dict The dictionnary that will be modified
 * \param filterLen size of the window
 * \param filterShift shift, in samples, between two consecutive frames. 
 * Typically, use \a filterShift = \a filterLen / 2 to get 50 percent overlap between windows
 * \param fftRealSize number of atoms (frequency bins) per frame. 
 * Typically, use \a fftRealSize = \a filterLen / 2 + 1 to have the block compute
 * windowed FFTs without zero padding.
 * \param windowType type of the window
 * \param windowOption optional shaping parameter of the windows
 * \param numFitPoints number of frequency points used on both sides of a local maximum to fit a chirp
 * \return one upon success, zero otherwise 
 *
 * \remark If \a fftRealSize is smaller than \a filterLen / 2 + 1,
 * no chirp block is added!
 *
 * \sa MP_Chirp_Block_c::MP_Chirp_Block_c()
 * \sa add_gabor_block()
 * \sa make_window()
 */
int add_chirp_block( MP_Dict_c *dict,
		     const unsigned long int filterLen,
		     const unsigned long int filterShift,
		     const unsigned long int fftRealSize,
		     const unsigned char windowType,
		     const double windowOption,
		     const unsigned int numFitPoints,
		     const unsigned int numIter );


/** \brief Add a family of Chirp blocks to a dictionary.
 *
 * The added blocks correspond to window sizes (= \a filterLen) that are
 * powers of two up to a maximum window size
 *
 * \param dict The dictionnary that will be modified
 * \param maxFilterLen Determines which dyadic window sizes are used
 * \f[2 \leq \mbox{filterLen} = 2^n \leq \mbox{maxFilterLen}.\f]
 * \param timeDensity Determines the shift between windows (= \a filterShift) as a function of
 * the window size (= \a filterLen):
 * \f[ \mbox{filterShift} = \frac{\mbox{filterLen}}{\mbox{timeDensity}}\f]
 * The larger \a timeDensity, the smaller the shift between adjacent windows, e.g., use
 * \a timeDensity = 2 to have a 50 percent overlap.
 *
 * \param freqDensity Determines the number of frequency bins 
 * (= \a fftRealSize)  as a function of the window size (= \a filterLen):
 * \f[\mbox{fftRealSize} = (\mbox{filterLen/2+1}) \times \mbox{freqDensity}\f]
 * If \a freqDensity is at least one, zero padding will be performed to have
 * an increased frequency resolution.
 *
 * \param setWindowType type of the windows
 * \param setWindowOption optional shaping parameter of the windows
 * \param numFitPoints number of frequency points used on both sides of a local maximum to fit a chirp
 *
 * \return the number of added blocks
 *
 * \remark \a timeDensity should preferably be no smaller than one to avoid gaps
 * betwen adjacent windows. 
 * \remark \a freqDensity should preferably be no smaller than one. When smaller than one,
 * it determines the percentage 
 * of the lowest frequencies that are computed by the block.
 * \remark the behaviour is undefined if either \a timeDensity or \a freqDensity is zero or negative.
 * \sa MP_Dict_c::add_chirp_block()
 * \sa MP_Dict_c::add_gabor_block()
 */
int add_chirp_blocks( MP_Dict_c *dict,
		      const unsigned long int maxFilterLen,
		      const MP_Real_t timeDensity,
		      const MP_Real_t freqDensity, 
		      const unsigned char setWindowType,
		      const double setWindowOption,
		      const unsigned int numFitPoints,
		     const unsigned int numIter );
 

#endif /* __chirp_block_h_ */
