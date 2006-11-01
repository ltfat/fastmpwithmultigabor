/******************************************************************************/
/*                                                                            */
/*                          mclt_block.h            	                      */
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

/****************************************************************/
/*                                                		*/
/* DEFINITION OF THE MCLT BLOCK CLASS           	 	*/
/* RELEVANT TO THE MCLT TIME-FREQUENCY TRANSFORM 		*/
/*                                                		*/
/****************************************************************/


#ifndef __mclt_block_h_
#define __mclt_block_h_


/********************************/
/* MCLT BLOCK CLASS    		*/
/********************************/

/** \brief Blocks corresponding to MCLT frames 
 *
 */
class MP_Mclt_Block_c:public MP_Mclt_Abstract_Block_c {
  /********/
  /* DATA */
  /********/

public:


  /** \brief Storage space for the real part of the correlation between
   *  complex atoms and  their conjugate. (DO NOT MALLOC OR FREE IT.) 
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
   */
  MP_Real_t *sqCorrel;

  /** \brief Storage space for a useful constant related to the atoms'
   * autocorrelations with their conjugate. (DO NOT MALLOC OR FREE IT.)
   * \sa sqCorrel 
   *
   * */
  MP_Real_t *cstCorrel;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief Factory function for a generalized mclt block
   *
   * \param filterLen the length of the signal window, in number of samples
   * \param filterShift the window shift, in number of samples
   * \param fftSize the size of the FFT, including zero padding
   * \param windowType the window type (see the doc of libdsp_windows.h)
   * \param windowOption the optional window parameter.
   * 
   */
  static MP_Mclt_Block_c* init( MP_Signal_c *s,
				 const unsigned long int filterLen,
				 const unsigned long int filterShift,
				 const unsigned long int fftSize,
				 const unsigned char windowType,
				 const double windowOption );

  /** \brief Factory function for a strict mclt block
   *
   * \param filterLen the length of the signal window, in number of samples
   * \param windowType the window type (see the doc of libdsp_windows.h)
   * \param windowOption the optional window parameter.
   *
   * Warning: In this case, the window type must be rectangle, cosine or kbd. 
   */
  static MP_Mclt_Block_c* init( MP_Signal_c *s,
				 const unsigned long int filterLen,
				 const unsigned char windowType,
				 const double windowOption );


  /** \brief an initializer for the parameters which ARE related to the signal */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal */
  virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Mclt_Block_c( void );

public:
  /* Destructor */
  virtual ~MP_Mclt_Block_c();


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
   *
   * \sa MP_Block_c::update_frame()
   * \sa MP_Block_c::update_ip()
   */
  virtual void update_frame( unsigned long int frameIdx, 
			     MP_Real_t *maxCorr, 
			     unsigned long int *maxFilterIdx ); 

  /** \brief Creates a new MCLT atom corresponding to (frameIdx,filterIdx)
   * 
   * The real valued atomic waveform stored in atomBuffer is the projection
   * of the signal on the subspace spanned by the complex atom and its
   * conjugate as given generally by
   * \f[
   * \frac{\mbox{amp}}{2} \cdot 
   * \left( e^{i\mbox{phase}} \mbox{atom} + e^{-i\mbox{phase}} \overline{\mbox{atom}}\right)
   * \f]
   * and for MCLT atoms :
   * \f[
   * \mbox{window}(t) \cdot \mbox{amp} \cdot cos \left[  \frac{\pi}{L} \left( t + \frac{1}{2} + \frac{L}{2} \right + \mbox{phase}\right ) \left( f + \frac{1}{2} \right) \right]
   * \f]
   */
  unsigned int create_atom( MP_Atom_c **atom,
			    const unsigned long int frameIdx,
			    const unsigned long int filterIdx );

protected:

  /** \brief Allocates the atom's autocorrelation.
   */
  int alloc_correl( MP_Real_t **reCorr, MP_Real_t **imCorr,
		    MP_Real_t **sqCorr, MP_Real_t **cstCorr );

  /** \brief Computes and tabulates the atom's autocorrelation.
   *
   * \sa compute_energy()
   */
  int fill_correl( MP_Real_t *reCorr, MP_Real_t *imCorr,
		   MP_Real_t *sqCorr, MP_Real_t *cstCorr );

  /** \brief Computes a special version of the power spectrum of an input signal buffer and
   * puts it in an output magnitude buffer. This version corresponds to the actual projection
   * of a real valued signal on the space spanned by a complex atom and its conjugate transpose.
   *
   * \param in the input signal buffer, only the first windowSize values are used.
   * \param reCorr real part of the pre-computed atom's autocorrelation.
   * \param imCorr imaginary part of the pre-computed atom's autocorrelation.
   * \param sqCorr pre-computed squared atom's autocorrelation.
   * \param cstCorr pre-computed useful constant related to the atom's autocorrelation.
   * \param outMag output FFT magnitude buffer, only the first numFreqs values are filled.
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
   * and the definition of \a sqCorrel and \a cstCorrel we have
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
   * and details about zero padding and the use of numFreqs.
   */  
  void compute_energy( MP_Real_t *in,
		       MP_Real_t *reCorr, MP_Real_t *imCorr,
		       MP_Real_t *sqCorr, MP_Real_t *cstCorr,
		       MP_Real_t *outMag );

};

#endif /* __mclt_block_h_ */
