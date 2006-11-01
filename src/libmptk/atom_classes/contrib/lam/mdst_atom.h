/******************************************************************************/
/*                                                                            */
/*                          mdst_atom.h       	                              */
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

/**********************************************************/
/*                                                	  */
/* DEFINITION OF THE MDST ATOM CLASS,            	  */
/* RELEVANT TO THE MDST TIME-FREQUENCY TRANSFORM 	  */
/*                                                	  */
/**********************************************************/


#ifndef __mdst_atom_h_
#define __mdst_atom_h_


/*******************************/
/* MDST ATOM CLASS    	       */
/*******************************/

/**
 * \brief A class that adds the specification of MDST atoms to the base Atom class.
 *
 * The Modified Discrete Sine Transform (MDST) is related to the MDCT and MCLT transforms.
 * Whereas the MDCT is the real part of the MCLT, the MDST is the imaginary part of the MCLT.
 * - A reference is: H. S. Malvar, Signal Processing with Lapped Transforms. Boston, MA:
 * Artech House, 1992.
 *
 * \sa build_waveform()
 */
class MP_Mdst_Atom_c: public MP_Atom_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief The shape of the atom window 
   * \sa make_window() */
  unsigned char windowType;

  /** \brief The optional window parameter (applicable to the Gauss,
   * generalized Hamming and exponential windows)
   * \sa make_window() */
  double windowOption;

  /** \brief The normalized frequency of the atom on all channels.
   *  When between 0 and 0.5 there should be no aliasing. */
  MP_Real_t freq;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

  /** \brief Factory function that allocates storage space for the amplitudes and phase 
   * \param setNumChans the desired number of channels
   * \param setWindowType the type of window (e.g. Gauss, Hamming, ... )
   * \param setWindowOption an optional parameter for Gauss, generalized Hamming and exponential windows
   * \sa make_window()
   */
  static MP_Mdst_Atom_c* init( const MP_Chan_t setNumChans,
                               const unsigned char setWindowType,
                               const double setWindowOption );

  /** \brief A factory function that reads from a file
   *
   * \param  fid A readable stream
   * \param  mode The reading mode (MP_TEXT or MP_BINARY) 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag <atom type="*"> ... </atom> is looked for
   * \sa read_atom()
   */
  static MP_Mdst_Atom_c* init( FILE *fid, const char mode );

 protected:

  /* Void constructor */
  MP_Mdst_Atom_c( void );

  /** \brief Internal allocations of the local vectors */
  int local_alloc( const MP_Chan_t setNumChans );

  /** \brief Internal allocations of all the vectors */
  int global_alloc( const MP_Chan_t setNumChans );

  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

public:

  /* Destructor */
  virtual ~MP_Mdst_Atom_c( void );


  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  virtual char * type_name( void );

  virtual int info( FILE *fid );
  virtual int info();

  /** \brief Build concatenated waveforms corresponding to each channel of a Mdst atom. 
   *
   * \param outBuffer the array of size \b totalChanLen which is filled with the  concatenated 
   * waveforms of all channels.
   *
   * For each channel \a chanIdx, the waveform is given by the expression
   * \f[
   * \mbox{window}(t) \cdot \mbox{amp} \cdot sin \left[  \frac{\pi}{L} \left( t + \frac{1}{2} + \frac{L}{2} \right) \left( f + \frac{1}{2} \right) \right]
   * \f]
   */
  virtual void build_waveform( MP_Sample_t *outBuffer );

  /** \brief Adds the representation of a Gabor atom to a time-frequency map */
  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

};


#endif /* __mdst_atom_h_ */
