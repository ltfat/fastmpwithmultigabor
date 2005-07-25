/******************************************************************************/
/*                                                                            */
/*                              gabor_block.h                                 */
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

/**************************************************/
/*                                                */
/* DEFINITION OF THE GABOR BLOCK CLASS            */
/* RELEVANT TO THE GABOR TIME-FREQUENCY TRANSFORM */
/*                                                */
/**************************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __gabor_block_h_
#define __gabor_block_h_


/************************/
/* GABOR BLOCK CLASS    */
/************************/

/** \brief Blocks corresponding to Gabor frames / modulated filter banks 
 *
 * Typically, for Gabor blocks one will have FFT real size = FFT complex size/2+1, 
 * so for speed reasons it might be preferable to use fftRealSize of the form 2^n+1 */
class MP_Gabor_Block_c:public MP_Block_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief FFT interface, which includes the window with which the signal is multiplied */
  MP_FFT_Interface_c *fft;

  /** \brief A (fft->fftRealSize x s->numChans) array which holds the frame-wise FFT results
      across channels */
  MP_Real_t *mag;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief allocate the storage space and init it to zero, and set up the FFT interface 
   *
   * The size of the complex FFT which is performed depends on filterLen (== the
   * window size) and fftRealSize (== the number of frequency bins) and is
   * typically 2*(fftRealSize-1), so for speed reasons it might be preferable
   * to use fftRealSize of the form 2^n+1
   * 
   * \sa MP_FFT_Generic_Interface_c::fftRealSize MP_FFT_Generic_Interface_c::exec_complex()
   */
  MP_Gabor_Block_c( MP_Signal_c *s,
		    const unsigned long int filterLen,
		    const unsigned long int filterShift,
		    const unsigned long int fftRealSize,
		    const unsigned char windowType,
		    const double windowOption );

  /* Destructor */
  virtual ~MP_Gabor_Block_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /* Type ouptut */
  virtual char *type_name( void );

  /* Readable text output */
  virtual int info( FILE *fid );

  /* Update of part of the inner products */
  MP_Support_t update_ip( const MP_Support_t *touch );
  
  /** \brief Creates a new Gabor atom corresponding to atomIdx in the flat array ip[]
   * 
   * The real valued atomic waveform stored in atomBuffer is the projection
   * of the signal on the subspace spanned by the complex atom and its
   * conjugate as given generally by
   * \f[
   * \frac{\mbox{amp}}{2} \cdot 
   * \left( e^{i\mbox{phase}} \mbox{atom} + e^{-i\mbox{phase}} \overline{\mbox{atom}}\right)
   * \f]
   * and for Gabor atoms :
   * \f[
   * \mbox{window}(t) \cdot \mbox{amp} \cdot \cos\left(2\pi \mbox{f} t+ \mbox{phase}\right)
   * \f]
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int atomIdx );

};


/*************/
/* FUNCTIONS */
/*************/

/** \brief Add a Gabor block to a dictionary
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
 * \return one upon success, zero otherwise 
 *
 * \remark If \a fftRealSize is smaller than \a filterLen / 2 + 1,
 * no Gabor block is added!
 *
 * \sa MP_Gabor_Block_c::MP_Gabor_Block_c()
 * \sa make_window()
 */
int add_gabor_block( MP_Dict_c *dict,
		     const unsigned long int filterLen,
		     const unsigned long int filterShift,
		     const unsigned long int fftRealSize,
		     const unsigned char windowType,
		     const double windowOption );


/** \brief Add a family of Gabor blocks to a dictionary.
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
 *
 * \return the number of added blocks
 *
 * \remark \a timeDensity should preferably be no smaller than one to avoid gaps
 * betwen adjacent windows. 
 * \remark \a freqDensity should preferably be no smaller than one. When smaller than one,
 * it determines the percentage 
 * of the lowest frequencies that are computed by the block.
 * \remark the behaviour is undefined if either \a timeDensity or \a freqDensity is zero or negative.
 * \sa MP_Dict_c::add_gabor_block()
 */
int add_gabor_blocks( MP_Dict_c *dict,
		      const unsigned long int maxFilterLen,
		      const MP_Real_t timeDensity,
		      const MP_Real_t freqDensity, 
		      const unsigned char setWindowType,
		      const double setWindowOption );
 

#endif /* __gabor_block_h_ */
