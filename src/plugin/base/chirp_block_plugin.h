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


#ifndef __chirp_block_plugin_h_
#define __chirp_block_plugin_h_


/* YOUR includes go here. */


/************************/
/* CHIRP BLOCK CLASS    */
/************************/

/** \brief Explain what YOUR block does here. */
class MP_Chirp_Block_Plugin_c:public MP_Gabor_Block_Plugin_c
  {

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
    static MP_Block_c* create(MP_Signal_c *setSignal, map<string, string, mp_ltstring> *paramMap);

    /** \brief an initializer for the parameters which ARE related to the signal */
    virtual int plug_signal( MP_Signal_c *setSignal );

  protected:
    /** \brief an initializer for the parameters which ARE NOT related to the signal
     * \param setFilterLen size of the window
     * \param setFilterShift shift, in samples, between two consecutive frames.
     * Typically, use \a filterShift = \a filterLen / 2 to get 50 percent overlap between windows
     * \param setFftSize number of atoms (frequency bins) per frame.
     * Typically, use \a fftRealSize = \a filterLen / 2 + 1 to have the block compute
     * windowed FFTs without zero padding.
     * \param setWindowType type of the window
     * \param setWindowOption optional shaping parameter of the windows
     * \param setNumFitPoints number of frequency points used on both sides of a local maximum to fit a chirp
     * \param setNumIter the number of iterations
     * \param setBlockOffset the block offset
     * \return one upon success, zero otherwise
     *
     * \remark If \a fftRealSize is smaller than \a filterLen / 2 + 1,
     * no chirp block is added!
     *
     */


    virtual int init_parameters( const unsigned long int setFilterLen,
                                 const unsigned long int setFilterShift,
                                 const unsigned long int setFftSize,
                                 const unsigned char setWindowType,
                                 const double setWindowOption,
                                 const unsigned int setNumFitPoints,
                                 const unsigned int setNumIter,
                                 const unsigned long int setBlockOffset );

    /** \brief an initializer for the parameters which ARE NOT related to the signal in a parameter map 
     * \param setFilterLen size of the window
     * \param setFilterShift shift, in samples, between two consecutive frames.
     * Typically, use \a filterShift = \a filterLen / 2 to get 50 percent overlap between windows
     * \param setFftSize number of atoms (frequency bins) per frame.
     * Typically, use \a fftRealSize = \a filterLen / 2 + 1 to have the block compute
     * windowed FFTs without zero padding.
     * \param setWindowType type of the window
     * \param setWindowOption optional shaping parameter of the windows
     * \param setNumFitPoints number of frequency points used on both sides of a local maximum to fit a chirp
     * \param setNumIter the number of iterations
     * \param setBlockOffset the block offset
     * \return one upon success, zero otherwise
     *
     * \remark If \a fftRealSize is smaller than \a filterLen / 2 + 1,
     * no chirp block is added!
     *
     */
     
    virtual int init_parameter_map( const unsigned long int setFilterLen,
                                    const unsigned long int setFilterShift,
                                    const unsigned long int setFftSize,
                                    const unsigned char setWindowType,
                                    const double setWindowOption,
                                    const unsigned int setNumFitPoints,
                                    const unsigned int setNumIter,
                                    const unsigned long int setBlockOffset );
    
    /** \brief nullification of the signal-related parameters */
    virtual void nullify_signal( void );

    /** \brief a constructor which initializes everything to zero or NULL */
    MP_Chirp_Block_Plugin_c ( void );

  public:
    /* Destructor */
    virtual ~MP_Chirp_Block_Plugin_c();


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

  private :
    /** \brief Sets the complex demodulation signal, and computes and tabulates
        the related atom's autocorrelations.
     */
    virtual int set_chirp_demodulator( MP_Real_t chirprate );


  };

#endif /* __chirp_block_h_ */
