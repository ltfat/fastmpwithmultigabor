/******************************************************************************/
/*                                                                            */
/*                               gabor_atom.h                                 */
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
/* DEFINITION OF THE GABOR ATOM CLASS,            */
/* RELEVANT TO THE GABOR TIME-FREQUENCY TRANSFORM */
/*                                                */
/**************************************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

#ifndef gabor_atom_plugin_h_
#define gabor_atom_plugin_h_

/***********************/
/* GABOR ATOM CLASS    */
/***********************/

/**
 * \brief A class that adds the specification of (chirp) Gabor atoms to the base Atom class.
 *
 * \sa
 * - basic Gabor atoms are described in S. Mallat and Z. Zhang, "Matching pursuit with 
 * time-frequency  dictionaries", IEEE Trans. on Signal Proc. vol. 41, No. 12, December 1993,
 * pp 3397--3415.
 * - chirped Gabor atoms are described in R. Gribonval, 
 * <A HREF="http://www.irisa.fr/metiss/gribonval/Papers/2001/IEEE_TSP/mp_chirps_TSP.pdf">
 * Fast matching pursuit with a multiscale dictionary of Gaussian chirps</A>,
 * IEEE Trans. Signal Proc., Vol. 49, No. 5, May 2001, pp 994-1001.
 *
 * \sa build_waveform()
 */
class MP_Gabor_Atom_Plugin_c:public MP_Atom_c {

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
  /** \brief The chirprate of the atom on all channels. */
  MP_Real_t chirp;
  /** \brief The phase of the atom on each channel. */
  MP_Real_t *phase;


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief a void Factory function that allocates storage space
   */

  static MP_Atom_c  * gabor_atom_create_empty(MP_Dict_c* dict);
  
  /** \brief A factory function that reads from a file
   *
   * \param  fid A readable stream
   * \param  mode The reading mode (MP_TEXT or MP_BINARY) 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag &lt;atom type="*"&gt; ... &lt;/atom&gt; is looked for
   * \sa read_atom()
   */

  static MP_Atom_c* create( FILE *fid, MP_Dict_c *dict, const char mode );
protected:

  /** \brief Void constructor */
  MP_Gabor_Atom_Plugin_c( MP_Dict_c* dict );





  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

public:

  /* Destructor */
  virtual ~MP_Gabor_Atom_Plugin_c( void );
  
    /** \brief Internal allocations of the local vectors */
  virtual int alloc_gabor_atom_param( const MP_Chan_t setNumChans );
  
  /** \brief Internal allocations of all the vectors */
 // virtual int global_alloc( const MP_Chan_t setNumChans );

  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  virtual const char * type_name( void );

  virtual int info( FILE *fid );
  virtual int info();

  /** \brief Build concatenated waveforms corresponding to each channel of a Gabor atom. 
   *
   * \param outBuffer the array of size \b totalChanLen which is filled with the  concatenated 
   * waveforms of all channels.
   *
   * For each channel \a chanIdx, the waveform is given by the expression
   * \f[
   * \mbox{amp} \cdot \mbox{window}(t) \cdot \cos\left(2\pi \left(\mbox{chirp} \cdot
   * \frac{t^2}{2} + \mbox{freq} \cdot t\right)+ \mbox{phase}\right)
   * \f]
   */
  virtual void build_waveform( MP_Real_t *outBuffer );

  virtual int       has_field( int field );
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx);

  /** \brief Adds the representation of a Gabor atom to a time-frequency map */
  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

  virtual MP_Real_t dist_to_tfpoint( MP_Real_t time, MP_Real_t freq , MP_Chan_t chanIdx );

};


#endif /* gabor_atom_plugin_h_ */
