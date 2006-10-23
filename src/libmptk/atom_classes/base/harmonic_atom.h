/******************************************************************************/
/*                                                                            */
/*                              harmonic_atom.h                         */
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
/* DEFINITION OF THE Harmonic ATOM CLASS,            */
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


#ifndef __harmonic_atom_h_
#define __harmonic_atom_h_


/************************/
/* HARMONIC ATOM CLASS  */
/************************/

/**
 * \brief Harmonic atoms correspond to linear combinations of few Gabor atoms in harmonic relation
 *
 * As described in R. Gribonval and E. Bacry, 
 * <A HREF="http://www.irisa.fr/metiss/gribonval/Papers/2003/IEEESP/harmonicMP.pdf"> 
 * Harmonic Decomposition of Audio Signals with Matching Pursuit </A>, 
 * IEEE Trans. on Signal Proc., Vol. 51, No. 1, January 2003, pp 101-111.
 *
 * \sa build_waveform()
 */
class MP_Harmonic_Atom_c:public MP_Gabor_Atom_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief The number of partials in the harmonic atom, including
  * the fundamental frequency. It is the size of the \b harmonicity array
  * and determines the size numChans*numPartials of the \b partialAmpStorage
  * and \b partialPhaseStorage arrays */
  unsigned int numPartials;
  /** \brief An array of size numPartials that store the harmonic relation between
   * the fundamental frequency and the frequency of the partials.
   *
   * Typically one has \b harmonicity[k-1] = k for \f$1 \leq k \leq \mbox{numPartials}\f$,
   * which correspond to partial frequencies \f$f_k = k \cdot \mbox{freq}\f$, but using other harmonicity
   * relations yields  \f$f_k = \mbox{harmonicity}[k-1] \cdot \mbox{freq}\f$.
   * \sa build_waveform()
   */
  MP_Real_t *harmonicity;
  /** \brief Storage space for the amplitude of the partials on each channel. */
  MP_Real_t *partialAmpStorage;
  /** \brief Storage space for the phase of the partials on each channel */
  MP_Real_t *partialPhaseStorage;
  /** \brief Pointers to access the amplitude in each channel separately 
   * as partialAmp[0] ... partialAmp[numChans-1]. 
   *
   * Examples : 
   * - the amplitude of the first partial (corresponding to the frequency \b harmonicity[0] x \b freq)
   *  of the second channel is partialAmp[1][0] or *(partialAmp[1]);
   * - the amplitude of the second partial (corresponding to the frequency \b harmonicity[1] x \b freq)
   * of the third channel is partialAmp[2][1]  or *(partialAmp[2]+1). 
   */
  MP_Real_t **partialAmp;
  /** \brief Same as partialAmp for direct access to the phase of partials on each channel */
  MP_Real_t **partialPhase;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Factory function that allocates storage space for the harmonicity as well as
   * the amplitudes and phases of both the fundamental and the partials 
   * \param setNumChans the desired number of channels
   * \param setWindowType the type of window (e.g. Gauss, Hamming, ... )
   * \param setWindowOption an optional parameter for Gauss, generalized Hamming and exponential windows
   * \param setNumPartials the desired number of partials in the atom
   * \remark By default \b harmonicity[k-1] is initialized to the value k.
   * \warning \a setNumPartials must be at least two, otherwise the behaviour is undefined
   * \sa MP_Gabor_Atom_c::MP_Gabor_Atom_c()
   * \sa make_window()
   */
  static MP_Harmonic_Atom_c* init( const MP_Chan_t setNumChans,
				   const unsigned char setWindowType,
				   const double setWindowOption, 
				   const unsigned int setNumPartials );

  /** \brief A factory function that reads from a file
   *
   * \param  fid A readable stream
   * \param  mode The reading mode (MP_TEXT or MP_BINARY) 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag <atom type="*"> ... </atom> is looked for
   * \sa read_atom() MP_Gabor_Atom_c::MP_Gabor_Atom_c()
   */
  static MP_Harmonic_Atom_c* init( FILE *fid, const char mode );

protected:

  /** \brief Void constructor */
  MP_Harmonic_Atom_c( void );

  /** \brief Internal allocations of the local vectors */
  int local_alloc( const MP_Chan_t setNumChans, const unsigned int setNumPartials );

  /** \brief Internal allocations of all the vectors */
  int global_alloc( const MP_Chan_t setNumChans, const unsigned int setNumPartials );

  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

public:
  /* Destructor */
  virtual ~MP_Harmonic_Atom_c( void );


  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  virtual char * type_name(void);

  virtual int info( FILE *fid );
  virtual int info();

  /** \brief Build concatenated waveforms corresponding to each channel of a harmonic atom. 
   *
   * \param outBuffer the array of size totalChanLen which is filled with the  concatenated 
   * waveforms of all channels.
   *
   * For each channel \a chanIdx, the waveform is given by the expression
   * \f[
   * \mbox{amp} \cdot \mbox{window}(t) \cdot \sum_{k=1}^{\mbox{numPartials}} a_k
   * \cdot \cos\left(2\pi \lambda_k \left(\mbox{chirp} \cdot \frac{t^2}{2}
   *      + \mbox{freq} \cdot t\right)+ \mbox{phase} + \phi_k\right)
   * \f]
   * where
   * - the window depends on \b windowType (and \b windowOption for some types of windows) and
   * \b support[chanIdx];
   * - \b amp[chanIdx] and \b phase[chanIdx] determine \f$(\mbox{amp},\mbox{phase})\f$;
   * - \b partialAmp[chanIdx][k-1] and \b phase[chanIdx][k-1] determine \f$(a_k,\phi_k)\f$;
   * - \b harmonicity[k-1] determines \f$\lambda_k\f$.
   */
  virtual void build_waveform( MP_Sample_t *outBuffer );

  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

};


#endif /* __harmonic_atom_h_ */
